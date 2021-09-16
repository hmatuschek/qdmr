#include "config.hh"
#include "config.h"
#include "rxgrouplist.hh"
#include "channel.hh"
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <cmath>
#include "csvreader.hh"
#include "csvwriter.hh"
#include "userdatabase.hh"


/* ********************************************************************************************* *
 * Implementation of Config
 * ********************************************************************************************* */
Config::Config(QObject *parent)
  : ConfigObject("config", parent), _modified(false), _radioIDs(new RadioIDList(this)),
    _contacts(new ContactList(this)), _rxGroupLists(new RXGroupLists(this)),
    _channels(new ChannelList(this)), _zones(new ZoneList(this)), _scanlists(new ScanLists(this)),
    _gpsSystems(new PositioningSystems(this)), _roaming(new RoamingZoneList(this)),
    _introLine1(), _introLine2(), _mic_level(2), _speech(false)
{
  /*connect(_radioIDs, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_contacts, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_rxGroupLists, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_channels, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_zones, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_scanlists, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_gpsSystems, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_roaming, SIGNAL(modified()), this, SLOT(onConfigModified()));*/
}

bool
Config::isModified() const {
  return _modified;
}
void
Config::setModified(bool modified) {
  _modified = modified;
  if (_modified)
    emit this->modified(this);
}

bool
Config::label(Context &context) {
  if (! ConfigObject::label(context))
    return false;

  if (! _radioIDs->label(context))
    return false;
  if (! _contacts->label(context))
    return false;
  if (! _rxGroupLists->label(context))
    return false;
  if (! _channels->label(context))
    return false;
  if (! _zones->label(context))
    return false;
  if (! _scanlists->label(context))
    return false;
  if (! _gpsSystems->label(context))
    return false;
  if (! _roaming->label(context))
    return false;

  return true;
}

bool
Config::toYAML(QTextStream &stream) {
  ConfigObject::Context context;
  if (! this->label(context))
    return false;
  YAML::Node doc = serialize(context);
  if (doc.IsNull())
    return false;
  YAML::Emitter emitter;
  emitter << YAML::BeginDoc << doc << YAML::EndDoc;
  stream << emitter.c_str();
  return true;
}

bool
Config::populate(YAML::Node &node, const Context &context)
{
  node["version"] = VERSION_STRING;

  YAML::Node settings;
  settings["micLevel"] = _mic_level;
  settings["speech"] = _speech;
  if (! _introLine1.isEmpty())
    settings["introLine1"] = _introLine1.toStdString();
  if (! _introLine2.isEmpty())
    settings["introLine2"] = _introLine2.toStdString();
  if (_radioIDs->defaultId() && context.contains(_radioIDs->defaultId()))
    settings["defaultID"] = context.getId(_radioIDs->defaultId()).toStdString();
  node["settings"] = settings;

  if ((node["radioIDs"] = _radioIDs->serialize(context)).IsNull())
    return false;

  if ((node["contacts"] = _contacts->serialize(context)).IsNull())
    return false;

  if ((node["groupLists"] = _rxGroupLists->serialize(context)).IsNull())
    return false;

  if ((node["channels"] = _channels->serialize(context)).IsNull())
    return false;

  if ((node["zones"] = _zones->serialize(context)).IsNull())
    return false;

  if (_scanlists->count()) {
    if ((node["scanLists"] = _scanlists->serialize(context)).IsNull())
      return false;
  }

  if (_gpsSystems->count()) {
    if ((node["positioning"] = _gpsSystems->serialize(context)).IsNull())
      return false;
  }

  if (_roaming->count()) {
    if ((node["roaming"] = _roaming->serialize(context)).IsNull())
      return false;
  }

  if (! ConfigObject::populate(node, context))
    return false;

  node.remove("id");
  return true;
}

RadioIDList *
Config::radioIDs() const {
  return _radioIDs;
}

ContactList *
Config::contacts() const {
  return _contacts;
}

RXGroupLists *
Config::rxGroupLists() const {
  return _rxGroupLists;
}

ChannelList *
Config::channelList() const {
  return _channels;
}

ZoneList *
Config::zones() const {
  return _zones;
}

ScanLists *
Config::scanlists() const {
  return _scanlists;
}

PositioningSystems *
Config::posSystems() const {
  return _gpsSystems;
}

RoamingZoneList *
Config::roaming() const {
  return _roaming;
}

bool
Config::requiresRoaming() const {
  // Check is roaming should be enabled
  bool chHasRoaming = false;
  for (int i=0; i<channelList()->count(); i++) {
    const DigitalChannel *digi = channelList()->channel(i)->as<const DigitalChannel>();
    if (nullptr == digi)
      continue;
    if (nullptr != digi->roamingZone()) {
      chHasRoaming = true;
      break;
    }
  }
  return chHasRoaming;
}

bool
Config::requiresGPS() const {
  // Check is GPS should be enabled
  bool chHasGPS = false;
  for (int i=0; i<channelList()->count(); i++) {
    Channel *ch = channelList()->channel(i);
    // For analog channels if APRS system is set or
    // for digital channels if any positioning system is set
    if ( (ch->is<AnalogChannel>() && ch->as<AnalogChannel>()->aprsSystem()) ||
         (ch->is<DigitalChannel>() && ch->as<DigitalChannel>()->aprsObj()) ) {
      chHasGPS = true;
      break;
    }
  }
  return chHasGPS;
}


const QString &
Config::introLine1() const {
  return _introLine1;
}
void
Config::setIntroLine1(const QString &line) {
  if (line == _introLine1)
    return;
  _introLine1 = line;
  emit modified(this);
}
const QString &
Config::introLine2() const {
  return _introLine2;
}
void
Config::setIntroLine2(const QString &line) {
  if (line == _introLine2)
    return;
  _introLine2 = line;
  emit modified(this);
}

uint
Config::micLevel() const {
  return _mic_level;
}
void
Config::setMicLevel(uint level) {
  level = std::min(10u, std::max(1u, level));
  if (level == _mic_level)
    return;
  _mic_level = level;
  emit modified(this);
}

bool
Config::speech() const {
  return _speech;
}
void
Config::setSpeech(bool enabled) {
  if (enabled == _speech)
    return;
  _speech = enabled;
  emit modified(this);
}

void
Config::reset() {
  // Reset lists
  _radioIDs->clear();
  _contacts->clear();
  _rxGroupLists->clear();
  _channels->clear();
  _zones->clear();
  _scanlists->clear();
  _gpsSystems->clear();
  _roaming->clear();

  _introLine1.clear();
  _introLine2.clear();
  _mic_level = 2;
  _speech    = false;

  foreach (ConfigObject *extension, _extensions) {
    extension->deleteLater();
  }

  emit modified(this);
}

void
Config::onConfigModified() {
  _modified = true;
  emit modified(this);
}

bool
Config::readCSV(const QString &filename, QString &errorMessage) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly))
    return false;
  QTextStream stream(&file);
  return readCSV(stream, errorMessage);
}

bool
Config::readCSV(QTextStream &stream, QString &errorMessage)
{
  if (CSVReader::read(this, stream, errorMessage))
    _modified = false;
  else
    return false;
  return true;
}

bool
Config::writeCSV(const QString &filename, QString &errorMessage) {
  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly)) {
    errorMessage = QString("Cannot open file %1: %2").arg(filename).arg(file.errorString());
    return false;
  }
  QTextStream stream(&file);
  return writeCSV(stream, errorMessage);
}

bool
Config::writeCSV(QTextStream &stream, QString &errorMessage)
{
  if (! CSVWriter::write(this, stream, errorMessage))
    return false;

  _modified = false;
  return true;
}


