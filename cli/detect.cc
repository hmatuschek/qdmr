#include "detect.hh"
#include <QCommandLineParser>
#include <QCoreApplication>

#include "logger.hh"
#include "radio.hh"
#include "radiointerface.hh"
#include "autodetect.hh"

int detect(QCommandLineParser &parser, QCoreApplication &app) {
  // Try to detect a radio
  Radio *radio = autoDetect(parser, app);
  logInfo() << "Found: '" << radio->name() << "'.";
  delete  radio;

  return 0;
}


