#include "encodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDebug>
#include "config.hh"
#include "uv390_codeplug.hh"
#include "rd5r_codeplug.hh"

#include "crc32.hh"


int encodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 != parser.positionalArguments().size())
    parser.showHelp(-1);

  QFile infile(parser.positionalArguments().at(0));
  if (! infile.open(QIODevice::ReadOnly)) {
    qDebug() << "Cannot encode CSV codeplug file" << infile.fileName() << ":" << infile.errorString();
    return -1;
  }

  if (! parser.isSet("radio")) {
    qDebug() << "You have to specify the radio using the --radio option.";
    parser.showHelp(-1);
  }

  if (("uv390"==parser.value("radio").toLower()) || ("rt3s"==parser.value("radio").toLower())) {
    Config config;
    QTextStream stream(&infile);
    if (! config.readCSV(stream)) {
      qDebug() << "Cannot parse CSV codeplug" << infile.fileName();
      return -1;
    }
    UV390Codeplug codeplug;
    codeplug.encode(&config);
    if (! codeplug.write(parser.positionalArguments().at(1))) {
      qDebug() << "Cannot write output codeplug file" << parser.positionalArguments().at(1)
               << ":" << codeplug.errorMessage();
      return -1;
    }
  } else if ("rd5r"==parser.value("radio").toLower()) {
    Config config;
    QTextStream stream(&infile);
    if (! config.readCSV(stream)) {
      qDebug() << "Cannot parse CSV codeplug" << infile.fileName();
      return -1;
    }
    RD5RCodeplug codeplug;
    codeplug.encode(&config);
    if (! codeplug.write(parser.positionalArguments().at(1))) {
      qDebug() << "Cannot write output codeplug file" << parser.positionalArguments().at(1)
               << ":" << codeplug.errorMessage();
      return -1;
    }
  }

  return 0;
}
