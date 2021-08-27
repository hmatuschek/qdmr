#include "genericconfigitem.hh"
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

using namespace Configuration;


/* ********************************************************************************************* *
 * Implementation of ConfigItem::Declaration
 * ********************************************************************************************* */
Item::Declaration::Declaration(const QString &description)
  : _description(description), _name("Item")
{
  // pass...
}

Item::Declaration::~Declaration() {
  // pass...
}

const QString &
Item::Declaration::description() const {
  return _description;
}

const QString &
Item::Declaration::name() const {
  return _name;
}

bool
Item::Declaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  return true;
}

Item *
Item::Declaration::parseDefine(const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  Item *item = parseAllocate(node, msg);
  if (nullptr == item)
    return nullptr;
  parsePopulate(item, node, ctx, msg);
  return item;
}

bool
Item::Declaration::parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItem
 * ********************************************************************************************* */
Item::Item(const Declaration *declaration, QObject *parent)
  : QObject(parent), _declaration(declaration)
{
  // pass...
}

Item::~Item() {
  // pass...
}

const Item::Declaration *
Item::declaraion() const {
  return _declaration;
}

QString
Item::dump(const QString &prefix) const {
  return QString("%1<Item @0x%2>").arg(prefix).arg(ulong(this),16,16,QChar('0'));
}

bool
Item::generateIds(QHash<const Item *, QString> &ctx, QString &message) const {
  return true;
}

YAML::Node
Item::generateNode(const QHash<const Item *, QString> &ctx, QString &message) const {
  return declaraion()->generate(this, ctx, message);
}


/* ********************************************************************************************* *
 * Implementation of ConfigScalarItem::Declaration
 * ********************************************************************************************* */
ScalarItem::Declaration::Declaration(const QString &description)
  : Item::Declaration(description)
{
  // pass...
}

bool
ScalarItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsScalar()) {
    msg = tr("Expected scalar, got %1.").arg(nodeTypeName(node));
    return false;
  }
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemScalar
 * ********************************************************************************************* */
ScalarItem::ScalarItem(const Declaration *declaration, QObject *parent)
  : Item(declaration, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemBool::Declaration
 * ********************************************************************************************* */
BoolItem::Declaration *BoolItem::Declaration::_instance = nullptr;

BoolItem::Declaration::Declaration(const QString &description)
  : ScalarItem::Declaration(description)
{
  _name = "Bool";
}

bool
BoolItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  QString val = QString::fromStdString(node.as<std::string>());
  if (("true" != val) && ("false" != val)) {
    msg = tr("Expected boolean value ('true' or 'false'), got '%1'.").arg(val);
    return false;
  }
  return true;
}

Item *
BoolItem::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new BoolItem(false, this);
}

bool
BoolItem::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  QString val = QString::fromStdString(node.as<std::string>());
  item->as<BoolItem>()->setValue((val=="true") ? true : false);
  return true;
}

YAML::Node
BoolItem::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  node = item->as<BoolItem>()->value();
  return node;
}

BoolItem::Declaration *
BoolItem::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(tr("A boolean value."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemBool
 * ********************************************************************************************* */
BoolItem::BoolItem(bool value, const Declaration *declaration, QObject *parent)
  : ScalarItem(declaration, parent), _value(value)
{
  // pass...
}

bool
BoolItem::value() const {
  return _value;
}

void
BoolItem::setValue(bool val) {
  _value = val;
  emit modified(this);
}

QString
BoolItem::dump(const QString &prefix) const {
  return QString("%1<Bool %2>").arg(prefix).arg(_value ? "true": "false");
}



/* ********************************************************************************************* *
 * Implementation of ConfigItemInt::Declaration
 * ********************************************************************************************* */
IntItem::Declaration::Declaration(
    qint64 min, qint64 max, const QString &description)
  : ScalarItem::Declaration(description), _min(min), _max(max)
{
  _name = "Int";
}

qint64
IntItem::Declaration::mininum() const {
  return _min;
}

qint64
IntItem::Declaration::maximum() const {
  return _max;
}

bool
IntItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  qint64 val = node.as<qint64>();
  if ((val < _min) || (val>_max)) {
    msg = tr("Value must be in range [%1, %2], got %3.")
        .arg(_min).arg(_max).arg(val);
    return false;
  }
  return true;
}

Item *
IntItem::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new IntItem(0, this);
}
bool
IntItem::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  qint64 val = node.as<qint64>();
  item->as<IntItem>()->setValue(val);
  return true;
}

