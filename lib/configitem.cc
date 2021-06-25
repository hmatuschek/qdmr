#include "configitem.hh"
#include <logger.hh>

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
 * Implementation of ConfigItem::Declaration
 * ********************************************************************************************* */
ConfigItem::Declaration::Declaration(bool mandatory, const QString &desc)
  : _description(desc), _mandatory(mandatory)
{
  // pass...
}

ConfigItem::Declaration::~Declaration() {
  // pass...
}

const QString &
ConfigItem::Declaration::description() const {
  return _description;
}

bool
ConfigItem::Declaration::isMandatory() const {
  return _mandatory;
}

bool
ConfigItem::Declaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  return true;
}

ConfigItem *
ConfigItem::Declaration::parseDefine(YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  ConfigItem *item = parseAllocate(node, msg);
  if (nullptr == item)
    return nullptr;
  parsePopulate(item, node, ctx, msg);
  return item;
}

bool
ConfigItem::Declaration::parseLink(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItem
 * ********************************************************************************************* */
ConfigItem::ConfigItem(const Declaration *declaration, QObject *parent)
  : QObject(parent), _declaration(declaration)
{
  // pass...
}

ConfigItem::~ConfigItem() {
  // pass...
}

const ConfigItem::Declaration *
ConfigItem::declaraion() const {
  return _declaration;
}


/* ********************************************************************************************* *
 * Implementation of ConfigScalarItem::Declaration
 * ********************************************************************************************* */
ConfigScalarItem::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigItem::Declaration(mandatory, description)
{
  // pass...
}

bool
ConfigScalarItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsScalar()) {
    msg = tr("Expected scalar, got %1.").arg(nodeTypeName(node));
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemScalar
 * ********************************************************************************************* */
ConfigScalarItem::ConfigScalarItem(const Declaration *declaration, QObject *parent)
  : ConfigItem(declaration, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemBool::Declaration
 * ********************************************************************************************* */
ConfigBoolItem::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigScalarItem::Declaration(mandatory, description)
{
  // pass...
}

bool
ConfigBoolItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  QString val = QString::fromStdString(node.as<std::string>());
  if (("true" != val) && ("false" != val)) {
    msg = tr("Expected boolean value ('true' or 'false'), got '%1'.").arg(val);
    return false;
  }
  return true;
}

ConfigItem *
ConfigBoolItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigBoolItem(false, this);
}

bool
ConfigBoolItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  QString val = QString::fromStdString(node.as<std::string>());
  item->as<ConfigBoolItem>()->setValue((val=="true") ? true : false);
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemBool
 * ********************************************************************************************* */
ConfigBoolItem::ConfigBoolItem(bool value, const Declaration *declaration, QObject *parent)
  : ConfigScalarItem(declaration, parent), _value(value)
{
  // pass...
}

bool
ConfigBoolItem::value() const {
  return _value;
}

void
ConfigBoolItem::setValue(bool val) {
  _value = val;
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemInt::Declaration
 * ********************************************************************************************* */
ConfigIntItem::Declaration::Declaration(
    qint64 min, qint64 max, bool mandatory, const QString &description)
  : ConfigScalarItem::Declaration(mandatory, description), _min(min), _max(max)
{
  // pass...
}

qint64
ConfigIntItem::Declaration::mininum() const {
  return _min;
}

qint64
ConfigIntItem::Declaration::maximum() const {
  return _max;
}

bool
ConfigIntItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  qint64 val = node.as<qint64>();
  if ((val < _min) || (val>_max)) {
    msg = tr("Value must be in range [%1, %2], got %3.")
        .arg(_min).arg(_max).arg(val);
    return false;
  }
  return true;
}

ConfigItem *
ConfigIntItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigIntItem(0, this);
}
bool
ConfigIntItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  qint64 val = node.as<qint64>();
  item->as<ConfigIntItem>()->setValue(val);
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemInt
 * ********************************************************************************************* */
ConfigIntItem::ConfigIntItem(qint64 value, const Declaration *declaration, QObject *parent)
  : ConfigScalarItem(declaration, parent), _value(0)
{
  setValue(value);
}

qint64
ConfigIntItem::value() const {
  return _value;
}

void
ConfigIntItem::setValue(qint64 value) {
  qint64 min = declaraion()->as<ConfigIntItem::Declaration>()->mininum(),
      max = declaraion()->as<ConfigIntItem::Declaration>()->maximum();
  _value = std::max(min, std::min(max, value));
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemDMRId::Declaration
 * ********************************************************************************************* */
ConfigDMRIdItem::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigIntItem::Declaration(0, 16777215, mandatory, description)
{
  // pass...
}

ConfigItem *
ConfigDMRIdItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigDMRIdItem(0, this);
}

bool
ConfigDMRIdItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  item->as<ConfigDMRIdItem>()->setValue(node.as<quint32>());
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemDMRId
 * ********************************************************************************************* */
