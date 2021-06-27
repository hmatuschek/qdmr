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

using namespace Configuration;


/* ********************************************************************************************* *
 * Implementation of ConfigItem::Declaration
 * ********************************************************************************************* */
Item::Declaration::Declaration(bool mandatory, const QString &desc)
  : _description(desc), _mandatory(mandatory), _name("Item")
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

bool
Item::Declaration::isMandatory() const {
  return _mandatory;
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
ScalarItem::Declaration::Declaration(bool mandatory, const QString &description)
  : Item::Declaration(mandatory, description)
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
BoolItem::Declaration::Declaration(bool mandatory, const QString &description)
  : ScalarItem::Declaration(mandatory, description)
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
}

QString
BoolItem::dump(const QString &prefix) const {
  return QString("%1<Bool %2>").arg(prefix).arg(_value ? "true": "false");
}



/* ********************************************************************************************* *
 * Implementation of ConfigItemInt::Declaration
 * ********************************************************************************************* */
IntItem::Declaration::Declaration(
    qint64 min, qint64 max, bool mandatory, const QString &description)
  : ScalarItem::Declaration(mandatory, description), _min(min), _max(max)
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
}

QString
IntItem::dump(const QString &prefix) const {
  return QString("%1<Int %2>").arg(prefix).arg(_value);
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemDMRId::Declaration
 * ********************************************************************************************* */
DMRId::Declaration::Declaration(bool mandatory, const QString &description)
  : IntItem::Declaration(0, 16777215, mandatory, description)
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

/* ********************************************************************************************* *
 * Implementation of ConfigItemDMRId
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
FloatItem::Declaration::Declaration(
    double min, double max, bool mandatory, const QString &description)
  : ScalarItem::Declaration(mandatory, description), _min(min), _max(max)
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
}

QString
FloatItem::dump(const QString &prefix) const {
  return QString("%1<Float %2>").arg(prefix).arg(_value);
}


/* ********************************************************************************************* *
 * Implementation of ConfigItemStr::Declaration
 * ********************************************************************************************* */
StringItem::Declaration::Declaration(bool mandatory, const QString &description)
  : ScalarItem::Declaration(mandatory, description)
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
}

QString
StringItem::dump(const QString &prefix) const {
  return QString("%1<String '%2'>").arg(prefix).arg(_value);
}


/* ********************************************************************************************* *
 * Implementation of ConfigDeclId
 * ********************************************************************************************* */
IdDeclaration::IdDeclaration(bool mandatory, const QString &description)
  : StringItem::Declaration(mandatory, description)
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


/* ********************************************************************************************* *
 * Implementation of ConfigReference::Declaration
 * ********************************************************************************************* */
Reference::Declaration::Declaration(bool mandatory, bool (*typeChk)(const Item *), const QString &description)
  : StringItem::Declaration(mandatory, description), _ref_type_check(typeChk)
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
Enum::Declaration::Declaration(const QHash<QString, uint32_t> &values, bool mandatory, const QString &description)
  : ScalarItem::Declaration(mandatory, description), _values(values)
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
  return true;
}

