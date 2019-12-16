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

  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << errorMessage;
    return -1;
  }

  radio->startUpload(&config, true);

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug upload error: " << radio->errorMessage();
    return -1;
  }

  return 0;
}
