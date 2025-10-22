#include "settings.hh"
#include "logger.hh"
#include "config.h"
#include "utils.hh"
#include <QStandardPaths>
#include <QDir>


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
Settings::repeaterUpdateNeeded(unsigned period) const {
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
Settings::repeaterBookSourceEnabled() const {
  return value("repeaterSource/repeaterBook", true).toBool();
}
void
Settings::enableRepeaterBookSource(bool enabled) {
  return setValue("repeaterSource/repeaterBook", enabled);
}

bool
Settings::repeaterMapSourceEnabled() const {
  return value("repeaterSource/repeaterMap", true).toBool();
}
void
Settings::enableRepeaterMapSource(bool enabled) {
  return setValue("repeaterSource/repeaterMap", enabled);
}

bool
Settings::hearhamSourceEnabled() const {
  return value("repeaterSource/hearham", false).toBool();
}
void
Settings::enableHearhamSource(bool enabled) {
  return setValue("repeaterSource/hearham", enabled);
}

bool
Settings::radioIdRepeaterSourceEnabled() const {
  return value("repeaterSource/radioId", true).toBool();
}
void
Settings::enableRadioIdRepeaterSource(bool enabled) {
  return setValue("repeaterSource/radioId", enabled);
}


RepeaterBookSource::Region
Settings::repeaterBookRegion() const {
  return value("repeaterBookRegion", RepeaterBookSource::Region::World).value<RepeaterBookSource::Region>();
}
void
Settings::setRepeaterBookRegion(RepeaterBookSource::Region region) {
  setValue("repeaterBookRegion", region);
}

bool
Settings::disableAutoDetect() const {
  return value("disableAutoDetect", false).toBool();
}
void
Settings::setDisableAutoDetect(bool disable) {
  setValue("disableAutoDetect", disable);
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
Settings::updateDeviceClock() const {
  return value("updateDeviceClock", false).toBool();
}
void
Settings::setUpdateDeviceClock(bool update) {
  setValue("updateDeviceClock", update);
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

Codeplug::Flags
Settings::codePlugFlags() const {
  Codeplug::Flags flags;
  flags.setUpdateDeviceClock(updateDeviceClock());
  flags.setUpdateCodeplug(updateCodeplug());
  flags.setAutoEnableGPS(autoEnableGPS());
  flags.setAutoEnableRoaming(autoEnableRoaming());
  return flags;
}

bool
Settings::limitCallSignDBEntries() const {
  return value("limitCallSignDBEntries", false).toBool();
}
void
Settings::setLimitCallSignDBEnties(bool enable) {
  setValue("limitCallSignDBEntries", enable);
}

unsigned
Settings::maxCallSignDBEntries() const {
  return value("maxCallSignDBEntries", 1).toInt();
}
void
Settings::setMaxCallSignDBEntries(unsigned max) {
  setValue("maxCallSignDBEntries", max);
}

bool
Settings::selectUsingUserDMRID() {
  int num = beginReadArray("callSignDBPrefixes"); endArray();
  return value("selectCallSignDBUsingUserDMRID", true).toBool() || (0 == num);
}
void
Settings::setSelectUsingUserDMRID(bool enable) {
  setValue("selectCallSignDBUsingUserDMRID", enable);
}

QSet<unsigned>
Settings::callSignDBPrefixes() {
  QSet<unsigned> prefixes;
  int num = beginReadArray("callSignDBPrefixes");
  for (int i=0; i<num; i++) {
    setArrayIndex(i);
    bool ok=true; unsigned prefix = value("prefix").toInt(&ok);
    if (ok)
      prefixes.insert(prefix);
  }
  endArray();
  return prefixes;
}
void
Settings::setCallSignDBPrefixes(const QSet<unsigned> &prefixes) {
  beginWriteArray("callSignDBPrefixes");
  unsigned i = 0;
  foreach (unsigned prefix, prefixes) {
    setArrayIndex(i);
    setValue("prefix", prefix);
    i++;
  }
  endArray();
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
Settings::showCommercialFeatures() const {
  return value("showCommercialFeatures", false).toBool();
}
void
Settings::setShowCommercialFeatures(bool show) {
  setValue("showCommercialFeatures", show);
}

bool
Settings::showExtensions() const {
  return value("showExtensions", false).toBool();
}
void
Settings::setShowExtensions(bool show) {
  setValue("showExtensions", show);
}

bool
Settings::hideChannelNote() const {
  return value("hideChannelNote", false).toBool();
}
void
Settings::setHideChannelNote(bool hide) {
  setValue("hideChannelNote", hide);
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
Settings::hideRoamingChannelNote() const {
  return value("hideRoamingChannelNote", false).toBool();
}

void
Settings::setHideRoamingChannelNote(bool hide) {
  setValue("hideRoamingChannelNote", hide);
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

ConfigMergeVisitor::ItemStrategy
Settings::configMergeItemStrategy() const {
  return (ConfigMergeVisitor::ItemStrategy)value(
        "configMergeItemStrategy", (uint) ConfigMergeVisitor::ItemStrategy::Duplicate).toUInt();
}
void
Settings::setConfigMergeItemStrategy(ConfigMergeVisitor::ItemStrategy strategy) {
  setValue("configMergeItemStrategy", (uint)strategy);
}

ConfigMergeVisitor::SetStrategy
Settings::configMergeSetStrategy() const {
  return (ConfigMergeVisitor::SetStrategy)value(
        "configMergeSetStrategy", (uint) ConfigMergeVisitor::SetStrategy::Merge).toUInt();
}
void
Settings::setConfigMergeSetStrategy(ConfigMergeVisitor::SetStrategy strategy) {
  setValue("configMergeSetStrategy", (uint)strategy);
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
Settings::headerState(const QString &objName) const {
  if (objName.isEmpty())
    return QByteArray();
  QString key = QString("headerState/%1").arg(objName);
  return value(key, QByteArray()).toByteArray();
}
void
Settings::setHeaderState(const QString &objName, const QByteArray &state) {
  if (objName.isEmpty())
    return;
  QString key = QString("headerState/%1").arg(objName);
  setValue(key, state);
}


bool
Settings::sortFilterEnabled(const QString &objName) const {
  if (objName.isEmpty())
    return false;
  QString key = QString("sortFilter/%1").arg(objName);
  return value(key, false).toBool();
}

void
Settings::enableSortFilter(const QString &objName, bool enable) {
  if (objName.isEmpty())
    return;
  QString key = QString("sortFilter/%1").arg(objName);
  setValue(key, enable);
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

  Ui::SettingsDialog::repeaterBookEnable->setChecked(settings.repeaterBookSourceEnabled());
  switch (settings.repeaterBookRegion()) {
  case RepeaterBookSource::World: Ui::SettingsDialog::repeaterBookRegion->setCurrentIndex(0); break;
  case RepeaterBookSource::NorthAmerica: Ui::SettingsDialog::repeaterBookRegion->setCurrentIndex(1); break;
  }
  Ui::SettingsDialog::repeaterMapEnable->setChecked(settings.repeaterMapSourceEnabled());
  Ui::SettingsDialog::hearhamEnable->setChecked(settings.hearhamSourceEnabled());
  Ui::SettingsDialog::radioIdEnable->setChecked(settings.radioIdRepeaterSourceEnabled());

  connect(Ui::SettingsDialog::ignoreFrequencyLimits, SIGNAL(toggled(bool)),
          this, SLOT(onIgnoreFrequencyLimitsSet(bool)));
  connect(queryLocation, SIGNAL(toggled(bool)), this, SLOT(onSystemLocationToggled(bool)));

  Ui::SettingsDialog::disableAutoDetect->setChecked(settings.disableAutoDetect());
  Ui::SettingsDialog::updateCodeplug->setChecked(settings.updateCodeplug());
  Ui::SettingsDialog::updateDeviceClock->setChecked(settings.updateDeviceClock());
  Ui::SettingsDialog::autoEnableGPS->setChecked(settings.autoEnableGPS());
  Ui::SettingsDialog::autoEnableRoaming->setChecked(settings.autoEnableRoaming());
  Ui::SettingsDialog::ignoreVerificationWarnings->setChecked(settings.ignoreVerificationWarning());
  Ui::SettingsDialog::ignoreFrequencyLimits->setChecked(settings.ignoreFrequencyLimits());

  Ui::SettingsDialog::dbLimitEnable->setChecked(settings.limitCallSignDBEntries());
  if (! settings.limitCallSignDBEntries())
    Ui::SettingsDialog::dbLimit->setEnabled(false);
  Ui::SettingsDialog::dbLimit->setValue(settings.maxCallSignDBEntries());
  Ui::SettingsDialog::useUserId->setChecked(settings.selectUsingUserDMRID());
  if (settings.selectUsingUserDMRID())
    Ui::SettingsDialog::prefixes->setEnabled(false);
  QSet<unsigned> prefs = settings.callSignDBPrefixes();
  QStringList prefs_text;
  foreach (unsigned prefix, prefs) {
    prefs_text.append(QString::number(prefix));
  }
  Ui::SettingsDialog::prefixes->setText(prefs_text.join(", "));

  Ui::SettingsDialog::commercialFeatures->setChecked(settings.showCommercialFeatures());
  Ui::SettingsDialog::showExtensions->setChecked(settings.showExtensions());

  connect(Ui::SettingsDialog::dbLimitEnable, SIGNAL(toggled(bool)), this, SLOT(onDBLimitToggled(bool)));
  connect(Ui::SettingsDialog::useUserId, SIGNAL(toggled(bool)), this, SLOT(onUseUserDMRIdToggled(bool)));
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
SettingsDialog::onDBLimitToggled(bool enable) {
  Ui::SettingsDialog::dbLimit->setEnabled(enable);
}

void
SettingsDialog::onUseUserDMRIdToggled(bool enable) {
  Ui::SettingsDialog::prefixes->setEnabled(! enable);
}


void
SettingsDialog::accept() {
  Settings settings;
  settings.setQueryPosition(queryLocation->isChecked());
  settings.setLocator(locatorEntry->text().simplified());
  settings.setDisableAutoDetect(disableAutoDetect->isChecked());
  settings.enableRepeaterBookSource(Ui::SettingsDialog::repeaterBookEnable->isChecked());
  if (0 == Ui::SettingsDialog::repeaterBookRegion->currentIndex())
    settings.setRepeaterBookRegion(RepeaterBookSource::World);
  else
    settings.setRepeaterBookRegion(RepeaterBookSource::NorthAmerica);
  settings.enableRepeaterMapSource(Ui::SettingsDialog::repeaterMapEnable->isChecked());
  settings.enableHearhamSource(Ui::SettingsDialog::hearhamEnable->isChecked());
  settings.enableRadioIdRepeaterSource(Ui::SettingsDialog::radioIdEnable->isChecked());

  settings.setUpdateCodeplug(updateCodeplug->isChecked());
  settings.setUpdateDeviceClock(updateDeviceClock->isChecked());
  settings.setAutoEnableGPS(autoEnableGPS->isChecked());
  settings.setAutoEnableRoaming(autoEnableRoaming->isChecked());
  settings.setIgnoreVerificationWarning(ignoreVerificationWarnings->isChecked());
  settings.setIgnoreFrequencyLimits(ignoreFrequencyLimits->isChecked());
  settings.setLimitCallSignDBEnties(dbLimitEnable->isChecked());
  settings.setMaxCallSignDBEntries(dbLimit->value());
  settings.setSelectUsingUserDMRID(useUserId->isChecked());

  QStringList prefs_text = prefixes->text().split(",");
  QSet<unsigned> prefs;
  foreach (QString pref, prefs_text) {
    bool ok=true; unsigned prefix = pref.toUInt(&ok);
    if (ok)
      prefs.insert(prefix);
  }
  settings.setCallSignDBPrefixes(prefs);

  settings.setShowCommercialFeatures(commercialFeatures->isChecked());
  settings.setShowExtensions(showExtensions->isChecked());

  QDialog::accept();
}



