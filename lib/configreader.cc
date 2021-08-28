#include "configreader.hh"
#include "config.hh"
#include "logger.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of ConfigObjectReader
 * ********************************************************************************************* */
AbstractConfigReader::AbstractConfigReader(QObject *parent)
  : QObject(parent), _errorMessage()
{
  // pass...
}

const QString &
AbstractConfigReader::errorMessage() const {
  return _errorMessage;
}


/* ********************************************************************************************* *
 * Implementation of ConfigReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> ConfigReader::_extensions;

ConfigReader::ConfigReader(QObject *parent)
  : AbstractConfigReader(parent)
{
  // pass...
}

bool
ConfigReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
}

ConfigObject *
ConfigReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  Q_UNUSED(node); Q_UNUSED(ctx);
  return new Config();
}

bool
ConfigReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx)
{
  Config *config = qobject_cast<Config *>(obj);
  if (nullptr == config) {
    _errorMessage = tr("Cannot read configuration: passed object is not of type 'Config'.");
    return false;
  }

  if (! node.IsMap()) {
    _errorMessage = tr("Cannot read configuration, element is not a map.");
    return false;
  }

  if (node["version"] && node["version"].IsScalar()) {
    ctx.setVersion(QString::fromStdString(node["version"].as<std::string>()));
  }

  if (node["mic-level"] && node["mic-level"].IsScalar()) {
    config->setMicLevel(node["mic-level"].as<uint>());
  }

  if (node["speech"] && node["speech"].IsScalar()) {
    config->setSpeech(node["speech"].as<bool>());
  }

  if (node["radio-ids"] && node["radio-ids"].IsSequence()) {
    RadioIdReader reader;
    for (YAML::const_iterator it=node["radio-ids"].begin(); it!=node["radio-ids"].end(); it++) {
      if ((*it)["dmr"] && (*it)["dmr"].IsMap()) {
        RadioID *id = qobject_cast<RadioID*>(reader.allocate((*it)["dmr"], ctx));
        if (nullptr == id) {
          _errorMessage = tr("Cannot parse configuration: Cannot allocate radio-id: %1")
              .arg(reader.errorMessage());
          return false;
        }
        if (! reader.parse(id, (*it)["dmr"], ctx)) {
          _errorMessage = tr("Cannot parse configuration: Cannot parse radio-id: %1")
              .arg(reader.errorMessage());
          return false;
        }
      } else {
        _errorMessage = tr("Cannot parse radio id: unknown type.");
      }
    }
  } else {
    _errorMessage = "Element 'radio-ids' missing or not a list.";
    return false;
  }

  if (node["channels"] && node["channels"].IsSequence()) {
    for (YAML::const_iterator it=node["channels"].begin(); it!=node["channels"].end(); it++) {
      if (node["channels"]["digital"] && node["channels"]["digital"].IsMap()) {
        YAML::Node chNode = node["channels"]["digital"];
        DigitalChannelReader reader;
        DigitalChannel *ch = reader.allocate(chNode, ctx)->as<DigitalChannel>();
        if (! ch) {
          _errorMessage = tr("Cannot allocate digital channel: %1").arg(reader.errorMessage());
          return false;
        }
        if (! reader.parse(ch, chNode, ctx)) {
          _errorMessage = tr("Cannot parse digital channel: %1").arg(reader.errorMessage());
          return false;
        }
      } else if (node["channels"]["analog"] && node["channels"]["analog"].IsMap()) {
        YAML::Node chNode = node["channels"]["analog"];
        AnalogChannelReader reader;
        AnalogChannel *ch = reader.allocate(chNode, ctx)->as<AnalogChannel>();
        if (! ch) {
          _errorMessage = tr("Cannot allocate analog channel: %1").arg(reader.errorMessage());
          return false;
        }
        if (! reader.parse(ch, chNode, ctx)) {
          _errorMessage = tr("Cannot parse analog channel: %1").arg(reader.errorMessage());
          return false;
        }
      } else {
        _errorMessage = tr("Cannot parse channel: Uknown type.");
        return false;
      }
    }
  } else {
    _errorMessage = "Element 'channels' missing or not a list.";
    return false;
  }

  if (node["zones"] && node["zones"].IsSequence()) {
    ZoneReader reader;
    for (YAML::const_iterator it=node["zones"].begin(); it!=node["zones"].end(); it++) {
      Zone *zone = reader.allocate(*it, ctx)->as<Zone>();
      if (nullptr == zone) {
        _errorMessage = tr("Cannot allocate zone: %1").arg(reader.errorMessage());
        return false;
      }
      if (! reader.parse(zone, node, ctx)) {
        _errorMessage = tr("Cannot parse zone: %1").arg(reader.errorMessage());
        return false;
      }
      config->zones()->add(zone);
    }
  } else {
    _errorMessage = "Element 'zones' missing or not a list.";
    return false;
  }

  if (node["scan-lists"] && node["scan-lists"].IsSequence()) {
    ScanListReader reader;
    for (YAML::const_iterator it=node["scan-lists"].begin(); it!=node["scan-lists"].end(); it++) {
      ScanList *sl = reader.allocate(*it, ctx)->as<ScanList>();
      if (nullptr == sl) {
        _errorMessage = tr("Cannot allocate scan-list: %1").arg(reader.errorMessage());
        return false;
      }
      if (! reader.parse(sl, *it, ctx)) {
        _errorMessage = tr("Cannot parse scan-list: %1").arg(reader.errorMessage());
        return false;
      }
      config->scanlists()->add(sl);
    }
  }

  if (node["contacts"] && node["contacts"].IsSequence()) {
    for (YAML::const_iterator it=node["contacts"].begin(); it!=node["contacts"].end(); it++) {
      if ((*it)["private"] && ((*it)["private"].IsMap())) {
        PrivateCallContactReader reader;
        Contact *contact = reader.allocate((*it)["private"], ctx)->as<Contact>();
        if (nullptr == contact) {
          _errorMessage = tr("Cannot allocate private call contact: %1").arg(reader.errorMessage());
          return false;
        }
        if (! reader.parse(contact, (*it)["private"], ctx)) {
          _errorMessage = tr("Cannot parse private call contact: %1").arg(reader.errorMessage());
          return false;
        }
        config->contacts()->add(contact);
      } else if ((*it)["group"] && ((*it)["group"].IsMap())) {
        GroupCallContactReader reader;
        Contact *contact = reader.allocate((*it)["group"], ctx)->as<Contact>();
        if (nullptr == contact) {
          _errorMessage = tr("Cannot allocate group call contact: %1").arg(reader.errorMessage());
          return false;
        }
        if (! reader.parse(contact, (*it)["group"], ctx)) {
          _errorMessage = tr("Cannot parse group call contact: %1").arg(reader.errorMessage());
          return false;
        }
        config->contacts()->add(contact);
      } else if ((*it)["all"] && ((*it)["all"].IsMap())) {
        AllCallContactReader reader;
        Contact *contact = reader.allocate((*it)["all"], ctx)->as<Contact>();
        if (nullptr == contact) {
          _errorMessage = tr("Cannot allocate all call contact: %1").arg(reader.errorMessage());
          return false;
        }
        if (! reader.parse(contact, (*it)["all"], ctx)) {
          _errorMessage = tr("Cannot parse all call contact: %1").arg(reader.errorMessage());
          return false;
        }
        config->contacts()->add(contact);
      } else {
        _errorMessage = tr("Cannot parse contact: Unknown type.");
        return false;
      }
    }
  } else {
    _errorMessage = "Element 'contacts' missing or not a list.";
    return false;
  }

  if (node["group-lists"] && node["group-lists"].IsSequence()) {
    GroupListReader reader;
    for (YAML::const_iterator it=node["group-lists"].begin(); it!=node["group-lists"].end(); it++) {
      RXGroupList *list = reader.allocate(*it, ctx)->as<RXGroupList>();
      if (nullptr == list) {
        _errorMessage = tr("Cannot allocate group list: %1").arg(reader.errorMessage());
        return false;
      }
      if (! reader.parse(list, *it, ctx)) {
        _errorMessage = tr("Cannot parse group list: %1").arg(reader.errorMessage());
        return false;
      }
    }
  } else {
    _errorMessage = "Element 'group-lists' missing or not a list.";
    return false;
  }

  if (node["positioning"] && node["positioning"].IsSequence()) {
    GPSSystemReader dmr;
    APRSSystemReader aprs;
    for (YAML::const_iterator it=node["positioning"].begin(); it!=node["positioning"].end(); it++) {
      if ((*it)["dmr"]) {
        PositioningSystem *sys = dmr.allocate((*it)["dmr"], ctx)->as<PositioningSystem>();
        if (nullptr == sys) {
          _errorMessage = tr("Cannot allocate GPS system: %1").arg(dmr.errorMessage());
          return false;
        }
        if (! dmr.parse(sys, (*it)["dmr"], ctx)) {
          _errorMessage = tr("Cannot parse GPS system: %1").arg(dmr.errorMessage());
          return false;
        }
        config->posSystems()->add(sys);
      } else if ((*it)["aprs"]) {
        PositioningSystem *sys = aprs.allocate((*it)["dmr"], ctx)->as<PositioningSystem>();
        if (nullptr == sys) {
          _errorMessage = tr("Cannot allocate APRS system: %1").arg(aprs.errorMessage());
          return false;
        }
        if (! aprs.parse(sys, (*it)["aprs"], ctx)) {
          _errorMessage = tr("Cannot parse ARPS system: %1").arg(aprs.errorMessage());
          return false;
        }
        config->posSystems()->add(sys);
      }
    }
  }

  if (node["roaming"] && node["roaming"].IsSequence()) {
    RoamingReader reader;
    for (YAML::const_iterator it=node["roaming"].begin(); it!=node["roaming"].end(); it++) {
      RoamingZone *zone = reader.allocate(*it, ctx)->as<RoamingZone>();
      if (nullptr == zone) {
        _errorMessage = tr("Cannot allocate roaming: ").arg(reader.errorMessage());
        return false;
      }
      if (! reader.parse(zone, *it, ctx)) {
        _errorMessage = tr("Cannot parse roaming: ").arg(reader.errorMessage());
        return false;
      }
      config->roaming()->add(zone);
    }
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse configuration: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse configuration: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      config->addExtension(name, ext);
    }
  }

  return true;
}

bool
ConfigReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx)
{
  Config *config = qobject_cast<Config *>(obj);

  { // link radio IDs
    RadioIdReader reader;
    for (int i=0; i<config->radioIDs()->count(); i++) {
      if (! reader.link(config->radioIDs()->getId(i), node["radio-ids"][i]["dmr"], ctx)) {
        _errorMessage = tr("Cannot link configuration: %1").arg(reader.errorMessage());
        return false;
      }
    }
  }

  { // link channels
    AnalogChannelReader ana;
    DigitalChannelReader digi;
    for (int i=0; i<config->channelList()->count(); i++) {
      if (config->channelList()->channel(i)->is<DigitalChannel>()) {
        if (! digi.link(config->channelList()->channel(i), node["channels"][i]["digital"], ctx)) {
          _errorMessage = tr("Cannot link configuration: %1").arg(digi.errorMessage());
          return false;
        }
      } else {
        if (! ana.link(config->channelList()->channel(i), node["channels"][i]["analog"], ctx)) {
          _errorMessage = tr("Cannot link configuration: %1").arg(digi.errorMessage());
          return false;
        }
      }
    }
  }

  { // link zones
    ZoneReader reader;
    for (int i=0; i<config->zones()->count(); i++) {
      if (! reader.link(config->zones()->zone(i), node["zones"][i], ctx)) {
        _errorMessage = tr("Cannot link configuration: %1").arg(reader.errorMessage());
        return false;
      }
    }
  }

  { // link scan-list
    ScanListReader reader;
    for (int i=0; i<config->scanlists()->count(); i++) {
      if (! reader.link(config->scanlists()->scanlist(i), node["scan-lists"][i], ctx)) {
        _errorMessage = tr("Cannot link configuration: %1").arg(reader.errorMessage());
        return false;
      }
    }
  }

  // link contacts
  for (int i=0; i<config->contacts()->count(); i++) {
    if (node["contacts"][i]["private"]) {
      PrivateCallContactReader reader;
      if (! reader.link(config->contacts()->contact(i), node["contacts"][i]["private"], ctx)) {
        _errorMessage = tr("Cannot link private call contact '%1': %2")
            .arg(config->contacts()->contact(i)->name()).arg(reader.errorMessage());
        return false;
      }
    } else if (node["contacts"][i]["group"]) {
      GroupCallContactReader reader;
      if (! reader.link(config->contacts()->contact(i), node["contacts"][i]["group"], ctx)) {
        _errorMessage = tr("Cannot link group call contact '%1': %2")
            .arg(config->contacts()->contact(i)->name()).arg(reader.errorMessage());
        return false;
      }
    } else if (node["contacts"][i]["all"]) {
      AllCallContactReader reader;
      if (! reader.link(config->contacts()->contact(i), node["contacts"][i]["all"], ctx)) {
        _errorMessage = tr("Cannot link all call contact '%1': %2")
            .arg(config->contacts()->contact(i)->name()).arg(reader.errorMessage());
        return false;
      }
    }
  }

  { // link group lists
    GroupListReader reader;
    for (int i=0; i<config->rxGroupLists()->count(); i++) {
      if (! reader.link(config->rxGroupLists()->list(i), node["group-lists"][i], ctx)) {
        _errorMessage = tr("Cannot link group list '%1': %2")
            .arg(config->rxGroupLists()->list(i)->name()).arg(reader.errorMessage());
        return false;
      }
    }
  }

  { // positioning
    GPSSystemReader gps;
    APRSSystemReader aprs;
    for (int i=0; i<config->posSystems()->count(); i++) {
      if (node["positioning"][i]["dmr"]) {
        if (! gps.link(config->posSystems()->system(i), node["positioning"][i]["dmr"], ctx)) {
          _errorMessage = tr("Cannot link GPS system '%1': %2")
              .arg(config->posSystems()->system(i)->name()).arg(gps.errorMessage());
          return false;
        }
      } else if (node["positioning"][i]["aprs"]) {
        if (!aprs.link(config->posSystems()->system(i), node["positioning"][i]["aprs"], ctx)) {
          _errorMessage = tr("Cannot link APRS system '%1': %2")
              .arg(config->posSystems()->system(i)->name()).arg(gps.errorMessage());
          return false;
        }
      }
      return false;
    }
  }

  { // roaming
    RoamingReader reader;
    for (int i=0; i<config->roaming()->count(); i++) {
      if (reader.link(config->roaming()->zone(i), node["roaming"][i], ctx)) {
        _errorMessage = tr("Cannot link roaming '%1': %2")
            .arg(config->roaming()->zone(i)->name()).arg(reader.errorMessage());
        return false;
      }
    }
  }

  // link extensions
  foreach (QString name, config->extensionNames()) {
    if (! _extensions[name]->link(config->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link configuration extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ObjectReader
 * ********************************************************************************************* */
