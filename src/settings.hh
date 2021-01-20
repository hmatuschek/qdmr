#ifndef SETTINGS_HH
#define SETTINGS_HH

#include <QSettings>
#include <QDateTime>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>

#include "ui_settingsdialog.h"


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

  bool hideGSPNote() const;
  void setHideGPSNote(bool hide);

  bool showDisclaimer() const;
  void setShowDisclaimer(bool show);

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
  bool updateCodeplug() const;

protected slots:
  void onSystemLocationToggled(bool enable);
  void positionUpdated(const QGeoPositionInfo &info);

protected:
  QGeoPositionInfoSource *_source;
};

#endif // SETTINGS_HH