YAML::Node
IntItem::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  node = item->as<IntItem>()->value();
  return node;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemInt
 * ********************************************************************************************* */
IntItem::IntItem(qint64 value, const Declaration *declaration, QObject *parent)
  : ScalarItem(declaration, parent), _value(0)
{
  setValue(value);
}

qint64
IntItem::value() const {
  return _value;
}

void
IntItem::setValue(qint64 value) {
  qint64 min = declaraion()->as<IntItem::Declaration>()->mininum(),
      max = declaraion()->as<IntItem::Declaration>()->maximum();
  _value = std::max(min, std::min(max, value));
  emit modified(this);
}

QString
IntItem::dump(const QString &prefix) const {
  return QString("%1<Int %2>").arg(prefix).arg(_value);
}


/* ********************************************************************************************* *
 * Implementation of DMRId::Declaration
 * ********************************************************************************************* */
DMRId::Declaration * DMRId::Declaration::_instance = nullptr;

DMRId::Declaration::Declaration()
  : IntItem::Declaration(0, 16777215, tr("Specifies a DMR ID."))
{
  _name = "DMRId";
}

Item *
DMRId::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new DMRId(0, this);
}

bool
DMRId::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  item->as<DMRId>()->setValue(node.as<quint32>());
  return true;
}

DMRId::Declaration *
DMRId::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of DMRId
 * ********************************************************************************************* */
DMRId::DMRId(uint32_t value, const Declaration *declaration, QObject *parent)
  : IntItem(value, declaration, parent)
{
  // pass...
}

QString
DMRId::dump(const QString &prefix) const {
  return QString("%1<DMR-ID %2>").arg(prefix).arg(_value);
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemFloat::Declaration
 * ********************************************************************************************* */
FloatItem::Declaration::Declaration(double min, double max, const QString &description)
  : ScalarItem::Declaration(description), _min(min), _max(max)
{
  _name = "Float";
}

double
FloatItem::Declaration::mininum() const {
  return _min;
}

double
FloatItem::Declaration::maximum() const {
  return _max;
}

bool
FloatItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  double val = node.as<double>();
  if ((val < _min) || (val>_max)) {
    msg = tr("Value must be in range [%1, %2], got %3.")
        .arg(_min).arg(_max).arg(val);
    return false;
  }
  return true;
}

Item *
FloatItem::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new FloatItem(0, this);
}

bool
FloatItem::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  item->as<FloatItem>()->setValue(node.as<double>());
  return true;
}

YAML::Node
FloatItem::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  node = item->as<FloatItem>()->value();
  return node;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemFloat
 * ********************************************************************************************* */
FloatItem::FloatItem(double value, const Declaration *declaration, QObject *parent)
  : ScalarItem(declaration, parent), _value(value)
{
  // pass...
}

double
FloatItem::value() const {
  return _value;
}
void
FloatItem::setValue(double value) {
  double min = declaraion()->as<FloatItem::Declaration>()->mininum(),
      max = declaraion()->as<FloatItem::Declaration>()->maximum();
  _value = std::max(min, std::min(max, value));
  emit modified(this);
}

QString
FloatItem::dump(const QString &prefix) const {
  return QString("%1<Float %2>").arg(prefix).arg(_value);
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemStr::Declaration
 * ********************************************************************************************* */
StringItem::Declaration *StringItem::Declaration::_instance = nullptr;

StringItem::Declaration::Declaration(const QString &description)
  : ScalarItem::Declaration(description)
{
  _name = "String";
}

bool
StringItem::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  return true;
}

Item *
StringItem::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new StringItem("", this);
}

bool
StringItem::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item*> &ctx, QString &msg) const {
  item->as<StringItem>()->setValue(QString::fromStdString(node.as<std::string>()));
  return true;
}

YAML::Node
StringItem::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  node = item->as<StringItem>()->value().toStdString();
  return node;
}

StringItem::Declaration *
StringItem::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(tr("A string."));
  return _instance;
}
/* ********************************************************************************************* *
 * Implementation of ConfigItemString
 * ********************************************************************************************* */
