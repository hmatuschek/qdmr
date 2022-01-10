#include "autodetect.hh"

#include "logger.hh"
#include "radioinfo.hh"
#include "usbdevice.hh"

QVariant
parseDeviceHandle(const QString &device) {
  QRegExp pattern("([0-9]+):([0-9]+)");
  if (pattern.exactMatch(device.simplified())) {
    return QVariant::fromValue(USBDeviceAddress(pattern.cap(1).toUInt(), pattern.cap(2).toUInt()));
  }
  return QVariant(device.simplified());
}

QString formatDeviceHandle(const USBDeviceDescriptor &device) {
  if (! device.isValid())
    return "[invalid]";
  if (USBDeviceInfo::Class::Serial == device.interfaceClass())
    return device.device().toString();
  USBDeviceAddress addr = device.device().value<USBDeviceAddress>();
  return QString("%1:%2").arg(addr.bus).arg(addr.device);
}

void
printDevices(QTextStream &out, const QList<USBDeviceDescriptor> &devices) {
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << " Device";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << "Type";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(60); out << "Description";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(60); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
  foreach (USBDeviceDescriptor device, devices) {
    if (USBDeviceInfo::Class::None == device.interfaceClass())
      continue;
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(8);
    if (USBDeviceInfo::Class::Serial == device.interfaceClass()) {
      out << device.device().toString();
    } else {
      USBDeviceAddress addr = device.device().value<USBDeviceAddress>();
      out << addr.bus << ":" << addr.device;
    }
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << device.description();
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << device.longDescription() << "\n";
  }
}

Radio *
autoDetect(QCommandLineParser &parser, QCoreApplication &app) {
  logDebug() << "Autodetect radios.";

  QList<USBDeviceDescriptor> interfaces = USBDeviceDescriptor::detect();
  if (interfaces.isEmpty()) {
    logError() << "No matching USB devices are found. Check connection?";
    return nullptr;
  }

  USBDeviceDescriptor device;
  if (parser.isSet("device")) {
    // If a device is passed by option, search for matching handle
    QVariant devHandle = parseDeviceHandle(parser.value("device"));
    foreach (USBDeviceDescriptor dev, interfaces) {
      if (dev.device() == devHandle) {
        device = dev;
        break;
      }
    }
    if (! device.isValid()) {
      LogMessage msg(LogMessage::ERROR, __FILE__, __LINE__);
      msg << "Device handle '" << parser.value("device") << "' not found in:\n";
      printDevices(msg, interfaces);
      return nullptr;
    }
  } else if (1 != interfaces.size()) {
    // If no device is specified, there should only be one interface
    logError() << "Cannot auto-detect radio, more than one matching USB devices found:"
               << " Use --device option to specifiy to which device to talk to.";
    LogMessage msg(LogMessage::INFO, __FILE__, __LINE__);
    msg << "Devices found:\n";
    printDevices(msg, interfaces);
    return nullptr;
  } else if (! interfaces.first().isSave()) {
    LogMessage msg(LogMessage::ERROR, __FILE__, __LINE__);
    msg << "It is not save to assume that the device:\n";
    printDevices(msg, interfaces);
    msg << "is a DMR radio. Please specify the device explicitly to verify correctness.";
    return nullptr;
  } else {
    // The first device is save to use
    device = interfaces.first();
  }

  LogMessage msg(LogMessage::DEBUG, __FILE__, __LINE__);
  msg << "Using device " << formatDeviceHandle(device) << ".";

  // Handle identifiability of radio
  if (parser.isSet("radio")) {
    RadioInfo radio = RadioInfo::byKey(parser.value("radio"));
    if (! radio.isValid()) {
      logError() << "Uknown radio '" << parser.value("radio") << "'.";
      return nullptr;
    }
    ErrorStack err;
    Radio *rad = Radio::detect(device, radio, err);
    if (nullptr == rad) {
      logError() << "Cannot detect radio: " << err.format();
      return nullptr;
    }
    return rad;
  } else if (! device.isIdentifiable()) {
    // Collect all radio keys for the device
    QStringList radios;
    foreach (RadioInfo info, RadioInfo::allRadios(device)) {
      radios.append(info.key());
    }
    logError() << "It is not possible to identify the radio connected to the device '"
               << formatDeviceHandle(device) << ". You have to specify which radio to use using "
               << "the --radio option. Possible radios for this device are "
               << radios.join(", ") << ".";
    return nullptr;
  }

  // Try auto-detect:
  ErrorStack err;
  Radio *rad = Radio::detect(device, RadioInfo(), err);
  if (nullptr == rad) {
    logError() << "Cannot auto-detect radio: " << err.format();
    return nullptr;
  }
  return rad;
}