ObjectReader::ObjectReader(QObject *parent)
  : AbstractConfigReader(parent)
{
  // pass...
}

bool
ObjectReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx)
{
  if (! node.IsMap()) {
    _errorMessage = "Cannot parse object: Passed node is not a map.";
    return false;
  }

  if (node["id"] && node["id"].IsScalar()) {
    QString id = QString::fromStdString(node["id"].as<std::string>());
    if (ctx.contains(id)) {
      _errorMessage = tr("Cannot parse radio-id '%1': ID already used.").arg(id);
      return false;
    }
    ctx.add(id, obj);
  } else {
    logWarn() << "No ID associated with object, it cannot be referenced later.";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioIDReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> RadioIdReader::_extensions;

RadioIdReader::RadioIdReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}

ConfigObject *
RadioIdReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new RadioID(0);
}

bool
RadioIdReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  RadioID *rid = qobject_cast<RadioID *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["number"] && node["number"].IsScalar()) {
    rid->setId(node["number"].as<uint>());
  } else {
    _errorMessage = tr("Cannot parse radio id: No number defined.");
    return false;
  }

  if (node["name"] && node["name"].IsScalar()) {
    //rid->setName(QString::fromStdString(radioIdNode["name"].to<std::string>()));
  } else {
    _errorMessage = tr("Cannot parse radio id: No number defined.");
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse radio id extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse radio id extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      rid->addExtension(name, ext);
    }
  }

  return true;
}