ConfigDMRIdItem::ConfigDMRIdItem(uint32_t value, const Declaration *declaration, QObject *parent)
  : ConfigIntItem(value, declaration, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemFloat::Declaration
 * ********************************************************************************************* */
ConfigFloatItem::Declaration::Declaration(
    double min, double max, bool mandatory, const QString &description)
  : ConfigScalarItem::Declaration(mandatory, description), _min(min), _max(max)
{
  // pass...
}

double
ConfigFloatItem::Declaration::mininum() const {
  return _min;
}

double
ConfigFloatItem::Declaration::maximum() const {
  return _max;
}

bool
ConfigFloatItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  double val = node.as<double>();
  if ((val < _min) || (val>_max)) {
    msg = tr("Value must be in range [%1, %2], got %3.")
        .arg(_min).arg(_max).arg(val);
    return false;
  }
  return true;
}

ConfigItem *
ConfigFloatItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigFloatItem(0, this);
}
bool
ConfigFloatItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  item->as<ConfigFloatItem>()->setValue(node.as<double>());
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemFloat
 * ********************************************************************************************* */
ConfigFloatItem::ConfigFloatItem(double value, const Declaration *declaration, QObject *parent)
  : ConfigScalarItem(declaration, parent), _value(value)
{
  // pass...
}

double
ConfigFloatItem::value() const {
  return _value;
}
void
ConfigFloatItem::setValue(double value) {
  double min = declaraion()->as<ConfigFloatItem::Declaration>()->mininum(),
      max = declaraion()->as<ConfigFloatItem::Declaration>()->maximum();
  _value = std::max(min, std::min(max, value));
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemStr::Declaration
 * ********************************************************************************************* */
ConfigStrItem::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigScalarItem::Declaration(mandatory, description)
{
  // pass...
}

bool
ConfigStrItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  return true;
}

ConfigItem *
ConfigStrItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigStrItem("", this);
}
bool
ConfigStrItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem*> &ctx, QString &msg) const {
  item->as<ConfigStrItem>()->setValue(QString::fromStdString(node.as<std::string>()));
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemString
 * ********************************************************************************************* */
ConfigStrItem::ConfigStrItem(const QString &value, const Declaration *declaration, QObject *parent)
  : ConfigScalarItem(declaration, parent), _value(value)
{
  // pass...
}

const QString &
ConfigStrItem::value() const {
  return _value;
}

void
ConfigStrItem::setValue(const QString &value) {
  _value = value;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclId
 * ********************************************************************************************* */
ConfigIdDeclaration::ConfigIdDeclaration(bool mandatory, const QString &description)
  : ConfigStrItem::Declaration(mandatory, description)
{
  // pass...
}

bool
ConfigIdDeclaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigStrItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  QString value = QString::fromStdString(node.as<std::string>());
  QRegExp pattern("^[a-zA-Z_]+[a-zA-Z0-9_]*$");
  if (! pattern.exactMatch(value)) {
    msg = QString("Identifier '%1' does not match pattern '%2'.").arg(value).arg(pattern.pattern());
    return false;
  }
  if (ctx.contains(value)) {
    msg = QString("Identifier '%1' already defined.").arg(value);
    return false;
  }
  ctx.insert(value);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclRef
 * ********************************************************************************************* */
ConfigReferenceDeclaration::ConfigReferenceDeclaration(bool mandatory, const QString &description)
  : ConfigStrItem::Declaration(mandatory, description)
{
  // pass...
}

bool
ConfigReferenceDeclaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigStrItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  QString value = QString::fromStdString(node.as<std::string>());
  QRegExp pattern("^[a-zA-Z_]+[a-zA-Z0-9_]*$");
  if (! pattern.exactMatch(value)) {
    msg = QString("Reference '%1' does not match pattern '%2'.").arg(value).arg(pattern.pattern());
    return false;
  }
  return true;
}

bool
ConfigReferenceDeclaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  QString value = QString::fromStdString(node.as<std::string>());
  if (! ctx.contains(value)) {
    msg = QString("Reference '%1' is not defined.").arg(value);
    return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemEnum::Declaration
 * ********************************************************************************************* */
ConfigEnumItem::Declaration::Declaration(
    const QSet<QString> &values, bool mandatory, const QString &description)
  : ConfigScalarItem::Declaration(mandatory, description), _values(values)
{
  // pass...
}

const QSet<QString> &
ConfigEnumItem::Declaration::possibleValues() const {
  return _values;
}

bool
ConfigEnumItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ConfigScalarItem::Declaration::verifyForm(node, ctx, msg))
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

ConfigItem *
ConfigEnumItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigEnumItem("", this);
}
bool
ConfigEnumItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  return item->as<ConfigEnumItem>()->setValue(
        QString::fromStdString(node.as<std::string>()));
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemEnum
 * ********************************************************************************************* */
ConfigEnumItem::ConfigEnumItem(const QString &value, const Declaration *declaration, QObject *parent)
  : ConfigScalarItem(declaration, parent), _value(value)
{
  // pass...
}

const QString &
ConfigEnumItem::value() const {
  return _value;
}

