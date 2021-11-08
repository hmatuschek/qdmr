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


int readCodeplug(QCommandLineParser &parser, QCoreApplication &app)
{
  Q_UNUSED(app);

  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);

  RadioInfo forceRadio;
  if (parser.isSet("radio")) {
    logWarn() << "You force the radio type to be '" << parser.value("radio").toUpper()
              << "' this is generally a very bad idea! You have been warned.";
    forceRadio = RadioInfo::byKey(parser.value("radio").toLower());
    if (! forceRadio.isValid()) {
      QStringList radios;
      foreach (RadioInfo info, RadioInfo::allRadios())
        radios.append(info.key());
      logError() << "Known radio key '" << parser.value("radio").toLower() << "'.";
      logError() << "Known radios " << radios.join(", ") << ".";
      return -1;
    }
  }

  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage, forceRadio);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << errorMessage;
    return -1;
  }

  QString filename = parser.positionalArguments().at(1);

  showProgress();
  QObject::connect(radio, &Radio::downloadProgress, updateProgress);

  Config config;
  if (! radio->startDownload(true)) {
    logError() << "Codeplug download error: " << radio->formatErrorMessages();
    return -1;
  }

  if (Radio::StatusError == radio->status()) {
    logError() << "Codeplug download error: " << radio->formatErrorMessages();
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
    if (! radio->codeplug().decode(&config)) {
      logError() << "Cannot decode codeplug: " << radio->formatErrorMessages();
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
    if (! radio->codeplug().write(filename)) {
      logError() << "Cannot dump codplug into file '" << filename << "': "
                 << radio->formatErrorMessages();
      return -1;
    }
  } else {
    logError() << "Cannot determine file output type from '" << filename << "'. "
               << "Consider using --csv, --yaml or --bin.";
    return -1;
  }

  return 0;
}