bool
RadioIdReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  RadioID *rid = qobject_cast<RadioID *>(obj);

  // link extensions
  foreach (QString name, rid->extensionNames()) {
    if (! _extensions[name]->link(rid->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link radio-id extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ChannelReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> ChannelReader::_extensions;

ChannelReader::ChannelReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}

bool
ChannelReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  Channel *channel = qobject_cast<Channel *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["name"] && node["name"].IsScalar()) {
    channel->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = "Cannot parse channel: No RX frequency set.";
    return false;
  }

  if (node["rx"] && node["rx"].IsScalar()) {
    channel->setRXFrequency(node["rx"].as<double>());
    channel->setTXFrequency(node["rx"].as<double>());
  } else {
    _errorMessage = "Cannot parse channel: No RX frequency set.";
    return false;
  }

  if (node["tx"] && node["tx"].IsScalar()) {
    channel->setTXFrequency(node["tx"].as<double>());
  } else if (node["tx-offset"] && node["tx-offset"].IsScalar()) {
    channel->setTXFrequency(channel->rxFrequency()+node["tx-offset"].as<double>());
  }

  if (node["power"] && node["power"].IsScalar()) {
    QString power = QString::fromStdString(node["power"].as<std::string>());
    if ("min" == power) {
      channel->setPower(Channel::MinPower);
    } else if ("low" == power) {
      channel->setPower(Channel::LowPower);
    } else if ("mid" == power) {
      channel->setPower(Channel::MidPower);
    } else if ("high" == power) {
      channel->setPower(Channel::HighPower);
    } else if ("max" == power) {
      channel->setPower(Channel::MaxPower);
    } else {
      _errorMessage = tr("Cannot parse channel %1: Unknown power level '%2'")
          .arg(channel->name()).arg(power);
      return false;
    }
  } else {
    _errorMessage = "Cannot parse channel: No RX frequency set.";
    return false;
  }


  if (node["tx-timeout"] && node["tx-timeout"].IsScalar()) {
    channel->setTimeout(node["tx-timeout"].as<uint>());
  }

  if (node["rx-only"] && node["rx-only"].IsScalar()) {
    channel->setTimeout(node["rx-only"].as<bool>());
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse radio id extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse radio id extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      channel->addExtension(name, ext);
    }
  }

  return true;
}

