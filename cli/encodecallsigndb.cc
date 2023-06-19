#include "encodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>

#include "logger.hh"
#include "config.hh"
#include "radioinfo.hh"
#include "dm1701_callsigndb.hh"
#include "uv390_callsigndb.hh"
#include "md2017_callsigndb.hh"
#include "opengd77_callsigndb.hh"
#include "gd77_callsigndb.hh"
#include "d868uv_callsigndb.hh"
#include "d878uv2_callsigndb.hh"
#include "crc32.hh"


int encodeCallsignDB(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  UserDatabase userdb;
  if (parser.isSet("database")) {
    if (! userdb.load(parser.value("database"))) {
      logError() << "Cannot load user-db from '" << parser.value("database") << "'.";
      return -1;
    }
  } else if (0 == userdb.count()) {
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
  ErrorStack err;

  if (RadioInfo::UV390 == radio) {
    UV390CallsignDB db;
    if (! db.encode(&userdb, selection, err)) {
      logError() << "Cannot encode call-sign DB: " << err.format();
      return -1;
    }
    if (! db.write(parser.positionalArguments().at(1), err)) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << err.format();
      return -1;
    }
  } else if (RadioInfo::MD2017 == radio) {
    MD2017CallsignDB db;
    if (! db.encode(&userdb, selection, err)) {
      logError() << "Cannot encode call-sign DB: " << err.format();
      return -1;
    }
    if (! db.write(parser.positionalArguments().at(1), err)) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << err.format();
      return -1;
    }
  } else if (RadioInfo::DM1701 == radio) {
    DM1701CallsignDB db;
    if (! db.encode(&userdb, selection, err)) {
      logError() << "Cannot encode call-sign DB: " << err.format();
      return -1;
    }
    if (! db.write(parser.positionalArguments().at(1), err)) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << err.format();
      return -1;
    }
  } else if (RadioInfo::OpenGD77 == radio) {
    OpenGD77CallsignDB db;
    if (! db.encode(&userdb, selection, err)) {
      logError() << "Cannot encode call-sign DB: " << err.format();
      return -1;
    }
    if (! db.write(parser.positionalArguments().at(1), err)) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << err.format();
      return -1;
    }
  } else if (RadioInfo::GD77 == radio) {
    GD77CallsignDB db;
    if (! db.encode(&userdb, selection, err)) {
      logError() << "Cannot encode call-sign DB: " << err.format();
      return -1;
    }
    if (! db.write(parser.positionalArguments().at(1), err)) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << err.format();
      return -1;
    }
  } else if ((RadioInfo::D868UVE == radio) || (RadioInfo::D878UV == radio)){
    D868UVCallsignDB db;
    if (! db.encode(&userdb, selection, err)) {
      logError() << "Cannot encode call-sign DB: " << err.format();
      return -1;
    }
    if (! db.write(parser.positionalArguments().at(1), err)) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << err.format();
      return -1;
    }
  } else if ((RadioInfo::D878UVII == radio) || (RadioInfo::D578UV == radio)){
    D878UV2CallsignDB db;
    if (! db.encode(&userdb, selection, err)) {
      logError() << "Cannot encode call-sign DB: " << err.format();
      return -1;
    }
    if (! db.write(parser.positionalArguments().at(1), err)) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << err.format();
      return -1;
    }
  } else {
    logError() << "Cannot encode calls-sign DB: Not implemented for '" << parser.value("radio") << "'.";
    return -1;
  }

  return 0;
}
