#include "autodetect.hh"

#include "logger.hh"
#include "radioinfo.hh"
#include "usbdevice.hh"

QVariant
parseDeviceHandle(const QString &device) {
  QRegExp pattern("([0-9]+):([0-9]+)");
  if (pattern.exactMatch(device.simplified())) {
    return QVariant::fromValue(USBDeviceHandle(pattern.cap(1).toUInt(), pattern.cap(2).toUInt()));
  }
  return QVariant(device.simplified());
}

void
printDevices(QTextStream &out, const QList<USBDeviceDescriptor> &devices) {
  foreach (USBDeviceDescriptor device, devices) {
    if (USBDeviceInfo::Class::None == device.interfaceClass())
      continue;
    out << "Device '";
    if (USBDeviceInfo::Class::Serial == device.interfaceClass()) {
      out << device.device().toString() << "'\n";
    } else if (USBDeviceInfo::Class::DFU == device.interfaceClass()) {
      USBDeviceHandle addr = device.device().value<USBDeviceHandle>();
      out << QString("%1:%2").arg(addr.bus).arg(addr.device) << "'\n";
    } else if (USBDeviceInfo::Class::HID == device.interfaceClass()) {
      USBDeviceHandle addr = device.device().value<USBDeviceHandle>();
      out << QString("%1:%2").arg(addr.bus).arg(addr.device) << "'\n";
    }
    out << " Type:        " << device.description() << "\n";
    out << " Description: " << device.longDescription() << "\n";
  }
}

Radio *
autoDetect(QCommandLineParser &parser, QCoreApplication &app, const ErrorStack &err) {
  Q_UNUSED(app)

  logDebug() << "Autodetect radios.";

  QList<USBDeviceDescriptor> interfaces = USBDeviceDescriptor::detect();
  if (interfaces.isEmpty()) {
    errMsg(err) << "No matching USB devices are found. Check connection?";
    return nullptr;
  }
  logDebug() << "Found " << interfaces.count() << " device(s):";
  foreach (USBDeviceDescriptor d, interfaces) {
    logDebug() << "  " << d.description() << ".";
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
      ErrorStack::MessageStream msg(err, __FILE__, __LINE__);
      msg << "Device handle '" << parser.value("device") << "' not found in:\n";
      printDevices(msg, interfaces);
      return nullptr;
    }
  } else if (1 != interfaces.size()) {
    // If no device is specified, there should only be one interface
    ErrorStack::MessageStream msg(err, __FILE__, __LINE__);
    msg << "Cannot auto-detect radio, more than one matching USB devices found:"
        << " Use --device option to specifiy to which device to talk to. Devices found:\n";
    printDevices(msg, interfaces);
    return nullptr;
  } else if (! interfaces.first().isSave()) {
    ErrorStack::MessageStream msg(err, __FILE__, __LINE__);
    msg << "It is not save to assume that the device:\n";
    printDevices(msg, interfaces);
    msg << "is a DMR radio. Please specify the device explicitly to verify correctness.";
    return nullptr;
  } else {
    // The first device is save to use
    device = interfaces.first();
  }

  logDebug() << "Using device " << device.deviceHandle() << ".";

  // Handle identifiability of radio
  if (parser.isSet("radio")) {
    RadioInfo radio = RadioInfo::byKey(parser.value("radio").toLower());
    if (! radio.isValid()) {
      errMsg(err) << "Uknown radio '" << parser.value("radio").toLower() << "'.";
      return nullptr;
    }
    Radio *rad = Radio::detect(device, radio, err);
    if (nullptr == rad) {
      logError() << "Cannot detect radio.";
      return nullptr;
    }
    return rad;
  } else if (! device.isIdentifiable()) {
    // Collect all radio keys for the device
    QStringList radios;
    foreach (RadioInfo info, RadioInfo::allRadios(device)) {
      radios.append(info.key());
    }
    errMsg(err) << "It is not possible to identify the radio connected to the device '"
                << device.deviceHandle() << ". You have to specify which radio to use using "
                << "the --radio option. Possible radios for this device are "
                << radios.join(", ") << ".";
    return nullptr;
  }

  // Try auto-detect:
  Radio *rad = Radio::detect(device, RadioInfo(), err);
  if (nullptr == rad) {
    errMsg(err) << "Cannot auto-detect radio.";
    return nullptr;
  }
  return rad;
}
