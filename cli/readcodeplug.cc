#include "readcodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDebug>

#include <QApplication>

#include "radio.hh"
#include "printprogress.hh"
#include "config.hh"
#include "codeplug.hh"


int readCodeplug(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (0 == parser.positionalArguments().size())
    parser.showHelp(-1);

  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    qDebug() << "Cannot detect radio:" << errorMessage;
    return -1;
  }

  QString filename = parser.positionalArguments()[0];

  if (!parser.isSet("csv") && !filename.endsWith(".conf") && !filename.endsWith(".csv") &&
      !parser.isSet("cpl") && !filename.endsWith(".bin") && !filename.endsWith(".dfu")) {
    qDebug() << "Cannot determine output filetype, consider using --csv or --bin options.";
    return -1;
  }

  Config config;
  radio->startDownload(&config, true);

  if (Radio::StatusError == radio->status()) {
    qDebug() << "Codeplug download error: " << radio->errorMessage();
    return -1;
  }

  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    if (! config.writeCSV(filename)) {
      qDebug() << "Cannot write CSV file" << filename;
      return -1;
    }
    return 0;
  }

  radio->codeplug().write(filename);
  return 0;
}
