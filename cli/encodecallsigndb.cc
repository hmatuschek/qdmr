#include "encodecodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>

#include "logger.hh"
#include "config.hh"
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

  if (! parser.isSet("radio")) {
    logError() << "You have to specify the radio using the --radio option.";
    parser.showHelp(-1);
    return -1;
  }

  if (("uv390"==parser.value("radio").toLower()) || ("rt3s"==parser.value("radio").toLower())) {
    UV390CallsignDB db;
    db.encode(&userdb, selection);
    if (! db.write(parser.positionalArguments().at(1))) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << db.errorMessage();
      return -1;
    }
  } else if (("md2017"==parser.value("radio").toLower()) || ("rt82"==parser.value("radio").toLower())) {
    MD2017CallsignDB db;
    db.encode(&userdb, selection);
    if (! db.write(parser.positionalArguments().at(1))) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << db.errorMessage();
      return -1;
    }
  } else if ("opengd77"==parser.value("radio").toLower()) {
    OpenGD77CallsignDB db;
    db.encode(&userdb, selection);
    if (! db.write(parser.positionalArguments().at(1))) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << db.errorMessage();
      return -1;
    }
  } else if ("gd77"==parser.value("radio").toLower()) {
    GD77CallsignDB db;
    db.encode(&userdb, selection);
    if (! db.write(parser.positionalArguments().at(1))) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << db.errorMessage();
      return -1;
    }
  } else if (("d878uv"==parser.value("radio").toLower()) || ("d868uv"==parser.value("radio").toLower()) ){
    D868UVCallsignDB db;
    db.encode(&userdb, selection);
    if (! db.write(parser.positionalArguments().at(1))) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << db.errorMessage();
      return -1;
    }
  } else if (("d878uv2"==parser.value("radio").toLower()) || ("d578uv"==parser.value("radio").toLower()) ){
    D878UV2CallsignDB db;
    db.encode(&userdb, selection);
    if (! db.write(parser.positionalArguments().at(1))) {
      logError() << "Cannot write output call-sign DB file '" << parser.positionalArguments().at(1)
                 << "': " << db.errorMessage();
      return -1;
    }
  } else {
    logError() << "Cannot encode calls-sign DB: Unknown radio '" << parser.value("radio") << "'.";
    return -1;
  }

  return 0;
}