StringItem::StringItem(const QString &value, const Declaration *declaration, QObject *parent)
  : ScalarItem(declaration, parent), _value(value)
{
  // pass...
}

const QString &
StringItem::value() const {
  return _value;
}

void
StringItem::setValue(const QString &value) {
  _value = value;
  emit modified(this);
}

QString
StringItem::dump(const QString &prefix) const {
  return QString("%1<String '%2'>").arg(prefix).arg(_value);
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclId
 * ********************************************************************************************* */
IdDeclaration *IdDeclaration::_instance = nullptr;

IdDeclaration::IdDeclaration(const QString &description)
  : StringItem::Declaration(description)
{
  _name = "Id";
}

bool
IdDeclaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! StringItem::Declaration::verifyForm(node, ctx, msg))
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

IdDeclaration *
IdDeclaration::get() {
  if (nullptr == _instance)
    _instance = new IdDeclaration(QObject::tr("An identifier string."));
  return _instance;
}


/* ********************************************************************************************* *
 * Implementation of ConfigReference::Declaration
 * ********************************************************************************************* */
Reference::Declaration::Declaration(bool (*typeChk)(const Item *), const QString &description)
  : StringItem::Declaration(description), _ref_type_check(typeChk)
{
  _name = "Reference";
}

bool
Reference::Declaration::isValidReference(const Item *item) const {
  return _ref_type_check(item);
}

bool
Reference::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! StringItem::Declaration::verifyForm(node, ctx, msg))
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
Reference::Declaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  QString value = QString::fromStdString(node.as<std::string>());
  if (! ctx.contains(value)) {
    msg = tr("Reference '%1' is not defined.").arg(value);
    return false;
  }
  return true;
}

Item *
Reference::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Reference(this);
}

bool
Reference::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  return true;
}

bool
Reference::Declaration::parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  QString id = QString::fromStdString(node.as<std::string>());
  if (! ctx.contains(id)) {
    msg = tr("Cannot resolve element '%1'.").arg(id);
    return false;
  }
  if (! isValidReference(ctx[id])) {
    msg = tr("Referenced object '%1' is of wrong type %3")
        .arg(id).arg(ctx[id]->declaraion()->name());
    return false;
  }
  logDebug() << "Resolve " << id << " to 0x" << QString::number(ulong(ctx.value(id, nullptr)), 16);
  item->as<Reference>()->set(ctx.value(id, nullptr));
  return true;
}

YAML::Node
Reference::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  if (! ctx.contains(item)) {
    msg = tr("Cannot reference element 0x%1, no ID assigned.").arg(ulong(item),0,16);
    return node;
  }
  node = ctx[item].toStdString();
  return node;
}

/* ********************************************************************************************* *
 * Implementation of ConfigReference
 * ********************************************************************************************* */
Reference::Reference(const Declaration *declaraion, QObject *parent)
  : Item(declaraion, parent), _reference(nullptr)
{
  // pass...
}

Reference::Reference(Object *obj, const Declaration *declaraion, QObject *parent)
  : Item(declaraion, parent), _reference(nullptr)
{
  _reference = declaraion->isValidReference(obj) ? obj : nullptr;
}

bool
Reference::isNull() const {
  return nullptr == _reference;
}

Item *
Reference::take() {
  if (nullptr == _reference)
    return nullptr;
  disconnect(_reference, SIGNAL(destroyed(QObject*)), this, SLOT(onReferenceDeleted(QObject*)));
  Item *ref = _reference;
  _reference = nullptr;
  emit modified(this);
  return ref;
}

bool
Reference::set(Item *item) {
  take();
  if (nullptr == item)
    return true;

  if (! declaraion()->as<Reference::Declaration>()->isValidReference(item)) {
    logDebug() << "Cannot set reference, target is not of matching type.";
    return false;
  }
  _reference = item;
  connect(_reference, SIGNAL(destroyed(QObject*)), this, SLOT(onReferenceDeleted(QObject*)));
  emit modified(this);
  return true;
}

void
Reference::onReferenceDeleted(QObject *obj) {
  if (_reference == obj)
    _reference = nullptr;
}

