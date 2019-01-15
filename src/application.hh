#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include "config.hh"


class QMainWindow;
class Radio;


class Application : public QApplication
{
  Q_OBJECT

public:
  Application(int &argc, char *argv[]);
  virtual ~Application();

  QMainWindow *mainWindow();

protected slots:
  void newCodeplug();
  void codeplugWizzard();
  void loadCodeplug();
  void saveCodeplug();

  void detectRadio();
  bool verifyCodeplug(Radio *radio=nullptr);

  void downloadCodeplug();
  void onCodeplugDownloadError(Radio *radio);
  void onCodeplugDownloaded(Radio *radio, Config *config);

  void uploadCodeplug();
  void onCodeplugUploadError(Radio *radio);
  void onCodeplugUploaded(Radio *radio);

  void showAbout();
  void showHelp();

  QMainWindow *createMainWindow();

  void onConfigModifed();
  void onDMRIDChanged();
  void onIntroLine1Changed();
  void onIntroLine2Changed();

  void onAddContact();
  void onRemContact();
  void onEditContact(const QModelIndex &idx);

  void onAddRxGroup();
  void onRemRxGroup();
  void onEditRxGroup(const QModelIndex &index);

protected:
  Config *_config;
  QMainWindow *_mainWindow;
};

#endif // APPLICATION_HH
