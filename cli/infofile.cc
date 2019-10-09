#include "infofile.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QDebug>

#include "dfufile.hh"


int infoFile(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app)

  if (1 != parser.positionalArguments().size())
    parser.showHelp(-1);

  QString filename = parser.positionalArguments().at(0);
  DFUFile file;
  if (!file.read(filename)) {
    qDebug() << file.errorMessage();
    return -1;
  }

  QTextStream out(stdout);
  file.dump(out);
  return 0;
}