bool
ChannelReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  Channel *channel = qobject_cast<Channel *>(obj);

  if (node["scan-list"] && node["scan-list"].IsScalar()) {
    QString sl = QString::fromStdString(node["scan-list"].as<std::string>());
    if ((! ctx.contains(sl)) || (ctx.getObj(sl)->is<ScanList>())) {
      _errorMessage = tr("Cannot link channel '%1': Scan list with id='%2' unknown.")
          .arg(channel->name()).arg(sl);
      return false;
    }
  }

  // link extensions
  foreach (QString name, channel->extensionNames()) {
    if (! _extensions[name]->link(channel->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link channel extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of DigitalChannelReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> DigitalChannelReader::_extensions;

DigitalChannelReader::DigitalChannelReader(QObject *parent)
  : ChannelReader(parent)
{
  // pass...
}

ConfigObject *
DigitalChannelReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new DigitalChannel("", 0,0, Channel::LowPower, -1, false, DigitalChannel::AdmitNone, 0,
                            DigitalChannel::TimeSlot1, nullptr, nullptr, nullptr, nullptr, nullptr,
                            nullptr);
}

bool
DigitalChannelReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! ChannelReader::parse(obj, node, ctx)) {
    return false;
  }

  DigitalChannel *channel = obj->as<DigitalChannel>();
  if (nullptr == channel) {
    _errorMessage = "Cannot parse digital channel: No DigitalChannel object passed.";
    return false;
  }

  if (node["color-code"] && node["color-code"].IsScalar()) {
    channel->setColorCode(node["color-code"].as<uint>());
  } else {
    _errorMessage = tr("Cannot parse digital channel '%1': No color-code set.")
        .arg(channel->name());
    return false;
  }

  if (node["admit"] && node["admit"].IsScalar()) {
    QString admit = QString::fromStdString(node["admit"].as<std::string>());
    if ("always" == admit) {
      channel->setAdmit(DigitalChannel::AdmitNone);
    } else if ("free" == admit) {
      channel->setAdmit(DigitalChannel::AdmitFree);
    } else if ("color-code" == admit) {
      channel->setAdmit(DigitalChannel::AdmitColorCode);
    } else {
      _errorMessage = tr("Cannot parse digital channel %1: Unknown admit criterion '%2'")
          .arg(channel->name()).arg(admit);
      return false;
    }
  } else {
    _errorMessage = "Cannot parse digital channel: No admit criterion set.";
    return false;
  }

  if (node["time-slot"] && node["time-slot"].IsScalar()) {
    uint ts = node["time-slot"].as<uint>();
    if (1 == ts) {
      channel->setTimeSlot(DigitalChannel::TimeSlot1);
    } else if (2 == ts) {
      channel->setTimeSlot(DigitalChannel::TimeSlot2);
    } else {
      _errorMessage = tr("Cannot parse digital channel %1: Unknown time-slot '%2'")
          .arg(channel->name()).arg(ts);
      return false;
    }
  } else {
    _errorMessage = "Cannot parse digital channel: No time-slot set.";
    return false;
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse digital channel extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse digital channel extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      channel->addExtension(name, ext);
    }
  }

  return true;
}

