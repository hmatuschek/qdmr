#include "application.hh"
#include <QFile>
#include <QTextStream>
#include <QMainWindow>
#include <QMessageBox>
#include "logger.hh"
#include "settings.hh"
#include <stdio.h>

int main(int argc, char *argv[])
{
  QTextStream out(stderr);
  Logger::get().addHandler(new StreamLogHandler(out));

  Application app(argc, argv);

  QMainWindow *mainWindow = app.mainWindow();
  mainWindow->show();

  Settings settings;
  if (settings.showDisclaimer()) {
    app.showAbout();
    settings.setShowDisclaimer(false);
  }

  app.exec();

  return 0;
}
