#include "readcodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDebug>

#include "radio.hh"
#include "printprogress.hh"
#include "config.hh"
#include "codeplug.hh"


int readCodeplug(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (0 == parser.positionalArguments().size())
    parser.showHelp(-1);

  Radio *radio = Radio::detect();
  if (nullptr == radio) {
    qDebug() << "Cannot detect radio.";
    return -1;
  }

  QString filename = parser.positionalArguments()[0];

  if (!parser.isSet("csv") && !filename.endsWith(".conf") && !filename.endsWith(".csv") &&
      !parser.isSet("cpl") && !filename.endsWith(".bin") && !filename.endsWith(".dfu")) {
    qDebug() << "Cannot determine output filetype, consider using --csv or --bin options.";
    return -1;
  }

  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly)) {
    qDebug() << "ERROR: Cannot open file" << filename;
    return -1;
  }

  app.connect(radio, SIGNAL(downloadError(Radio *radio)), &app, SLOT(quit()));
  app.connect(radio, SIGNAL(downloadFinished()), &app, SLOT(quit()));
  app.connect(radio, &Radio::downloadProgress, print_progress);

  Config config;
  if (radio->startDownload(&config))
    app.exec();

  radio->wait(10000);

  if (Radio::StatusError == radio->status()) {
    qDebug() << "Codeplug download error: " << radio->errorMessage();
    return -1;
  }

  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    qDebug() << "BUG: CSV dump not implemented yet.";
    return -1;
  }

  radio->codeplug().write(filename);
  return 0;
}
