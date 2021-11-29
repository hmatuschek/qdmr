#include "writecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include "logger.hh"
#include "radio.hh"
#include "config.hh"
#include "progressbar.hh"


int writeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

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

  RadioInfo forceRadio;
  if (parser.isSet("radio")) {
    logWarn() << "You force the radio type to be '" << parser.value("radio").toUpper()
              << "' this is generally a very bad idea! You have been warned.";
    forceRadio = RadioInfo::byKey(parser.value("radio").toLower());
    if (! forceRadio.isValid()) {
      QStringList radios;
      foreach (RadioInfo info, RadioInfo::allRadios())
        radios.append(info.key());
      logError() << "Known radio key '" << parser.value("radio").toLower() << "'.";
      logError() << "Known radios " << radios.join(", ") << ".";
      return -1;
    }
  }

  ErrorStack err;
  Radio *radio = Radio::detect(forceRadio, err);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << err.format();
    return -1;
  }

  VerifyFlags verify_flags;
  if (parser.isSet("ignore-limits"))
    verify_flags.ignoreFrequencyLimits = true;

  bool verified = true;
  QList<VerifyIssue> issues;
  if (VerifyIssue::WARNING <= radio->verifyConfig(&config, issues, verify_flags)) {
    foreach(const VerifyIssue &issue, issues) {
      if (VerifyIssue::WARNING == issue.type()) {
        logWarn() << "Verification Issue: " << issue.message();
      } else if (VerifyIssue::ERROR == issue.type()) {
        logError() << "Verification Issue: " << issue.message();
        verified = false;
      }
    }
  }
  if (! verified) {
    logError() << "Cannot upload codeplug to device: Codeplug cannot be verified with radio.";
    return -1;
  }

  showProgress();
  QObject::connect(radio, &Radio::uploadProgress, updateProgress);

  Codeplug::Flags flags;
  if (parser.isSet("init-codeplug"))
    flags.updateCodePlug = false;
  if (parser.isSet("auto-enable-gps"))
    flags.autoEnableGPS = true;
  if (parser.isSet("auto-enable-roaming"))
    flags.autoEnableRoaming = true;

  logDebug() << "Start upload to " << radio->name() << ".";
  if (! radio->startUpload(&config, true, flags, err)) {
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