bool
ConfigEnumItem::setValue(const QString &key) {
  if (! declaraion()->as<ConfigEnumItem::Declaration>()->possibleValues().contains(key))
    return false;
  _value = key;
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigDeclDispatch
 * ********************************************************************************************* */
ConfigDispatchDeclaration::ConfigDispatchDeclaration(
    const QHash<QString, ConfigItem::Declaration *> &elements, bool mandatory, const QString &description)
  : ConfigItem::Declaration(mandatory, description), _elements()
{
  for (QHash<QString, ConfigItem::Declaration *>::const_iterator it=elements.begin(); it!=elements.end(); it++) {
    add(it.key(), it.value());
  }
}

bool
ConfigDispatchDeclaration::add(const QString &name, Declaration *element) {
  if (nullptr == element)
    return false;
  if (_elements.contains(name))
    return false;
  _elements[name] = element;
  return true;
}

bool
ConfigDispatchDeclaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsMap()) {
    msg = QString("Expected dispatch, got %1.").arg(nodeTypeName(node));
    return false;
  }

  if (1 != node.size()) {
    msg = QString("Dispatch requires exactly one element.");
    return false;
  }
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  if (! _elements.contains(ename)) {
    msg = QString("Unkown element '%1'.").arg(ename);
    return false;
  }

  if (! _elements[ename]->verifyForm(it->second, ctx, msg)) {
    msg = QString("Element '%1': %2").arg(ename).arg(msg);
    return false;
  }

  return true;
}

bool
ConfigDispatchDeclaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  if (! _elements[ename]->verifyReferences(it->second, ctx, msg)) {
    msg = QString("Element '%1': %2").arg(ename).arg(msg);
    return false;
  }

  return true;
}

ConfigItem *
ConfigDispatchDeclaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return nullptr;
}
bool
ConfigDispatchDeclaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  return false;
}

ConfigItem *
ConfigDispatchDeclaration::parseDefine(YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  return _elements[ename]->parseDefine(node, ctx, msg);
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemMap::Declaration
 * ********************************************************************************************* */
ConfigMapItem::Declaration::Declaration(
    const QHash<QString, ConfigItem::Declaration *> &elements, bool mandatory, const QString &description)
  : ConfigItem::Declaration(mandatory, description), _elements()
{
  for (QHash<QString, ConfigItem::Declaration*>::const_iterator it=elements.begin(); it!=elements.end(); it++) {
    add(it.key(), it.value());
  }
}

bool
ConfigMapItem::Declaration::add(const QString &name, ConfigItem::Declaration *element) {
  if (nullptr == element)
    return false;
  if (_elements.contains(name))
    return false;
  if (element->isMandatory())
    _mandatoryElements.insert(name);
  _elements[name] = element;
  return true;
}

bool
ConfigMapItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsMap()) {
    msg = tr("Expected object, got %1.").arg(nodeTypeName(node));
    return false;
  }

  QSet<QString> found;
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if (! _elements.contains(ename)) {
      logWarn() << QString("Unkown element '%1'.").arg(ename);
      continue;
    }
    if (! _elements[ename]->verifyForm(it->second, ctx, msg)) {
      msg = QString("Element '%1': %2").arg(ename).arg(msg);
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
ConfigMapItem::Declaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
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

ConfigItem *
ConfigMapItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigMapItem(this);
}
bool
ConfigMapItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    ConfigItem *citem = _elements[ename]->parseDefine(node, ctx, msg);
    if (nullptr == citem)
      return false;
    if (! item->as<ConfigMapItem>()->add(ename, citem)) {
      msg = tr("Cannot add item '%1' to map.").arg(ename);
      return false;
    }
  }
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemMap
 * ********************************************************************************************* */
ConfigMapItem::ConfigMapItem(const Declaration *declaration, QObject *parent)
  : ConfigItem(declaration, parent), _elements()
{
  // pass...
}

bool
ConfigMapItem::has(const QString &name) const {
  return _elements.contains(name);
}

bool
ConfigMapItem::add(const QString &name, ConfigItem *element) {
  if (_elements.contains(name))
    return false;
  set(name, element);
  return true;
}

void
ConfigMapItem::set(const QString &name, ConfigItem *element) {
  if (_elements.contains(name) && _elements[name]) {
    disconnect(_elements[name], SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject *)));
  }
  _elements[name] = element;
  if (element) {
    connect(element, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject *)));
    element->setParent(this);
  }
}

ConfigItem *
ConfigMapItem::get(const QString &name) const {
  return _elements.value(name, nullptr);
}

ConfigItem *
ConfigMapItem::take(const QString &name) {
  if (! _elements.contains(name))
    return nullptr;
  ConfigItem *element = _elements.take(name);
  if (element) {
    disconnect(element, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
  }
  return element;
}

bool
ConfigMapItem::del(const QString &name) {
  if (! _elements.contains(name))
    return false;
  ConfigItem *element = take(name);
  if (element)
    element->deleteLater();
  return true;
}

void
ConfigMapItem::onElementDeleted(QObject *element) {
  foreach (const QString &key, _elements.keys()) {
    if (element == _elements[key]) {
      _elements.remove(key);
      return;
    }
  }
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemObj::Declaration
 * ********************************************************************************************* */
ConfigObjItem::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigMapItem::Declaration(QHash<QString, ConfigItem::Declaration *>(), mandatory, description)
{
  add("id", new ConfigIdDeclaration(true, QString("Specifies the ID of the object.")));
}

ConfigItem *
ConfigObjItem::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigObjItem(this);
}

bool
ConfigObjItem::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if ("id" == ename)
      continue;
    ConfigItem *citem = _elements[ename]->parseDefine(node, ctx, msg);
    if (nullptr == citem)
      return false;
    if (! item->as<ConfigMapItem>()->add(ename, citem)) {
      msg = tr("Cannot add item '%1' to map.").arg(ename);
      return false;
    }
  }
  return true;
}