QString
Reference::dump(const QString &prefix) const {
  return QString("%1<Ref to 0x%2>").arg(prefix).arg(ulong(_reference),16,16,QChar('0'));
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemEnum::Declaration
 * ********************************************************************************************* */
Enum::Declaration::Declaration(const QHash<QString, uint32_t> &values, const QString &description)
  : ScalarItem::Declaration(description), _values(values)
{
  _name = "Enum";
}

const QHash<QString, uint32_t> &
Enum::Declaration::possibleValues() const {
  return _values;
}

QString
Enum::Declaration::key(uint32_t value) const {
  QString key="";
  foreach (QString k, _values.keys()) {
    if (_values[k] == value) {
      key = k; break;
    }
  }
  return key;
}

bool
Enum::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! ScalarItem::Declaration::verifyForm(node, ctx, msg))
    return false;
  QString value = QString::fromStdString(node.as<std::string>());
  if (! _values.contains(value)) {
    QStringList pval;
    foreach (QString val, _values.keys())
      pval.append(val);
    msg = tr("Expected one of (%1), got %2.")
        .arg(pval.join(", ")).arg(value);
    return false;
  }
  return true;
}

Item *
Enum::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Enum(this);
}
bool
Enum::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  return item->as<Enum>()->setKey(QString::fromStdString(node.as<std::string>()));
}

YAML::Node
Enum::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  node = key(item->as<Enum>()->value()).toStdString();
  return node;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemEnum
 * ********************************************************************************************* */
Enum::Enum(const Declaration *declaration, QObject *parent)
  : ScalarItem(declaration, parent), _value(std::numeric_limits<uint32_t>::max())
{
  // pass...
}

Enum::Enum(uint32_t value, const Declaration *declaration, QObject *parent)
  : ScalarItem(declaration, parent)
{
  setValue(value);
}

uint32_t Enum::value(uint32_t default_value) const {
  if (std::numeric_limits<uint32_t>::max() == _value)
    return default_value;
  return _value;
}

bool
Enum::setKey(const QString &key) {
  if (! declaraion()->as<Enum::Declaration>()->possibleValues().contains(key))
    return false;
  _value = declaraion()->as<Enum::Declaration>()->possibleValues()[key];
  emit modified(this);
  return true;
}

bool
Enum::setValue(uint32_t value) {
  const QHash<QString, uint32_t> table =
      declaraion()->as<Enum::Declaration>()->possibleValues();
  foreach (uint32_t x, table.values()) {
    if (value == x) {
      _value = value;
      emit modified(this);
      return true;
    }
  }
  _value = std::numeric_limits<uint32_t>::max();
  return false;
}

QString
Enum::dump(const QString &prefix) const {
  QString key = _declaration->as<Enum::Declaration>()->key(_value);
  if (key.isEmpty())
    key = "<unknown>";
  return QString("%1<Enum %2 (%3)>").arg(prefix).arg(_value).arg(key);
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclDispatch
 * ********************************************************************************************* */
DispatchDeclaration::DispatchDeclaration(const QHash<QString, Item::Declaration *> &elements, const QString &description)
  : Item::Declaration(description), _elements()
{
  _name = "Dispatch";
  for (QHash<QString, Item::Declaration *>::const_iterator it=elements.begin(); it!=elements.end(); it++) {
    add(it.key(), it.value());
  }
}

bool
DispatchDeclaration::add(const QString &name, Declaration *element) {
  if (nullptr == element)
    return false;
  if (_elements.contains(name))
    return false;
  _elements[name] = element;
  return true;
}

bool
DispatchDeclaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
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
DispatchDeclaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  if (! _elements[ename]->verifyReferences(it->second, ctx, msg)) {
    msg = QString("Element '%1': %2").arg(ename).arg(msg);
    return false;
  }

  return true;
}

Item *
DispatchDeclaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return nullptr;
}
bool
DispatchDeclaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  return false;
}

Item *
DispatchDeclaration::parseDefine(const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  return _elements[ename]->parseDefine(it->second, ctx, msg);
}

bool
DispatchDeclaration::parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  YAML::const_iterator it = node.begin();
  QString ename = QString::fromStdString(it->first.as<std::string>());
  return _elements[ename]->parseLink(item, it->second, ctx, msg);
}

