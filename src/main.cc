#include "application.hh"
#include <QFile>
#include <QTextStream>
#include <QMainWindow>
#include "logger.hh"
#include <stdio.h>

int main(int argc, char *argv[])
{
  QTextStream out(stderr);
  Logger::get().addHandler(new StreamLogHandler(out));

  Application app(argc, argv);

  QMainWindow *mainWindow = app.mainWindow();

  mainWindow->show();

  app.exec();

  return 0;
}