ConfigItem *
ConfigObjItem::Declaration::parseDefine(YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  ConfigItem *item = parseAllocate(node, msg);
  if (nullptr == item)
    return nullptr;
  if (! parsePopulate(item, node, ctx, msg)) {
    item->deleteLater();
    return nullptr;
  }
  QString id = QString::fromStdString(node["id"].as<std::string>());
  ctx.insert(id, item);
  return item;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemObj
 * ********************************************************************************************* */
ConfigObjItem::ConfigObjItem(const Declaration *declaration, QObject *parent)
  : ConfigMapItem(declaration, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemList::Declaration
 * ********************************************************************************************* */
ConfigList::Declaration::Declaration(ConfigItem::Declaration *element, const QString &description)
  : ConfigItem::Declaration(element->isMandatory(), description), _element(element)
{
  // pass...
}

bool
ConfigList::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
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
ConfigList::Declaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  for (size_t i=0; i<node.size(); i++) {
    if (! _element->verifyReferences(node[i], ctx, msg)) {
      msg = tr("List element %1: %2").arg(i).arg(msg);
      return false;
    }
  }
  return true;
}

ConfigItem *
ConfigList::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigList(this);
}

bool
ConfigList::Declaration::parsePopulate(ConfigItem *item, YAML::Node &node, QHash<QString, ConfigItem *> &ctx, QString &msg) const {
  for (size_t i=0; i<node.size(); i++) {
    ConfigItem *element = _element->parseDefine(node, ctx, msg);
    if (nullptr == element)
      return false;
    item->as<ConfigList>()->add(element);
  }
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemList
 * ********************************************************************************************* */
ConfigList::ConfigList(const Declaration *declaration, QObject *parent)
  : ConfigItem(declaration, parent), _items()
{
  // pass...
}

int
ConfigList::count() const {
  return _items.count();
}

void
ConfigList::add(ConfigItem *item) {
  if (nullptr == item)
    return;
  _items.append(item);
  item->setParent(this);
  connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(onItemDeleted(QObject*)));
}

ConfigItem *
ConfigList::get(int i) const {
  if (i >= count())
    return nullptr;
  return _items[i];
}

ConfigItem *
ConfigList::take(int i) {
  if (i >= count())
    return nullptr;
  ConfigItem *item = _items.takeAt(i);
  disconnect(item, SIGNAL(destroyed(QObject*)), this, SLOT(onItemDeleted(QObject*)));
  return item;
}

bool
ConfigList::del(int i) {
  ConfigItem *item = take(i);
  if (! item)
    return false;
  item->deleteLater();
  return true;
}

void
ConfigList::onItemDeleted(QObject *item) {
  _items.removeAll(qobject_cast<ConfigItem *>(item));
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclRadioId
 * ********************************************************************************************* */
ConfigRadioId::Declaration *ConfigRadioId::Declaration::_instance = nullptr;

ConfigRadioId::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, QString("Specifies the name of the radio ID.")));
  add("number", new ConfigDMRIdItem::Declaration(true, QString("Specifies the radio/DMR id.")));
}

ConfigItem *
ConfigRadioId::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigRadioId();
}

