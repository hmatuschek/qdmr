#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include "config.hh"
#include <QGeoPositionInfoSource>
#include "releasenotes.hh"

class QMainWindow;
class Radio;
class RepeaterDatabase;
class UserDatabase;
class CodePlug;


class Application : public QApplication
{
  Q_OBJECT

public:
  Application(int &argc, char *argv[]);
  virtual ~Application();

  QMainWindow *mainWindow();
  RepeaterDatabase *repeater() const;
  bool hasPosition() const;
  QGeoCoordinate position() const;

public slots:
  void newCodeplug();
  void codeplugWizzard();
  void loadCodeplug();
  void saveCodeplug();
  void quitApplication();

  void detectRadio();
  bool verifyCodeplug(Radio *radio=nullptr, bool showSuccess=true);

  void downloadCodeplug();
  void uploadCodeplug();
  void uploadCallsignDB();

  void showSettings();
  void showAbout();
  void showHelp();

private slots:
  QMainWindow *createMainWindow();

  void onCodeplugDownloadError(Radio *radio);
  void onCodeplugDownloaded(Radio *radio, CodePlug *codeplug);

  void onCodeplugUploadError(Radio *radio);
  void onCodeplugUploaded(Radio *radio);

  void onConfigModifed();
  void onDMRIDChanged();
  void onNameChanged();
  void onIntroLine1Changed();
  void onIntroLine2Changed();
  void onMicLevelChanged();
  void onSpeechChanged();

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

  void onAddGPS();
  void onRemGPS();
  void onGPSUp();
  void onGPSDown();
  void onEditGPS(const QModelIndex &index);
  void onHideGPSNote();

  void positionUpdated(const QGeoPositionInfo &info);

protected:
  Config *_config;
  QMainWindow *_mainWindow;
  RepeaterDatabase *_repeater;
  UserDatabase *_users;
  QGeoPositionInfoSource *_source;
  QGeoCoordinate _currentPosition;
  ReleaseNotes _releaseNotes;
};

#endif // APPLICATION_HH
