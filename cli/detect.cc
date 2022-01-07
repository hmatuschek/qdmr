#include "detect.hh"
#include <QCommandLineParser>
#include <QCoreApplication>

#include "logger.hh"
#include "radio.hh"
#include "radiointerface.hh"


int detect(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(parser);
  Q_UNUSED(app);

  QList<RadioInterface::Descriptor> interfaces = RadioInterface::detect();
  if (interfaces.isEmpty()) {
    logError() << "No compatible devices found.";
    return -1;
  }

  foreach (RadioInterface::Descriptor interface, interfaces) {
    logInfo() << "Found " << interface.description() << ". " << interface.longDescription() << ".";
  }

  if (1 < interfaces.count()) {
    logInfo() << "More than one interface detected. Do not proceed.";
    return 0;
  }

  logDebug() << "Only a single known radio interface detected, try to detect specific radio.";

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


