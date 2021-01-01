#include "decodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>

#include "logger.hh"
#include "config.hh"
#include "uv390_codeplug.hh"
#include "rd5r_codeplug.hh"
#include "gd77_codeplug.hh"
#include "opengd77_codeplug.hh"
#include "d878uv_codeplug.hh"


int decodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);
  QString errorMessage;

  if (("uv390"==parser.value("radio").toLower()) || ("rt3s"==parser.value("radio").toLower())) {
    UV390Codeplug codeplug;
    if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "' : " << codeplug.errorMessage();
      return -1;
    }
    Config config;
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
    }

    if (3 <= parser.positionalArguments().size()) {
      QFile outfile(parser.positionalArguments().at(2));
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName()
                   << "': " << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream, errorMessage);
      outfile.close();
    } else {
      QTextStream stream(stdout);
      config.writeCSV(stream, errorMessage);
    }
  } else if ("rd5r"==parser.value("radio").toLower()) {
    RD5RCodeplug codeplug;
    if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
    }
    Config config;
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
    }

    if (3 <= parser.positionalArguments().size()) {
      QFile outfile(parser.positionalArguments().at(2));
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName()
                   << "': " << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream, errorMessage);
      outfile.close();
    } else {
      QTextStream stream(stdout);
      config.writeCSV(stream, errorMessage);
    }
  } else if ("gd77"==parser.value("radio").toLower()) {
    GD77Codeplug codeplug;
    if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
    }
    Config config;
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
    }

    if (3 <= parser.positionalArguments().size()) {
      QFile outfile(parser.positionalArguments().at(2));
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName()
                   << "': " << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream, errorMessage);
      outfile.close();
    } else {
      QTextStream stream(stdout);
      config.writeCSV(stream, errorMessage);
    }
  } else if ("opengd77"==parser.value("radio").toLower()) {
    OpenGD77Codeplug codeplug;
    if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
    }
    Config config;
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
    }

    if (3 <= parser.positionalArguments().size()) {
      QFile outfile(parser.positionalArguments().at(2));
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName()
                   << "': " << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream, errorMessage);
      outfile.close();
    } else {
      QTextStream stream(stdout);
      config.writeCSV(stream, errorMessage);
    }
  } else if ("d878uv"==parser.value("radio").toLower()) {
    D878UVCodeplug codeplug;
    if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
      return -1;
    }
    Config config;
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
      return -1;
    }

    if (3 <= parser.positionalArguments().size()) {
      QFile outfile(parser.positionalArguments().at(2));
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName() << "':" << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream, errorMessage);
      outfile.close();
    } else {
      QTextStream stream(stdout);
      config.writeCSV(stream, errorMessage);
    }
  } else if (parser.isSet("radio")) {
    logError() << "Unknown radio type '" << parser.value("radio") << "'.";
    return -1;
  } else {
    logError() << "Cannot determine binary codeplug type. Consider specifying the radio type using --radio.";
    return -1;
  }

  return 0;
}

