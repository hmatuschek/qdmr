#include "decodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>

#include "logger.hh"
#include "config.hh"
#include "uv390_codeplug.hh"
#include "uv390_filereader.hh"
#include "md2017_codeplug.hh"
#include "md2017_filereader.hh"
#include "rd5r_codeplug.hh"
#include "rd5r_filereader.hh"
#include "gd77_codeplug.hh"
#include "gd77_filereader.hh"
#include "opengd77_codeplug.hh"
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
  Config config;

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
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
    }
  } else if (("md2017"==radio) || ("rt82"==radio)) {
    MD2017Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! MD2017FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "': " << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "' : " << codeplug.errorMessage();
      return -1;
    }
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
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
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
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
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
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
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "': " << codeplug.errorMessage();
      return -1;
    }
  } else if ("d878uv" == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" << radio << "'.";
      return -1;
    }
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
  } else if ("d868uv" == radio) {
    D868UVCodeplug codeplug;
    if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
      return -1;
    }
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
      return -1;
    }
  } else if ("d578uv" == radio) {
    D578UVCodeplug codeplug;
    if (! codeplug.read(filename)) {
      logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
      return -1;
    }
    if (! codeplug.decode(&config)) {
      logError() << "Cannot decode binary codeplug file '" << filename << "':" << codeplug.errorMessage();
      return -1;
    }
  } else if (parser.isSet("radio")) {
    logError() << "Unknown radio type '" << parser.value("radio") << "'.";
    return -1;
  } else {
    logError() << "Cannot determine binary codeplug type. Consider specifying the radio type using --radio.";
    return -1;
  }

  if (3 <= parser.positionalArguments().size()) {
    QFileInfo info(parser.positionalArguments().at(2));
    if (("conf" == info.suffix()) || ("csv" == info.suffix()) || parser.isSet("csv")) {
      QFile outfile(info.filePath());
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName()
                   << "': " << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.writeCSV(stream, errorMessage);
      outfile.close();
    } else if (("yaml" == info.suffix()) || parser.isSet("yaml")) {
      QFile outfile(info.filePath());
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName()
                   << "': " << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      config.toYAML(stream);
      outfile.close();
    } else {
      logError() << "Cannot determine codeplug output file format. Consider using --csv or --yaml.";
      return -1;
    }
  } else {
    QTextStream stream(stdout);
    if (parser.isSet("csv")) {
      if (! config.writeCSV(stream, errorMessage)) {
        logError() << "Cannot serialize codeplug into CSV: " << errorMessage;
        return -1;
      }
    } else if (parser.isSet("yaml")) {
      if (! config.toYAML(stream)) {
        logError() << "Cannot serialize codeplug into YAML.";
        return -1;
      }
    } else {
      logError() << "Cannot determine codeplug output file format. Consider using --csv or --yaml.";
      return -1;
    }
  }

  return 0;
}

