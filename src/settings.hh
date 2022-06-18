#ifndef SETTINGS_HH
#define SETTINGS_HH

#include <QDir>
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
	bool repeaterUpdateNeeded(unsigned period=7) const;
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

  Codeplug::Flags codePlugFlags() const;

  bool limitCallSignDBEntries() const;
  void setLimitCallSignDBEnties(bool enable);
  unsigned maxCallSignDBEntries() const;
  void  setMaxCallSignDBEntries(unsigned max);
  bool selectUsingUserDMRID();
  void setSelectUsingUserDMRID(bool enable);
  QSet<unsigned> callSignDBPrefixes();
  void setCallSignDBPrefixes(const QSet<unsigned> &prefixes);

  bool ignoreVerificationWarning() const;
  void setIgnoreVerificationWarning(bool ignore);

  bool ignoreFrequencyLimits() const;
  void setIgnoreFrequencyLimits(bool ignore);

  bool showCommercialFeatures() const;
  void setShowCommercialFeatures(bool show);

  bool showExtensions() const;
  void setShowExtensions(bool show);

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

  QByteArray headerState(const QString &objName) const;
  void setHeaderState(const QString &objName, const QByteArray &state);

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
  void onIgnoreFrequencyLimitsSet(bool enabled);
  void onDBLimitToggled(bool enable);
  void onUseUserDMRIdToggled(bool enable);

protected:
  QGeoPositionInfoSource *_source;
};

#endif // SETTINGS_HH
