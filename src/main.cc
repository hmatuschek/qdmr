#include "application.hh"
#include <QFile>
#include <QTextStream>
#include <QMainWindow>


int main(int argc, char *argv[])
{
  Application app(argc, argv);

  QMainWindow *mainWindow = app.mainWindow();

  mainWindow->show();

  app.exec();

  return 0;
}
