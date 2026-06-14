#include "usbserial.hh"
#include "logger.hh"
#include <QFileInfo>
#include <QSerialPortInfo>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <dirent.h>
#include <cstdlib>
#include <climits>

namespace {

/* ******************************************************************************************** *
 * sysfs VID/PID 回退读取（Flatpak 沙箱兼容）
 *
 * 背景：Flatpak 运行时配合 QT_SERIALPORT_SKIP_UDEV_LOOKUP=1 时，QSerialPortInfo
 * 对部分 USB 转串口驱动（典型如 ch341/CH340）无法返回 VID/PID，导致
 * USBSerial::detect() 跳过该端口。本函数从 sysfs 设备树逐级向上查找
 * idVendor / idProduct 文件，兼容 usb-serial-port / usb_interface /
 * usb_device 三种层级，填补沙箱内枚举空白。
 * ******************************************************************************************** */

/// 从 sysfs 读取串口设备的 VID/PID。
/// 使用 realpath() 解析符号链接，再逐级向上查找 idVendor / idProduct。
/// @param portName 串口名称，如 "ttyUSB0"。
/// @param vid 成功时写入 VID（16 进制解析）。
/// @param pid 成功时写入 PID（16 进制解析）。
/// @return 同时读取到有效 VID 与 PID 返回 true，否则 false。
bool
readSysfsVidPid(const QString &portName, uint16_t &vid, uint16_t &pid) {
  if (portName.isEmpty())
    return false;

  // 解析符号链接获取真实路径：/sys/class/tty/<port>/device -> 实际 PCI/USB 路径
  QByteArray devPath = QStringLiteral("/sys/class/tty/%1/device")
                           .arg(portName).toUtf8();
  char resolved[PATH_MAX];
  if (nullptr == realpath(devPath.constData(), resolved))
    return false;

  // 从真实路径逐级向上查找 idVendor / idProduct（最多 4 级）
  QString current = QString::fromUtf8(resolved);
  for (int level = 0; level < 4; ++level) {
    bool vidOk = false, pidOk = false;
    uint16_t v = 0, p = 0;

    // 读取 idVendor
    QFile vidFile(current + QStringLiteral("/idVendor"));
    if (vidFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      bool ok = false;
      uint val = QString::fromUtf8(vidFile.readAll()).trimmed().toUInt(&ok, 16);
      if (ok && val <= 0xFFFF) {
        v = static_cast<uint16_t>(val);
        vidOk = true;
      }
      vidFile.close();
    }

    // 读取 idProduct
    QFile pidFile(current + QStringLiteral("/idProduct"));
    if (pidFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      bool ok = false;
      uint val = QString::fromUtf8(pidFile.readAll()).trimmed().toUInt(&ok, 16);
      if (ok && val <= 0xFFFF) {
        p = static_cast<uint16_t>(val);
        pidOk = true;
      }
      pidFile.close();
    }

    if (vidOk && pidOk) {
      vid = v;
      pid = p;
      return true;
    }

    // 向上一级
    int idx = current.lastIndexOf(QLatin1Char('/'));
    if (idx <= 0)
      break;
    current = current.left(idx);
  }

  return false;
}

} // namespace

/* ******************************************************************************************** *
 * Implementation of USBSerial::Info
 * ******************************************************************************************** */
USBSerial::Descriptor::Descriptor(uint16_t vid, uint16_t pid, const QString &device, bool isSave)
  : USBDeviceDescriptor(USBDeviceInfo(Class::Serial, vid, pid, isSave), device)
{
  // pass...
}

/* ******************************************************************************************** *
 * Implementation of USBSerial
 * ******************************************************************************************** */
