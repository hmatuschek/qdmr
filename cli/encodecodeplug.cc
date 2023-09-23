#include "encodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>

#include "logger.hh"
#include "config.hh"
#include "radioinfo.hh"
#include "rd5r_codeplug.hh"
#include "gd77_codeplug.hh"
#include "opengd77_codeplug.hh"
#include "openrtx_codeplug.hh"
#include "md390_codeplug.hh"
#include "uv390_codeplug.hh"
#include "md2017_codeplug.hh"
#include "d868uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv2_codeplug.hh"
#include "d578uv_codeplug.hh"
#include "dmr6x2uv_codeplug.hh"
#include "crc32.hh"


template <class T>
bool encode(Config &config, Codeplug::Flags flags, QCommandLineParser &parser) {
  ErrorStack err;
  T codeplug;

  Config *intermediate = codeplug.preprocess(&config, err);
  if (nullptr == intermediate) {
    logError() << "Cannot pre-process codeplug: " << err.format();
    return false;
  }

  if (! codeplug.encode(intermediate, flags, err)) {
    logError() << "Cannot encode codeplug: " << err.format();
    delete intermediate;
    return false;
  }
  delete intermediate;

  codeplug.image(0).sort();
  if (! codeplug.write(parser.positionalArguments().at(2), err)) {
    logError() << "Cannot write output codeplug file '" << parser.positionalArguments().at(1)
               << "': " << err.format();
    return false;
  }

  return true;
}


int encodeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (3 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QFileInfo fileinfo(parser.positionalArguments().at(1));

  if (! parser.isSet("radio")) {
    logError() << "You have to specify the radio using the --radio option.";
    parser.showHelp(-1);
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

  Codeplug::Flags flags;
  flags.updateCodePlug = false;
  if (parser.isSet("auto-enable-gps"))
    flags.autoEnableGPS = true;
  if (parser.isSet("auto-enable-roaming"))
    flags.autoEnableRoaming = true;

  Config config;
  ErrorStack err;
  if (parser.isSet("csv") || ("conf" == fileinfo.suffix()) || ("csv" == fileinfo.suffix())) {
    QString errorMessage;
    QFile infile(fileinfo.canonicalFilePath());
    if (! infile.open(QIODevice::ReadOnly)) {
      logError() << "Cannot encode CSV codeplug file '" << fileinfo.fileName() << "': " << infile.errorString();
      return -1;
    }
    QTextStream stream(&infile);
    if (! config.readCSV(stream, errorMessage)) {
      logError() << "Cannot parse CSV codeplug '" << infile.fileName() << "': " << errorMessage;
      return -1;
    }
  } else if (parser.isSet("yaml") || ("yaml" == fileinfo.suffix())) {
    if (! config.readYAML(fileinfo.canonicalFilePath(), err)) {
      logError() << "Cannot parse YAML codeplug '" << fileinfo.fileName()
                 << "':\n" << err.format(" ");
      return -1;
    }
  } else {
    logError() << "Cannot determine input file type, consider using --csv or --yaml.";
    return -1;
  }

  if ((RadioInfo::MD390 == radio) && (! encode<MD390Codeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::UV390 == radio) && (! encode<UV390Codeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::MD2017 == radio) && (! encode<MD2017Codeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::RD5R == radio) && (! encode<RD5RCodeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::GD77 == radio) && (! encode<GD77Codeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::OpenGD77 == radio) && (! encode<OpenGD77Codeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::OpenRTX == radio) && (! encode<OpenRTXCodeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::D868UVE == radio) && (! encode<D868UVCodeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::D878UV == radio) && (! encode<D878UVCodeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::D878UVII == radio) && (! encode<D878UV2Codeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::D578UV == radio) && (! encode<D578UVCodeplug>(config, flags, parser)))
    return -1;
  else if ((RadioInfo::DMR6X2UV == radio) &&  (! encode<DMR6X2UVCodeplug>(config, flags, parser)))
    return -1;
  else {
    logError() << "Cannot encode codeplug: Unknown radio '" << parser.value("radio") << "'.";
    return -1;
  }

  return 0;
}