bool
DigitalChannelReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  DigitalChannel *channel = qobject_cast<DigitalChannel *>(obj);

  if (node["group-list"] && node["group-list"].IsScalar()) {
    QString gl = QString::fromStdString(node["group-list"].as<std::string>());
    if ((! ctx.contains(gl)) || (ctx.getObj(gl)->is<RXGroupList>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Group list with id='%2' is unknown.")
          .arg(channel->name()).arg(gl);
      return false;
    }
    channel->setRXGroupList(ctx.getObj(gl)->as<RXGroupList>());
  } else {
    _errorMessage = tr("Cannot link digital channel '%1': No group list defined.")
        .arg(channel->name());
    return false;
  }

  if (node["tx-contact"] && node["tx-contact"].IsScalar()) {
    QString c = QString::fromStdString(node["tx-contact"].as<std::string>());
    if ((! ctx.contains(c)) || (ctx.getObj(c)->is<DigitalContact>())) {
      _errorMessage = tr("Cannot link digital channel '%1': TX contact with id='%2' is unknown.")
          .arg(channel->name()).arg(c);
      return false;
    }
    channel->setTXContact(ctx.getObj(c)->as<DigitalContact>());
  }

  if (node["aprs"] && node["aprs"].IsScalar()) {
    QString aprs = QString::fromStdString(node["aprs"].as<std::string>());
    if ((! ctx.contains(aprs)) || (ctx.getObj(aprs)->is<PositioningSystem>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Positioning system with id='%2' is unknown.")
          .arg(channel->name()).arg(aprs);
      return false;
    }
    channel->setPosSystem(ctx.getObj(aprs)->as<PositioningSystem>());
  }

  if (node["roaming"] && node["roaming"].IsScalar()) {
    QString roaming = QString::fromStdString(node["roaming"].as<std::string>());
    if ((! ctx.contains(roaming)) || (ctx.getObj(roaming)->is<RoamingZone>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Roaming zone with id='%2' is unknown.")
          .arg(channel->name()).arg(roaming);
      return false;
    }
    channel->setRoaming(ctx.getObj(roaming)->as<RoamingZone>());
  }

  if (node["radio-id"] && node["radio-id"].IsScalar()) {
    QString id = QString::fromStdString(node["dmr-id"].as<std::string>());
    if ((! ctx.contains(id)) || (ctx.getObj(id)->is<RadioID>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Radio ID with id='%2' is unknown.")
          .arg(channel->name()).arg(id);
      return false;
    }
    channel->setRadioId(ctx.getObj(id)->as<RadioID>());
  }

  // link extensions
  foreach (QString name, channel->extensionNames()) {
    if (! _extensions[name]->link(channel->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link digital channel extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnalogChannelReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> AnalogChannelReader::_extensions;

AnalogChannelReader::AnalogChannelReader(QObject *parent)
  : ChannelReader(parent)
{
  // pass...
}

ConfigObject *
AnalogChannelReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new AnalogChannel("", 0,0, Channel::LowPower, -1, false, AnalogChannel::AdmitNone,
                           1, Signaling::SIGNALING_NONE, Signaling::SIGNALING_NONE,
                           AnalogChannel::BWNarrow, nullptr, nullptr);
}

bool
AnalogChannelReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  AnalogChannel *channel = obj->as<AnalogChannel>();

  if (! ChannelReader::parse(obj, node, ctx)) {
    return false;
  }

  if (node["squelch"] && node["squelch"].IsScalar()) {
    channel->setSquelch(node["squelch"].as<int>());
  }

  if (node["rx-tone"] && node["rx-tone"].IsMap()) {
    if (node["rx-tone"]["ctcss"] && node["rx-tone"]["ctcss"].IsScalar()) {
      channel->setRXTone(Signaling::fromCTCSSFrequency(node["rx-tone"]["ctcss"].as<double>()));
    } else if (node["rx-tone"]["dcs"] && node["rx-tone"]["dcs"].IsScalar()) {
      int code = node["rx-tone"]["dcs"].as<int>();
      bool inverted = (code < 0); code = std::abs(code);
      channel->setRXTone(Signaling::fromDCSNumber(code, inverted));
    }
  }

  if (node["tx-tone"] && node["tx-tone"].IsMap()) {
    if (node["tx-tone"]["ctcss"] && node["tx-tone"]["ctcss"].IsScalar()) {
      channel->setTXTone(Signaling::fromCTCSSFrequency(node["tx-tone"]["ctcss"].as<double>()));
    } else if (node["tx-tone"]["dcs"] && node["tx-tone"]["dcs"].IsScalar()) {
      int code = node["tx-tone"]["dcs"].as<int>();
      bool inverted = (code < 0); code = std::abs(code);
      channel->setTXTone(Signaling::fromDCSNumber(code, inverted));
    }
  }

  if (node["band-width"] && node["band-width"].IsScalar()) {
    QString bw = QString::fromStdString(node["band-width"].as<std::string>());
    if ("narrow" == bw) {
      channel->setBandwidth(AnalogChannel::BWNarrow);
    } else {
      channel->setBandwidth(AnalogChannel::BWWide);
    }
  }

  if (node["admit"] && node["admit"].IsScalar()) {
    QString admit = QString::fromStdString(node["admit"].as<std::string>());
    if ("always" == admit) {
      channel->setAdmit(AnalogChannel::AdmitNone);
    } else if ("free" == admit) {
      channel->setAdmit(AnalogChannel::AdmitFree);
    } else if ("tone" == admit) {
      channel->setAdmit(AnalogChannel::AdmitTone);
    } else {
      _errorMessage = tr("Cannot parse digital channel %1: Unknown admit criterion '%2'")
          .arg(channel->name()).arg(admit);
      return false;
    }
  } else {
    _errorMessage = "Cannot parse digital channel: No admit criterion set.";
    return false;
  }


  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse analog channel extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse analog channel extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      channel->addExtension(name, ext);
    }
  }

  return true;
}

bool
AnalogChannelReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  AnalogChannel *channel = qobject_cast<AnalogChannel *>(obj);

  if (node["aprs"] && node["aprs"].IsScalar()) {
    QString aprs = QString::fromStdString(node["aprs"].as<std::string>());
    if ((! ctx.contains(aprs)) || (ctx.getObj(aprs)->is<APRSSystem>())) {
      _errorMessage = tr("Cannot link analog channel '%1': APRS system with id='%2' is unknown.")
          .arg(channel->name()).arg(aprs);
      return false;
    }
    channel->setAPRSSystem(ctx.getObj(aprs)->as<APRSSystem>());
  }

  /*if (node["radio-id"] && node["radio-id"].IsScalar()) {
    QString id = QString::fromStdString(node["dmr-id"].as<std::string>());
    if ((! ctx.contains(id)) || (ctx.getObj(id)->is<RadioID>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Radio ID with id='%2' is unknown.")
          .arg(channel->name()).arg(id);
      return false;
    }
  }*/

  // link extensions
  foreach (QString name, channel->extensionNames()) {
    if (! _extensions[name]->link(channel->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link analog channel extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ZoneReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> ZoneReader::_extensions;

ZoneReader::ZoneReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}

ConfigObject *
ZoneReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new Zone("");
}

bool
ZoneReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  Zone *zone = qobject_cast<Zone *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["name"] && node["name"].IsScalar()) {
    zone->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = tr("Cannot parse zone: No name defined");
    return false;
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse zone extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse zone extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      zone->addExtension(name, ext);
    }
  }

  return true;
}

bool
ZoneReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  Zone *zone = qobject_cast<Zone *>(obj);

  // link A channels
  if (node["A"] && node["A"].IsSequence()) {
    for (YAML::const_iterator it=node["A"].begin(); it!=node["A"].end(); it++) {
      if (!it->IsScalar()) {
        _errorMessage = tr("Cannot link zone '%1': Channel reference of wrong type. Must be id.")
            .arg(zone->name());
        return false;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (ctx.getObj(id)->is<Channel>())) {
        _errorMessage = _errorMessage = tr("Cannot link zone '%1': '%2' does not refer to a channel.")
            .arg(zone->name()).arg(id);
        return false;
      }
      zone->A()->addChannel(ctx.getObj(id)->as<Channel>());
    }
  } else {
    _errorMessage = tr("Cannot link zone '%1': No A channel list defined.").arg(zone->name());
    return false;
  }

  // link optional B channels
  if (node["B"] && node["B"].IsSequence()) {
    for (YAML::const_iterator it=node["B"].begin(); it!=node["B"].end(); it++) {
      if (!it->IsScalar()) {
        _errorMessage = tr("Cannot link zone '%1': Channel reference of wrong type. Must be id.")
            .arg(zone->name());
        return false;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (ctx.getObj(id)->is<Channel>())) {
        _errorMessage = _errorMessage = tr("Cannot link zone '%1': '%2' does not refer to a channel.")
            .arg(zone->name()).arg(id);
        return false;
      }
      zone->B()->addChannel(ctx.getObj(id)->as<Channel>());
    }
  }

  // link extensions
  foreach (QString name, zone->extensionNames()) {
    if (! _extensions[name]->link(zone->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link zone extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ContactReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> ContactReader::_extensions;

ContactReader::ContactReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}

bool
ContactReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  Contact *contact = qobject_cast<Contact *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["name"] && node["name"].IsScalar()) {
    contact->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = tr("Cannot parse zone: No name defined");
    return false;
  }

  if (node["ring"] && node["ring"].IsScalar()) {
    contact->setRXTone(node["ring"].as<bool>());
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse contact extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse contact extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      contact->addExtension(name, ext);
    }
  }

  return true;
}

