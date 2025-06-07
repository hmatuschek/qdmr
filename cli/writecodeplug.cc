#include "writecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include "logger.hh"
#include "radio.hh"
#include "config.hh"
#include "progressbar.hh"
#include "autodetect.hh"
#include "radiolimits.hh"


int writeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);
  QFileInfo fileinfo(filename);

  QString errorMessage;
  Config config;
  if (parser.isSet("csv") || ("csv" == fileinfo.suffix()) || ("conf"==fileinfo.suffix())) {
    if (! config.readCSV(filename, errorMessage)) {
      logError() << "Cannot read CSV file '" << filename << "': " << errorMessage;
      return -1;
    }
  } else if (parser.isSet("yaml") || ("yaml" == fileinfo.suffix())) {
    ErrorStack err;
    if (! config.readYAML(fileinfo.canonicalFilePath(), err)) {
      logError() << "Cannot parse YAML codeplug '" << fileinfo.fileName() << "': " << err.format();
      return -1;
    }
  }
  logDebug() << "Read codeplug from '" << filename << "'.";

  ErrorStack err;
  Radio *radio = autoDetect(parser, app, err);
  if (nullptr == radio) {
    logError() << "Cannot detect radio:" << err.format();
    return -1;
  }

  RadioLimitContext ctx(parser.isSet("ignore-limits"));

  Config *intermediate = radio->codeplug().preprocess(&config, err);
  if (nullptr == intermediate) {
    logError() << "Cannot pre-process codeplug: " << err.format();
    return -1;
  }

  bool verified = true;
  radio->limits().verifyConfig(intermediate, ctx);

  // Only print warnings
  for (int i=0; i<ctx.count(); i++) {
    switch (ctx.message(i).severity()) {
    case RadioLimitIssue::Warning:
      logWarn() << "Verification Issue: " << ctx.message(i).format();
      break;
    case RadioLimitIssue::Critical:
      logError() << "Verification Issue: " << ctx.message(i).format();
      break;
    default:
      break;
    }
  }

  if (! verified) {
    logError() << "Cannot upload codeplug to device: Codeplug cannot be verified with radio.";
    delete intermediate;
    return -1;
  }

  showProgress();
  QObject::connect(radio, &Radio::uploadProgress, updateProgress);

  Codeplug::Flags flags;
  flags.setBlocking(true);
  flags.setUpdateDeviceClock(parser.isSet("update-device-clock"));
  flags.setUpdateCodeplug(! parser.isSet("init-codeplug"));
  flags.setAutoEnableGPS(parser.isSet("auto-enable-gps"));
  flags.setAutoEnableRoaming(parser.isSet("auto-enable-roaming"));

  logDebug() << "Start upload to " << radio->name() << ".";
  if (! radio->startUpload(intermediate, flags, err)) {
    logError() << "Codeplug upload error: " << err.format();
    return -1;
  }

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug upload error: " << err.format();
    return -1;
  }

  logDebug() << "Upload completed.";
  return 0;
}