YAML::Node
DispatchDeclaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  foreach (QString key, _elements.keys()) {
    if (_elements[key] == item->declaraion()) {
      YAML::Node element = _elements[key]->generate(item, ctx, msg);
      if (element.IsNull()) {
        msg = QString("'%1' -> %2").arg(key).arg(msg);
        return element;
      }
      node[key.toStdString()] = element;
      break;
    }
  }
  return node;
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemMap::Declaration
 * ********************************************************************************************* */
Map::Declaration::Declaration(const QString &description)
  : Item::Declaration(description), _elements()
{
  _name = "Map";
}

bool
Map::Declaration::add(const QString &name, Item::Declaration *element, bool mandatory, const QString &description) {
  if (nullptr == element)
    return false;
  if (_elements.contains(name))
    return false;
  if (mandatory)
    _mandatoryElements.insert(name);
  _elements[name] = element;
  _order.append(name);
  _descriptions.insert(name, description);
  return true;
}

bool
Map::Declaration::has(const QString &name) const {
  return _elements.contains(name);
}

Item::Declaration *
Map::Declaration::get(const QString &name) const {
  return _elements.value(name, nullptr);
}

bool
Map::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
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
Map::Declaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
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

Item *
Map::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Map(this);
}

bool
Map::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if (! _elements.contains(ename)) {
      logWarn() << "Skip element '" << ename << "': Element name not defined in object.";
      continue;
    }
    Item *citem = _elements[ename]->parseDefine(it->second, ctx, msg);
    if (nullptr == citem)
      return false;
    if (! item->as<Map>()->add(ename, citem)) {
      msg = tr("Cannot add item '%1' to map.").arg(ename);
      return false;
    }
  }
  return true;
}

bool
Map::Declaration::parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  Map *map = item->as<Map>();
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if (! _elements.contains(ename)) {
      logWarn() << "Skip element '" << ename << "': Element name not defined in object.";
      continue;
    }
    if (! _elements[ename]->parseLink(map->get(ename), it->second, ctx, msg))
      return false;
  }
  return true;
}

YAML::Node
Map::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  const Map *map = item->as<Map>();
  foreach (QString key, _order) {
    if (! map->_elements.contains(key))
      continue;
    Item *value = map->get(key);
    while (value && value->is<Reference>())
      value = value->as<Reference>()->get();
    YAML::Node element = _elements[key]->generate(value, ctx, msg);
    if (element.IsNull()) {
      msg = tr("'%1' -> %2").arg(key).arg(msg);
      return element;
    }
    node[key.toStdString()] = element;
  }
  return node;
}

/* ********************************************************************************************* *
 * Implementation of ConfigItemMap
 * ********************************************************************************************* */
Map::Map(const Declaration *declaration, QObject *parent)
  : Item(declaration, parent), _elements()
{
  // pass...
}

bool
Map::has(const QString &name) const {
  return _elements.contains(name);
}

bool
Map::add(const QString &name, Item *element) {
  if (_elements.contains(name))
    return false;
  return set(name, element);
}

bool
Map::set(const QString &name, Item *element) {
  if (nullptr == element) {
    logDebug() << "Cannot set element '" << name
               << "': to null.";
    return false;
  }
  if (! declaraion()->as<Map::Declaration>()->has(name)) {
    logDebug() << "Cannot set element '" << name
               << "' in map. Element name is not defined in declaration.";
    return false;
  }
  if (_elements.contains(name)) {
    disconnect(_elements[name], SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject *)));
    disconnect(_elements[name], SIGNAL(modified(Item*)), this, SLOT(onElementModified(Item*)));
  }
  _elements[name] = element;
  connect(element, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject *)));
  connect(element, SIGNAL(modified(Item*)), this, SLOT(onElementModified(Item*)));
  element->setParent(this);
  emit modified(this);
  return true;
}