bool
ContactReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  Contact *contact = qobject_cast<Contact *>(obj);

  // link extensions
  foreach (QString name, contact->extensionNames()) {
    if (! _extensions[name]->link(contact->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link contact extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of DigitalContactReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> DigitalContactReader::_extensions;

DigitalContactReader::DigitalContactReader(QObject *parent)
  : ContactReader(parent)
{
  // pass...
}

bool
DigitalContactReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  DigitalContact *contact = qobject_cast<DigitalContact *>(obj);

  if (! ContactReader::parse(obj, node, ctx))
    return false;

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse contact extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse contact extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      contact->addExtension(name, ext);
    }
  }

  return true;
}

bool
DigitalContactReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  DigitalContact *contact = qobject_cast<DigitalContact *>(obj);

  // link extensions
  foreach (QString name, contact->extensionNames()) {
    if (! _extensions[name]->link(contact->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link contact extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of PrivateCallContactReader
 * ********************************************************************************************* */
PrivateCallContactReader::PrivateCallContactReader(QObject *parent)
  : DigitalContactReader(parent)
{
  // pass...
}

ConfigObject *
PrivateCallContactReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new DigitalContact(DigitalContact::PrivateCall, "", 0);
}

bool
PrivateCallContactReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  DigitalContact *contact = qobject_cast<DigitalContact *>(obj);

  if (! DigitalContactReader::parse(obj, node, ctx))
    return false;

  if (node["number"] && node["number"].IsScalar()) {
    contact->setNumber(node["number"].as<uint>());
  } else {
    _errorMessage = tr("Cannot parse private call '%1': No number specified.").arg(contact->name());
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GroupCallContactReader
 * ********************************************************************************************* */
GroupCallContactReader::GroupCallContactReader(QObject *parent)
  : DigitalContactReader(parent)
{
  // pass...
}

ConfigObject *
GroupCallContactReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new DigitalContact(DigitalContact::GroupCall, "", 0);
}

bool
GroupCallContactReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  DigitalContact *contact = qobject_cast<DigitalContact *>(obj);

  if (! DigitalContactReader::parse(obj, node, ctx))
    return false;

  if (node["number"] && node["number"].IsScalar()) {
    contact->setNumber(node["number"].as<uint>());
  } else {
    _errorMessage = tr("Cannot parse group call '%1': No number specified.").arg(contact->name());
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of AllCallContactReader
 * ********************************************************************************************* */
AllCallContactReader::AllCallContactReader(QObject *parent)
  : DigitalContactReader(parent)
{
  // pass...
}

ConfigObject *
AllCallContactReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new DigitalContact(DigitalContact::AllCall, "", 16777215);
}

bool
AllCallContactReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! DigitalContactReader::parse(obj, node, ctx))
    return false;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of PositioningReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> PositioningReader::_extensions;

PositioningReader::PositioningReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}

bool
PositioningReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  PositioningSystem *system = qobject_cast<PositioningSystem *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["name"] && node["name"].IsScalar()) {
    system->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = tr("Cannot parse positioning system: No name defined");
    return false;
  }

  if (node["period"] && node["period"].IsScalar()) {
    system->setPeriod(node["period"].as<uint>());
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot allocate positioning system extension: %1")
            .arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse positioning system extension: %1")
            .arg(_extensions[name]->errorMessage());
        return false;
      }
      system->addExtension(name, ext);
    }
  }

  return true;
}

