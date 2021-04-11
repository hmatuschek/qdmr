#include "encodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>

#include "logger.hh"
#include "config.hh"
#include "uv390_codeplug.hh"
#include "rd5r_codeplug.hh"
#include "gd77_codeplug.hh"
#include "opengd77_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "crc32.hh"


int encodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (3 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QFile infile(parser.positionalArguments().at(1));
  if (! infile.open(QIODevice::ReadOnly)) {
    logError() << "Cannot encode CSV codeplug file '" << infile.fileName() << "': " << infile.errorString();
    return -1;
  }

  if (! parser.isSet("radio")) {
    logError() << "You have to specify the radio using the --radio option.";
    parser.showHelp(-1);
  }

  CodePlug::Flags flags;
  if (parser.isSet("init-codeplug"))
    flags.updateCodePlug = false;
  if (parser.isSet("auto-enable-gps"))
    flags.autoEnableGPS = true;
  if (parser.isSet("auto-enable-roaming"))
    flags.autoEnableRoaming = true;

  if (("uv390"==parser.value("radio").toLower()) || ("rt3s"==parser.value("radio").toLower())) {
    Config config;
    QString errorMessage;
    QTextStream stream(&infile);
    if (! config.readCSV(stream, errorMessage)) {
      logError() << "Cannot parse CSV codeplug '" << infile.fileName() << "': " << errorMessage;
      return -1;
    }

    UV390Codeplug codeplug;
    codeplug.encode(&config, flags);
    if (! codeplug.write(parser.positionalArguments().at(2))) {
      logError() << "Cannot write output codeplug file '" << parser.positionalArguments().at(1)
                 << "': " << codeplug.errorMessage();
      return -1;
    }
  } else if ("rd5r"==parser.value("radio").toLower()) {
    Config config;
    QString errorMessage;
    QTextStream stream(&infile);
    if (! config.readCSV(stream, errorMessage)) {
      logError() << "Cannot parse CSV codeplug '" << infile.fileName() << "': " << errorMessage;
      return -1;
    }
    RD5RCodeplug codeplug;
    codeplug.encode(&config, flags);
    if (! codeplug.write(parser.positionalArguments().at(2))) {
      logError() << "Cannot write output codeplug file '" << parser.positionalArguments().at(1)
                 << "': " << codeplug.errorMessage();
      return -1;
    }
  } else if ("gd77"==parser.value("radio").toLower()) {
    Config config;
    QString errorMessage;
    QTextStream stream(&infile);
    if (! config.readCSV(stream, errorMessage)) {
      logError() << "Cannot parse CSV codeplug '" << infile.fileName() << "': " << errorMessage;
      return -1;
    }
    GD77Codeplug codeplug;
    codeplug.encode(&config, flags);
    if (! codeplug.write(parser.positionalArguments().at(2))) {
      logError() << "Cannot write output codeplug file '" << parser.positionalArguments().at(1)
                 << "': " << codeplug.errorMessage();
      return -1;
    }
  } else if ("opengd77"==parser.value("radio").toLower()) {
    Config config;
    QString errorMessage;
    QTextStream stream(&infile);
    if (! config.readCSV(stream, errorMessage)) {
      logError() << "Cannot parse CSV codeplug '" << infile.fileName() << "': " << errorMessage;
      return -1;
    }
    OpenGD77Codeplug codeplug;
    codeplug.encode(&config, flags);
    if (! codeplug.write(parser.positionalArguments().at(2))) {
      logError() << "Cannot write output codeplug file '" << parser.positionalArguments().at(1)
                 << "': " << codeplug.errorMessage();
      return -1;
    }
  } else if ("d878uv"==parser.value("radio").toLower()) {
    Config config;
    QString errorMessage;
    QTextStream stream(&infile);
    if (! config.readCSV(stream, errorMessage)) {
      logError() << "Cannot parse CSV codeplug '" << infile.fileName() << "': " << errorMessage;
      return -1;
    }
    D878UVCodeplug codeplug;
    codeplug.setBitmaps(&config);
    codeplug.allocateUpdated();
    codeplug.allocateForEncoding();
    codeplug.encode(&config, flags);
    codeplug.image(0).sort();
    if (! codeplug.write(parser.positionalArguments().at(2))) {
      logError() << "Cannot write output codeplug file '" << parser.positionalArguments().at(1)
                 << "': " << codeplug.errorMessage();
      return -1;
    }
  } else {
    logError() << "Cannot encode codeplug: Unknown radio '" << parser.value("radio") << "'.";
    return -1;
  }

  return 0;
}