Item *
Map::take(const QString &name) {
  if (! _elements.contains(name))
    return nullptr;
  Item *element = _elements.take(name);
  if (element) {
    disconnect(element, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
    disconnect(element, SIGNAL(modified(Item*)), this, SLOT(onElementModified(Item*)));
    element->setParent(nullptr);
  }
  emit modified(this);
  return element;
}

bool
Map::del(const QString &name) {
  if (! _elements.contains(name))
    return false;
  Item *element = take(name);
  if (element)
    element->deleteLater();
  emit modified(this);
  return true;
}

void
Map::onElementDeleted(QObject *element) {
  foreach (const QString &key, _elements.keys()) {
    if (element == _elements[key]) {
      _elements.remove(key);
      emit elementDeleted(key);
      emit modified(this);
      return;
    }
  }
}

void
Map::onElementModified(Item *item) {
  foreach (const QString &key, _elements.keys()) {
    if (item == _elements[key]) {
      emit elementModified(key);
      emit modified(this);
      return;
    }
  }
}

bool
Map::generateIds(QHash<const Item *, QString> &ctx, QString &message) const {
  foreach (Item *item, _elements.values()) {
    if (! item->generateIds(ctx, message))
      return false;
  }
  return true;
}

QString
Map::dump(const QString &prefix) const {
  QString rep = QString("%1<Map @0x%2").arg(prefix).arg(ulong(this),16,16,QChar('0'));
  foreach (QString key, _elements.keys()) {
    rep += QString("%1\n%2:\n%3").arg(prefix+"  ").arg(key).arg(_elements[key]->dump(prefix+"    "));
  }
  rep += ">";
  return rep;
}


/* ********************************************************************************************* *
 * Implementation of Object::Declaration
 * ********************************************************************************************* */
Object::Declaration::Declaration(const QString &description)
  : Map::Declaration(description)
{
  _name = "Object";
  add("id", IdDeclaration::get(), false, tr("Specifies the ID of the object."));
}

Item *
Object::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  Q_UNUSED(node);
  Q_UNUSED(msg);
  return new Object(this);
}

bool
Object::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if ("id" == ename)
      continue;
    if (! _elements.contains(ename)) {
      logWarn() << "Skip element '" << ename << "': Element name not defined in object.";
      continue;
    }
    Item *citem = _elements[ename]->parseDefine(it->second, ctx, msg);
    if (nullptr == citem)
      return false;
    if (! item->as<Map>()->add(ename, citem)) {
      msg = tr("Cannot add item '%1' to map.").arg(ename);
      return false;
    }
  }
  return true;
}

Item *
Object::Declaration::parseDefine(const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  Item *item = parseAllocate(node, msg);
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

bool
Object::Declaration::parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  Object *map = item->as<Object>();
  for (YAML::const_iterator it=node.begin(); it!=node.end(); it++) {
    QString ename = QString::fromStdString(it->first.as<std::string>());
    if ("id" == ename)
      continue;
    if (! _elements.contains(ename)) {
      logWarn() << "Skip element '" << ename << "': Element name not defined in object.";
      continue;
    }
    if (! _elements[ename]->parseLink(map->get(ename), it->second, ctx, msg))
      return false;
  }
  return true;
}

YAML::Node
Object::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node = Map::Declaration::generate(item, ctx, msg);
  if (node.IsNull())
    return node;
  if (! ctx.contains(item)) {
    msg = "Cannot create object, no ID assigned to it.";
    return YAML::Node();
  }
  node["id"] = ctx[item].toStdString();
  return node;
}

/* ********************************************************************************************* *
 * Implementation of Object
 * ********************************************************************************************* */
Object::Object(const Declaration *declaration, QObject *parent)
  : Map(declaration, parent), _idTemplate("obj%1")
{
  // pass...
}

bool
Object::generateIds(QHash<const Item *, QString> &ctx, QString &message) const {
  if (! Map::generateIds(ctx, message))
    return false;

  uint i=0;
  for (; ctx.values().contains(_idTemplate.arg(i)); i++) { }
  ctx[this] = _idTemplate.arg(i);

  return true;
}

QString
Object::dump(const QString &prefix) const {
  QString rep = QString("%1<Object @0x%2").arg(prefix).arg(ulong(this),16,16,QChar('0'));
  foreach (QString key, _elements.keys()) {
    rep += QString("\n%1%2:\n%3").arg(prefix+"  ").arg(key).arg(_elements[key]->dump(prefix+"    "));
  }
  rep += ">";
  return rep;
}


/* ********************************************************************************************* *
 * Implementation of ConfigAbstractList::Declaration
 * ********************************************************************************************* */
AbstractList::Declaration::Declaration(
    Item::Declaration *element, bool (*typeChk)(const Item *),
    const QString &description)
  : Item::Declaration(description), _element(element),
    _element_type_check(typeChk)
{
  _name = "AbstractList";
}