bool
PositioningReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  PositioningSystem *system = qobject_cast<PositioningSystem *>(obj);

  // link extensions
  foreach (QString name, system->extensionNames()) {
    if (! _extensions[name]->link(system->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link positioning system extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GPSSystemReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> GPSSystemReader::_extensions;

GPSSystemReader::GPSSystemReader(QObject *parent)
  : PositioningReader(parent)
{
  // pass...
}

ConfigObject *
GPSSystemReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new GPSSystem("");
}

bool
GPSSystemReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  GPSSystem *system = qobject_cast<GPSSystem *>(obj);

  if (! PositioningReader::parse(obj, node, ctx))
    return false;

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse GPS system extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse GPS system extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      system->addExtension(name, ext);
    }
  }

  return true;
}

bool
GPSSystemReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  GPSSystem *system = qobject_cast<GPSSystem *>(obj);

  // link destination contact
  if (node["destination"] && node["destination"].IsScalar()) {
    QString cont = QString::fromStdString(node["destination"].as<std::string>());
    if ((! ctx.contains(cont)) || (! ctx.getObj(cont)->is<DigitalContact>())) {
      _errorMessage = tr("Cannot link GPS system '%1': Destination contact '%2' not digital contact.")
          .arg(system->name()).arg(cont);
      return false;
    }
    system->setContact(ctx.getObj(cont)->as<DigitalContact>());
  } else {
    _errorMessage = tr("Cannot link GPS system '%1': No destination contact defined.")
        .arg(system->name());
    return false;
  }

  // link revert channel
  if (node["channel"] && node["channel"].IsScalar()) {
    QString ch = QString::fromStdString(node["channel"].as<std::string>());
    if ((! ctx.contains(ch)) || (! ctx.getObj(ch)->is<DigitalChannel>())) {
      _errorMessage = tr("Cannot link GPS system '%1': Revert channel '%2' not digital channel.")
          .arg(system->name()).arg(ch);
      return false;
    }
    system->setRevertChannel(ctx.getObj(ch)->as<DigitalChannel>());
  }

  // link extensions
  foreach (QString name, system->extensionNames()) {
    if (! _extensions[name]->link(system->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link GPS system extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of APRSSystemReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> APRSSystemReader::_extensions;

APRSSystemReader::APRSSystemReader(QObject *parent)
  : PositioningReader(parent)
{
  // pass...
}

ConfigObject *
APRSSystemReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new APRSSystem("",nullptr, "", 0, "", 0);
}

bool
APRSSystemReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  APRSSystem *system = qobject_cast<APRSSystem *>(obj);

  if (! PositioningReader::parse(obj, node, ctx))
    return false;

  if (node["source"] && node["source"].IsScalar()) {
    QString source = QString::fromStdString(node["source"].as<std::string>());
    QRegExp pattern("^([A-Z0-9]+)-(1?[0-9])$");
    if (! pattern.exactMatch(source)) {
      _errorMessage = tr("Cannot parse APRS system '%1': '%2' not valid source call and SSID.")
          .arg(system->name()).arg(source);
      return false;
    }
    system->setSource(pattern.cap(1), pattern.cap(2).toUInt());
  } else {
    _errorMessage = tr("Cannot parse APRS system '%1': No source specified.")
        .arg(system->name());
    return false;
  }

  if (node["destination"] && node["destination"].IsScalar()) {
    QString source = QString::fromStdString(node["destination"].as<std::string>());
    QRegExp pattern("^([A-Z0-9]+)-(1?[0-9])$");
    if (! pattern.exactMatch(source)) {
      _errorMessage = tr("Cannot parse APRS system '%1': '%2' not valid destination call and SSID.")
          .arg(system->name()).arg(source);
      return false;
    }
    system->setDestination(pattern.cap(1), pattern.cap(2).toUInt());
  } else {
    _errorMessage = tr("Cannot parse APRS system '%1': No destination specified.")
        .arg(system->name());
    return false;
  }

  if (node["path"] && node["path"].IsSequence()) {
    QStringList path;
    for (YAML::const_iterator it=node["path"].begin(); it!=node["path"].end(); it++) {
      if (it->IsScalar())
        path.append(QString::fromStdString(it->as<std::string>()));
    }
    system->setPath(path.join(""));
  }

  if (node["icon"] && node["icon"].IsScalar()) {
    system->setIcon(
          name2aprsicon(
            QString::fromStdString(node["node"].as<std::string>())));
  } else {
    _errorMessage = tr("Cannot parse APRS system '%1': No icon specified.").arg(system->name());
    return false;
  }

  if (node["message"] && node["message"].IsScalar()) {
    system->setMessage(QString::fromStdString(node["message"].as<std::string>()));
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse GPS system extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse GPS system extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      system->addExtension(name, ext);
    }
  }

  return true;
}

bool
APRSSystemReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  APRSSystem *system = qobject_cast<APRSSystem *>(obj);

  // link transmit channel
  if (node["channel"] && node["channel"].IsScalar()) {
    QString ch = QString::fromStdString(node["channel"].as<std::string>());
    if ((! ctx.contains(ch)) || (! ctx.getObj(ch)->is<AnalogChannel>())) {
      _errorMessage = tr("Cannot link APRS system '%1': Transmit channel '%2' not analog channel.")
          .arg(system->name()).arg(ch);
      return false;
    }
    system->setChannel(ctx.getObj(ch)->as<AnalogChannel>());
  } else {
    _errorMessage = tr("Cannot link APRS system '%1': Transmit channel not defined.")
        .arg(system->name());
    return false;
  }

  // link extensions
  foreach (QString name, system->extensionNames()) {
    if (! _extensions[name]->link(system->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link GPS system extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ScanListReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> ScanListReader::_extensions;

ScanListReader::ScanListReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}


ConfigObject *
ScanListReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new ScanList("");
}

bool
ScanListReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  ScanList *list = qobject_cast<ScanList *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["name"] && node["name"].IsScalar()) {
    list->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = tr("Cannot parse scan-list: No name defined");
    return false;
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse scan-list extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse scan-list extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      list->addExtension(name, ext);
    }
  }

  return true;
}

