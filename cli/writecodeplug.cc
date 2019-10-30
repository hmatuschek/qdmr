#include "writecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>

#include "radio.hh"
#include "config.hh"


int writeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (0 == parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments()[0];

  Config config;
  if (! config.readCSV(filename)) {
    qDebug() << "Cannot read CSV file" << filename;
    return -1;
  }

  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    qDebug() << "Cannot detect radio:" << errorMessage;
    return -1;
  }

  radio->startUpload(&config, true);

  if (Radio::StatusError == radio->status()) {
    qDebug() << "Codeplug upload error: " << radio->errorMessage();
    return -1;
  }

  return 0;
}
