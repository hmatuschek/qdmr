#include "config.hh"
#include "config.h"

#include "rxgrouplist.hh"
#include "channel.hh"
#include "encryptionextension.hh"
#include "csvreader.hh"
#include "userdatabase.hh"
#include "logger.hh"

#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QMetaProperty>
#include <cmath>


/* ********************************************************************************************* *
 * Implementation of Config
 * ********************************************************************************************* */
Config::Config(QObject *parent)
  : ConfigItem(parent), _modified(false), _settings(new RadioSettings(this)),
    _radioIDs(new RadioIDList(this)), _contacts(new ContactList(this)),
    _rxGroupLists(new RXGroupLists(this)), _channels(new ChannelList(this)),
    _zones(new ZoneList(this)), _scanlists(new ScanLists(this)),
    _gpsSystems(new PositioningSystems(this)), _roaming(new RoamingZoneList(this)),
    _tytExtension(nullptr), _commercialExtension(new CommercialExtension(this))
{
  connect(_settings, SIGNAL(modified(ConfigItem*)), this, SLOT(onConfigModified()));
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

  connect(_commercialExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onConfigModified()));
}

bool
Config::copy(const ConfigItem &other) {
  const Config *conf = other.as<Config>();
  if ((nullptr==conf) || (! ConfigItem::copy(other)))
    return false;

  _settings->copy(*conf->settings());
  _radioIDs->copy(*conf->radioIDs());
  _contacts->copy(*conf->contacts());
  _rxGroupLists->copy(*conf->rxGroupLists());
  _channels->copy(*conf->channelList());
  _zones->copy(*conf->zones());
  _scanlists->copy(*conf->scanlists());
  _gpsSystems->copy(*conf->posSystems());
  _roaming->copy(*conf->roaming());

  return true;
}

ConfigItem *
Config::clone() const {
  Config *conf = new Config();
  if (! conf->copy(*this)) {
    conf->deleteLater();
    return nullptr;
  }
  return conf;
}

bool
Config::isModified() const {
  return _modified;
}
void
Config::setModified(bool modified) {
  _modified = modified;
}

bool
Config::toYAML(QTextStream &stream, const ErrorStack &err) {
  ConfigItem::Context context;
  // Label all codeplug elements
  if (! this->label(context, err))
    return false;
  // Serialize into YAML
  YAML::Node doc = serialize(context, err);
  if (doc.IsNull())
    return false;
  // Print YAML
  YAML::Emitter emitter;
  emitter << YAML::BeginDoc << doc << YAML::EndDoc;
  stream << emitter.c_str();
  return true;
}

bool
Config::populate(YAML::Node &node, const Context &context, const ErrorStack &err)
{
  node["version"] = VERSION_STRING;

  if ((node["settings"]= _settings->serialize(context, err)).IsNull())
    return false;

  if (_radioIDs->defaultId() && context.contains(_radioIDs->defaultId()))
    node["settings"]["defaultID"] = context.getId(_radioIDs->defaultId()).toStdString();

  if ((node["radioIDs"] = _radioIDs->serialize(context, err)).IsNull())
    return false;

  if ((node["contacts"] = _contacts->serialize(context, err)).IsNull())
    return false;

  if ((node["groupLists"] = _rxGroupLists->serialize(context, err)).IsNull())
    return false;

  if ((node["channels"] = _channels->serialize(context, err)).IsNull())
    return false;

  if ((node["zones"] = _zones->serialize(context, err)).IsNull())
    return false;

  if (_scanlists->count()) {
    if ((node["scanLists"] = _scanlists->serialize(context, err)).IsNull())
      return false;
  }

  if (_gpsSystems->count()) {
    if ((node["positioning"] = _gpsSystems->serialize(context, err)).IsNull())
      return false;
  }

  if (_roaming->count()) {
    if ((node["roaming"] = _roaming->serialize(context, err)).IsNull())
      return false;
  }

  if (! ConfigItem::populate(node, context, err))
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
    const DMRChannel *digi = channelList()->channel(i)->as<const DMRChannel>();
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
    if ( (ch->is<FMChannel>() && ch->as<FMChannel>()->aprsSystem()) ||
         (ch->is<DMRChannel>() && ch->as<DMRChannel>()->aprsObj()) ) {
      chHasGPS = true;
      break;
    }
  }
  return chHasGPS;
}

