#ifndef SETTINGS_HH
#define SETTINGS_HH

#include <QSettings>
#include <QDateTime>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>

#include "ui_settingsdialog.h"

#include "codeplug.hh"


class Settings : public QSettings
{
	Q_OBJECT

public:
	explicit Settings(QObject *parent=nullptr);

	QDateTime lastRepeaterUpdate() const;
	bool repeaterUpdateNeeded(uint period=7) const;
	void repeaterUpdated();

  bool queryPosition() const;
  void setQueryPosition(bool enable);

  QString locator() const;
  void setLocator(const QString &locator);
  QGeoCoordinate position() const;

  bool updateCodeplug() const;
  void setUpdateCodeplug(bool update);

  bool autoEnableGPS() const;
  void setAutoEnableGPS(bool enable);

  bool autoEnableRoaming() const;
  void setAutoEnableRoaming(bool enable);

  QDir lastDirectory() const;
  void setLastDirectoryDir(const QDir &dir);

  CodePlug::Flags codePlugFlags() const;

  bool ignoreVerificationWarning() const;
  void setIgnoreVerificationWarning(bool ignore);

  bool hideGSPNote() const;
  void setHideGPSNote(bool hide);

  bool hideRoamingNote() const;
  void setHideRoamingNote(bool hide);

  bool hideZoneNote() const;
  void setHideZoneNote(bool hide);

  bool showDisclaimer() const;
  void setShowDisclaimer(bool show);

  QByteArray mainWindowState() const;
  void setMainWindowState(const QByteArray &state);

  QByteArray channelListHeaderState() const;
  void setChannelListHeaderState(const QByteArray &state);

  QByteArray contactListHeaderState() const;
  void setContactListHeaderState(const QByteArray &state);

  QByteArray positioningHeaderState() const;
  void setPositioningHeaderState(const QByteArray &state);

  bool isUpdated() const;
  void markUpdated();
};


class SettingsDialog: public QDialog, private Ui::SettingsDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog(QWidget *parent=nullptr);

  bool systemLocationEnabled() const;
  QString locator() const;

public slots:
  void accept();

protected slots:
  void onSystemLocationToggled(bool enable);
  void positionUpdated(const QGeoPositionInfo &info);

protected:
  QGeoPositionInfoSource *_source;
};

#endif // SETTINGS_HH