USBSerial::USBSerial(const USBDeviceDescriptor &descriptor, BaudRate rate, const ErrorStack &err, QObject *parent)
  : QSerialPort(parent), RadioInterface()
{
  if (USBDeviceInfo::Class::Serial != descriptor.interfaceClass()) {
    errMsg(err) << "Cannot open serial port for a non-serial descriptor: "
                << descriptor.description();
  }

  logDebug() << "Try to open " << descriptor.description() << ".";
  QSerialPortInfo port(descriptor.device().toString());
  this->setPort(port);
  if (! setParity(QSerialPort::NoParity)) {
    logWarn() << "Cannot set parity of the serial port to none.";
  }
  if (! setStopBits(QSerialPort::OneStop)) {
    logWarn() << "Cannot set stop bit.";
  }
  if (! setBaudRate(rate)) {
    logWarn() << "Cannot set speed to " << rate << " baud.";
  }
  if (! setFlowControl(QSerialPort::HardwareControl)) {
    logWarn() << "Cannot enable hardware flow control.";
  }

  if (! this->open(QIODevice::ReadWrite)) {
#ifdef Q_OS_UNIX
    QFileInfo portFileInfo(port.systemLocation());
    if (portFileInfo.exists() &&
        ((! portFileInfo.isReadable()) || (! portFileInfo.isWritable())))
    {
      QString owner = QString(portFileInfo.permission(QFileDevice::ReadOwner) ? "r" : "-")
          + (portFileInfo.permission(QFile::WriteOwner) ? "w" : "-")
          + (portFileInfo.permission(QFile::ExeOwner) ? "x" : "-");
      QString group = QString(portFileInfo.permission(QFileDevice::ReadGroup) ? "r" : "-")
          + (portFileInfo.permission(QFile::WriteGroup) ? "w" : "-")
          + (portFileInfo.permission(QFile::ExeGroup) ? "x" : "-");
      QString other = QString(portFileInfo.permission(QFileDevice::ReadOther) ? "r" : "-")
          + (portFileInfo.permission(QFile::WriteOther) ? "w" : "-")
          + (portFileInfo.permission(QFile::ExeOther) ? "x" : "-");
      errMsg(err) << "Insufficient rights to read or write '" << port.systemLocation()
                  << "' (" << port.description() << "): "
                  << portFileInfo.owner() << ": " << owner << ", "
                  << portFileInfo.group() << ": " << group << ", other: " << other << ".";
      if (portFileInfo.permission(QFile::ReadGroup | QFile::WriteGroup))
        errMsg(err) << "A membership in the group " << portFileInfo.group()
                    << " would grant access.";
    }
#endif
    errMsg(err) << "Cannot open serial port '" << port.portName()
                << "': " << this->errorString() << ".";
    return;
  }

  logDebug() << "Opened serial port " << this->portName() << " with "
             << this->baudRate() << "baud.";

  connect(this, SIGNAL(aboutToClose()), this, SLOT(onClose()));
  connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
          this, SLOT(onError(QSerialPort::SerialPortError)));
  connect(this, SIGNAL(dataTerminalReadyChanged(bool)), this, SLOT(signalingChanged()));
  connect(this, SIGNAL(requestToSendChanged(bool)), this, SLOT(signalingChanged()));
}

USBSerial::~USBSerial() {
  if (isOpen())
    close();
}

bool
USBSerial::isOpen() const {
  return QSerialPort::isOpen();
}

void
USBSerial::close() {
  if (isOpen())
    QSerialPort::close();
}

void
USBSerial::onError(QSerialPort::SerialPortError err) {
  logError() << "Serial port error: (" << err << ") " << errorString() << ".";
}

void
USBSerial::onClose() {
  logDebug() << "Serial port will close now.";
}

void
USBSerial::signalingChanged() {
  logDebug() << "Pinout signals changed to " << formatPinoutSignals() << ".";
}

QList<USBDeviceDescriptor>
USBSerial::detect(uint16_t vid, uint16_t pid, bool isSave) {
  QList<USBDeviceDescriptor> interfaces;
  // Find matching serial port by VID/PID.
  logDebug() << "Search for serial port with matching VID:PID " <<
                QString::number(vid, 16) << ":" << QString::number(pid, 16) << ".";
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  foreach (QSerialPortInfo port, ports) {
    bool vidPidMatch = false;
    // 优先使用 QSerialPortInfo 提供的 VID/PID（宿主机 udev 正常时的主路径）
    if (port.hasProductIdentifier() && port.hasVendorIdentifier()) {
      vidPidMatch = (vid == port.vendorIdentifier()) && (pid == port.productIdentifier());
    } else {
      // 回退：Flatpak 沙箱或部分驱动（如 ch341）下 QSerialPortInfo 读不到
      // VID/PID 时，从 sysfs 设备树逐级向上读取 idVendor / idProduct。
      uint16_t sysVid = 0, sysPid = 0;
      if (readSysfsVidPid(port.portName(), sysVid, sysPid)) {
        logDebug() << "sysfs fallback: " << port.portName()
                   << " -> VID:PID " << QString::number(sysVid, 16)
                   << ":" << QString::number(sysPid, 16) << ".";
        vidPidMatch = (vid == sysVid) && (pid == sysPid);
      }
    }
    if (vidPidMatch) {
      interfaces.append(Descriptor(vid, pid, port.portName(), isSave));
      logDebug() << "Found " << port.portName() << " (USB "
                 << QString::number(vid, 16) << ":" << QString::number(pid, 16) << ").";
    }
  }
  return interfaces;
}


