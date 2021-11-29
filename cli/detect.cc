#include "detect.hh"
#include <QCommandLineParser>
#include <QCoreApplication>

#include "logger.hh"
#include "radio.hh"
#include "radiointerface.hh"


int detect(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(parser);
  Q_UNUSED(app);

  ErrorStack err;
  Radio *radio = Radio::detect(RadioInfo(), err);
  if (nullptr == radio) {
    logError() << "No compatible radio found: " << err.format();
    return -1;
  }

  logInfo() << "Found: '" << radio->name() << "'.";
  delete  radio;

  return 0;
}


