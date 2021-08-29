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

bool
AbstractConfigReader::parseExtensions(
    const QHash<QString, AbstractConfigReader *> &extensions, ConfigObject *obj,
    const YAML::Node &node, ConfigObject::Context &ctx)
{
  // Parse extensions:
  foreach (QString name, extensions.keys()) {
    if (node[name.toStdString()]) {
      YAML::Node extNode = node[name.toStdString()];
      ConfigObject *ext = extensions[name]->allocate(extNode, ctx);
      if (!ext) {
        _errorMessage = tr("%1:%2: Cannot allocate extension '%3': %1")
            .arg(extNode.Mark().line).arg(extNode.Mark().column)
            .arg(name).arg(extensions[name]->errorMessage());
        return false;
      }
      if (! extensions[name]->parse(ext, extNode, ctx)) {
        _errorMessage = tr("%1:%2: Cannot parse extension '%3': %1")
            .arg(extNode.Mark().line).arg(extNode.Mark().column)
            .arg(name).arg(extensions[name]->errorMessage());
        return false;
      }
      obj->addExtension(name, ext);
    }
  }

  return true;
}

bool
AbstractConfigReader::linkExtensions(
    const QHash<QString, AbstractConfigReader *> &extensions, ConfigObject *obj,
    const YAML::Node &node, const ConfigObject::Context &ctx)
{
  foreach (QString name, obj->extensionNames()) {
    YAML::Node extNode = node[name.toStdString()];
    if (! extensions[name]->link(obj->extension(name), extNode, ctx)) {
      _errorMessage = tr("%1:%2: Cannot link configuration extension '%3': %4")
          .arg(extNode.Mark().line).arg(extNode.Mark().column)
          .arg(name).arg(extensions[name]->errorMessage());
      return false;
    }
  }
  return true;
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

bool
ConfigReader::read(Config *obj, const QString &filename) {
  YAML::Node node;
  try {
     node = YAML::LoadFile(filename.toStdString());
  } catch (const YAML::Exception &err) {
    _errorMessage = tr("Cannot read YAML codeplug from file '%1': %2")
        .arg(filename).arg(QString::fromStdString(err.msg));
    return false;
  }

  if (! node) {
    _errorMessage = tr("Cannot read YAML codeplug from file '%1'")
        .arg(filename);
    return false;
  }

  obj->reset();
  ConfigObject::Context context;

  if (! parse(obj, node, context))
    return false;

  if (! link(obj, node, context))
    return false;

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
    _errorMessage = tr("%1:%2: Cannot read configuration: passed object is not of type 'Config'.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  if (! node.IsMap()) {
    _errorMessage = tr("%1:%2: Cannot read configuration, element is not a map.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  if (node["version"] && node["version"].IsScalar()) {
    ctx.setVersion(QString::fromStdString(node["version"].as<std::string>()));
    logDebug() << "Using format version " << ctx.version() << ".";
  } else {
    logWarn() << "No version string set, assuming 0.9.0.";
    ctx.setVersion("0.9.0");
  }

  if (! parseSettings(config, node, ctx))
    return false;

  if (! parseRadioIDs(config, node["radio-ids"], ctx))
    return false;

  if (! parseContacts(config, node["contacts"], ctx))
    return false;

  if (! parseGroupLists(config, node["group-lists"], ctx))
    return false;

  if (! parseChannels(config, node["channels"], ctx))
    return false;

  if (! parseZones(config, node["zones"], ctx))
    return false;

  if (! parseScanLists(config, node["scan-lists"], ctx))
    return false;

  if (! parsePositioningSystems(config, node["positioning"], ctx))
    return false;

  if (! parseRoamingZones(config, node["roaming"], ctx))
    return false;

  if (! parseExtensions(_extensions, config, node, ctx))
    return false;

  return true;
}

bool
ConfigReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx)
{
  Config *config = qobject_cast<Config *>(obj);

  if (! linkSettings(config, node, ctx))
    return false;

  if (! linkRadioIDs(config, node["radio-ids"], ctx))
    return false;

  if (! linkContacts(config, node["contacts"], ctx))
    return false;

  if (! linkGroupLists(config, node["group-lists"], ctx))
    return false;

  if (! linkChannels(config, node["channels"], ctx))
    return false;

  if (! linkZones(config, node["zones"], ctx))
    return false;

  if (! linkScanLists(config, node["scan-lists"], ctx))
    return false;

  if (! linkPositioningSystems(config, node["positioning"], ctx))
    return false;

  if (! linkRoamingZones(config, node["roaming"], ctx))
    return false;

  // link extensions
  if (! linkExtensions(_extensions, config, node, ctx))
    return false;

  return true;
}


bool
ConfigReader::parseSettings(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (node["mic-level"] && node["mic-level"].IsScalar()) {
    config->setMicLevel(node["mic-level"].as<uint>());
  }

  if (node["speech"] && node["speech"].IsScalar()) {
    config->setSpeech(node["speech"].as<bool>());
  }

  return true;
}

bool
ConfigReader::linkSettings(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return true;
}


bool
ConfigReader::parseRadioIDs(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (!node) {
    _errorMessage = tr("No radio IDs defined.");
    return false;
  }

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'radio-ids' is not a sequence.");
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parseRadioID(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parseRadioID(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if ((! node.IsMap()) || (1 != node.size())) {
    _errorMessage = tr("%1:%2: Radio ID must map with a single element specifying the type.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;

  }

  std::string type = node.begin()->first.as<std::string>();
  if ("dmr" == type) {
    if (! parseDMRRadioID(config, node[type], ctx))
      return false;
  } else {
    _errorMessage = tr("%1:%2: Cannot parse radio id: unknown type '%3'.")
        .arg(node.Mark().line).arg(node.Mark().column).arg(QString::fromStdString(type));
  }

  return true;
}

bool
ConfigReader::parseDMRRadioID(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  RadioIdReader reader;
  RadioID *id = qobject_cast<RadioID*>(reader.allocate(node, ctx));
  if (nullptr == id) {
    _errorMessage = tr("%1:%2: Cannot allocate radio-id: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }
  if (! reader.parse(id, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse radio-id: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }
  config->radioIDs()->add(id);
  return true;
}

bool
ConfigReader::linkRadioIDs(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->radioIDs()->count()); it++, i++) {
    if (! linkRadioID(config->radioIDs()->getId(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkRadioID(RadioID *id, const YAML::Node &node, const ConfigObject::Context &ctx) {
  std::string type = node.begin()->first.as<std::string>();
  if ("dmr" == type) {
    if (! linkDMRRadioID(id, node[type], ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkDMRRadioID(RadioID *id, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return RadioIdReader().link(id, node, ctx);
}


bool
ConfigReader::parseChannels(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! node) {
    _errorMessage = tr("No channels defined.");
    return false;
  }

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'channels' element is not a sequence.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parseChannel(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parseChannel(Config *config, const YAML::Node &node, ConfigObject::Context &ctx)  {
  if ((! node.IsMap()) || (1 != node.size())) {
    _errorMessage = tr("%1:%2: Expected map with a single element.")
        .arg(node.Mark().line, node.Mark().column);
    return false;
  }

  std::string type = node.begin()->first.as<std::string>();
  if ("analog" == type) {
    if (! parseAnalogChannel(config, node[type], ctx))
      return false;
  } else if ("digital" == type) {
    if (! parseDigitalChannel(config, node[type], ctx))
      return false;
  } else {
    _errorMessage = tr("%1:%2: Uknown channel type '%3'")
        .arg(node.Mark().line).arg(node.Mark().column).arg(QString::fromStdString(type));
    return false;
  }

  return true;
}

bool
ConfigReader::parseDigitalChannel(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  DigitalChannelReader reader;
  DigitalChannel *ch = reader.allocate(node, ctx)->as<DigitalChannel>();
  if (! ch) {
    _errorMessage = tr("%1:%2: Cannot allocate digital channel: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }
  if (! reader.parse(ch, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse digital channel: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }
  config->channelList()->add(ch);
  return true;
}

bool
ConfigReader::parseAnalogChannel(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  AnalogChannelReader reader;
  AnalogChannel *ch = reader.allocate(node, ctx)->as<AnalogChannel>();
  if (! ch) {
    _errorMessage = tr("%1:%2: Cannot allocate analog channel: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }
  if (! reader.parse(ch, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse analog channel: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }
  config->channelList()->add(ch);
  return true;
}

bool
ConfigReader::linkChannels(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->channelList()->count()); it++, i++) {
    if (! linkChannel(config->channelList()->channel(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkChannel(Channel *channel, const YAML::Node &node, const ConfigObject::Context &ctx) {
  std::string type = node.begin()->first.as<std::string>();
  if ("analog" == type) {
    if (! linkAnalogChannel(channel->as<AnalogChannel>(), node[type], ctx))
      return false;
  } else if ("digital" == type) {
    if (! linkDigitalChannel(channel->as<DigitalChannel>(), node[type], ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkAnalogChannel(AnalogChannel *channel, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return AnalogChannelReader().link(channel, node, ctx);
}

bool
ConfigReader::linkDigitalChannel(DigitalChannel *channel, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return DigitalChannelReader().link(channel, node, ctx);
}


bool
ConfigReader::parseZones(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! node) {
    _errorMessage = tr("No zones defined.");
    return false;
  }

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'zones' element is not a sequence.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parseZone(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parseZone(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  ZoneReader reader;

  Zone *zone = reader.allocate(node, ctx)->as<Zone>();
  if (nullptr == zone) {
    _errorMessage = tr("%1:%2: Cannot allocate zone: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(zone, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse zone: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->zones()->add(zone);

  return true;
}

bool
ConfigReader::linkZones(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->zones()->count()); it++, i++) {
    if (! linkZone(config->zones()->zone(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkZone(Zone *zone, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return ZoneReader().link(zone, node, ctx);
}


bool
ConfigReader::parseScanLists(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! node)
    return true;

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'scan-lists' element is not a sequence.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parseScanList(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parseScanList(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  ScanListReader reader;

  ScanList *list = reader.allocate(node, ctx)->as<ScanList>();
  if (nullptr == list) {
    _errorMessage = tr("%1:%2: Cannot allocate scan list: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(list, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse scan list: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->scanlists()->add(list);

  return true;
}

bool
ConfigReader::linkScanLists(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->scanlists()->count()); it++, i++) {
    if (! linkScanList(config->scanlists()->scanlist(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkScanList(ScanList *list, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return ScanListReader().link(list, node, ctx);
}


bool
ConfigReader::parseContacts(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! node)
    return true;

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'contacts' element is not a sequence.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parseContact(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parseContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx)  {
  if ((! node.IsMap()) || (1 != node.size())) {
    _errorMessage = tr("%1:%2: Expected map with a single element.")
        .arg(node.Mark().line, node.Mark().column);
    return false;
  }

  std::string type = node.begin()->first.as<std::string>();
  if ("private" == type) {
    if (! parsePrivateCallContact(config, node[type], ctx))
      return false;
  } else if ("group" == type) {
    if (! parseGroupCallContact(config, node[type], ctx))
      return false;
  } else if ("all" == type) {
    if (! parseAllCallContact(config, node[type], ctx))
      return false;
  } else if ("dtmf" == type) {
    if (! parseDTMFContact(config, node[type], ctx))
      return false;
  } else {
    _errorMessage = tr("%1:%2: Uknown contact type '%3'")
        .arg(node.Mark().line).arg(node.Mark().column).arg(QString::fromStdString(type));
    return false;
  }

  return true;
}

bool
ConfigReader::parsePrivateCallContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  PrivateCallContactReader reader;
  DigitalContact *cont = reader.allocate(node, ctx)->as<DigitalContact>();
  if (nullptr == cont) {
    _errorMessage = tr("%1:%2: Cannot allocate private call contact: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(cont, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse private call contact: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->contacts()->add(cont);

  return true;
}

bool
ConfigReader::parseGroupCallContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  GroupCallContactReader reader;
  DigitalContact *cont = reader.allocate(node, ctx)->as<DigitalContact>();
  if (nullptr == cont) {
    _errorMessage = tr("%1:%2: Cannot allocate group call contact: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(cont, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse group call contact: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->contacts()->add(cont);

  return true;
}

bool
ConfigReader::parseAllCallContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  AllCallContactReader reader;
  DigitalContact *cont = reader.allocate(node, ctx)->as<DigitalContact>();
  if (nullptr == cont) {
    _errorMessage = tr("%1:%2: Cannot allocate all call contact: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(cont, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse all call contact: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->contacts()->add(cont);

  return true;
}

bool
ConfigReader::parseDTMFContact(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  _errorMessage = tr("%1:%2: DTMF contact reader not implemented yet.")
      .arg(node.Mark().line).arg(node.Mark().column);
  return false;
}

bool
ConfigReader::linkContacts(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->contacts()->count()); it++, i++) {
    if (! linkContact(config->contacts()->contact(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkContact(Contact *contact, const YAML::Node &node, const ConfigObject::Context &ctx) {
  std::string type = node.begin()->first.as<std::string>();
  if ("private" == type) {
    if (! linkPrivateCallContact(contact->as<DigitalContact>(), node[type], ctx))
      return false;
  } else if ("group" == type) {
    if (! linkGroupCallContact(contact->as<DigitalContact>(), node[type], ctx))
      return false;
  } else if ("all" == type) {
    if (! linkAllCallContact(contact->as<DigitalContact>(), node[type], ctx))
      return false;
  } else if ("dtmf" == type) {
    if (! linkDTMFContact(contact->as<DTMFContact>(), node[type], ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkPrivateCallContact(DigitalContact *contact, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return PrivateCallContactReader().link(contact, node, ctx);
}

bool
ConfigReader::linkGroupCallContact(DigitalContact *contact, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return GroupCallContactReader().link(contact, node, ctx);
}

bool
ConfigReader::linkAllCallContact(DigitalContact *contact, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return AllCallContactReader().link(contact, node, ctx);
}

bool
ConfigReader::linkDTMFContact(DTMFContact *contact, const YAML::Node &node, const ConfigObject::Context &ctx) {
  _errorMessage = tr("%1:%2: DTMF contact not implemented yet.");
  return false;
}


bool
ConfigReader::parseGroupLists(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! node) {
    _errorMessage = tr("No group lists defined.");
    return false;
  }

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'group-lists' element is not a sequence.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parseGroupList(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parseGroupList(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  GroupListReader reader;

  RXGroupList *list = reader.allocate(node, ctx)->as<RXGroupList>();
  if (nullptr == list) {
    _errorMessage = tr("%1:%2: Cannot allocate group list: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(list, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse group list: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->rxGroupLists()->add(list);

  return true;
}

bool
ConfigReader::linkGroupLists(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->rxGroupLists()->count()); it++, i++) {
    if (! linkGroupList(config->rxGroupLists()->list(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkGroupList(RXGroupList *list, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return GroupListReader().link(list, node, ctx);
}


bool
ConfigReader::parsePositioningSystems(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! node)
    return true;

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'positioning' element is not a sequence.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parsePositioningSystem(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parsePositioningSystem(Config *config, const YAML::Node &node, ConfigObject::Context &ctx)  {
  if ((! node.IsMap()) || (1 != node.size())) {
    _errorMessage = tr("%1:%2: Expected map with a single element.")
        .arg(node.Mark().line, node.Mark().column);
    return false;
  }

  std::string type = node.begin()->first.as<std::string>();
  if ("dmr" == type) {
    if (! parseGPSSystem(config, node[type], ctx))
      return false;
  } else if ("aprs" == type) {
    if (! parseAPRSSystem(config, node[type], ctx))
      return false;
  } else {
    _errorMessage = tr("%1:%2: Uknown positioning system type '%3'")
        .arg(node.Mark().line).arg(node.Mark().column).arg(QString::fromStdString(type));
    return false;
  }

  return true;
}

bool
ConfigReader::parseGPSSystem(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  GPSSystemReader reader;

  GPSSystem *sys = reader.allocate(node, ctx)->as<GPSSystem>();
  if (nullptr == sys) {
    _errorMessage = tr("%1:%2: Cannot allocate GPS system: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(sys, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse GPS system: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->posSystems()->add(sys);

  return true;
}

bool
ConfigReader::parseAPRSSystem(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  APRSSystemReader reader;

  APRSSystem *sys = reader.allocate(node, ctx)->as<APRSSystem>();
  if (nullptr == sys) {
    _errorMessage = tr("%1:%2: Cannot allocate APRS system: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(sys, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse APRS system: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->posSystems()->add(sys);

  return true;
}

bool
ConfigReader::linkPositioningSystems(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->posSystems()->count()); it++, i++) {
    if (! linkPositioningSystem(config->posSystems()->system(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkPositioningSystem(PositioningSystem *system, const YAML::Node &node, const ConfigObject::Context &ctx) {
  std::string type = node.begin()->first.as<std::string>();
  if ("dmr" == type) {
    if (! linkGPSSystem(system->as<GPSSystem>(), node[type], ctx))
      return false;
  } else if ("aprs" == type) {
    if (! linkAPRSSystem(system->as<APRSSystem>(), node[type], ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkGPSSystem(GPSSystem *system, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return GPSSystemReader().link(system, node, ctx);
}

bool
ConfigReader::linkAPRSSystem(APRSSystem *system, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return APRSSystemReader().link(system, node, ctx);
}


bool
ConfigReader::parseRoamingZones(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! node)
    return true;

  if (! node.IsSequence()) {
    _errorMessage = tr("%1:%2: 'roaming' element is not a sequence.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    if (! parseRoamingZone(config, *it, ctx))
      return false;
  }

  return true;
}

bool
ConfigReader::parseRoamingZone(Config *config, const YAML::Node &node, ConfigObject::Context &ctx) {
  RoamingReader reader;

  RoamingZone *zone = reader.allocate(node, ctx)->as<RoamingZone>();
  if (nullptr == zone) {
    _errorMessage = tr("%1:%2: Cannot allocate roaming zone: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  if (! reader.parse(zone, node, ctx)) {
    _errorMessage = tr("%1:%2: Cannot parse roaming zone: %3")
        .arg(node.Mark().line).arg(node.Mark().column).arg(reader.errorMessage());
    return false;
  }

  config->roaming()->add(zone);

  return true;
}

bool
ConfigReader::linkRoamingZones(Config *config, const YAML::Node &node, const ConfigObject::Context &ctx) {
  YAML::const_iterator it=node.begin();
  int i=0;
  for (; (it!=node.end()) && (i<config->roaming()->count()); it++, i++) {
    if (! linkRoamingZone(config->roaming()->zone(i), *it, ctx))
      return false;
  }
  return true;
}

bool
ConfigReader::linkRoamingZone(RoamingZone *zone, const YAML::Node &node, const ConfigObject::Context &ctx) {
  return RoamingReader().link(zone, node, ctx);
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
      _errorMessage = tr("Cannot parse object '%1': ID already used.").arg(id);
      return false;
    }
    logDebug() << "Register object " << obj << " as '" << id << "'.";
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

bool
RadioIdReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
}

ConfigObject *
RadioIdReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new RadioID("", 0);
}

bool
RadioIdReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  RadioID *rid = qobject_cast<RadioID *>(obj);

  if (! ObjectReader::parse(obj, node, ctx))
    return false;

  if (node["number"] && node["number"].IsScalar()) {
    rid->setId(node["number"].as<uint>());
  } else {
    _errorMessage = tr("%1:%2: Cannot parse radio id: No number defined.")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  if (node["name"] && node["name"].IsScalar()) {
    rid->setName(QString::fromStdString(node["name"].as<std::string>()));
  } else {
    _errorMessage = tr("%1:%2: Cannot parse radio id: No name defined.")
        .arg(node.Mark().line).arg(node.Mark().column);
  }

  if (! parseExtensions(_extensions, rid, node, ctx))
    return false;

  return true;
}

bool
RadioIdReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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
ChannelReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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
  } else {
    _errorMessage = "Cannot parse channel: No RX frequency set.";
    return false;
  }

  if (node["tx"] && node["tx-offset"]) {
    _errorMessage = tr("%1,%2: Both 'tx' and 'tx-offset' specified. Only one can be defined per channel!")
        .arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  channel->setTXFrequency(channel->rxFrequency());
  if (node["tx"] && node["tx"].IsScalar()) {
    channel->setTXFrequency(node["tx"].as<double>());
  } else if (node["tx-offset"] && node["tx-offset"].IsScalar()) {
    channel->setTXFrequency(channel->rxFrequency()+node["tx-offset"].as<double>());
  }

  channel->setPower(Channel::HighPower);
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
  }

  channel->setTimeout(0);
  if (node["tx-timeout"] && node["tx-timeout"].IsScalar()) {
    channel->setTimeout(node["tx-timeout"].as<uint>());
  }

  channel->setRXOnly(false);
  if (node["rx-only"] && node["rx-only"].IsScalar()) {
    channel->setTimeout(node["rx-only"].as<bool>());
  }

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
ChannelReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  Channel *channel = qobject_cast<Channel *>(obj);

  if (node["scan-list"] && node["scan-list"].IsScalar()) {
    QString sl = QString::fromStdString(node["scan-list"].as<std::string>());
    if ((! ctx.contains(sl)) || (! ctx.getObj(sl)->is<ScanList>())) {
      _errorMessage = tr("Cannot link channel '%1': Scan list with id='%2' unknown.")
          .arg(channel->name()).arg(sl);
      return false;
    }
  }

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
DigitalChannelReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
DigitalChannelReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  DigitalChannel *channel = qobject_cast<DigitalChannel *>(obj);

  if (! ChannelReader::link(obj, node, ctx))
    return false;

  if (node["group-list"] && node["group-list"].IsScalar()) {
    QString gl = QString::fromStdString(node["group-list"].as<std::string>());
    if ((! ctx.contains(gl)) || (! ctx.getObj(gl)->is<RXGroupList>())) {
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
    if ((! ctx.contains(c)) || (! ctx.getObj(c)->is<DigitalContact>())) {
      _errorMessage = tr("Cannot link digital channel '%1': TX contact with id='%2' is unknown.")
          .arg(channel->name()).arg(c);
      return false;
    }
    channel->setTXContact(ctx.getObj(c)->as<DigitalContact>());
  }

  if (node["aprs"] && node["aprs"].IsScalar()) {
    QString aprs = QString::fromStdString(node["aprs"].as<std::string>());
    if ((! ctx.contains(aprs)) || (! ctx.getObj(aprs)->is<PositioningSystem>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Positioning system with id='%2' is unknown.")
          .arg(channel->name()).arg(aprs);
      return false;
    }
    channel->setPosSystem(ctx.getObj(aprs)->as<PositioningSystem>());
  }

  if (node["roaming"] && node["roaming"].IsScalar()) {
    QString roaming = QString::fromStdString(node["roaming"].as<std::string>());
    if ((! ctx.contains(roaming)) || (! ctx.getObj(roaming)->is<RoamingZone>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Roaming zone with id='%2' is unknown.")
          .arg(channel->name()).arg(roaming);
      return false;
    }
    channel->setRoaming(ctx.getObj(roaming)->as<RoamingZone>());
  }

  if (node["radio-id"] && node["radio-id"].IsScalar()) {
    QString id = QString::fromStdString(node["dmr-id"].as<std::string>());
    if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<RadioID>())) {
      _errorMessage = tr("Cannot link digital channel '%1': Radio ID with id='%2' is unknown.")
          .arg(channel->name()).arg(id);
      return false;
    }
    channel->setRadioId(ctx.getObj(id)->as<RadioID>());
  }

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
AnalogChannelReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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

  channel->setSquelch(1);
  if (node["squelch"] && node["squelch"].IsScalar()) {
    channel->setSquelch(node["squelch"].as<int>());
  }

  channel->setTXTone(Signaling::SIGNALING_NONE);
  if (node["rx-tone"] && node["rx-tone"].IsMap()) {
    if (node["rx-tone"]["ctcss"] && node["rx-tone"]["ctcss"].IsScalar()) {
      channel->setRXTone(Signaling::fromCTCSSFrequency(node["rx-tone"]["ctcss"].as<double>()));
    } else if (node["rx-tone"]["dcs"] && node["rx-tone"]["dcs"].IsScalar()) {
      int code = node["rx-tone"]["dcs"].as<int>();
      bool inverted = (code < 0); code = std::abs(code);
      channel->setRXTone(Signaling::fromDCSNumber(code, inverted));
    }
  }

  channel->setTXTone(Signaling::SIGNALING_NONE);
  if (node["tx-tone"] && node["tx-tone"].IsMap()) {
    if (node["tx-tone"]["ctcss"] && node["tx-tone"]["ctcss"].IsScalar()) {
      channel->setTXTone(Signaling::fromCTCSSFrequency(node["tx-tone"]["ctcss"].as<double>()));
    } else if (node["tx-tone"]["dcs"] && node["tx-tone"]["dcs"].IsScalar()) {
      int code = node["tx-tone"]["dcs"].as<int>();
      bool inverted = (code < 0); code = std::abs(code);
      channel->setTXTone(Signaling::fromDCSNumber(code, inverted));
    }
  }

  channel->setBandwidth(AnalogChannel::BWNarrow);
  if (node["band-width"] && node["band-width"].IsScalar()) {
    QString bw = QString::fromStdString(node["band-width"].as<std::string>());
    if ("narrow" == bw) {
      channel->setBandwidth(AnalogChannel::BWNarrow);
    } else {
      channel->setBandwidth(AnalogChannel::BWWide);
    }
  }

  channel->setAdmit(AnalogChannel::AdmitNone);
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
  }

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
AnalogChannelReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  AnalogChannel *channel = qobject_cast<AnalogChannel *>(obj);

  if (! ChannelReader::link(obj, node, ctx))
    return false;

  if (node["aprs"] && node["aprs"].IsScalar()) {
    QString aprs = QString::fromStdString(node["aprs"].as<std::string>());
    if ((! ctx.contains(aprs)) || (! ctx.getObj(aprs)->is<APRSSystem>())) {
      _errorMessage = tr("Cannot link analog channel '%1': APRS system with id='%2' is unknown.")
          .arg(channel->name()).arg(aprs);
      return false;
    }
    channel->setAPRSSystem(ctx.getObj(aprs)->as<APRSSystem>());
  }

   if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
ZoneReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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
    _errorMessage = tr("No name defined");
    return false;
  }

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

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
      if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<Channel>())) {
        _errorMessage = _errorMessage = tr("Cannot link zone '%1' A: '%2' does not refer to a channel.")
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
        _errorMessage = tr("Cannot link zone '%1' B: Channel reference of wrong type. Must be id.")
            .arg(zone->name());
        return false;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<Channel>())) {
        _errorMessage = _errorMessage = tr("Cannot link zone '%1' B: '%2' does not refer to a channel.")
            .arg(zone->name()).arg(id);
        return false;
      }
      zone->B()->addChannel(ctx.getObj(id)->as<Channel>());
    }
  }

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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
ContactReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
ContactReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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
DigitalContactReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
}

bool
DigitalContactReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! ContactReader::parse(obj, node, ctx))
    return false;

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
DigitalContactReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  if (! ContactReader::link(obj, node, ctx))
    return false;

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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
PositioningReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
PositioningReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
GPSSystemReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
}

ConfigObject *
GPSSystemReader::allocate(const YAML::Node &node, const ConfigObject::Context &ctx) {
  return new GPSSystem("");
}

bool
GPSSystemReader::parse(ConfigObject *obj, const YAML::Node &node, ConfigObject::Context &ctx) {
  if (! PositioningReader::parse(obj, node, ctx))
    return false;

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
GPSSystemReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  GPSSystem *system = qobject_cast<GPSSystem *>(obj);

  if (! PositioningReader::link(obj, node, ctx))
    return false;

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

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
APRSSystemReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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
            QString::fromStdString(node["icon"].as<std::string>())));
  } else {
    _errorMessage = tr("Cannot parse APRS system '%1': No icon specified.").arg(system->name());
    return false;
  }

  if (node["message"] && node["message"].IsScalar()) {
    system->setMessage(QString::fromStdString(node["message"].as<std::string>()));
  }

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
APRSSystemReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  APRSSystem *system = qobject_cast<APRSSystem *>(obj);

  if (! PositioningReader::link(obj, node, ctx))
    return false;

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

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
ScanListReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

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
      if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<Channel>())) {
        logWarn() << "Cannot link priority channel " << (n+1) << ": Not a reference to a channel.";
        continue;
      }
      if (0 == n)
        list->setPriorityChannel(ctx.getObj(id)->as<Channel>());
      else
        list->setSecPriorityChannel(ctx.getObj(id)->as<Channel>());
    }
  }

  if (node["revert"] && node["revert"].IsScalar()) {
    QString id = QString::fromStdString(node["revert"].as<std::string>());
    if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<Channel>())) {
      _errorMessage = _errorMessage = tr("Cannot link scan-list '%1': '%2' does not refer to a channel.")
          .arg(list->name()).arg(id);
      return false;
    }
    list->setTXChannel(ctx.getObj(id)->as<Channel>());
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
      if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<Channel>())) {
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

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
GroupListReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}

bool
GroupListReader::link(ConfigObject *obj, const YAML::Node &node, const ConfigObject::Context &ctx) {
  RXGroupList *list = qobject_cast<RXGroupList *>(obj);

  // link group calls
  if (node["contacts"] && node["contacts"].IsSequence()) {
    for (YAML::const_iterator it=node["contacts"].begin(); it!=node["contacts"].end(); it++) {
      if (!it->IsScalar()) {
        _errorMessage = tr("Cannot link group list '%1': Contact reference of wrong type.")
            .arg(list->name());
        return false;
      }
      QString id = QString::fromStdString(it->as<std::string>());
      if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<DigitalContact>())) {
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

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

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

bool
RoamingReader::addExtension(const QString &name, AbstractConfigReader *reader) {
  if (_extensions.contains(name))
    return false;
  _extensions[name] = reader;
  return true;
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

  if (! parseExtensions(_extensions, obj, node, ctx))
    return false;

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
      if ((! ctx.contains(id)) || (! ctx.getObj(id)->is<DigitalChannel>())) {
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

  if (! linkExtensions(_extensions, obj, node, ctx))
    return false;

  return true;
}