QList<USBDeviceDescriptor>
USBSerial::detect() {
  QList<USBDeviceDescriptor> interfaces;
  logDebug() << "Search for serial ports.";
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  logDebug() << "Qt found " << ports.size() << " serial ports.";
  foreach (QSerialPortInfo port, ports) {
    uint16_t vid = 0, pid = 0;
    // 优先使用 QSerialPortInfo 提供的 VID/PID（宿主机 udev 正常时的主路径）
    if (port.hasProductIdentifier() && port.hasVendorIdentifier()) {
      vid = port.vendorIdentifier();
      pid = port.productIdentifier();
    } else if (readSysfsVidPid(port.portName(), vid, pid)) {
      // 回退：Flatpak 沙箱或部分驱动（如 ch341）下从 sysfs 读取
      logDebug() << "sysfs fallback: " << port.portName()
                 << " -> VID:PID " << QString::number(vid, 16)
                 << ":" << QString::number(pid, 16) << ".";
    }
    if (vid && pid) {
      interfaces.append(Descriptor(vid, pid, port.portName(), false));
      logDebug() << "Found " << port.portName() << " (USB "
                 << QString::number(vid, 16) << ":"
                 << QString::number(pid, 16) << ").";
    }
  }

  // 最终回退：Flatpak 沙箱内 QSerialPortInfo::availablePorts() 可能完全返回空
  // （无 udev 环境下 Qt 枚举失效）。此时直接扫描 /sys/class/tty/ 下的
  // ttyUSB* / ttyACM* 条目，从 sysfs 读取 VID/PID，填补枚举空白。
  // 使用原生 opendir/readdir，避免 QDir 对 sysfs 符号链接的过滤缺陷。
  if (interfaces.isEmpty()) {
    logDebug() << "Qt serial port enumeration empty, falling back to sysfs scan.";
    DIR *dir = opendir("/sys/class/tty");
    if (dir) {
      struct dirent *ent;
      while (nullptr != (ent = readdir(dir))) {
        QString entry(QString::fromLatin1(ent->d_name));
        if (entry.startsWith("ttyUSB") || entry.startsWith("ttyACM")) {
          uint16_t vid = 0, pid = 0;
          if (readSysfsVidPid(entry, vid, pid)) {
            interfaces.append(Descriptor(vid, pid, entry, false));
            logDebug() << "sysfs scan found " << entry
                       << " (USB " << QString::number(vid, 16)
                       << ":" << QString::number(pid, 16) << ").";
          }
        }
      }
      closedir(dir);
    } else {
      logDebug() << "Cannot open /sys/class/tty.";
    }
  }

  return interfaces;
}

QString
USBSerial::formatPinoutSignals() {
  if (QSerialPort::NoSignal == pinoutSignals())
    return "None";

  QStringList res;
  if (QSerialPort::DataTerminalReadySignal & pinoutSignals())
    res.append("Data Terminal Ready");
  if (QSerialPort::DataCarrierDetectSignal & pinoutSignals())
    res.append("Data Carrier Detect");
  if (QSerialPort::DataSetReadySignal & pinoutSignals())
    res.append("Data Set Ready");
  if (QSerialPort::RingIndicatorSignal & pinoutSignals())
    res.append("Ring Indicator");
  if (QSerialPort::RequestToSendSignal & pinoutSignals())
    res.append("Request To Send");
  if (QSerialPort::ClearToSendSignal & pinoutSignals())
    res.append("Clear To Send");
  if (QSerialPort::SecondaryTransmittedDataSignal & pinoutSignals())
    res.append("Secondary Transmitted Data");
  if (QSerialPort::SecondaryReceivedDataSignal & pinoutSignals())
    res.append("Secondary Received Data");

  return res.join(", ");
}