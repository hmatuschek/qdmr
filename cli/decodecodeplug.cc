#include "decodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

#include "config.hh"
#include "uv390_codeplug.hh"
#include "rd5r_codeplug.hh"


int decodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (1 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QFile infile(parser.positionalArguments().at(0));
  if (! infile.open(QIODevice::ReadOnly)) {
    qDebug() << "Cannot decode binary codeplug file" << infile.fileName() << ":" << infile.errorString();
    return -1;
  }

  // Determine type by file size
  QFileInfo info(infile.fileName());
  if ((0xd0000 == info.size()) || ("uv390"==parser.value("radio").toLower()) || ("rt3s"==parser.value("radio").toLower())) {
    UV390Codeplug codeplug;
    if (codeplug.size() != size_t(infile.read((char *)codeplug.data(0), codeplug.size()))) {
      qDebug() << "Cannot decode binary codeplug file" << infile.fileName() << ":" << infile.errorString();
      return -1;
    }
    Config config;
    codeplug.decode(&config);
    infile.close();

    if (2 <= parser.positionalArguments().size()) {
      QFile outfile(parser.positionalArguments().at(1));
      if (! outfile.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot write CSV codeplug file" << outfile.fileName() << ":" << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream);
      outfile.close();
    } else {
      QTextStream stream(stdout);
      config.writeCSV(stream);
    }
  } else if ((0x20000 == info.size()) || ("rd5r"==parser.value("radio").toLower())) {
    RD5RCodeplug codeplug;
    if (codeplug.size() != size_t(infile.read((char *)codeplug.data(0), codeplug.size()))) {
      qDebug() << "Cannot decode binary codeplug file" << infile.fileName() << ":" << infile.errorString();
      return -1;
    }
    Config config;
    codeplug.decode(&config);
    infile.close();

    if (2 <= parser.positionalArguments().size()) {
      QFile outfile(parser.positionalArguments().at(1));
      if (! outfile.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot write CSV codeplug file" << outfile.fileName() << ":" << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream);
      outfile.close();
    } else {
      QTextStream stream(stdout);
      config.writeCSV(stream);
    }
  } else if (parser.isSet("radio")) {
    qDebug() << "Unknown radio type" << parser.value("radio");
    return -1;
  } else {
    qDebug() << "Cannot determine binary codeplug type. Consider specifying the radio type using --radio.";
    return -1;
  }

  return 0;
}