bool
ScanListReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  ScanList *list = qobject_cast<ScanList *>(obj);

  // link priority channels if defined
  if (node["priority"] && node["priority"].IsSequence()) {
    uint n=0; YAML::const_iterator it=node["priority"].begin();
    for(; (it!=node["priority"].end()) && (n<2); it++, n++) {
      if (!it->IsScalar()) {
        logWarn() << "Cannot link priority channel " << (n+1) << ": Not a reference.";
        continue;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (ctx.getObj(id)->is<Channel>())) {
        logWarn() << "Cannot link priority channel " << (n+1) << ": Not a reference to a channel.";
        continue;
      }
      if (0 == n)
        list->setPriorityChannel(ctx.getObj(id)->as<Channel>());
      else
        list->setSecPriorityChannel(ctx.getObj(id)->as<Channel>());
    }
  }

  // link channels
  if (node["channels"] && node["channels"].IsSequence()) {
    for (YAML::const_iterator it=node["channels"].begin(); it!=node["channels"].end(); it++) {
      if (!it->IsScalar()) {
        _errorMessage = tr("Cannot link scan-list '%1': Channel reference of wrong type. Must be id.")
            .arg(list->name());
        return false;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (ctx.getObj(id)->is<Channel>())) {
        _errorMessage = _errorMessage = tr("Cannot link scan-list '%1': '%2' does not refer to a channel.")
            .arg(list->name()).arg(id);
        return false;
      }
      list->addChannel(ctx.getObj(id)->as<Channel>());
    }
  } else {
    _errorMessage = tr("Cannot link scan-list '%1': No A channel list defined.").arg(list->name());
    return false;
  }

  // link extensions
  foreach (QString name, list->extensionNames()) {
    if (! _extensions[name]->link(list->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link scan-list extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GroupListReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> GroupListReader::_extensions;

GroupListReader::GroupListReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}


ConfigObject *
GroupListReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new RXGroupList("");
}

bool
GroupListReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  RXGroupList *list = qobject_cast<RXGroupList *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["name"] && node["name"].IsScalar()) {
    list->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = tr("Cannot parse group list: No name defined");
    return false;
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse group list extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse group list extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      list->addExtension(name, ext);
    }
  }

  return true;
}

bool
GroupListReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  RXGroupList *list = qobject_cast<RXGroupList *>(obj);

  // link group calls
  if (node["members"] && node["members"].IsSequence()) {
    for (YAML::const_iterator it=node["members"].begin(); it!=node["members"].end(); it++) {
      if (!it->IsScalar()) {
        _errorMessage = tr("Cannot link group list '%1': Contact reference of wrong type.")
            .arg(list->name());
        return false;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (ctx.getObj(id)->is<DigitalContact>())) {
        _errorMessage = _errorMessage = tr("Cannot link group list '%1': '%2' does not refer to a digital contact.")
            .arg(list->name()).arg(id);
        return false;
      }
      list->addContact(ctx.getObj(id)->as<DigitalContact>());
    }
  } else {
    _errorMessage = tr("Cannot link group list '%1': No member list defined.").arg(list->name());
    return false;
  }

  // link extensions
  foreach (QString name, list->extensionNames()) {
    if (! _extensions[name]->link(list->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link group list extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of RoamingReader
 * ********************************************************************************************* */
QHash<QString, AbstractConfigReader *> RoamingReader::_extensions;

RoamingReader::RoamingReader(QObject *parent)
  : ObjectReader(parent)
{
  // pass...
}


ConfigObject *
RoamingReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new RoamingZone("");
}

bool
RoamingReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  RoamingZone *zone = qobject_cast<RoamingZone *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["name"] && node["name"].IsScalar()) {
    zone->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = tr("Cannot parse roaming: No name defined");
    return false;
  }

  // Parse extensions:
  foreach (QString name, _extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = _extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("Cannot parse roaming extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      if (! _extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("Cannot parse roaming extension: %1").arg(_extensions[name]->errorMessage());
        return false;
      }
      zone->addExtension(name, ext);
    }
  }

  return true;
}

bool
RoamingReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  RoamingZone *zone = qobject_cast<RoamingZone *>(obj);

  // link channels
  if (node["channels"] && node["channels"].IsSequence()) {
    for (YAML::const_iterator it=node["channels"].begin(); it!=node["channels"].end(); it++) {
      if (!it->IsScalar()) {
        _errorMessage = tr("Cannot link roaming '%1': Digital channel reference of wrong type.")
            .arg(zone->name());
        return false;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (ctx.getObj(id)->is<DigitalChannel>())) {
        _errorMessage = _errorMessage = tr("Cannot link roaming '%1': '%2' does not refer to a digital channel.")
            .arg(zone->name()).arg(id);
        return false;
      }
      zone->addChannel(ctx.getObj(id)->as<DigitalChannel>());
    }
  } else {
    _errorMessage = tr("Cannot link roaming '%1': No channel list defined.").arg(zone->name());
    return false;
  }

  // link extensions
  foreach (QString name, zone->extensionNames()) {
    if (! _extensions[name]->link(zone->extension(name), node, ctx)) {
      _errorMessage = tr("Cannot link roaming extension '%1': %2")
          .arg(name).arg(_extensions[name]->errorMessage());
    }
  }

  return true;
}