bool
AbstractList::Declaration::isValidType(const Item *item) const {
  return _element_type_check(item);
}

Item::Declaration *
AbstractList::Declaration::elementDeclaration() const {
  return _element;
}

bool
AbstractList::Declaration::verifyForm(const YAML::Node &node, QSet<QString> &ctx, QString &msg) const {
  if (! node.IsSequence()) {
    msg = tr("Expected list, got %2.").arg(nodeTypeName(node));
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
AbstractList::Declaration::verifyReferences(const YAML::Node &node, const QSet<QString> &ctx, QString &msg) const {
  for (size_t i=0; i<node.size(); i++) {
    if (! _element->verifyReferences(node[i], ctx, msg)) {
      msg = tr("List element %1: %2").arg(i).arg(msg);
      return false;
    }
  }
  return true;
}

bool
AbstractList::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  if (! node.IsSequence()) {
    logError() << "Cannot parse list: Expected sequence, got " << nodeTypeName(node) << ".";
    return false;
  }
  for (size_t i=0; i<node.size(); i++) {
    Item *element = _element->parseDefine(node[i], ctx, msg);
    if (nullptr == element)
      return false;
    if (! isValidType(element)) {
      msg = tr("Cannot create list: Elment %1 is of wrong type.").arg(i);
      return false;
    }
    if (! item->as<AbstractList>()->add(element))
      return false;
  }
  return true;
}

bool
AbstractList::Declaration::parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  for (size_t i=0; i<node.size(); i++) {
    if(! _element->parseLink(item->as<AbstractList>()->get(i), node[i], ctx, msg))
      return false;
  }
  return true;
}

