#include "settings.hh"
#include "logger.hh"
#include "config.h"
#include <QStandardPaths>
#include <QDir>

QGeoCoordinate loc2deg(const QString &loc) {
  double lon = 0, lat = 0;
  if (2 > loc.size())
    return QGeoCoordinate();

  QChar l = loc[0].toUpper();
  QChar c = loc[1].toUpper();
  lon += double(int(l.toLatin1())-'A')*20;
  lat += double(int(c.toLatin1())-'A')*10;

  if (4 > loc.size()) {
    lon = lon - 180;
    lat = lat - 90;
    return QGeoCoordinate(lat, lon);
  }

  l = loc[2].toUpper();
  c = loc[3].toUpper();
  lon += double(int(l.toLatin1())-'0')*2;
  lat += double(int(c.toLatin1())-'0')*1;

  if (6 > loc.size()){
    lon = lon - 180;
    lat = lat - 90;
    return QGeoCoordinate(lat, lon);
  }

  l = loc[4].toUpper();
  c = loc[5].toUpper();
  lon += double(int(l.toLatin1())-'A')/12;
  lat += double(int(c.toLatin1())-'A')/24;

  lon = lon - 180;
  lat = lat - 90;
  return QGeoCoordinate(lat, lon);
}

QString deg2loc(const QGeoCoordinate &coor) {
  QString loc;
  double lon = coor.longitude()+180;
  double lat = coor.latitude()+90;
  char l = char(lon/20); lon -= 20*double(l);
  char c = char(lat/10); lat -= 10*double(c);
  loc.append(l+'A'); loc.append(c+'A');
  l = char(lon/2); lon -= 2*double(l);
  c = char(lat/1); lat -= 1*double(c);
  loc.append(l+'0'); loc.append(c+'0');
  l = char(lon*12); lon -= double(l)/12;
  c = char(lat*24); lat -= double(c)/24;
  loc.append(l+'a'); loc.append(c+'a');
  return loc;
}


/* ********************************************************************************************* *
 * Implementation of SettingsDialog
 * ********************************************************************************************* */
Settings::Settings(QObject *parent)
  : QSettings(parent)
{
  // pass...
}

QDateTime
Settings::lastRepeaterUpdate() const {
  if (! contains("lastRepeaterUpdate"))
    return QDateTime();
  return value("lastRepeaterUpdate").toDateTime();
}

void
Settings::repeaterUpdated() {
  setValue("lastRepeaterUpdate", QDateTime::currentDateTime());
}

bool
Settings::repeaterUpdateNeeded(uint period) const {
  QDateTime last = lastRepeaterUpdate();
  if (! last.isValid())
    return true;
  QDateTime now = QDateTime::currentDateTime();
  return last.daysTo(now) >= period;
}


bool
Settings::queryPosition() const {
  return value("queryPosition", false).toBool();
}

void
Settings::setQueryPosition(bool enable) {
  setValue("queryPosition", enable);
}

QString
Settings::locator() const {
  return value("locator","").toString();
}

void
Settings::setLocator(const QString &locator) {
  setValue("locator", locator);
}

QGeoCoordinate
Settings::position() const {
  return loc2deg(locator());
}

bool
Settings::updateCodeplug() const {
  return value("updateCodeplug", true).toBool();
}
void
Settings::setUpdateCodeplug(bool update) {
  setValue("updateCodeplug", update);
}

bool
Settings::autoEnableGPS() const {
  return value("autoEnableGPS", false).toBool();
}
void
Settings::setAutoEnableGPS(bool update) {
  setValue("autoEnableGPS", update);
}

bool
Settings::autoEnableRoaming() const {
  return value("autoEnableRoaming", false).toBool();
}
void
Settings::setAutoEnableRoaming(bool update) {
  setValue("autoEnableRoaming", update);
}

QDir
Settings::lastDirectory() const {
  return QDir(value("lastDir", QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first()).toString());
}
void
Settings::setLastDirectoryDir(const QDir &dir) {
  setValue("lastDir", dir.absolutePath());
}

CodePlug::Flags
Settings::codePlugFlags() const {
  CodePlug::Flags flags;
  flags.updateCodePlug = updateCodeplug();
  flags.autoEnableGPS  = autoEnableGPS();
  flags.autoEnableRoaming = autoEnableRoaming();
  return flags;
}

bool
Settings::ignoreVerificationWarning() const {
  return value("ignoreVerificationWarning", true).toBool();
}
void
Settings::setIgnoreVerificationWarning(bool ignore) {
  setValue("ignoreVerificationWarning", ignore);
}

bool
Settings::ignoreFrequencyLimits() const {
  return value("ignoreFrequencyLimits", false).toBool();
}
void
Settings::setIgnoreFrequencyLimits(bool ignore) {
  setValue("ignoreFrequencyLimits", ignore);
}

bool
Settings::hideGSPNote() const {
  return value("hideGPSNote", false).toBool();
}

