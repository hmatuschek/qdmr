#include "verify.hh"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <iostream>

#include "logger.hh"
#include "config.hh"
#include "csvreader.hh"
#include "dfufile.hh"
#include "radiolimits.hh"
#include "rd5r.hh"
#include "uv390.hh"
#include "md2017.hh"
#include "gd77.hh"
#include "opengd77.hh"
#include "d868uv.hh"
#include "d878uv.hh"
#include "d878uv2.hh"
#include "d578uv.hh"


int verify(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    logError() << "Cannot open file" << filename;
    return -1;
  }

  Config config;

  // Determine type by ending or flag
  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    QTextStream stream(&file);
    QString errorMessage;
    if (! CSVReader::read(&config, stream, errorMessage)) {
      logError() << "Cannot read config file '" << filename << "': " << errorMessage;
      return -1;
    }
    logInfo() << "Verify '" << filename << "': No syntax issues found.";
  } else if (parser.isSet("bin") || (filename.endsWith(".bin") || filename.endsWith(".dfu"))) {
    logError() << "Verification of binary code-plugs makes no sense.";
    return -1;
  } else if (parser.isSet("yaml") || (filename.endsWith(".yaml") || filename.endsWith(".yml"))) {
    ErrorStack err;
    if (! config.readYAML(filename,err)) {
      logError() << "Cannot read codeplug file '" << filename
                 << "': " << err.format();
      return -1;
    }
  } else {
    logError() << "Cannot determine filetype from filename '" << filename
               << "': Consider using --csv.";
    return -1;
  }

  if (! parser.isSet("radio")) {
    logInfo() << "To verify the codeplug against a specific radio, conser using the --radio=RADIO option.";
    return 0;
  }

  RadioLimitContext ctx;
  QString radio = parser.value("radio").toLower();
  if ("rd5r" == radio) {
    RD5R radio; radio.limits().verifyConfig(&config, ctx);
  } else if (("uv390" == radio) || ("rt3s" == radio)) {
    UV390 radio; radio.limits().verifyConfig(&config, ctx);
  } else if (("md2017" == radio) || ("rt82" == radio)) {
    MD2017 radio; radio.limits().verifyConfig(&config, ctx);
  } else if ("gd77" == radio) {
    GD77 radio; radio.limits().verifyConfig(&config, ctx);
  } else if ("opengd77" == radio) {
    OpenGD77 radio; radio.limits().verifyConfig(&config, ctx);
  } else if ("d868uv" == radio) {
    D868UV radio; radio.limits().verifyConfig(&config, ctx);
  } else if ("d878uv" == radio) {
    D878UV radio; radio.limits().verifyConfig(&config, ctx);
  } else if ("d878uv2" == radio) {
    D878UV2 radio; radio.limits().verifyConfig(&config, ctx);
  } else if ("d578uv" == radio) {
    D578UV radio; radio.limits().verifyConfig(&config, ctx);
  } else {
    logError() << "Cannot verify code-plug against unknown radio '" << radio << "'.";
    return -1;
  }

  bool valid = true;
  for (int i=0; i<ctx.count(); i++) {
    switch (ctx.message(i).severity()) {
    case RadioLimitIssue::Silent:
      logDebug() << ctx.message(i).format();
      break;
    case RadioLimitIssue::Hint:
      logInfo() << ctx.message(i).format();
      break;
    case RadioLimitIssue::Warning:
      logWarn() << ctx.message(i).format();
      break;
    case RadioLimitIssue::Critical:
      logError() << ctx.message(i).format();
      valid = false;
      break;
    }
  }

  return (valid ? 0 : -1);
}
