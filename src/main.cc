#include "application.hh"
#include <QFile>
#include <QTextStream>
#include "mainwindow.hh"
#include <QMessageBox>
#include "logger.hh"
#include "settings.hh"
#include <QSplashScreen>

#include <QCommandLineParser>

int main(int argc, char *argv[])
{
  QTextStream out(stderr);
  auto handler = new StreamLogHandler(out, LogMessage::Level::INFO);
  Logger::get().addHandler(handler);

  Application app(argc, argv);


  // Parse options
  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("codeplug", QCoreApplication::translate("main", "Codeplug file to load."));
  parser.addOption({"loglevel", QCoreApplication::translate("main", "Specifies applications log-level to stdout. Must be one of `debug`, `info`, `warning`, `error` or `fatal`."), "loglevel", "info"});
  parser.parse(app.arguments());

  // Handle args (if there are some)
  if (parser.positionalArguments().count()) {
    ErrorStack err;
    if (! app.loadCodeplug(parser.positionalArguments().at(0), err))
      logError() << "Cannot open codeplug file '" << argv[1] << "': " << err.format();
  }

  if (parser.isSet("loglevel")) {
    if ("debug" == parser.value("loglevel")) {
      handler->setMinLevel(LogMessage::Level::DEBUG);
    } else if ("info" == parser.value("loglevel")) {
      handler->setMinLevel(LogMessage::Level::INFO);
    } else if ("warning" == parser.value("loglevel")) {
      handler->setMinLevel(LogMessage::Level::WARNING);
    } else if ("error" == parser.value("loglevel")) {
      handler->setMinLevel(LogMessage::Level::ERROR);
    } else if ("fatal" == parser.value("loglevel")) {
      handler->setMinLevel(LogMessage::Level::FATAL);
    }
  }

  auto mainWindow = app.mainWindow();
  mainWindow->show();

  Settings settings;
  if (settings.showDisclaimer()) {
    app.showAbout();
    settings.setShowDisclaimer(false);
  }

  app.exec();

  return 0;
}
