#include "decodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>

#include "logger.hh"
#include "config.hh"
#include "uv390_codeplug.hh"
#include "uv390_filereader.hh"
#include "rd5r_codeplug.hh"
#include "rd5r_filereader.hh"
#include "gd77_codeplug.hh"
#include "gd77_filereader.hh"
#include "opengd77_codeplug.hh"
#include "anytone_filereader.hh"
#include "d868uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv2_codeplug.hh"
#include "d578uv_codeplug.hh"


int decodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);
  QString errorMessage;
  QString radio = parser.value("radio").toLower();
  if (("uv390"==radio) || ("rt3s"==radio)) {
    UV390Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! UV390FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "': " << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename)) {
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
  } else if ("rd5r"==radio) {
    RD5RCodeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! RD5RFileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "': " << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename)) {
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
  } else if ("gd77" == radio) {
    GD77Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! GD77FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "': " << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename)) {
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
  } else if ("opengd77" == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" << radio << "'.";
      return -1;
    }
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
  } else if (("d878uv" == radio) || ("d878uv2" == radio) || ("d868uv" == radio) || ("d578uv" == radio)) {
    Config config;
    if (! parser.isSet("manufacturer")) {
      // If DFU format -> read file, dispatch by radio
      if ("d868uv" == radio) {
        D868UVCodeplug codeplug;
        if (! codeplug.read(filename)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
        if (! codeplug.decode(&config)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
      } else if ("d878uv" == radio) {
        D878UVCodeplug codeplug;
        if (! codeplug.read(filename)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
        if (! codeplug.decode(&config)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
      } else if ("d878uv2" == radio) {
        D878UV2Codeplug codeplug;
        if (! codeplug.read(filename)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
        if (! codeplug.decode(&config)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
      } if ("d578uv" == radio) {
        D578UVCodeplug codeplug;
        if (! codeplug.read(filename)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
        if (! codeplug.decode(&config)) {
          logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
          return -1;
        }
      }
    } else {
      // If manufacturer codeplug file
      QString message;
      if (! AnytoneFileReader::read(filename, &config, message)) {
        logError() << message;
        return -1;
      }
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