void
Settings::setHideGPSNote(bool hide) {
  setValue("hideGPSNote", hide);
}

bool
Settings::hideRoamingNote() const {
  return value("hideRoamingNote", false).toBool();
}

void
Settings::setHideRoamingNote(bool hide) {
  setValue("hideRoamingNote", hide);
}

bool
Settings::hideZoneNote() const {
  return value("hideZoneNote", false).toBool();
}

void
Settings::setHideZoneNote(bool hide) {
  setValue("hideZoneNote", hide);
}

bool
Settings::showDisclaimer() const {
  return value("showDisclaimer", true).toBool();
}
void
Settings::setShowDisclaimer(bool show) {
  setValue("showDisclaimer", show);
}

QByteArray
Settings::mainWindowState() const {
  return value("mainWindowState", QByteArray()).toByteArray();
}
void
Settings::setMainWindowState(const QByteArray &state) {
  setValue("mainWindowState", state);
}

QByteArray
Settings::channelListHeaderState() const {
  return value("channelListHeaderState", QByteArray()).toByteArray();
}
void
Settings::setChannelListHeaderState(const QByteArray &state) {
  setValue("channelListHeaderState", state);
}

QByteArray
Settings::contactListHeaderState() const {
  return value("contactListHeaderState", QByteArray()).toByteArray();
}
void
Settings::setContactListHeaderState(const QByteArray &state) {
  setValue("contactListHeaderState", state);
}

QByteArray
Settings::positioningHeaderState() const {
  return value("positioningHeaderState", QByteArray()).toByteArray();
}
void
Settings::setPositioningHeaderState(const QByteArray &state) {
  setValue("positioningHeaderState", state);
}

bool
Settings::isUpdated() const {
  if (! contains("version"))
    return false;
  return VERSION_STRING == value("version").toString();
}
void
Settings::markUpdated() {
  setValue("version", VERSION_STRING);
}


/* ********************************************************************************************* *
 * Implementation of SettingsDialog
 * ********************************************************************************************* */
SettingsDialog::SettingsDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi(this);

  Settings settings;

  _source = QGeoPositionInfoSource::createDefaultSource(this);
  if (_source) {
    connect(_source, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));
    if (settings.queryPosition())
      _source->startUpdates();
  }

  queryLocation->setChecked(settings.queryPosition());
  locatorEntry->setText(settings.locator());
  if (queryLocation->isChecked())
    locatorEntry->setEnabled(false);

  connect(Ui::SettingsDialog::ignoreFrequencyLimits, SIGNAL(toggled(bool)),
          this, SLOT(onIgnoreFrequencyLimitsSet(bool)));
  connect(queryLocation, SIGNAL(toggled(bool)), this, SLOT(onSystemLocationToggled(bool)));

  Ui::SettingsDialog::updateCodeplug->setChecked(settings.updateCodeplug());
  Ui::SettingsDialog::autoEnableGPS->setChecked(settings.autoEnableGPS());
  Ui::SettingsDialog::autoEnableRoaming->setChecked(settings.autoEnableRoaming());
  Ui::SettingsDialog::ignoreVerificationWarnings->setChecked(settings.ignoreVerificationWarning());
  Ui::SettingsDialog::ignoreFrequencyLimits->setChecked(settings.ignoreFrequencyLimits());

}

bool
SettingsDialog::systemLocationEnabled() const {
  return queryLocation->isChecked();
}

QString
SettingsDialog::locator() const {
  return locatorEntry->text().simplified();
}

void
SettingsDialog::onSystemLocationToggled(bool enabled) {
  locatorEntry->setEnabled(! enabled);
  if (enabled && _source)
    _source->startUpdates();
  else if (_source)
    _source->stopUpdates();
}

void
SettingsDialog::onIgnoreFrequencyLimitsSet(bool enabled) {
  if (enabled) {
    Ui::SettingsDialog::ignoreFrequencyLimits->setText(tr("Warning!"));
  } else {
    Ui::SettingsDialog::ignoreFrequencyLimits->setText("");
  }
}

void
SettingsDialog::positionUpdated(const QGeoPositionInfo &info) {
  logDebug() << "Application: Current position: " << info.coordinate().toString();
  if (info.isValid() && queryLocation->isChecked()) {
    locatorEntry->setText(deg2loc(info.coordinate()));
  }
}

void
SettingsDialog::accept() {
  Settings settings;
  settings.setQueryPosition(queryLocation->isChecked());
  settings.setLocator(locatorEntry->text().simplified());
  settings.setUpdateCodeplug(updateCodeplug->isChecked());
  settings.setAutoEnableGPS(autoEnableGPS->isChecked());
  settings.setAutoEnableRoaming(autoEnableRoaming->isChecked());
  settings.setIgnoreVerificationWarning(ignoreVerificationWarnings->isChecked());
  settings.setIgnoreFrequencyLimits(ignoreFrequencyLimits->isChecked());
  QDialog::accept();
}



