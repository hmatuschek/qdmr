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
#include "progressbar.hh"
#include "autodetect.hh"


int readCodeplug(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  ErrorStack err;
  Radio *radio = autoDetect(parser, app, err);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << err.format();
    return -1;
  }

  QString filename = parser.positionalArguments().at(1);

  showProgress();
  QObject::connect(radio, &Radio::downloadProgress, updateProgress);

  Config config;
  if (! radio->startDownload(true, err)) {
    logError() << "Codeplug download error: " << err.format();
    return -1;
  }

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug download error: " << err.format();
    return -1;
  }

  logDebug() << "Save codeplug at '" << filename << "'.";
  // If output is CSV -> decode code-plug
  if (parser.isSet("csv") || (filename.endsWith(".conf") || filename.endsWith(".csv"))) {
    logError() << "Export of the old table based format was disabled with 0.9.0. "
                  "Import still works.";
    return -1;
  } else if (parser.isSet("yaml") || filename.endsWith(".yaml")) {
    // decode codeplug
    if (! radio->codeplug().decode(&config, err)) {
      logError() << "Cannot decode codeplug: " << err.format();
      return -1;
    }

    // try to write YAML file
    QFile file(filename);
    if (! file.open(QIODevice::WriteOnly)) {
      logError() << "Cannot write YAML file '" << filename << "': " << file.errorString();
      return -1;
    }

    QTextStream stream(&file);
    if (! config.toYAML(stream)) {
      logError() << "Cannot serialize config to YAML file '" << filename << "'.";
      return -1;
    }
    stream.flush();
    file.close();
  } else if (parser.isSet("bin") || filename.endsWith(".bin") || filename.endsWith(".dfu")) {
    // otherwise write binary code-plug
    if (! radio->codeplug().write(filename, err)) {
      logError() << "Cannot dump codplug into file '" << filename << "': " << err.format();
      return -1;
    }
  } else {
    logError() << "Cannot determine file output type from '" << filename << "'. "
               << "Consider using --csv, --yaml or --bin.";
    return -1;
  }

  return 0;
}
