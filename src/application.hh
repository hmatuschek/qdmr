#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include "config.hh"
#include "repeaterdatabase.hh"


class QMainWindow;
class Radio;


class Application : public QApplication
{
  Q_OBJECT

public:
  Application(int &argc, char *argv[]);
  virtual ~Application();

  QMainWindow *mainWindow();
  const RepeaterDatabase &repeater() const;


public slots:
  void newCodeplug();
  void codeplugWizzard();
  void loadCodeplug();
  void saveCodeplug();
  void quitApplication();

  void detectRadio();
  bool verifyCodeplug(Radio *radio=nullptr);

  void downloadCodeplug();
  void uploadCodeplug();

  void showAbout();
  void showHelp();

private slots:
  QMainWindow *createMainWindow();

  void onCodeplugDownloadError(Radio *radio);
  void onCodeplugDownloaded(Radio *radio, Config *config);

  void onCodeplugUploadError(Radio *radio);
  void onCodeplugUploaded(Radio *radio);

  void onConfigModifed();
  void onDMRIDChanged();
  void onNameChanged();
  void onIntroLine1Changed();
  void onIntroLine2Changed();

  void onAddContact();
  void onRemContact();
  void onEditContact(const QModelIndex &idx);
  void onContactUp();
  void onContactDown();

  void onAddRxGroup();
  void onRemRxGroup();
  void onRxGroupUp();
  void onRxGroupDown();
  void onEditRxGroup(const QModelIndex &index);

  void onAddAnalogChannel();
  void onAddDigitalChannel();
  void onRemChannel();
  void onChannelUp();
  void onChannelDown();
  void onEditChannel(const QModelIndex &index);

  void onAddZone();
  void onRemZone();
  void onZoneUp();
  void onZoneDown();
  void onEditZone(const QModelIndex &index);

  void onAddScanList();
  void onRemScanList();
  void onScanListUp();
  void onScanListDown();
  void onEditScanList(const QModelIndex &index);

protected:
  Config *_config;
  QMainWindow *_mainWindow;
  RepeaterDatabase *_repeater;
};

#endif // APPLICATION_HH
