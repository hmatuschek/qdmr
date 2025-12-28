#include "decodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>

#include "logger.hh"
#include "config.hh"
#include "radioinfo.hh"
#include "dummyfilereader.hh"
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
#include "gd73_codeplug.hh"
#include "gd73_filereader.hh"
#include "opengd77_codeplug.hh"
#include "openuv380_codeplug.hh"
#include "openrtx_codeplug.hh"
#include "anytone_filereader.hh"
#include "d868uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv2_codeplug.hh"
#include "d578uv_codeplug.hh"
#include "dmr6x2uv_codeplug.hh"
#include "dmr6x2uv2_codeplug.hh"
#include "dr1801uv_codeplug.hh"
#include "dr1801uv_filereader.hh"
#include "dm32uv_codeplug.hh"


template <class Cpl, class Rdr>
bool decode(Config &config, const QString &filename, QCommandLineParser &parser, const ErrorStack &err=ErrorStack()) {
  Cpl codeplug;
  if (parser.isSet("manufacturer")) {
    if (! Rdr::read(filename, &codeplug, err)) {
      errMsg(err) << "Cannot decode manufacturer codeplug file '" << filename << "'.";
      return false;
    }
  } else if (! codeplug.read(filename, err)) {
    errMsg(err) << "Cannot decode binary codeplug file '" << filename << "'.";
    return false;
  }
  if (! codeplug.decode(&config, err)) {
    errMsg(err) << "Cannot decode binary codeplug file '" << filename << "'.";
    return false;
  }
  if (! codeplug.postprocess(&config, err)) {
    logError() << "Cannot post-process binary codeplug file '" << filename << "'.";
    return false;
  }

  return true;
}


int
decodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);
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

  switch (radio) {
  case RadioInfo::MD390:
    if (! decode<MD390Codeplug, MD390FileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::UV390:
    if (! decode<UV390Codeplug, UV390FileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::MD2017:
    if (! decode<MD2017Codeplug, MD2017FileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::DM1701:
    if (! decode<DM1701Codeplug, DM1701FileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::RD5R:
    if (! decode<RD5RCodeplug, RD5RFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::GD73:
    if (! decode<GD73Codeplug, GD73FileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::GD77:
    if (! decode<GD77Codeplug, GD77FileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::OpenGD77:
    if (! decode<OpenGD77Codeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::OpenUV380:
    if (! decode<OpenUV380Codeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::OpenRTX:
    if (! decode<OpenRTXCodeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::D868UVE:
    if (! decode<D868UVCodeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::D878UV:
    if (! decode<D878UVCodeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::D878UVII:
    if (! decode<D878UV2Codeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::D578UV:
    if (! decode<D578UVCodeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::DMR6X2UV:
    if (! decode<DMR6X2UVCodeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::DMR6X2UV2:
    if (! decode<DMR6X2UV2Codeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::DR1801UV:
    if (! decode<DR1801UVCodeplug, DR1801UVFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  case RadioInfo::DM32UV:
    if (! decode<DM32UVCodeplug, DummyFileReader>(config, filename, parser, err)) {
      logError() << "Cannot decode codeplug '" << filename << "':\n" << err.format();
      return -1;
    } break;
  default:
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