void
Config::clear() {
  ConfigItem::clear();

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

const Config *
Config::config() const {
  return this;
}


CommercialExtension *
Config::commercialExtension() const {
  return _commercialExtension;
}

TyTConfigExtension *
Config::tytExtension() const {
  return _tytExtension;
}
void
Config::setTyTExtension(TyTConfigExtension *ext) {
  if (_tytExtension == ext)
    return;
  if (_tytExtension)
    _tytExtension->deleteLater();
  _tytExtension = ext;
  if (_tytExtension) {
    _tytExtension->setParent(this);
    connect(_tytExtension, SIGNAL(modified(ConfigItem*)), this, SLOT(onConfigModified()));
  }
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
Config::readYAML(const QString &filename, const ErrorStack &err) {
  YAML::Node node;
  try {
    QFile file(filename);
    if (! file.open(QIODevice::ReadOnly)) {
      errMsg(err) << "Cannot open file '" << filename << "': " << file.errorString() << ".";
      errMsg(err) << "Cannot read YAML codeplug from file '" << filename << "'.";
      return false;
    }
    QByteArray content = file.readAll();
    node = YAML::Load(content.constData());
  } catch (const YAML::Exception &exc) {
    errMsg(err) << "Cannot read YAML codeplug from file '"<< filename
                << "': " << QString::fromStdString(exc.msg) << ".";
    return false;
  }

  if (! node) {
    errMsg(err) << "Cannot read YAML codeplug from file '" << filename << "'.";
    return false;
  }

  clear();
  ConfigItem::Context context;

  if (! parse(node, context, err))
    return false;

  if (! link(node, context, err))
    return false;

  return true;
}

bool
Config::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err)
{
  if (! node.IsMap()) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot read configuration"
                << ": Element is not a map.";
    return false;
  }

  if (node["version"] && node["version"].IsScalar()) {
    ctx.setVersion(QString::fromStdString(node["version"].as<std::string>()));
    logDebug() << "Using format version " << ctx.version() << ".";
  } else {
    logWarn() << "No version string set, assuming 0.9.0.";
    ctx.setVersion("0.9.0");
  }

  if (node["settings"] && (! _settings->parse(node["settings"], ctx, err)))
    return false;
  if (node["radioIDs"] && (! _radioIDs->parse(node["radioIDs"], ctx, err)))
    return false;
  if (node["contacts"] && (! _contacts->parse(node["contacts"], ctx, err)))
    return false;
  if (node["groupLists"] && (! _rxGroupLists->parse(node["groupLists"], ctx, err)))
    return false;
  if (node["channels"] && (! _channels->parse(node["channels"], ctx, err)))
    return false;
  if (node["zones"] && (! _zones->parse(node["zones"], ctx, err)))
    return false;
  if (node["scanLists"] && (! _scanlists->parse(node["scanLists"], ctx, err)))
    return false;
  if (node["positioning"] && (! _gpsSystems->parse(node["positioning"], ctx, err)))
    return false;
  if (node["roaming"] && (! _roaming->parse(node["roaming"], ctx, err)))
    return false;

  // also parses extensions
  if (! ConfigItem::parse(node, ctx, err))
    return false;

  return true;
}

bool
Config::link(const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  // radio IDs must be linked before settings, as they may refer to the default DMR ID

  if (! _radioIDs->link(node["radioIDs"], ctx, err))
    return false;

  if (! _settings->link(node["settings"], ctx, err))
    return false;

  // Link default radio ID separately as it is not a property of the settings but defined there
  if (node["settings"] && node["settings"]["defaultID"] && node["settings"]["defaultID"].IsScalar()) {
    YAML::Node defIDNode = node["settings"]["defaultID"];
    QString id = QString::fromStdString(defIDNode.as<std::string>());
    if (ctx.contains(id) && ctx.getObj(id)->is<DMRRadioID>()) {
      DMRRadioID *def = ctx.getObj(id)->as<DMRRadioID>();
      radioIDs()->setDefaultId(radioIDs()->indexOf(def));
      logDebug() << "Set default radio ID to '" << def->name() << "'.";
    } else {
      errMsg(err) << defIDNode.Mark().line << ":" << defIDNode.Mark().column
                  << "Default radio ID '" << id << " does not refer to a radio ID.";
      return false;
    }
  } else if (radioIDs()->count()) {
    // If no default is set, use first one.
    radioIDs()->setDefaultId(0);
  }

  if (node["contacts"] && (! _contacts->link(node["contacts"], ctx, err)))
    return false;
  if (node["groupLists"] && (! _rxGroupLists->link(node["groupLists"], ctx, err)))
    return false;
  if (node["channels"] && (! _channels->link(node["channels"], ctx, err)))
    return false;
  if (node["zones"] && (! _zones->link(node["zones"], ctx, err)))
    return false;
  if (node["scanLists"] && (! _scanlists->link(node["scanLists"], ctx, err)))
    return false;
  if (node["positioning"] && (! _gpsSystems->link(node["positioning"], ctx, err)))
    return false;
  if (node["roaming"] && (! _roaming->link(node["roaming"], ctx, err)))
    return false;

  // also links extensions
  if (! ConfigItem::link(node, ctx, err))
    return false;

  return true;
}
