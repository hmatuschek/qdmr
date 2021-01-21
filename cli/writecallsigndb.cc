#include "writecallsigndb.hh"

#include <QCoreApplication>
#include <QCommandLineParser>

#include "logger.hh"
#include "radio.hh"
#include "userdatabase.hh"


int writeCallsignDB(QCommandLineParser &parser, QCoreApplication &app) {
  UserDatabase userdb;
  if (0 == userdb.count()) {
    logInfo() << "Downloading call-sign DB...";
    // Wait for download to finish
    QEventLoop loop;
    QObject::connect(&userdb, SIGNAL(loaded()), &loop, SLOT(quit()));
    QObject::connect(&userdb, SIGNAL(error(QString)), &loop, SLOT(quit()));
    loop.exec();
    // Check if call-sign DB has been loaded
    if (0 == userdb.count()) {
      logError() << "Could not download/load call-sign DB.";
      return -1;
    }
  }

  if (parser.isSet("id")) {
    bool ok=true;
    uint32_t id = parser.value("id").toUInt(&ok);
    if (! ok) {
      logError() << "Please specify a valid DMR ID for --id option.";
      return -1;
    }
    logDebug() << "Sort call-sign DB w.r.t. DMR ID " << id << ".";
    userdb.sortUsers(id);
  } else {
    logWarn() << "No ID is specified, a more or less random set of call-signs will be used "
              << "if the radio cannot hold the entire call-sign DB of " << userdb.count()
              << " entries. Specify your DMR ID with --id=YOUR_DMR_ID. dmrconf will then "
              << "select those entries 'closest' to you. I.e., DMR IDs with the same prefix.";
  }

  QString msg;
  Radio *radio = Radio::detect(msg);
  if (nullptr == radio) {
    logError() << "Could not detect a known radio. Check connection?";
    return -1;
  }

  if (! radio->startUploadCallsignDB(&userdb, true)) {
    logError() << "Could not upload call-sign DB to radio: " << radio->errorMessage();
    return -1;
  }

  return 0;
}
