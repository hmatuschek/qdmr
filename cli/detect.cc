#include "detect.hh"
#include <QCommandLineParser>
#include <QCoreApplication>

#include "logger.hh"
#include "radio.hh"
#include "radiointerface.hh"
#include "autodetect.hh"

int detect(QCommandLineParser &parser, QCoreApplication &app) {
  // Try to detect a radio
  ErrorStack err;
  Radio *radio = autoDetect(parser, app, err);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: \n" << err.format("  ");
    return -1;
  }

  logInfo() << "Found: '" << radio->name() << "'.";
  delete  radio;

  return 0;
}


