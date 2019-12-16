#include "verify.hh"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "logger.hh"
#include "config.hh"
#include "csvreader.hh"


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

  // Determin type by ending or flag
  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    QTextStream stream(&file);
    QString errorMessage;
    Config config;
    if (! CSVReader::read(&config, stream, errorMessage)) {
      logError() << "Cannot read config file '" << filename << "': " << errorMessage;
      return -1;
    }
    logDebug() << "Verify '" << filename << "': No issues found.";
    return 0;
  }

  logError() << "Cannot determine filetype from filename '" << filename
             << "': Consider using --csv or --bin.";
  return -1;
}
