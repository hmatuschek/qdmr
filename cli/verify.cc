#include "verify.hh"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "logger.hh"
#include "config.hh"
#include "csvreader.hh"
#include "dfufile.hh"
#include "rd5r.hh"
#include "uv390.hh"
#include "gd77.hh"
#include "opengd77.hh"


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

  // Determin type by ending or flag
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
  } else {
    logError() << "Cannot determine filetype from filename '" << filename
               << "': Consider using --csv.";
    return -1;
  }

  if (! parser.isSet("radio")) {
    logInfo() << "To verify the codeplug against a specific radio, conser using the --radio=RADIO option.";
    return 0;
  }

  QList<VerifyIssue> issues;
  QString radio = parser.value("radio").toLower();
  if ("rd5r" == radio) {
    RD5R radio; radio.verifyConfig(&config, issues);
  } else if (("uv390" == radio) && ("rt3s" == radio)) {
    UV390 radio; radio.verifyConfig(&config, issues);
  } else if ("gd77" == radio) {
    GD77 radio; radio.verifyConfig(&config, issues);
  } else if ("opengd77" == radio) {
    OpenGD77 radio; radio.verifyConfig(&config, issues);
  } else {
    logError() << "Cannot verify code-plug against unknown radio '" << radio << "'.";
    return -1;
  }

  bool valid = true;
  if (0 == issues.size()) {
    logInfo() << "No issues found for code-plug '" << filename
              << "' with radio '" << radio << "'.";
  } else {
    logInfo() << "Some issues found for code-plug '" << filename
              << "' with radio '" << radio << "'.";
    foreach (const VerifyIssue &issue, issues) {
      if (VerifyIssue::WARNING == issue.type()) {
        logWarn() << issue.message();
      } else if (VerifyIssue::ERROR == issue.type()) {
        logError() << issue.message();
        valid = false;
      }
    }
  }

  return (valid ? 0 : -1);
}