ConfigRadioId::Declaration *
ConfigRadioId::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(
          true, QString("Specifies a radio ID, that is a pair of DMR ID and name for the radio."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigRadioIdItem
 * ********************************************************************************************* */
ConfigRadioId::ConfigRadioId(QObject *parent)
  : ConfigObjItem(Declaration::get(), parent)
{
  // pass...
}

const QString &
ConfigRadioId::name() const {
  return get("name")->as<ConfigStrItem>()->value();
}

uint32_t
ConfigRadioId::number() const {
  return get("number")->as<ConfigDMRIdItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigContactItem::Declaration
 * ********************************************************************************************* */
ConfigContact::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, tr("Specifies the name of the contact.")));
  add("ring", new ConfigBoolItem::Declaration(false, tr("Enables ring tone (optional).")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigContactItem
 * ********************************************************************************************* */
ConfigContact::ConfigContact(Declaration *declaraion, QObject *parent)
  : ConfigObjItem(declaraion, parent)
{
  // pass...
}

const QString &
ConfigContact::name() const {
  return get("name")->as<ConfigStrItem>()->value();
}

bool
ConfigContact::ring() const {
  return get("ring")->as<ConfigBoolItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDigitalContactItem::Declaration
 * ********************************************************************************************* */
ConfigDigitalContact::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigContact::Declaration(mandatory, description)
{
  // pass...
}

/* ********************************************************************************************* *
 * Implementation of ConfigDigitalContactItem
 * ********************************************************************************************* */
ConfigDigitalContact::ConfigDigitalContact(Declaration *declaraion, QObject *parent)
  : ConfigContact(declaraion, parent)
{
  // pas...
}


/* ********************************************************************************************* *
 * Implementation of ConfigPrivateCallItem::Declaration
 * ********************************************************************************************* */
ConfigPrivateCall::Declaration *ConfigPrivateCall::Declaration::_instance = nullptr;

ConfigPrivateCall::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigDigitalContact::Declaration(mandatory, description)
{
  add("number", new ConfigDMRIdItem::Declaration(
        true, tr("Specifies the DMR ID of the private call.")));
}

ConfigItem *
ConfigPrivateCall::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigPrivateCall();
}

ConfigPrivateCall::Declaration *
ConfigPrivateCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a private call."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigPrivateCallItem
 * ********************************************************************************************* */
ConfigPrivateCall::ConfigPrivateCall(QObject *parent)
  : ConfigDigitalContact(Declaration::get(), parent)
{
  // pass...
}

uint32_t
ConfigPrivateCall::number() const {
  return get("number")->as<ConfigDMRIdItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigGroupCallItem::Declaration
 * ********************************************************************************************* */
ConfigGroupCall::Declaration *ConfigGroupCall::Declaration::_instance = nullptr;

ConfigGroupCall::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigDigitalContact::Declaration(mandatory, description)
{
  add("number", new ConfigDMRIdItem::Declaration(
        true, tr("Specifies the DMR ID of the private call.")));
}

ConfigItem *
ConfigGroupCall::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigGroupCall();
}

ConfigGroupCall::Declaration *
ConfigGroupCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new ConfigGroupCall::Declaration(true, QString("Specifies a group call."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigGroupCallItem
 * ********************************************************************************************* */
ConfigGroupCall::ConfigGroupCall(QObject *parent)
  : ConfigDigitalContact(Declaration::get(), parent)
{
  // pass...
}

uint32_t
ConfigGroupCall::number() const {
  return get("number")->as<ConfigDMRIdItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigAllCallItem::Declaration
 * ********************************************************************************************* */
ConfigAllCall::Declaration *ConfigAllCall::Declaration::_instance = nullptr;

ConfigAllCall::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigDigitalContact::Declaration(mandatory, description)
{
  // pass, no number of all call
}

ConfigItem *
ConfigAllCall::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigAllCall();
}

ConfigAllCall::Declaration *
ConfigAllCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies an all-call."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAllCallItem
 * ********************************************************************************************* */
ConfigAllCall::ConfigAllCall(QObject *parent)
  : ConfigDigitalContact(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigGroupListItem::Declaration
 * ********************************************************************************************* */
ConfigGroupList::Declaration *ConfigGroupList::Declaration::_instance = nullptr;

ConfigGroupList::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, tr("Specifies the name of the contact.")));
  add("members", new ConfigList::Declaration(
        new ConfigReferenceDeclaration(true, tr("A reference to one of the group calls.")),
        tr("The list of group calls forming the group list.")));
}

ConfigItem *
ConfigGroupList::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigGroupList();
}


ConfigGroupList::Declaration *
ConfigGroupList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Defines a RX group list."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigGroupListItem::Declaration
 * ********************************************************************************************* */
ConfigGroupList::ConfigGroupList(QObject *parent)
  : ConfigObjItem(Declaration::get(), parent)
{
  // pass...
}

const QString &
ConfigGroupList::name() const {
  return get("name")->as<ConfigStrItem>()->value();
}

ConfigList *
ConfigGroupList::members() const {
  return get("members")->as<ConfigList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigChannelItem::Declaration
 * ********************************************************************************************* */
ConfigChannel::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, tr("Specifies the name of the channel.")));
  add("rx", new ConfigFloatItem::Declaration(0, 10e3, true, tr("Specifies the RX frequency of the channel in MHz.")));
  add("tx", new ConfigFloatItem::Declaration(-10e3, 10e3, false, tr("Specifies the TX frequency of the channel or offset in MHz.")));
  add("power", new ConfigEnumItem::Declaration(
        {"min", "low", "mid", "high", "max"}, true,
        tr("Specifies the transmit power on the channel.")));
  add("tx-timeout", new ConfigIntItem::Declaration(
        0, 10000, false, tr("Specifies the transmit timeout in seconds. None if 0 or omitted.")));
  add("rx-only", new ConfigBoolItem::Declaration(
        false, tr("If true, TX is disabled for this channel.")));
  add("scan-list", new ConfigReferenceDeclaration(false, tr("References the scanlist associated with this channel.")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigChannelItem
 * ********************************************************************************************* */
ConfigChannel::ConfigChannel(Declaration *declaraion, QObject *parent)
  : ConfigObjItem(declaraion, parent)
{
  // pass...
}

const QString &
ConfigChannel::name() const {
  return get("name")->as<ConfigStrItem>()->value();
}

double
ConfigChannel::rx() const {
  return get("rx")->as<ConfigFloatItem>()->value();
}

double
ConfigChannel::tx() const {
  return get("tx")->as<ConfigFloatItem>()->value();
}

const QString &
ConfigChannel::power() const {
  return get("power")->as<ConfigEnumItem>()->value();
}

qint64
ConfigChannel::txTimeout() const {
  return get("tx-timeout")->as<ConfigIntItem>()->value();
}

bool
ConfigChannel::rxOnly() const {
  return get("rx-only")->as<ConfigBoolItem>()->value();
}

ConfigScanList *
ConfigChannel::scanlist() const {
  return get("scan-list")->as<ConfigScanList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDigitalChannelItem::Declaration
 * ********************************************************************************************* */
ConfigDigitalChannel::Declaration *ConfigDigitalChannel::Declaration::_instance = nullptr;

ConfigDigitalChannel::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigChannel::Declaration(mandatory, description)
{
  add("admit", new ConfigEnumItem::Declaration(
    {"always", "free", "color-code"},
    true, tr("Specifies the transmit admit criterion.")));
  add("color-code", new ConfigIntItem::Declaration(1,16, true, tr("Specifies the color-code of the channel.")));
  add("time-slot", new ConfigIntItem::Declaration(1,2, true, tr("Specifies the time-slot of the channel.")));
  add("group-list", new ConfigReferenceDeclaration(true, tr("References the RX group list of the channel.")));
  add("tx-contact", new ConfigReferenceDeclaration(false, tr("References the default TX contact of the channel.")));
  add("aprs", new ConfigReferenceDeclaration(false, tr("References the positioning system used by this channel.")));
  add("roaming-zone", new ConfigReferenceDeclaration(false, tr("References the roaming zone used by this channel.")));
  add("dmr-id", new ConfigReferenceDeclaration(false, tr("Specifies the DMR ID to use on this channel.")));
}

ConfigItem *
ConfigDigitalChannel::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigDigitalChannel();
}

ConfigDigitalChannel::Declaration *
ConfigDigitalChannel::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a digial channel."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigDigitalChannelItem
 * ********************************************************************************************* */
ConfigDigitalChannel::ConfigDigitalChannel(QObject *parent)
  : ConfigChannel(Declaration::get(), parent)
{
  // pass...
}

const QString &
ConfigDigitalChannel::admit() const {
  return get("admit")->as<ConfigStrItem>()->value();
}

uint8_t
ConfigDigitalChannel::colorCode() const {
  return get("color-code")->as<ConfigIntItem>()->value();
}

uint8_t
ConfigDigitalChannel::timeSlot() const {
  return get("time-slot")->as<ConfigIntItem>()->value();
}

ConfigGroupList *
ConfigDigitalChannel::groupList() const {
  return get("group-list")->as<ConfigGroupList>();
}

ConfigDigitalContact *
ConfigDigitalChannel::txContact() const {
  if (! has("tx-contact"))
    return nullptr;
  return get("tx-contact")->as<ConfigDigitalContact>();
}

ConfigPositioning *
ConfigDigitalChannel::aprs() const {
  if (! has("aprs"))
    return nullptr;
  return get("aprs")->as<ConfigPositioning>();
}

ConfigRoamingZone *
ConfigDigitalChannel::roamingZone() const {
  if (! has("roaming-zone"))
    return nullptr;
  return get("roaming-zone")->as<ConfigRoamingZone>();
}

ConfigRadioId *
ConfigDigitalChannel::radioId() const {
  if (! has("dmr-id"))
    return nullptr;
  return get("dmr-id")->as<ConfigRadioId>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigAnalogChannelItem::Declaration
 * ********************************************************************************************* */
ConfigAnalogChannel::Declaration *ConfigAnalogChannel::Declaration::_instance = nullptr;

ConfigAnalogChannel::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigChannel::Declaration(mandatory, description)
{
  add("admit", new ConfigEnumItem::Declaration({"always", "free", "tone"},
                                               false, tr("Specifies the transmit admit criterion.")));
  add("squelch", new ConfigIntItem::Declaration(1,10, true, tr("Specifies the squelch level.")));
  add("rx-tone", new ConfigDispatchDeclaration(
  { {"ctcss", new ConfigFloatItem::Declaration(0,300, true, tr("Specifies the CTCSS frequency."))},
    {"dcs", new ConfigIntItem::Declaration(-300,300, true, tr("Specifies the DCS code."))} },
        false, tr("Specifies the DCS/CTCSS signaling for RX.")));
  add("tx-tone", new ConfigDispatchDeclaration(
  { {"ctcss", new ConfigFloatItem::Declaration(0,300, true, tr("Specifies the CTCSS frequency."))},
    {"dcs", new ConfigIntItem::Declaration(-300,300, true, tr("Specifies the DCS code."))} },
        false, tr("Specifies the DCS/CTCSS signaling for TX.")));
  add("band-width", new ConfigEnumItem::Declaration({"narrow","wide"}, true, tr("Specifies the bandwidth of the channel.")));
  add("aprs", new ConfigReferenceDeclaration(false, tr("References the APRS system used by this channel.")));
}

ConfigItem *
ConfigAnalogChannel::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigAnalogChannel();
}

ConfigAnalogChannel::Declaration *
ConfigAnalogChannel::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a digial channel."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAnalogChannelItem
 * ********************************************************************************************* */
ConfigAnalogChannel::ConfigAnalogChannel(QObject *parent)
  : ConfigChannel(Declaration::get(), parent)
{
  // pass...
}

const QString &
ConfigAnalogChannel::admit() const {
  return get("admit")->as<ConfigEnumItem>()->value();
}

uint32_t
ConfigAnalogChannel::rxSignalling() const {
  if (! has("rx-tone"))
    return 0;
  if (get("rx-tone")->as<ConfigObjItem>()->has("ctcss"))
    return get("rx-tone")->as<ConfigObjItem>()->get("ctcss")->as<ConfigFloatItem>()->value();
  // handle as DCS
  return get("rx-tone")->as<ConfigObjItem>()->get("dcs")->as<ConfigIntItem>()->value();
}


uint32_t
ConfigAnalogChannel::txSignalling() const {
  if (! has("tx-tone"))
    return 0;
  if (get("tx-tone")->as<ConfigObjItem>()->has("ctcss"))
    return get("tx-tone")->as<ConfigObjItem>()->get("ctcss")->as<ConfigFloatItem>()->value();
  // handle as DCS
  return get("tx-tone")->as<ConfigObjItem>()->get("dcs")->as<ConfigIntItem>()->value();
}

const QString &
ConfigAnalogChannel::bandWidth() const {
  return get("band-width")->as<ConfigEnumItem>()->value();
}

ConfigAPRSPositioning *
ConfigAnalogChannel::aprs() const {
  if (! has("aprs"))
    return nullptr;
  return get("aprs")->as<ConfigAPRSPositioning>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigZoneItem::Declaration
 * ********************************************************************************************* */
ConfigZone::Declaration *ConfigZone::Declaration::_instance = nullptr;

ConfigZone::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, tr("Specifies the name of the zone.")));
  add("A", new ConfigList::Declaration(new ConfigReferenceDeclaration(false, tr("Channel references."))));
  add("B", new ConfigList::Declaration(new ConfigReferenceDeclaration(false, tr("Channel references."))));
}

ConfigItem *
ConfigZone::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigZone();
}

