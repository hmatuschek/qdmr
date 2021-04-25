#include "configdeclaration.hh"
#include "logger.hh"


inline QString nodeTypeName(const YAML::Node &node) {
  QString type = QObject::tr("Null");
  switch (node.Type()) {
  case YAML::NodeType::Null: break;
  case YAML::NodeType::Undefined: type = QObject::tr("Undefined"); break;
  case YAML::NodeType::Scalar: type = QObject::tr("Scalar"); break;
  case YAML::NodeType::Sequence: type = QObject::tr("List"); break;
  case YAML::NodeType::Map: type = QObject::tr("Object"); break;
  }
  return type;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclItem
 * ********************************************************************************************* */
ConfigDeclItem::ConfigDeclItem(
    bool mandatory, const QString &desc, QObject *parent)
  : QObject(parent), _description(desc), _mandatory(mandatory)
{
  // pass...
}

ConfigDeclItem::~ConfigDeclItem() {
  // pass...
}

const QString &
ConfigDeclItem::description() const {
  return _description;
}

bool
ConfigDeclItem::isMandatory() const {
  return _mandatory;
}

bool
ConfigDeclItem::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclScalar
 * ********************************************************************************************* */
ConfigDeclScalar::ConfigDeclScalar(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclItem(mandatory, description, parent)
{
  // pass...
}

bool
ConfigDeclScalar::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsScalar()) {
    msg = tr("Expected scalar, got %1.").arg(nodeTypeName(node));
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclBool
 * ********************************************************************************************* */
ConfigDeclBool::ConfigDeclBool(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclScalar(mandatory, description, parent)
{
  // pass...
}

bool
ConfigDeclBool::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigDeclScalar::verifyForm(node, ctx, msg))
    return false;
  QString val = QString::fromStdString(node.as<std::string>());
  if (("true" != val) && ("false" != val)) {
    msg = tr("Expected boolean value ('true' or 'false'), got '%1'.").arg(val);
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclInt
 * ********************************************************************************************* */
ConfigDeclInt::ConfigDeclInt(
    qint64 min, qint64 max, bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclScalar(mandatory, description, parent), _min(min), _max(max)
{
  // pass...
}

qint64
ConfigDeclInt::mininum() const {
  return _min;
}

qint64
ConfigDeclInt::maximum() const {
  return _max;
}

bool
ConfigDeclInt::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigDeclScalar::verifyForm(node, ctx, msg))
    return false;
  qint64 val = node.as<qint64>();
  if ((val < _min) || (val>_max)) {
    msg = tr("Value must be in range [%1, %2], got %3.")
        .arg(_min).arg(_max).arg(val);
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclInt
 * ********************************************************************************************* */
ConfigDeclDMRId::ConfigDeclDMRId(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclInt(0, 16777215, mandatory, description, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclDouble
 * ********************************************************************************************* */
ConfigDeclFloat::ConfigDeclFloat(
    double min, double max, bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclScalar(mandatory, description, parent), _min(min), _max(max)
{
  // pass...
}

double
ConfigDeclFloat::mininum() const {
  return _min;
}

double
ConfigDeclFloat::maximum() const {
  return _max;
}

bool
ConfigDeclFloat::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigDeclScalar::verifyForm(node, ctx, msg))
    return false;
  double val = node.as<double>();
  if ((val < _min) || (val>_max)) {
    msg = tr("Value must be in range [%1, %2], got %3.")
        .arg(_min).arg(_max).arg(val);
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclStr
 * ********************************************************************************************* */
ConfigDeclStr::ConfigDeclStr(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclScalar(mandatory, description, parent)
{
  // pass...
}

bool
ConfigDeclStr::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigDeclScalar::verifyForm(node, ctx, msg))
    return false;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclId
 * ********************************************************************************************* */
ConfigDeclID::ConfigDeclID(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclStr(mandatory, description, parent)
{
  // pass...
}

bool
ConfigDeclID::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigDeclStr::verifyForm(node, ctx, msg))
    return false;
  QString value = QString::fromStdString(node.as<std::string>());
  QRegExp pattern("^[a-zA-Z_]+[a-zA-Z0-9_]*$");
  if (! pattern.exactMatch(value)) {
    msg = tr("Identifier '%1' does not match pattern '%2'.").arg(value).arg(pattern.pattern());
    return false;
  }
  if (ctx.contains(value)) {
    msg = tr("Identifier '%1' already defined.").arg(value);
    return false;
  }
  ctx.insert(value);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclRef
 * ********************************************************************************************* */
ConfigDeclRef::ConfigDeclRef(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclStr(mandatory, description, parent)
{
  // pass...
}

bool
ConfigDeclRef::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigDeclStr::verifyForm(node, ctx, msg))
    return false;
  QString value = QString::fromStdString(node.as<std::string>());
  QRegExp pattern("^[a-zA-Z_]+[a-zA-Z0-9_]*$");
  if (! pattern.exactMatch(value)) {
    msg = tr("Reference '%1' does not match pattern '%2'.").arg(value).arg(pattern.pattern());
    return false;
  }
  return true;
}

bool
ConfigDeclRef::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  QString value = QString::fromStdString(node.as<std::string>());
  if (! ctx.contains(value)) {
    msg = tr("Reference '%1' is not defined.").arg(value);
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclEnum
 * ********************************************************************************************* */
ConfigDeclEnum::ConfigDeclEnum(
    const QSet<QString> &values, bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclScalar(mandatory, description, parent), _values(values)
{
  // pass...
}

const QSet<QString> &
ConfigDeclEnum::possibleValues() const {
  return _values;
}

bool
ConfigDeclEnum::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigDeclScalar::verifyForm(node, ctx, msg))
    return false;
  QString value = QString::fromStdString(node.as<std::string>());
  if (! _values.contains(value)) {
    QStringList pval;
    foreach (QString val, _values)
      pval.append(val);
    msg = tr("Expected one of (%1), got %2.")
        .arg(pval.join(", ")).arg(value);
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclDispatch
 * ********************************************************************************************* */
ConfigDeclDispatch::ConfigDeclDispatch(
    const QHash<QString, ConfigDeclItem *> &elements, bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclItem(mandatory, description, parent), _elements()
{
  for (QHash<QString, ConfigDeclItem*>::const_iterator it=elements.begin(); it!=elements.end(); it++) {
    add(it.key(), it.value());
  }
}

bool
ConfigDeclDispatch::add(const QString &name, ConfigDeclItem *element) {
  if (nullptr == element)
    return false;
  if (_elements.contains(name))
    return false;
  element->setParent(this);
  _elements[name] = element;
  return true;
}

bool
ConfigDeclDispatch::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsMap()) {
    msg = tr("Expected dispatch, got %1.").arg(nodeTypeName(node));
    return false;
  }

  if (1 != node.size()) {
    msg = tr("Dispatch requires exactly one element.");
    return false;
  }
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  if (! _elements.contains(ename)) {
    msg = tr("Unkown element '%1'.").arg(ename);
    return false;
  }

  if (! _elements[ename]->verifyForm(it->second, ctx, msg)) {
    msg = tr("Element '%1': %2").arg(ename).arg(msg);
    return false;
  }

  return true;
}

bool
ConfigDeclDispatch::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  if (! _elements[ename]->verifyReferences(it->second, ctx, msg)) {
    msg = tr("Element '%1': %2").arg(ename).arg(msg);
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclObj
 * ********************************************************************************************* */
ConfigDeclObj::ConfigDeclObj(
    const QHash<QString, ConfigDeclItem *> &elements, bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclItem(mandatory, description, parent), _elements()
{
  for (QHash<QString, ConfigDeclItem*>::const_iterator it=elements.begin(); it!=elements.end(); it++) {
    add(it.key(), it.value());
  }
}

bool
ConfigDeclObj::add(const QString &name, ConfigDeclItem *element) {
  if (nullptr == element)
    return false;
  if (_elements.contains(name))
    return false;
  element->setParent(this);
  if (element->isMandatory())
    _mandatoryElements.insert(name);
  _elements[name] = element;
  return true;
}

bool
ConfigDeclObj::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsMap()) {
    msg = tr("Expected object, got %1.").arg(nodeTypeName(node));
    return false;
  }

  QSet<QString> found;
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if (! _elements.contains(ename)) {
      logWarn() << tr("Unkown element '%1'.").arg(ename);
      continue;
    }
    if (! _elements[ename]->verifyForm(it->second, ctx, msg)) {
      msg = tr("Element '%1': %2").arg(ename).arg(msg);
      return false;
    }
    found.insert(ename);
  }

  QSet<QString> missing = _mandatoryElements-found;
  if (! missing.empty()) {
    QStringList mis;
    foreach (QString el, missing)
      mis.append(QString("'%1'").arg(el));
    msg = tr("Mandatory element(s) %1 are missing.").arg(mis.join(", "));
    return false;
  }
  return true;
}

bool
ConfigDeclObj::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if (! _elements.contains(ename))
      continue;
    if (! _elements[ename]->verifyReferences(it->second, ctx, msg)) {
      msg = tr("Element '%1': %2").arg(ename).arg(msg);
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclList
 * ********************************************************************************************* */
ConfigDeclList::ConfigDeclList(ConfigDeclItem *element, const QString &description, QObject *parent)
  : ConfigDeclItem(element->isMandatory(), description, parent), _element(element)
{
  _element->setParent(this);
}

bool
ConfigDeclList::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsSequence()) {
    msg = tr("Expected list, got %2.").arg(nodeTypeName(node));
    return false;
  }

  if ((0 == node.size()) && isMandatory()) {
    msg = tr("List cannot be empty!");
    return false;
  }

  for (size_t i=0; i<node.size(); i++) {
    if (! _element->verifyForm(node[i], ctx, msg)) {
      msg = tr("List element %1: %2").arg(i).arg(msg);
      return false;
    }
  }
  return true;
}

bool
ConfigDeclList::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  for (size_t i=0; i<node.size(); i++) {
    if (! _element->verifyReferences(node[i], ctx, msg)) {
      msg = tr("List element %1: %2").arg(i).arg(msg);
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclContact
 * ********************************************************************************************* */
ConfigDeclContact::ConfigDeclContact(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclObj(QHash<QString, ConfigDeclItem*>(), mandatory, description, parent)
{
  add("id", new ConfigDeclID(true, tr("Specifies the id of the contact for referencing it within the codeplug.")));
  add("name", new ConfigDeclStr(true, tr("Specifies the name of the contact.")));
  add("ring", new ConfigDeclBool(false, tr("Enables ring tone (optional).")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigDeclPrivateCall
 * ********************************************************************************************* */
ConfigDeclPrivateCall::ConfigDeclPrivateCall(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclContact(mandatory, description, parent)
{
  add("number", new ConfigDeclDMRId(true, tr("Specifies the DMR ID of the private call.")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigDeclGroupCall
 * ********************************************************************************************* */
ConfigDeclGroupCall::ConfigDeclGroupCall(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclContact(mandatory, description, parent)
{
  add("number", new ConfigDeclDMRId(true, tr("Specifies the DMR ID of the private call.")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigDeclAllCall
 * ********************************************************************************************* */
ConfigDeclAllCall::ConfigDeclAllCall(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclContact(mandatory, description, parent)
{
  // pass, no number of all call
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclChannel
 * ********************************************************************************************* */
ConfigDeclChannel::ConfigDeclChannel(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclObj(QHash<QString, ConfigDeclItem*>(), mandatory, description, parent)
{
  add("id", new ConfigDeclID(true, tr("Specifies the id of the channel for referencing it within the codeplug.")));
  add("name", new ConfigDeclStr(true, tr("Specifies the name of the channel.")));
  add("rx", new ConfigDeclFloat(0, 10e3, true, tr("Specifies the RX frequency of the channel in MHz.")));
  add("tx", new ConfigDeclFloat(-10e3, 10e3, false, tr("Specifies the TX frequency of the channel or offset in MHz.")));
  add("power", new ConfigDeclEnum({"min", "low", "mid", "high", "max"}, true,
                                  tr("Specifies the transmit power on the channel.")));
  add("tx-timeout", new ConfigDeclInt(
        0, 10000, false, tr("Specifies the transmit timeout in seconds. None if 0 or omitted.")));
  add("rx-only", new ConfigDeclBool(
        false, tr("If true, TX is disabled for this channel.")));
  add("scan-list", new ConfigDeclRef(false, tr("References the scanlist associated with this channel.")));
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclAnalogChannel
 * ********************************************************************************************* */
ConfigDeclAnalogChannel::ConfigDeclAnalogChannel(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclChannel(mandatory, description, parent)
{
  add("admit", new ConfigDeclEnum({"always", "free", "tone"},
                                  false, tr("Specifies the transmit admit criterion.")));
  add("squelch", new ConfigDeclInt(1,10, true, tr("Specifies the squelch level.")));
  add("rx-tone", new ConfigDeclDispatch(
  { {"ctcss", new ConfigDeclFloat(0,300, true, tr("Specifies the CTCSS frequency."))},
    {"dcs", new ConfigDeclInt(-300,300, true, tr("Specifies the DCS code."))} },
        false, tr("Specifies the DCS/CTCSS signaling for RX.")));
  add("tx-tone", new ConfigDeclDispatch(
  { {"ctcss", new ConfigDeclFloat(0,300, true, tr("Specifies the CTCSS frequency."))},
    {"dcs", new ConfigDeclInt(-300,300, true, tr("Specifies the DCS code."))} },
        false, tr("Specifies the DCS/CTCSS signaling for TX.")));
  add("band-width", new ConfigDeclEnum({"narrow","wide"}, true, tr("Specifies the bandwidth of the channel.")));
  add("aprs", new ConfigDeclRef(false, tr("References the APRS system used by this channel.")));
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclDigitalChannel
 * ********************************************************************************************* */
ConfigDeclDigitalChannel::ConfigDeclDigitalChannel(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclChannel(mandatory, description, parent)
{
  add("admit", new ConfigDeclEnum({"always", "free", "color-code"},
                                  false, tr("Specifies the transmit admit criterion.")));
  add("color-code", new ConfigDeclInt(1,16, true, tr("Specifies the color-code of the channel.")));
  add("time-slot", new ConfigDeclInt(1,2, true, tr("Specifies the time-slot of the channel.")));
  add("group-list", new ConfigDeclRef(true, tr("References the RX group list of the channel.")));
  add("tx-contact", new ConfigDeclRef(false, tr("References the default TX contact of the channel.")));
  add("aprs", new ConfigDeclRef(false, tr("References the positioning system used by this channel.")));
  add("roaming-zone", new ConfigDeclRef(false, tr("References the roaming zone used by this channel.")));
  add("dmr-id", new ConfigDeclRef(false, tr("Specifies the DMR ID to use on this channel.")));
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclPositioning
 * ********************************************************************************************* */
ConfigDeclPositioning::ConfigDeclPositioning(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclObj(QHash<QString, ConfigDeclItem *>(), mandatory, description, parent)
{
  add("name", new ConfigDeclStr(true, tr("Specifies the name of the positioning system.")));
  add("id", new ConfigDeclID(true, tr("Specifies the id of the positioning system for reference within the codeplug.")));
  add("period", new ConfigDeclInt(1, 1000, true, tr("Specifies the update period in seconds.")));
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclDMRPos
 * ********************************************************************************************* */
ConfigDeclDMRPos::ConfigDeclDMRPos(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclPositioning(mandatory, description, parent)
{
  add("destination", new ConfigDeclRef(true, tr("Specifies the destination contact.")));
  add("channel", new ConfigDeclRef(false, tr("Specifies the optional revert channel.")));
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclAPRSPos
 * ********************************************************************************************* */
ConfigDeclAPRSPos::ConfigDeclAPRSPos(bool mandatory, const QString &description, QObject *parent)
  : ConfigDeclPositioning(mandatory, description, parent)
{
  add("source", new ConfigDeclStr(true, tr("Specifies the source call and SSID.")));
  add("destination", new ConfigDeclStr(true, tr("Specifies the destination call and SSID.")));
  add("channel", new ConfigDeclRef(true, tr("Specifies the optional revert channel.")));
  add("path", new ConfigDeclList(
        new ConfigDeclStr(false, tr("Specifies a path element of the APRS packet.")),
        tr("Specifies the APRS path as a list.")));
  add("icon", new ConfigDeclStr(true, tr("Specifies the icon name.")));
  add("message", new ConfigDeclStr(false, tr("Specifies the optional APRS message.")));
}



/* ********************************************************************************************* *
 * Implementation of ConfigDeclaration
 * ********************************************************************************************* */
ConfigDeclaration::ConfigDeclaration(QObject *parent)
  : ConfigDeclObj(QHash<QString, ConfigDeclItem *>(), true, "", parent)
{
  add("version", new ConfigDeclStr(false, tr("Specifies the optional version number of the config format.")));
  add("intro-line1", new ConfigDeclStr(false, tr("Specifies the optional first boot display line.")));
  add("intro-line2", new ConfigDeclStr(false, tr("Specifies the optional second boot display line.")));

  add("radio-ids", new ConfigDeclList(
        new ConfigDeclObj({ {"id", new ConfigDeclID(true, tr("Specifies the id of the radio ID for referencing it within the codeplug."))},
                            {"name", new ConfigDeclStr(true, tr("Specifies the name of the contact."))},
                            {"number", new ConfigDeclDMRId(true, tr("Specifies the DMR ID of the contact."))}},
                          true, tr("Specifies a radio ID, that is a pair of DMR ID and name for the radio.")),
        tr("This list specifies the DMR IDs and names for the radio.")));

  add("channels",
      new ConfigDeclList(
        new ConfigDeclDispatch({ {"analog", new ConfigDeclAnalogChannel(false, tr("Specifies an analog channel.")) },
                                 {"digital", new ConfigDeclDigitalChannel(false, tr("Specifies a digial channel.")) }},
                               true),
        tr("The list of all channels within the codeplug.")));

  add("zones",
      new ConfigDeclList(
        new ConfigDeclObj({ {"id", new ConfigDeclID(false, tr("Specifies the id of the zone for referencing it within the codeplug."))},
                            {"name", new ConfigDeclStr(true, tr("Specifies the name of the zone."))},
                            {"A", new ConfigDeclList(new ConfigDeclRef(false, tr("Channel references.")))},
                            {"B", new ConfigDeclList(new ConfigDeclRef(false, tr("Channel references.")))}},
                          true, tr("Defines a zone within the codeplug.")),
        tr("Defines the list of zones.")));

  add("scan-lists", new ConfigDeclList(
        new ConfigDeclObj({ {"id", new ConfigDeclID(true, tr("Specifies the id of the scan list for referencing it within the codeplug."))},
                            {"name", new ConfigDeclStr(true, tr("Name of the scan list."))},
                            {"pimary", new ConfigDeclRef(false, tr("Reference to the primary priority channel."))},
                            {"secondary", new ConfigDeclRef(false, tr("Reference to the secondary priority channel."))},
                            {"revert", new ConfigDeclRef(false, tr("Reference to the revert (transmit) channel."))},
                            {"channels", new ConfigDeclList(
                             new ConfigDeclRef(false, tr("Reference to a channel.")), tr("List of channels in scan list."))}},
                          false, tr("Defines a scan list")),
        tr("Defines the list of all scan lists.")));

  add("contacts",
      new ConfigDeclList(
        new ConfigDeclDispatch({ {"private", new ConfigDeclPrivateCall(true, tr("Specifies a private call."))},
                                 {"group", new ConfigDeclGroupCall(true, tr("Specifies a group call."))},
                                 {"all", new ConfigDeclAllCall(true, tr("Specifies an all call."))} },
                               true, tr("One of the contact types, 'private', 'group', 'all'.")),
        tr("Specifies the list of contacts.")));

  add("group-lists",
      new ConfigDeclList(
        new ConfigDeclObj({ {"id", new ConfigDeclID(true, tr("Specifies the id of the group list for referencing it within the codeplug."))},
                            {"name", new ConfigDeclStr(true, tr("Specifies the name of the contact."))},
                            {"members", new ConfigDeclList(
                             new ConfigDeclRef(true, tr("A reference to one of the group calls.")),
                             tr("The list of group calls forming the group list."))} },
                          true, tr("Defines a RX group list.")),
        tr("Lists all RX group lists within the codeplug.")));

  add("positioning",
      new ConfigDeclList(
        new ConfigDeclDispatch({ {"dmr", new ConfigDeclDMRPos(true, tr("Specifies a DMR positioning system."))},
                                 {"aprs", new ConfigDeclAPRSPos(true, tr("Specifies an APRS positioning system."))} },
                               false),
        tr("List of all positioning systems.")));

}

bool
ConfigDeclaration::verify(const YAML::Node &doc, QString &message) {
  QSet<QString> ctx;
  if (! this->verifyForm(doc, ctx, message))
    return false;
  if (! this->verifyReferences(doc, ctx, message))
    return false;
  return true;
}
