#include "verify.hh"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "config.hh"
#include "csvreader.hh"


int verify(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (0 == parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments()[0];
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    qDebug() << "ERROR: Cannot open file" << filename;
    return -1;
  }

  // Determin type by ending or flag
  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    QTextStream stream(&file);
    Config config;
    if (! CSVReader::read(&config, stream))
      return -1;
    qDebug() << "Verify" << filename << ": No issues found.";
    return 0;
  }

  qDebug() << "Cannot determine filetype from filename" << filename <<": Consider using --csv or --bin.";
  return -1;
}