ConfigZone::Declaration *
ConfigZone::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Defines a zone within the codeplug."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigZone
 * ********************************************************************************************* */
ConfigZone::ConfigZone(QObject *parent)
  : ConfigObjItem(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigScanListItem::Declaration
 * ********************************************************************************************* */
ConfigScanList::Declaration *ConfigScanList::Declaration::_instance = nullptr;

ConfigScanList::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, tr("Name of the scan list.")));
  add("pimary", new ConfigReferenceDeclaration(false, tr("Reference to the primary priority channel.")));
  add("secondary", new ConfigReferenceDeclaration(false, tr("Reference to the secondary priority channel.")));
  add("revert", new ConfigReferenceDeclaration(false, tr("Reference to the revert (transmit) channel.")));
  add("channels", new ConfigList::Declaration(
        new ConfigReferenceDeclaration(false, tr("Reference to a channel.")), tr("List of channels in scan list.")));
}

ConfigItem *
ConfigScanList::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigScanList();
}

ConfigScanList::Declaration *
ConfigScanList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(false, tr("Defines a scan list"));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigScanListItem
 * ********************************************************************************************* */
ConfigScanList::ConfigScanList(QObject *parent)
  : ConfigObjItem(Declaration::get(), parent)
{
  // pass...
}

const QString &
ConfigScanList::name() const {
  return get("name")->as<ConfigStrItem>()->value();
}

