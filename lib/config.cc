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
  : ConfigObject("", parent), _modified(false), _settings(new RadioSettings(this)),
    _radioIDs(new RadioIDList(this)), _contacts(new ContactList(this)),
    _rxGroupLists(new RXGroupLists(this)), _channels(new ChannelList(this)),
    _zones(new ZoneList(this)), _scanlists(new ScanLists(this)),
    _gpsSystems(new PositioningSystems(this)), _roaming(new RoamingZoneList(this))
{
  connect(_settings, SIGNAL(modified(ConfigObject*)), this, SLOT(onConfigModified()));
  connect(_radioIDs, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_radioIDs, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_radioIDs, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
  connect(_contacts, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_contacts, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_contacts, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
  connect(_rxGroupLists, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_rxGroupLists, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_rxGroupLists, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
  connect(_channels, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_channels, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_channels, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
  connect(_zones, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_zones, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_zones, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
  connect(_scanlists, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_scanlists, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_scanlists, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
  connect(_gpsSystems, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_gpsSystems, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_gpsSystems, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
  connect(_roaming, SIGNAL(elementAdded(int)), this, SLOT(onConfigModified()));
  connect(_roaming, SIGNAL(elementRemoved(int)), this, SLOT(onConfigModified()));
  connect(_roaming, SIGNAL(elementModified(int)), this, SLOT(onConfigModified()));
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

  if (! _settings->label(context))
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

  if ((node["settings"]= _settings->serialize(context)).IsNull())
    return false;

  if (_radioIDs->defaultId() && context.contains(_radioIDs->defaultId()))
    node["settings"]["defaultID"] = context.getId(_radioIDs->defaultId()).toStdString();

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

  return true;
}

RadioSettings *
Config::settings() const {
  return _settings;
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


void
Config::clear() {
  ConfigObject::clear();

  // Reset lists
  _settings->clear();
  _radioIDs->clear();
  _contacts->clear();
  _rxGroupLists->clear();
  _channels->clear();
  _zones->clear();
  _scanlists->clear();
  _gpsSystems->clear();
  _roaming->clear();

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

