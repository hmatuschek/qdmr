#include "readcodeplug.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>

#include <QApplication>

#include "logger.hh"
#include "radio.hh"
#include "printprogress.hh"
#include "config.hh"
#include "codeplug.hh"


int readCodeplug(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << errorMessage;
    return -1;
  }

  QString filename = parser.positionalArguments().at(1);

  if (!parser.isSet("csv") && !filename.endsWith(".conf") && !filename.endsWith(".csv") &&
      !parser.isSet("cpl") && !filename.endsWith(".bin") && !filename.endsWith(".dfu")) {
    logError() << "Cannot determine output filetype, consider using --csv or --bin options.";
    return -1;
  }

  Config config;
  if (! radio->startDownload(&config, true)) {
    logError() << "Codeplug download error: " << radio->errorMessage();
    return -1;
  }

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug download error: " << radio->errorMessage();
    return -1;
  }

  logDebug() << "Save codeplug at '" << filename << "'.";
  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    if (! config.writeCSV(filename, errorMessage)) {
      logError() << "Cannot write CSV file '" << filename << "': " << errorMessage;
      return -1;
    }
    return 0;
  }

  radio->codeplug().write(filename);
  return 0;
}