ConfigChannel *
ConfigScanList::primary() const {
  if (! has("primary"))
    return nullptr;
  return get("primary")->as<ConfigChannel>();
}

ConfigChannel *
ConfigScanList::secondary() const {
  if (! has("secondary"))
    return nullptr;
  return get("secondary")->as<ConfigChannel>();
}

ConfigChannel *
ConfigScanList::revert() const {
  if (! has("revert"))
    return nullptr;
  return get("revert")->as<ConfigChannel>();
}

ConfigList *
ConfigScanList::channels() const {
  if (! has("channels"))
    return nullptr;
  return get("channels")->as<ConfigList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigPositioningItem::Declaration
 * ********************************************************************************************* */
ConfigPositioning::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, tr("Specifies the name of the positioning system.")));
  add("period", new ConfigIntItem::Declaration(1, 1000, true, tr("Specifies the update period in seconds.")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigPositioningItem
 * ********************************************************************************************* */
ConfigPositioning::ConfigPositioning(Declaration *declaraion, QObject *parent)
  : ConfigObjItem(declaraion, parent)
{
  // pass...
}

const QString &
ConfigPositioning::name() const {
  return get("name")->as<ConfigStrItem>()->value();
}

uint32_t
ConfigPositioning::period() const {
  return get("period")->as<ConfigIntItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDMRPosItem::Declaration
 * ********************************************************************************************* */
ConfigDMRPositioning::Declaration *ConfigDMRPositioning::Declaration::_instance = nullptr;

ConfigDMRPositioning::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigPositioning::Declaration(mandatory, description)
{
  add("destination", new ConfigReferenceDeclaration(true, tr("Specifies the destination contact.")));
  add("channel", new ConfigReferenceDeclaration(false, tr("Specifies the optional revert channel.")));
}

ConfigItem *
ConfigDMRPositioning::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigDMRPositioning();
}

ConfigDMRPositioning::Declaration *
ConfigDMRPositioning::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a DMR positioning system."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigDMRPositioning
 * ********************************************************************************************* */
ConfigDMRPositioning::ConfigDMRPositioning(QObject *parent)
  : ConfigPositioning(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigAPRSPosItem::Declaration
 * ********************************************************************************************* */
ConfigAPRSPositioning::Declaration *ConfigAPRSPositioning::Declaration::_instance = nullptr;

ConfigAPRSPositioning::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigPositioning::Declaration(mandatory, description)
{
  add("source", new ConfigStrItem::Declaration(true, tr("Specifies the source call and SSID.")));
  add("destination", new ConfigStrItem::Declaration(true, tr("Specifies the destination call and SSID.")));
  add("channel", new ConfigReferenceDeclaration(true, tr("Specifies the optional revert channel.")));
  add("path", new ConfigList::Declaration(
        new ConfigStrItem::Declaration(false, tr("Specifies a path element of the APRS packet.")),
        tr("Specifies the APRS path as a list.")));
  add("icon", new ConfigStrItem::Declaration(true, tr("Specifies the icon name.")));
  add("message", new ConfigStrItem::Declaration(false, tr("Specifies the optional APRS message.")));
}

ConfigItem *
ConfigAPRSPositioning::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigAPRSPositioning();
}

ConfigAPRSPositioning::Declaration *
ConfigAPRSPositioning::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies an APRS positioning system."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAPRSPositioning
 * ********************************************************************************************* */
ConfigAPRSPositioning::ConfigAPRSPositioning(QObject *parent)
  : ConfigPositioning(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigRoamingZoneItem::Declaration
 * ********************************************************************************************* */
ConfigRoamingZone::Declaration *ConfigRoamingZone::Declaration::_instance = nullptr;

ConfigRoamingZone::Declaration::Declaration(bool mandatory, const QString &description)
  : ConfigObjItem::Declaration(mandatory, description)
{
  add("name", new ConfigStrItem::Declaration(true, tr("Specifies name of the roaming zone.")));
  add("channels", new ConfigList::Declaration(
        new ConfigReferenceDeclaration(false, tr("References a channel")),
        tr("Specifies the list of channels for the roaming zone.")));
}

ConfigItem *
ConfigRoamingZone::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new ConfigRoamingZone();
}

ConfigRoamingZone::Declaration *
ConfigRoamingZone::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(false, tr("Specifies a roaming zone."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigRoamingZone
 * ********************************************************************************************* */
ConfigRoamingZone::ConfigRoamingZone(QObject *parent)
  : ConfigObjItem(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of Configuration::Declaration
 * ********************************************************************************************* */
Configuration::Declaration *Configuration::Declaration::_instance = nullptr;

Configuration::Declaration::Declaration()
  : ConfigMapItem::Declaration(QHash<QString, ConfigItem::Declaration *>(), true, "")
{
  add("version", new ConfigStrItem::Declaration(false, tr("Specifies the optional version number of the config format.")));
  add("intro-line1", new ConfigStrItem::Declaration(false, tr("Specifies the optional first boot display line.")));
  add("intro-line2", new ConfigStrItem::Declaration(false, tr("Specifies the optional second boot display line.")));

  add("radio-ids",
      new ConfigList::Declaration(
        ConfigRadioId::Declaration::get(),
        tr("This list specifies the DMR IDs and names for the radio.")));

  add("channels",
      new ConfigList::Declaration(
        new ConfigDispatchDeclaration({ {"analog", ConfigAnalogChannel::Declaration::get()},
                                        {"digital", ConfigDigitalChannel::Declaration::get()}},
                                      true),
        tr("The list of all channels within the codeplug.")));

  add("zones",
      new ConfigList::Declaration(ConfigZone::Declaration::get(),
                                      tr("Defines the list of zones.")));

  add("scan-lists",
      new ConfigList::Declaration(
        ConfigScanList::Declaration::get(),
        tr("Defines the list of all scan lists.")));

  add("contacts",
      new ConfigList::Declaration(
        new ConfigDispatchDeclaration({ {"private", ConfigPrivateCall::Declaration::get()},
                                        {"group", ConfigGroupCall::Declaration::get()},
                                        {"all", ConfigAllCall::Declaration::get()} },
                                      true, tr("One of the contact types, 'private', 'group', 'all'.")),
        tr("Specifies the list of contacts.")));

  add("group-lists",
      new ConfigList::Declaration(
        ConfigGroupList::Declaration::get(),
        tr("Lists all RX group lists within the codeplug.")));

  add("positioning",
      new ConfigList::Declaration(
        new ConfigDispatchDeclaration({ {"dmr", ConfigDMRPositioning::Declaration::get()},
                                        {"aprs", ConfigAPRSPositioning::Declaration::get()} },
                                      false),
        tr("List of all positioning systems.")));

  add("roaming",
      new ConfigList::Declaration(
        ConfigRoamingZone::Declaration::get(),
        tr("List of all roaming zones.")));

}

bool
Configuration::Declaration::verify(const YAML::Node &doc, QString &message) {
  QSet<QString> ctx;
  if (! this->verifyForm(doc, ctx, message))
    return false;
  if (! this->verifyReferences(doc, ctx, message))
    return false;
  return true;
}

ConfigItem *
Configuration::Declaration::parseAllocate(YAML::Node &node, QString &msg) const {
  return new Configuration();
}

Configuration::Declaration *
Configuration::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Configuration::Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of Configuration
 * ********************************************************************************************* */
Configuration::Configuration(QObject *parent)
  : ConfigMapItem(Declaration::get(), parent)
{
  // pass...
}
