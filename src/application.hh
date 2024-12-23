#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include <QGroupBox>
#include <QIcon>
#include "config.hh"
#include <QGeoPositionInfoSource>
#include "releasenotes.hh"
#include "radio.hh"

class QMainWindow;
class QTranslator;
class RepeaterDatabase;
class UserDatabase;
class TalkGroupDatabase;
class SatelliteDatabase;
class RadioIDListView;
class GeneralSettingsView;
class ContactListView;
class GroupListsView;
class ChannelListView;
class ZoneListView;
class ScanListsView;
class PositioningSystemListView;
class RoamingChannelListView;
class RoamingZoneListView;
class ExtensionView;



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

  Radio *autoDetect(const ErrorStack &err=ErrorStack());

  bool isDarkMode() const;
  bool isDarkMode(const QPalette &palette) const;

public slots:
  void newCodeplug();
  void loadCodeplug();
  void saveCodeplug();
  void exportCodeplugToChirp();
  void importCodeplug();
  void quitApplication();

  void detectRadio();
  bool verifyCodeplug(Radio *radio=nullptr, bool showSuccess=true);

  void downloadCodeplug();
  void uploadCodeplug();
  void uploadCallsignDB();
  void uploadSatellites();

  void showSettings();
  void showAbout();
  void showHelp();

  void editSatellites();

private slots:
  QMainWindow *createMainWindow();

  void onCodeplugDownloadError(Radio *radio);
  void onCodeplugDownloaded(Radio *radio, Codeplug *codeplug);

  void onCodeplugUploadError(Radio *radio);
  void onCodeplugUploaded(Radio *radio);

  void onConfigModifed();

  void positionUpdated(const QGeoPositionInfo &info);

  void onPaletteChanged(const QPalette &palette);

protected:
  Config *_config;
  QMainWindow *_mainWindow;
  QTranslator *_translator;

  GeneralSettingsView *_generalSettings;
  RadioIDListView *_radioIdTab;
  ContactListView *_contactList;
  GroupListsView *_groupLists;
  ChannelListView *_channelList;
  ZoneListView *_zoneList;
  ScanListsView *_scanLists;
  PositioningSystemListView *_posSysList;
  RoamingChannelListView *_roamingChannelList;
  RoamingZoneListView *_roamingZoneList;
  ExtensionView *_extensionView;

  RepeaterDatabase *_repeater;
  UserDatabase *_users;
  TalkGroupDatabase *_talkgroups;
  SatelliteDatabase *_satellites;

  QGeoPositionInfoSource *_source;
  QGeoCoordinate _currentPosition;

  ReleaseNotes _releaseNotes;

  // Last detected device:
  USBDeviceDescriptor _lastDevice;
};

#endif // APPLICATION_HH
