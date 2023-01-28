#include "decodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>

#include "logger.hh"
#include "config.hh"
#include "radioinfo.hh"
#include "md390_codeplug.hh"
#include "md390_filereader.hh"
#include "uv390_codeplug.hh"
#include "uv390_filereader.hh"
#include "md2017_codeplug.hh"
#include "md2017_filereader.hh"
#include "dm1701_codeplug.hh"
#include "dm1701_filereader.hh"
#include "rd5r_codeplug.hh"
#include "rd5r_filereader.hh"
#include "gd77_codeplug.hh"
#include "gd77_filereader.hh"
#include "opengd77_codeplug.hh"
#include "openrtx_codeplug.hh"
#include "anytone_filereader.hh"
#include "d868uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv2_codeplug.hh"
#include "d578uv_codeplug.hh"
#include "dmr6x2uv_codeplug.hh"
#include "dr1801uv_codeplug.hh"
#include "dr1801uv_filereader.hh"


int decodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);
  QString errorMessage;
  ErrorStack err;

  if (! parser.isSet("radio")) {
    logError() << "No radio type is specified! Use the --radio option.";
    return -1;
  }

  if (! RadioInfo::hasRadioKey(parser.value("radio").toLower())) {
    QStringList radios;
    foreach (RadioInfo info, RadioInfo::allRadios())
      radios.append(info.key());
    logError() << "Unknown radio '" << parser.value("radio").toLower() << ".";
    logError() << "Known radios " << radios.join(", ") << ".";
    return -1;
  }

  RadioInfo::Radio radio = RadioInfo::byKey(parser.value("radio").toLower()).id();
  Config config;

  if (RadioInfo::MD390 == radio) {
    MD390Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! MD390FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "':\n" << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "' :\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::UV390 == radio) {
    UV390Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! UV390FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "': " << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "' :\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::MD2017 == radio) {
    MD2017Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! MD2017FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "':\n" << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "' :\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::DM1701 == radio) {
    DM1701Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! DM1701FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "':\n" << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "' :\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::RD5R == radio) {
    RD5RCodeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! RD5RFileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "':\n" << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::GD77 == radio) {
    GD77Codeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! GD77FileReader::read(filename, &codeplug, errorMessage)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "':\n" << errorMessage;
        return -1;
      }
    } else if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::OpenGD77 == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" <<
                    RadioInfo::byID(radio).name() << "'.";
      return -1;
    }
    OpenGD77Codeplug codeplug;
    if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::OpenRTX == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" <<
                    RadioInfo::byID(radio).name() << "'.";
      return -1;
    }
    OpenRTXCodeplug codeplug;
    if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::D868UVE == radio) {
    D868UVCodeplug codeplug;
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '"
                 << RadioInfo::byID(radio).name() << "'.";
      return -1;
    }
    if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::D878UV == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" <<
                    RadioInfo::byID(radio).name() << "'.";
      return -1;
    }
    D878UVCodeplug codeplug;
    if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::D878UVII == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" <<
                    RadioInfo::byID(radio).name() << "'.";
      return -1;
    }
    D878UV2Codeplug codeplug;
    if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename <<
                    "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::D578UV == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" <<
                    RadioInfo::byID(radio).name() << "'.";
      return -1;
    }
    D578UVCodeplug codeplug;
    if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::DMR6X2UV == radio) {
    if (parser.isSet("manufacturer")) {
      logError() << "Decoding of manufacturer codeplug is not implemented for radio '" <<
                    RadioInfo::byID(radio).name() << "'.";
      return -1;
    }
    DMR6X2UVCodeplug codeplug;
    if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else if (RadioInfo::DR1801UV == radio) {
    DR1801UVCodeplug codeplug;
    if (parser.isSet("manufacturer")) {
      if (! DR1801UVFileReader::read(filename, &codeplug, err)) {
        logError() << "Cannot decode manufacturer codeplug file '" << filename
                   << "':\n" << err.format();
        return -1;
      }
    } else if (! codeplug.read(filename, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "' :\n" << err.format();
      return -1;
    }
    if (! codeplug.decode(&config, err)) {
      logError() << "Cannot decode binary codeplug file '" << filename
                 << "':\n" << err.format();
      return -1;
    }
  } else {
    logError() << "Decoding not implemented for " << RadioInfo::byID(radio).name() << ".";
    return -1;
  }

  if (3 <= parser.positionalArguments().size()) {
    QFileInfo info(parser.positionalArguments().at(2));
    if (("conf" == info.suffix()) || ("csv" == info.suffix()) || parser.isSet("csv")) {
      logError() << "Export of the old table based format was disabled with 0.9.0. "
                    "Import still works.";
      return -1;
    } else if (("yaml" == info.suffix()) || parser.isSet("yaml")) {
      QFile outfile(info.filePath());
      if (! outfile.open(QIODevice::WriteOnly)) {
        logError() << "Cannot write CSV codeplug file '" << outfile.fileName()
                   << "':\n" << outfile.errorString();
        return -1;
      }
      QTextStream stream(&outfile);
      if (! config.toYAML(stream, err)) {
        logError() << "Cannot serialize codeplug to YAML:\n"
                   << err.format(" ");
      }
      outfile.close();
    } else {
      logError() << "Cannot determine codeplug output file format. Consider using --csv or --yaml.";
      return -1;
    }
  } else {
    QTextStream stream(stdout);
    if (parser.isSet("csv")) {
      logError() << "Export of the old table based format was disabled with 0.9.0. "
                    "Import still works.";
      return -1;
    } else if (parser.isSet("yaml")) {
      if (! config.toYAML(stream, err)) {
        logError() << "Cannot serialize codeplug into YAML:\n"
                   << err.format(" ");
        return -1;
      }
    } else {
      logError() << "Cannot determine codeplug output file format. Consider using --csv or --yaml.";
      return -1;
    }
  }

  return 0;
}