YAML::Node
AbstractList::Declaration::generate(const Item *item, const QHash<const Item *, QString> &ctx, QString &msg) const {
  YAML::Node node;
  const AbstractList *list = item->as<AbstractList>();
  for (int i=0; i<list->count(); i++) {
    YAML::Node element = _element->generate(list->get(i), ctx, msg);
    if (element.IsNull()) {
      msg = tr("[%1] -> %2").arg(i).arg(msg);
      return element;
    }
    node.push_back(element);
  }
  return node;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAbstractList
 * ********************************************************************************************* */
AbstractList::AbstractList(const Declaration *declaration, QObject *parent)
  : Item(declaration, parent), _items()
{
  // pass...
}

AbstractList::~AbstractList() {
  // pass...
}

int
AbstractList::count() const {
  return _items.count();
}

bool AbstractList::add(Item *item) {
  if (nullptr == item)
    return false;
  if (! declaraion()->as<AbstractList::Declaration>()->isValidType(item))
    return false;
  _items.append(item);
  connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
  connect(item, SIGNAL(modified(Item*)), this, SLOT(onElementModified(Item*)));
  emit modified(this);
  return true;
}

Item *
AbstractList::get(int i) const {
  if (i >= count())
    return nullptr;
  return _items[i];
}

Item *
AbstractList::take(int i) {
  if (i >= count())
    return nullptr;
  Item *item = _items.takeAt(i);
  disconnect(item, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
  disconnect(item, SIGNAL(modified(const Item*)), this, SLOT(onItemModified(Item*)));
  emit modified(this);
  return item;
}

bool
AbstractList::del(int i) {
  Item *item = take(i);
  if (! item)
    return false;
  item->deleteLater();
  return true;
}

bool
AbstractList::moveUp(int idx) {
  if ((0>=idx) || (idx>=count()))
    return false;
  std::swap(_items[idx], _items[idx-1]);
  emit modified(this);
  return true;
}

bool
AbstractList::moveUp(int first, int last) {
  if ((0>=first) || (last>=count()))
    return false;
  for (int row=first; row<=last; row++)
    std::swap(_items[row], _items[row-1]);
  emit modified(this);
  return true;
}

bool
AbstractList::moveDown(int row) {
  if ((0>row) || ((row+1)>=count()))
    return false;
  std::swap(_items[row], _items[row+1]);
  emit modified(this);
  return true;
}

bool
AbstractList::moveDown(int first, int last) {
  if ((0>first) || ((last+1)>=count()))
    return false;
  for (int row=last; row>=first; row--)
    std::swap(_items[row], _items[row+1]);
  emit modified(this);
  return true;
}

void
AbstractList::onElementDeleted(QObject *item) {
  if (_items.contains(qobject_cast<Item *>(item)))
    return;
  emit elementDeleted(_items.indexOf(qobject_cast<Item *>(item)));
  emit modified(this);
  _items.removeAll(qobject_cast<Item *>(item));
}

void
AbstractList::onElementModified(Item *item) {
  if (! _items.contains(item))
    return;
  emit elementModified(_items.indexOf(item));
  emit modified(this);
}

QString
AbstractList::dump(const QString &prefix) const {
  QString rep = QString("%1<List (#%2) @0x%3").arg(prefix).arg(count()).arg(ulong(this),16,16,QChar('0'));
  foreach (Item *item, _items) {
    rep += QString("\n%3").arg(item->dump(prefix+"  "));
  }
  rep += ">";
  return rep;
}

bool
AbstractList::generateIds(QHash<const Item *, QString> &ctx, QString &message) const {
  for (int i=0; i<count(); i++) {
    if (! _items[i]->generateIds(ctx, message))
      return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of List::Declaration
 * ********************************************************************************************* */
List::Declaration::Declaration(Item::Declaration *element, bool (*typeChk)(const Item *), const QString &description)
  : AbstractList::Declaration(element, typeChk, description)
{
  _name = "List";
}

Item *
List::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new List(this);
}

/* ********************************************************************************************* *
 * Implementation of List
 * ********************************************************************************************* */
List::List(const Declaration *declaration, QObject *parent)
  : AbstractList(declaration, parent)
{
  // pass...
}

bool
List::add(Item *item) {
  if (! AbstractList::add(item))
    return false;
  item->setParent(this);
  return true;
}

Item *
List::take(int i) {
  Item *item = AbstractList::take(i);
  if (nullptr == item)
    return nullptr;
  item->setParent(nullptr);
  return item;
}

bool
List::del(int i) {
  Item *item = take(i);
  if (nullptr == item)
    return false;
  item->deleteLater();
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigRefList::Declaration
 * ********************************************************************************************* */
RefList::Declaration::Declaration(Item::Declaration *element, bool (*typeChk)(const Item *), const QString &description)
  : AbstractList::Declaration(element, typeChk, description)
{
  _name = "RefList";
}

Item *
RefList::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new RefList(this);
}

bool
RefList::Declaration::parsePopulate(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  // Polulation of list is delayed to link stage.
  return true;
}

bool
RefList::Declaration::parseLink(Item *item, const YAML::Node &node, QHash<QString, Item *> &ctx, QString &msg) const {
  for (size_t i=0; i<node.size(); i++) {
    if (! node[i].IsScalar()) {
      msg = tr("Expected ID string, got '%1'").arg(nodeTypeName(node[i]));
      return false;
    }
    QString id = QString::fromStdString(node[i].as<std::string>());
    if (! ctx.contains(id)) {
      msg = tr("Cannot resolve ID '%1'.").arg(id);
      return false;
    }
    Item *element = ctx.value(id, nullptr);
    if (nullptr == element) {
      msg = tr("Cannot create ref-list: %1-th element '%2' not defined.").arg(i).arg(id);
      return false;
    }
    if (! isValidType(element)) {
      msg = tr("Cannot create ref-list: %1-th element is of wrong type. Expected %2, got %3")
          .arg(i).arg(_element->name()).arg(element->declaraion()->name());
      return false;
    }
    if (! item->as<RefList>()->add(element)) {
      msg = tr("Cannot add %1-th element to list").arg(i);
      return false;
    }
  }
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ConfigRefList
 * ********************************************************************************************* */
RefList::RefList(const Declaration *declaration, QObject *parent)
  : AbstractList(declaration, parent)
{
  // pass...
}

QString
RefList::dump(const QString &prefix) const {
  QString rep = QString("%1<RefList (#%2) @0x%3").arg(prefix).arg(count()).arg(ulong(this),16,16,QChar('0'));
  foreach (Item *item, _items) {
    rep += QString("\n%1<Ref to 0x%2>").arg(prefix+"  ").arg(ulong(item),16,16,QChar('0'));
  }
  rep += ">";
  return rep;
}

