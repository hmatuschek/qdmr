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
class RadioIDListView;
class GeneralSettingsView;
class ContactListView;
class GroupListsView;
class ChannelListView;
class ZoneListView;
class ScanListsView;
class PositioningSystemListView;
class RoamingZoneListView;


class Application : public QApplication
{
  Q_OBJECT

public:
  Application(int &argc, char *argv[]);
  virtual ~Application();

  QMainWindow *mainWindow();

  UserDatabase *user() const;
  RepeaterDatabase *repeater() const;
  TalkGroupDatabase *talkgroup() const;

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
  void onCodeplugDownloaded(Radio *radio, Codeplug *codeplug);

  void onCodeplugUploadError(Radio *radio);
  void onCodeplugUploaded(Radio *radio);

  void onConfigModifed();

  void positionUpdated(const QGeoPositionInfo &info);

protected:
  Config *_config;
  QMainWindow *_mainWindow;

  GeneralSettingsView *_generalSettings;
  RadioIDListView *_radioIdTab;
  ContactListView *_contactList;
  GroupListsView *_groupLists;
  ChannelListView *_channelList;
  ZoneListView *_zoneList;
  ScanListsView *_scanLists;
  PositioningSystemListView *_posSysList;
  RoamingZoneListView *_roamingZoneList;

  RepeaterDatabase *_repeater;
  UserDatabase *_users;
  TalkGroupDatabase *_talkgroups;

  QGeoPositionInfoSource *_source;
  QGeoCoordinate _currentPosition;

  ReleaseNotes _releaseNotes;
};

#endif // APPLICATION_HH
