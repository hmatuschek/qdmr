#include "writecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>

#include "logger.hh"
#include "radio.hh"
#include "config.hh"
#include "progressbar.hh"


int writeCodeplug(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);

  QString errorMessage;
  Config config;
  if (! config.readCSV(filename, errorMessage)) {
    logError() << "Cannot read CSV file '" << filename << "': " << errorMessage;
    return -1;
  }
  logDebug() << "Read codeplug from '" << filename << "'.";

  QString forceRadio="";
  if (parser.isSet("radio")) {
    logWarn() << "You force the radio type to be '" << parser.value("radio").toUpper()
              << "' this is generally a very bad idea! You have been warned.";
    forceRadio = parser.value("radio");
  }

  Radio *radio = Radio::detect(errorMessage, forceRadio);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << errorMessage;
    return -1;
  }

  bool verified = true;
  QList<VerifyIssue> issues;
  if (radio->verifyConfig(&config, issues)) {
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

  logDebug() << "Start upload to " << radio->name() << ".";
  if (! radio->startUpload(&config, true)) {
    logError() << "Codeplug upload error: " << radio->errorMessage();
    return -1;
  }

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug upload error: " << radio->errorMessage();
    return -1;
  }

  logDebug() << "Upload completed.";
  return 0;
}
