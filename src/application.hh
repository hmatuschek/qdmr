#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include <QGroupBox>
#include "config.hh"
#include <QGeoPositionInfoSource>
#include "releasenotes.hh"
#include "radio.hh"

class QMainWindow;
class RepeaterDatabase;
class UserDatabase;
class TalkGroupDatabase;


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
  void loadCodeplug();
  void saveCodeplug();
  void quitApplication();

  void detectRadio();
  bool verifyCodeplug(Radio *radio=nullptr, bool showSuccess=true,
                      const VerifyFlags &flags=VerifyFlags());

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
  void onDMRIDSelected(int idx);
  void onAddDMRID();
  void onRemDMRID();

  void onNameChanged();
  void onIntroLine1Changed();
  void onIntroLine2Changed();
  void onMicLevelChanged();
  void onSpeechChanged();

  void onAddRadioId();
  void onRemRadioId();
  void onEditRadioId(const QModelIndex &idx);
  void onRadioIdUp();
  void onRadioIdDown();
  void loadRadioIdListSectionState();
  void storeRadioIdListSectionState();

  void onAddContact();
  void onRemContact();
  void onEditContact(const QModelIndex &idx);
  void onContactUp();
  void onContactDown();
  void loadContactListSectionState();
  void storeContactListSectionState();

  void onAddRxGroup();
  void onRemRxGroup();
  void onRxGroupUp();
  void onRxGroupDown();
  void onEditRxGroup(const QModelIndex &index);

  void onAddAnalogChannel();
  void onAddDigitalChannel();
  void onCloneChannel();
  void onRemChannel();
  void onChannelUp();
  void onChannelDown();
  void onEditChannel(const QModelIndex &index);
  void loadChannelListSectionState();
  void storeChannelListSectionState();

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
  void onAddAPRS();
  void onRemGPS();
  void onGPSUp();
  void onGPSDown();
  void onEditGPS(const QModelIndex &index);
  void onHideGPSNote();
  void loadPositioningSectionState();
  void storePositioningSectionState();

  void onAddRoamingZone();
  void onGenRoamingZone();
  void onRemRoamingZone();
  void onRoamingZoneUp();
  void onRoamingZoneDown();
  void onEditRoamingZone(const QModelIndex &index);
  void onHideRoamingNote();

  void positionUpdated(const QGeoPositionInfo &info);

protected:
  Config *_config;
  QMainWindow *_mainWindow;
  QGroupBox *_dmrIDBox;
  QGroupBox *_defaultIDBox;
  QWidget *_radioIdTab;
  RepeaterDatabase *_repeater;
  UserDatabase *_users;
  TalkGroupDatabase *_talkgroups;
  QGeoPositionInfoSource *_source;
  QGeoCoordinate _currentPosition;
  ReleaseNotes _releaseNotes;
};

#endif // APPLICATION_HH
