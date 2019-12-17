#include "writecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>

#include "logger.hh"
#include "radio.hh"
#include "config.hh"


int writeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);

  QString errorMessage;
  Config config;
  if (! config.readCSV(filename, errorMessage)) {
    logError() << "Cannot read CSV file '" << filename << "': " << errorMessage;
    return -1;
  }
  logDebug() << "Read codeplug from '" << filename << "'.";

  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << errorMessage;
    return -1;
  }
  logDebug() << "Start upload to " << radio->name() << ".";
  if (! radio->startUpload(&config, true)) {
    logError() << "Codeplug upload error: " << radio->errorMessage();
    return -1;
  }

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug upload error: " << radio->errorMessage();
    return -1;
  }

  logDebug() << "Upload completed.";
  return 0;
}
