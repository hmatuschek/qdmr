#include "detect.hh"
#include <QCommandLineParser>
#include <QCoreApplication>

#include "logger.hh"
#include "radio.hh"
#include "radiointerface.hh"


int detect(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(parser);
  Q_UNUSED(app);

  QList<USBDeviceDescriptor> interfaces = USBDeviceDescriptor::detect();
  if (interfaces.isEmpty()) {
    logError() << "No compatible devices found.";
    return -1;
  }

  foreach (USBDeviceDescriptor interface, interfaces) {
    logInfo() << "Found " << interface.description() << ". " << interface.longDescription() << ".";
  }

  if (1 < interfaces.count()) {
    logInfo() << "More than one interface detected. Please specify which device to use.";
    return 0;
  }

  if (! interfaces.first().isSave()) {
    logError() << "Is it not save to assume that the detected device " << interfaces.first().description()
               << " is a DMR radio. Confirm by specifying the interface explicitly using --device."
               << " This issue arises if a radio uses a generic USB-to-serial chip which may also be"
               << " used by other devices.";
    return -1;
  }

  logDebug() << "Only a single known radio interface detected, try to detect specific radio.";
  if (! interfaces.first().isIdentifiable()) {
    logError() << "The protocol implemented by the radio does not provide means for identifying the"
               << " device. You need to specify the radio explicitly using --radio.";
    return -1;
  }

  ErrorStack err;
  Radio *radio = Radio::detect(interfaces.first(), RadioInfo(), err);
  if (nullptr == radio) {
    logError() << err.format();
    return -1;
  }

  logInfo() << "Found: '" << radio->name() << "' at " << interfaces.first().description() << ".";
  delete  radio;

  return 0;
}


