#include "infofile.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>

#include "logger.hh"
#include "dfufile.hh"


int infoFile(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app)

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(1);
  DFUFile file;
  if (! file.read(filename)) {
    logError() << "Cannot read codeplug file '" << filename
               << "': " << file.formatErrorMessages();
    return -1;
  }

  QTextStream out(stdout);
  file.dump(out);
  return 0;
}