bool
Enum::setValue(uint32_t value) {
  const QHash<QString, uint32_t> table =
      declaraion()->as<Enum::Declaration>()->possibleValues();
  foreach (uint32_t x, table.values()) {
    if (value == x) {
      _value = value;
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
DispatchDeclaration::DispatchDeclaration(
    const QHash<QString, Item::Declaration *> &elements, bool mandatory, const QString &description)
  : Item::Declaration(mandatory, description), _elements()
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
Map::Declaration::Declaration(
    const QHash<QString, Item::Declaration *> &elements, bool mandatory, const QString &description)
  : Item::Declaration(mandatory, description), _elements()
{
  _name = "Map";
  for (QHash<QString, Item::Declaration*>::const_iterator it=elements.begin(); it!=elements.end(); it++) {
    add(it.key(), it.value());
  }
}

bool
Map::Declaration::add(const QString &name, Item::Declaration *element) {
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
  foreach (QString key, map->_elements.keys()) {
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
  return true;
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
  }
  _elements[name] = element;
  connect(element, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject *)));
  element->setParent(this);
  return true;
}

Item *
Map::take(const QString &name) {
  if (! _elements.contains(name))
    return nullptr;
  Item *element = _elements.take(name);
  if (element) {
    disconnect(element, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
    element->setParent(nullptr);
  }
  return element;
}

bool
Map::del(const QString &name) {
  if (! _elements.contains(name))
    return false;
  Item *element = take(name);
  if (element)
    element->deleteLater();
  return true;
}

void
Map::onElementDeleted(QObject *element) {
  foreach (const QString &key, _elements.keys()) {
    if (element == _elements[key]) {
      _elements.remove(key);
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
Object::Declaration::Declaration(bool mandatory, const QString &description)
  : Map::Declaration(QHash<QString, Item::Declaration *>(), mandatory, description)
{
  _name = "Object";
  add("id", new IdDeclaration(true, QString("Specifies the ID of the object.")));
}

Item *
Object::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
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
  : Item::Declaration(element->isMandatory(), description), _element(element),
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
  connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(onItemDeleted(QObject*)));
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
  disconnect(item, SIGNAL(destroyed(QObject*)), this, SLOT(onItemDeleted(QObject*)));
  return item;
}

bool
AbstractList::del(int i) {
  Item *item = take(i);
  if (! item)
    return false;
  return true;
}

void
AbstractList::onItemDeleted(QObject *item) {
  _items.removeAll(qobject_cast<Item *>(item));
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


/* ********************************************************************************************* *
 * Implementation of ConfigDeclRadioId
 * ********************************************************************************************* */
RadioId::Declaration *RadioId::Declaration::_instance = nullptr;

RadioId::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "RadioId";
  add("name", new StringItem::Declaration(true, QString("Specifies the name of the radio ID.")));
  add("number", new DMRId::Declaration(true, QString("Specifies the radio/DMR id.")));
}

Item *
RadioId::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new RadioId();
}

RadioId::Declaration *
RadioId::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(
          true, QString("Specifies a radio ID, that is a pair of DMR ID and name for the radio."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigRadioIdItem
 * ********************************************************************************************* */
RadioId::RadioId(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "id%1";
}

const QString &
RadioId::name() const {
  return get("name")->as<StringItem>()->value();
}

uint32_t
RadioId::number() const {
  return get("number")->as<DMRId>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigContactItem::Declaration
 * ********************************************************************************************* */
Contact::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "Contact";
  add("name", new StringItem::Declaration(true, tr("Specifies the name of the contact.")));
  add("ring", new BoolItem::Declaration(false, tr("Enables ring tone (optional).")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigContactItem
 * ********************************************************************************************* */
Contact::Contact(Declaration *declaraion, QObject *parent)
  : Object(declaraion, parent)
{
  _idTemplate = "con%1";
}

const QString &
Contact::name() const {
  return get("name")->as<StringItem>()->value();
}

bool
Contact::ring() const {
  return get("ring")->as<BoolItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDigitalContactItem::Declaration
 * ********************************************************************************************* */
Configuration::DigitalContact::Declaration::Declaration(bool mandatory, const QString &description)
  : Contact::Declaration(mandatory, description)
{
  _name = "DigitalContact";
}

/* ********************************************************************************************* *
 * Implementation of ConfigDigitalContactItem
 * ********************************************************************************************* */
Configuration::DigitalContact::DigitalContact(Declaration *declaraion, QObject *parent)
  : Contact(declaraion, parent)
{
  // pas...
}


/* ********************************************************************************************* *
 * Implementation of ConfigPrivateCallItem::Declaration
 * ********************************************************************************************* */
PrivateCall::Declaration *PrivateCall::Declaration::_instance = nullptr;

PrivateCall::Declaration::Declaration(bool mandatory, const QString &description)
  : DigitalContact::Declaration(mandatory, description)
{
  _name = "PrivateCall";
  add("number", new DMRId::Declaration(
        true, tr("Specifies the DMR ID of the private call.")));
}

Item *
PrivateCall::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new PrivateCall();
}

PrivateCall::Declaration *
PrivateCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a private call."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigPrivateCallItem
 * ********************************************************************************************* */
PrivateCall::PrivateCall(QObject *parent)
  : DigitalContact(Declaration::get(), parent)
{
  // pass...
}

uint32_t
PrivateCall::number() const {
  return get("number")->as<DMRId>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigGroupCallItem::Declaration
 * ********************************************************************************************* */
GroupCall::Declaration *GroupCall::Declaration::_instance = nullptr;

GroupCall::Declaration::Declaration(bool mandatory, const QString &description)
  : DigitalContact::Declaration(mandatory, description)
{
  _name = "GroupCall";
  add("number", new DMRId::Declaration(
        true, tr("Specifies the DMR ID of the private call.")));
}

Item *
GroupCall::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new GroupCall();
}

GroupCall::Declaration *
GroupCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new GroupCall::Declaration(true, QString("Specifies a group call."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigGroupCallItem
 * ********************************************************************************************* */
GroupCall::GroupCall(QObject *parent)
  : DigitalContact(Declaration::get(), parent)
{
  // pass...
}

uint32_t
GroupCall::number() const {
  return get("number")->as<DMRId>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigAllCallItem::Declaration
 * ********************************************************************************************* */
AllCall::Declaration *AllCall::Declaration::_instance = nullptr;

AllCall::Declaration::Declaration(bool mandatory, const QString &description)
  : DigitalContact::Declaration(mandatory, description)
{
  _name = "AllCall";
}

Item *
AllCall::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new AllCall();
}

AllCall::Declaration *
AllCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies an all-call."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAllCallItem
 * ********************************************************************************************* */
AllCall::AllCall(QObject *parent)
  : DigitalContact(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigGroupListItem::Declaration
 * ********************************************************************************************* */
GroupList::Declaration *GroupList::Declaration::_instance = nullptr;

GroupList::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "GroupList";
  add("name", new StringItem::Declaration(true, tr("Specifies the name of the contact.")));
  add("members",
      RefList::Declaration::get<DigitalContact>(
        false, tr("The list of group calls forming the group list.")));
}

Item *
GroupList::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new GroupList();
}


GroupList::Declaration *
GroupList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Defines a RX group list."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigGroupListItem::Declaration
 * ********************************************************************************************* */
GroupList::GroupList(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "grp%1";
}

const QString &
GroupList::name() const {
  return get("name")->as<StringItem>()->value();
}

AbstractList *
GroupList::members() const {
  return get("members")->as<AbstractList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigChannelItem::Declaration
 * ********************************************************************************************* */
Configuration::Channel::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "Channel";
  add("name", new StringItem::Declaration(true, tr("Specifies the name of the channel.")));
  add("rx", new FloatItem::Declaration(0, 10e3, true, tr("Specifies the RX frequency of the channel in MHz.")));
  add("tx", new FloatItem::Declaration(-10e3, 10e3, false, tr("Specifies the TX frequency of the channel or offset in MHz.")));
  add("power", new Enum::Declaration(
    { {"min", ::Channel::MinPower},
      {"low", ::Channel::LowPower},
      {"mid", ::Channel::MidPower},
      {"high", ::Channel::HighPower},
      {"max", ::Channel::MaxPower} }, true, tr("Specifies the transmit power on the channel.")));
  add("tx-timeout", new IntItem::Declaration(
        0, 10000, false, tr("Specifies the transmit timeout in seconds. None if 0 or omitted.")));
  add("rx-only", new BoolItem::Declaration(
        false, tr("If true, TX is disabled for this channel.")));
  add("scan-list", Reference::Declaration::get<ScanList>(
        false, tr("References the scanlist associated with this channel.")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigChannelItem
 * ********************************************************************************************* */
Configuration::Channel::Channel(Declaration *declaraion, QObject *parent)
  : Object(declaraion, parent)
{
  _idTemplate = "ch%1";
}

const QString &
Configuration::Channel::name() const {
  return get("name")->as<StringItem>()->value();
}

double
Configuration::Channel::rx() const {
  return get("rx")->as<FloatItem>()->value();
}

double
Configuration::Channel::tx() const {
  return get("tx")->as<FloatItem>()->value();
}

::Channel::Power
Configuration::Channel::power() const {
  return get("power")->as<Enum>()->as<::Channel::Power>(::Channel::MaxPower);
}

qint64
Configuration::Channel::txTimeout() const {
  return get("tx-timeout")->as<IntItem>()->value();
}

bool
Configuration::Channel::rxOnly() const {
  return get("rx-only")->as<BoolItem>()->value();
}

Configuration::ScanList *
Configuration::Channel::scanlist() const {
  return get("scan-list")->as<Configuration::ScanList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDigitalChannelItem::Declaration
 * ********************************************************************************************* */
Configuration::DigitalChannel::Declaration *
Configuration::DigitalChannel::Declaration::_instance = nullptr;

Configuration::DigitalChannel::Declaration::Declaration(bool mandatory, const QString &description)
  : Channel::Declaration(mandatory, description)
{
  _name = "DigitalChannel";
  add("admit", new Enum::Declaration(
    { {"always", ::DigitalChannel::AdmitNone},
      {"free", ::DigitalChannel::AdmitFree},
      {"color-code", ::DigitalChannel::AdmitColorCode} }, true, tr("Specifies the transmit admit criterion.")));
  add("color-code", new IntItem::Declaration(1,16, true, tr("Specifies the color-code of the channel.")));
  add("time-slot", new IntItem::Declaration(1,2, true, tr("Specifies the time-slot of the channel.")));
  add("group-list", Reference::Declaration::get<GroupList>(
        true, tr("References the RX group list of the channel.")));
  add("tx-contact", Reference::Declaration::get<DigitalContact>(
        false, tr("References the default TX contact of the channel.")));
  add("aprs", Reference::Declaration::get<Positioning>(
        false, tr("References the positioning system used by this channel.")));
  add("roaming-zone", Reference::Declaration::get<RoamingZone>(
        false, tr("References the roaming zone used by this channel.")));
  add("dmr-id", Reference::Declaration::get<RadioId>(
        false, tr("Specifies the DMR ID to use on this channel.")));
}

Item *
Configuration::DigitalChannel::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new DigitalChannel();
}

Configuration::DigitalChannel::Declaration *
Configuration::DigitalChannel::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a digial channel."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigDigitalChannelItem
 * ********************************************************************************************* */
Configuration::DigitalChannel::DigitalChannel(QObject *parent)
  : Channel(Declaration::get(), parent)
{
  // pass...
}

::DigitalChannel::Admit
Configuration::DigitalChannel::admit() const {
  return get("admit")->as<Enum>()->as<::DigitalChannel::Admit>(::DigitalChannel::AdmitColorCode);
}

uint8_t
Configuration::DigitalChannel::colorCode() const {
  if (! has("color-code"))
    return 1;
  return get("color-code")->as<IntItem>()->value();
}

::DigitalChannel::TimeSlot
Configuration::DigitalChannel::timeSlot() const {
  if (! has("time-slot"))
    return ::DigitalChannel::TimeSlot1;
  return get("time-slot")->as<Enum>()->as<::DigitalChannel::TimeSlot>(::DigitalChannel::TimeSlot1);
}

GroupList *
Configuration::DigitalChannel::groupList() const {
  return getRef<GroupList>("group-list");
}

Configuration::DigitalContact *
Configuration::DigitalChannel::txContact() const {
  return getRef<Configuration::DigitalContact>("tx-contact");
}

Configuration::Positioning *
Configuration::DigitalChannel::aprs() const {
  if (! has("aprs"))
    return nullptr;
  return get("aprs")->as<Configuration::Positioning>();
}

Configuration::RoamingZone *
Configuration::DigitalChannel::roamingZone() const {
  return getRef<Configuration::RoamingZone>("roaming-zone");
}

RadioId *
Configuration::DigitalChannel::radioId() const {
  return getRef<RadioId>("dmr-id");
}


/* ********************************************************************************************* *
 * Implementation of ConfigAnalogChannelItem::Declaration
 * ********************************************************************************************* */
Configuration::AnalogChannel::Declaration *
Configuration::AnalogChannel::Declaration::_instance = nullptr;

Configuration::AnalogChannel::Declaration::Declaration(bool mandatory, const QString &description)
  : Channel::Declaration(mandatory, description)
{
  _name = "AnalogChannel";
  add("admit", new Enum::Declaration(
    {{"always", ::AnalogChannel::AdmitNone},
     {"free", ::AnalogChannel::AdmitFree},
     {"tone", ::AnalogChannel::AdmitTone} }, false, tr("Specifies the transmit admit criterion.")));
  add("squelch", new IntItem::Declaration(1,10, true, tr("Specifies the squelch level.")));
  add("rx-tone", new DispatchDeclaration(
  { {"ctcss", new FloatItem::Declaration(0,300, true, tr("Specifies the CTCSS frequency."))},
    {"dcs", new IntItem::Declaration(-300,300, true, tr("Specifies the DCS code."))} },
        false, tr("Specifies the DCS/CTCSS signaling for RX.")));
  add("tx-tone", new DispatchDeclaration(
  { {"ctcss", new FloatItem::Declaration(0,300, true, tr("Specifies the CTCSS frequency."))},
    {"dcs", new IntItem::Declaration(-300,300, true, tr("Specifies the DCS code."))} },
        false, tr("Specifies the DCS/CTCSS signaling for TX.")));
  add("band-width", new Enum::Declaration(
    {{"narrow", ::AnalogChannel::BWNarrow},
     {"wide", ::AnalogChannel::BWWide}}, true, tr("Specifies the bandwidth of the channel.")));
  add("aprs", Reference::Declaration::get<APRSPositioning>(
        false, tr("References the APRS system used by this channel.")));
}

Item *
Configuration::AnalogChannel::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new AnalogChannel();
}

Configuration::AnalogChannel::Declaration *
Configuration::AnalogChannel::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a digial channel."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAnalogChannelItem
 * ********************************************************************************************* */
Configuration::AnalogChannel::AnalogChannel(QObject *parent)
  : Channel(Declaration::get(), parent)
{
  // pass...
}

::AnalogChannel::Admit
Configuration::AnalogChannel::admit() const {
  return get("admit")->as<Enum>()->as<::AnalogChannel::Admit>(::AnalogChannel::AdmitNone);
}

::Signaling::Code
Configuration::AnalogChannel::rxSignalling() const {
  if (! has("rx-tone"))
    return Signaling::SIGNALING_NONE;

  if (get("rx-tone")->is<FloatItem>()) {
    double f = get("rx-tone")->as<FloatItem>()->value();
    return Signaling::fromCTCSSFrequency(f);
  } else if (get("rx-tone")->is<IntItem>()) {
    // handle as DCS
    qint64 num = get("rx-tone")->as<IntItem>()->value();
    return Signaling::fromDCSNumber(std::abs(num), num<0);
  }

  return Signaling::SIGNALING_NONE;
}


Signaling::Code
Configuration::AnalogChannel::txSignalling() const {
  if (! has("rx-tone"))
    return ::Signaling::SIGNALING_NONE;
  if (get("rx-tone")->is<FloatItem>()) {
    double f = get("rx-tone")->as<FloatItem>()->value();
    return Signaling::fromCTCSSFrequency(f);
  } else if (get("rx-tone")->is<IntItem>()) {
    // handle as DCS
    qint64 num = get("rx-tone")->as<IntItem>()->value();
    return Signaling::fromDCSNumber(std::abs(num), num<0);
  }
  return Signaling::SIGNALING_NONE;
}

::AnalogChannel::Bandwidth
Configuration::AnalogChannel::bandWidth() const {
  return get("band-width")->as<Enum>()->as<::AnalogChannel::Bandwidth>(::AnalogChannel::BWNarrow);
}

APRSPositioning *
Configuration::AnalogChannel::aprs() const {
  if (! has("aprs"))
    return nullptr;
  return get("aprs")->as<APRSPositioning>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigZoneItem::Declaration
 * ********************************************************************************************* */
Zone::Declaration *Zone::Declaration::_instance = nullptr;

Zone::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "Zone";
  add("name", new StringItem::Declaration(true, tr("Specifies the name of the zone.")));
  add("A",
      RefList::Declaration::get<Channel>(false, tr("Channel references.")));
  add("B",
      RefList::Declaration::get<Channel>(false, tr("Channel references.")));
}

Item *
Zone::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Zone();
}

Zone::Declaration *
Zone::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Defines a zone within the codeplug."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigZone
 * ********************************************************************************************* */
Zone::Zone(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "zone%1";
}


/* ********************************************************************************************* *
 * Implementation of ConfigScanListItem::Declaration
 * ********************************************************************************************* */
Configuration::ScanList::Declaration *
Configuration::ScanList::Declaration::_instance = nullptr;

Configuration::ScanList::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "ScanList";
  add("name", new StringItem::Declaration(true, tr("Name of the scan list.")));
  add("pimary", Reference::Declaration::get<Channel>(
        false, tr("Reference to the primary priority channel.")));
  add("secondary", Reference::Declaration::get<Channel>(
        false, tr("Reference to the secondary priority channel.")));
  add("revert", Reference::Declaration::get<Channel>(
        false, tr("Reference to the revert (transmit) channel.")));
  add("channels", RefList::Declaration::get<Channel>(
        false, tr("Reference to a channel.")));
}

Item *
Configuration::ScanList::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Configuration::ScanList();
}

Configuration::ScanList::Declaration *
Configuration::ScanList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(false, tr("Defines a scan list"));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigScanListItem
 * ********************************************************************************************* */
Configuration::ScanList::ScanList(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "scan%1";
}

const QString &
Configuration::ScanList::name() const {
  return get("name")->as<StringItem>()->value();
}

Configuration::Channel *
Configuration::ScanList::primary() const {
  return getRef<Configuration::Channel>("primary");
}

Configuration::Channel *
Configuration::ScanList::secondary() const {
  return getRef<Configuration::Channel>("secondary");
}

Configuration::Channel *
Configuration::ScanList::revert() const {
  return getRef<Configuration::Channel>("revert");
}

RefList *
Configuration::ScanList::channels() const {
  return get("channels")->as<RefList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigPositioning::Declaration
 * ********************************************************************************************* */
Positioning::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "Positioning";
  add("name", new StringItem::Declaration(true, tr("Specifies the name of the positioning system.")));
  add("period", new IntItem::Declaration(1, 1000, true, tr("Specifies the update period in seconds.")));
}

/* ********************************************************************************************* *
 * Implementation of ConfigPositioning
 * ********************************************************************************************* */
Positioning::Positioning(Declaration *declaraion, QObject *parent)
  : Object(declaraion, parent)
{
  _idTemplate = "pos%1";
}

const QString &
Positioning::name() const {
  return get("name")->as<StringItem>()->value();
}

uint32_t
Positioning::period() const {
  return get("period")->as<IntItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDMRPosItem::Declaration
 * ********************************************************************************************* */
DMRPositioning::Declaration *DMRPositioning::Declaration::_instance = nullptr;

DMRPositioning::Declaration::Declaration(bool mandatory, const QString &description)
  : Positioning::Declaration(mandatory, description)
{
  _name = "DMRPositioning";
  add("destination", Reference::Declaration::get<DigitalContact>(
        true, tr("Specifies the destination contact.")));
  add("channel", Reference::Declaration::get<DigitalChannel>(
        false, tr("Specifies the optional revert channel.")));
}

Item *
DMRPositioning::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new DMRPositioning();
}

DMRPositioning::Declaration *
DMRPositioning::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies a DMR positioning system."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigDMRPositioning
 * ********************************************************************************************* */
DMRPositioning::DMRPositioning(QObject *parent)
  : Positioning(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigAPRSPosItem::Declaration
 * ********************************************************************************************* */
APRSPositioning::Declaration *APRSPositioning::Declaration::_instance = nullptr;

APRSPositioning::Declaration::Declaration(bool mandatory, const QString &description)
  : Positioning::Declaration(mandatory, description)
{
  _name = "APRSPositioning";
  add("source", new StringItem::Declaration(true, tr("Specifies the source call and SSID.")));
  add("destination", new StringItem::Declaration(true, tr("Specifies the destination call and SSID.")));
  add("channel", Reference::Declaration::get<AnalogChannel>(
        true, tr("Specifies the optional revert channel.")));
  add("path", List::Declaration::get<StringItem>(
        new StringItem::Declaration(false, tr("Specifies a path element of the APRS packet.")),
        tr("Specifies the APRS path as a list.")));
  add("icon", new StringItem::Declaration(true, tr("Specifies the icon name.")));
  add("message", new StringItem::Declaration(false, tr("Specifies the optional APRS message.")));
}

Item *
APRSPositioning::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new APRSPositioning();
}

APRSPositioning::Declaration *
APRSPositioning::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(true, tr("Specifies an APRS positioning system."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAPRSPositioning
 * ********************************************************************************************* */
APRSPositioning::APRSPositioning(QObject *parent)
  : Positioning(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigRoamingZoneItem::Declaration
 * ********************************************************************************************* */
Configuration::RoamingZone::Declaration *
Configuration::RoamingZone::Declaration::_instance = nullptr;

Configuration::RoamingZone::Declaration::Declaration(bool mandatory, const QString &description)
  : Object::Declaration(mandatory, description)
{
  _name = "RoamingZone";
  add("name", new StringItem::Declaration(true, tr("Specifies name of the roaming zone.")));
  add("channels", RefList::Declaration::get<Configuration::DigitalChannel>(
        false, tr("References a channel")));
}

Item *
Configuration::RoamingZone::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Configuration::RoamingZone();
}

Configuration::RoamingZone::Declaration *
Configuration::RoamingZone::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration(false, tr("Specifies a roaming zone."));
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigRoamingZone
 * ********************************************************************************************* */
Configuration::RoamingZone::RoamingZone(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "roam%1";
}


/* ********************************************************************************************* *
 * Implementation of Configuration::Declaration
 * ********************************************************************************************* */
Configuration::Config::Declaration *
Configuration::Config::Declaration::_instance = nullptr;

Configuration::Config::Declaration::Declaration()
  : Map::Declaration(QHash<QString, Item::Declaration *>(), true, "")
{
  _name = "Config";
  add("version", new StringItem::Declaration(false, tr("Specifies the optional version number of the config format.")));
  add("intro-line1", new StringItem::Declaration(false, tr("Specifies the optional first boot display line.")));
  add("intro-line2", new StringItem::Declaration(false, tr("Specifies the optional second boot display line.")));

  add("radio-ids",
      List::Declaration::get<RadioId>(
        tr("This list specifies the DMR IDs and names for the radio.")));

  add("channels",
      List::Declaration::get<Channel>(
        new DispatchDeclaration({ {"analog", AnalogChannel::Declaration::get()},
                                  {"digital", DigitalChannel::Declaration::get()}},
                                true),
        tr("The list of all channels within the codeplug.")));

  add("zones",
      List::Declaration::get<Zone>(
        tr("Defines the list of zones.")));

  add("scan-lists",
      List::Declaration::get<ScanList>(
        tr("Defines the list of all scan lists.")));

  add("contacts",
      List::Declaration::get<Contact>(
        new DispatchDeclaration({ {"private", PrivateCall::Declaration::get()},
                                  {"group", GroupCall::Declaration::get()},
                                  {"all", AllCall::Declaration::get()} },
                                true, tr("One of the contact types, 'private', 'group', 'all'.")),
        tr("Specifies the list of contacts.")));

  add("group-lists",
      List::Declaration::get<GroupList>(
        tr("Lists all RX group lists within the codeplug.")));

  add("positioning",
      List::Declaration::get<Positioning>(
        new DispatchDeclaration({ {"dmr", DMRPositioning::Declaration::get()},
                                  {"aprs", APRSPositioning::Declaration::get()} },
                                false),
        tr("List of all positioning systems.")));

  add("roaming",
      List::Declaration::get<RoamingZone>(
        tr("List of all roaming zones.")));

}

bool
Configuration::Config::Declaration::verify(const YAML::Node &doc, QString &message) {
  QSet<QString> ctx;
  if (! this->verifyForm(doc, ctx, message))
    return false;
  if (! this->verifyReferences(doc, ctx, message))
    return false;
  return true;
}

bool
Configuration::Config::Declaration::parse(Config *config, YAML::Node &doc, QString &message) {
  if (! verify(doc, message))
    return false;

  QHash<QString, Config::Item *> ctx;
  if (! parsePopulate(config, doc, ctx, message))
    return false;
  if (! parseLink(config, doc, ctx, message))
    return false;

  return true;
}

Configuration::Config *
Configuration::Config::Declaration::parse(YAML::Node &doc, QString &message) {
  Item *item = parseAllocate(doc, message);
  if (nullptr == item)
    return nullptr;
  if (! parse(item->as<Config>(), doc, message)) {
    item->deleteLater();
    return nullptr;
  }
  return item->as<Config>();
}

Item *
Configuration::Config::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Config();
}

Configuration::Config::Declaration *
Configuration::Config::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Config::Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of Configuration
 * ********************************************************************************************* */
Configuration::Config::Config(QObject *parent)
  : Map(Declaration::get(), parent)
{
  // pass...
}

YAML::Node
Configuration::Config::generate(QString &message) const {
  QHash<const Item *, QString> ctx;
  if (! this->generateIds(ctx, message))
    return YAML::Node();
  return this->generateNode(ctx, message);
}
