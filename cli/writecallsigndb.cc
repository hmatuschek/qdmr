#include "writecallsigndb.hh"

#include <QCoreApplication>
#include <QCommandLineParser>

#include "logger.hh"
#include "radio.hh"
#include "userdatabase.hh"
#include "progressbar.hh"
#include "callsigndb.hh"
#include "autodetect.hh"


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
    QStringList prefixes_text = parser.value("id").split(",");
    QSet<unsigned> prefixes;
    foreach (QString prefix_text, prefixes_text) {
      bool ok=true; uint32_t prefix = prefix_text.toUInt(&ok);
      if (ok)
        prefixes.insert(prefix);
    }

    if (prefixes.isEmpty()) {
      logError() << "Please specify a valid DMR ID or a list of DMR prefixes for --id option.";
      return -1;
    }
    prefixes_text.clear();
    foreach (unsigned prefix, prefixes) {
      prefixes_text.append(QString::number(prefix));
    }
    logDebug() << "Sort call-sign DB w.r.t. DMR ID(s) {" << prefixes_text.join(", ") << "}.";
    userdb.sortUsers(prefixes);
  } else {
    logWarn() << "No ID is specified, a more or less random set of call-signs will be used "
              << "if the radio cannot hold the entire call-sign DB of " << userdb.count()
              << " entries. Specify your DMR ID with --id=YOUR_DMR_ID. dmrconf will then "
              << "select those entries 'closest' to you. I.e., DMR IDs with the same prefix.";
  }

  CallsignDB::Selection selection;
  if (parser.isSet("limit")) {
    bool ok=true;
    selection.setCountLimit(parser.value("limit").toUInt(&ok));
    if (! ok) {
      logError() << "Please specify a valid limit for the number of callsign db entries using the -n/--limit option.";
      return -1;
    }
  }

  ErrorStack err;
  Radio *radio = autoDetect(parser, app, err);
  if (nullptr == radio) {
    logError() << "Could not detect radio: " << err.format();
    return -1;
  }

  showProgress();
  QObject::connect(radio, &Radio::uploadProgress, updateProgress);

  if (! radio->startUploadCallsignDB(&userdb, true, selection, err)) {
    logError() << "Could not upload call-sign DB to radio: " << err.format();
    return -1;
  }

  return 0;
}
