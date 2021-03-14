#include "readcodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>

#include <QApplication>

#include "logger.hh"
#include "radio.hh"
#include "printprogress.hh"
#include "config.hh"
#include "codeplug.hh"
#include "progressbar.hh"


int readCodeplug(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString forceRadio="";
  if (parser.isSet("radio")) {
    logWarn() << "You force the radio type to be '" << parser.value("radio").toUpper()
              << "' this is generally a very bad idea! You have been warned.";
    forceRadio = parser.value("radio");
  }

  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage, forceRadio);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << errorMessage;
    return -1;
  }

  QString filename = parser.positionalArguments().at(1);

  if (!parser.isSet("csv") && !filename.endsWith(".conf") && !filename.endsWith(".csv") &&
      !parser.isSet("cpl") && !filename.endsWith(".bin") && !filename.endsWith(".dfu")) {
    logError() << "Cannot determine output filetype, consider using --csv or --bin options.";
    return -1;
  }

  showProgress();
  QObject::connect(radio, &Radio::downloadProgress, updateProgress);

  Config config;
  if (! radio->startDownload(true)) {
    logError() << "Codeplug download error: " << radio->errorMessage();
    return -1;
  }

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug download error: " << radio->errorMessage();
    return -1;
  }

  logDebug() << "Save codeplug at '" << filename << "'.";
  // If output is CSV -> decode code-plug
  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    // decode codeplug
    if (! radio->codeplug().decode(&config)) {
      logError() << "Cannot decode codeplug: " << radio->errorMessage();
      return -1;
    }

    // try to write CSV file
    if (! config.writeCSV(filename, errorMessage)) {
      logError() << "Cannot write CSV file '" << filename << "': " << errorMessage;
      return -1;
    }

    return 0;
  }

  // otherwise write binary code-plug
  radio->codeplug().write(filename);

  return 0;
}
