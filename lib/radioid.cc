#include "radioid.hh"
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of RadioID
 * ********************************************************************************************* */
RadioID::RadioID(const QString &name, uint32_t id, QObject *parent)
  : ConfigObject("id", parent), _name(name), _number(id)
{
  // pass...
}

RadioID::RadioID(QObject *parent)
  : ConfigObject("id", parent), _name(""), _number(0)
{
  // pass...
}

const QString &
RadioID::name() const {
  return _name;
}

void
RadioID::setName(const QString &name) {
  _name = name.simplified();
}

uint32_t
RadioID::number() const {
  return _number;
}

void
RadioID::setNumber(uint32_t id) {
  if (id == _number)
    return;
  _number = id;
  emit modified();
}

YAML::Node
RadioID::serialize(const Context &context) {
  YAML::Node node = ConfigObject::serialize(context);
  if (node.IsNull())
    return node;
  YAML::Node type;
  node.SetStyle(YAML::EmitterStyle::Flow);
  type["dmr"] = node;
  return type;
}

ConfigItem *
RadioID::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx)
  // No extensions defined yet for RadioID
  return nullptr;
}

bool
RadioID::parse(const YAML::Node &node, ConfigItem::Context &ctx) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg() << node.Mark().line << ":" << node.Mark().column
             << ": Cannot parse radio id: Expected object with one child.";
    return false;
  }

  return ConfigObject::parse(node.begin()->second, ctx);
}

bool
RadioID::link(const YAML::Node &node, const ConfigItem::Context &ctx) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg() << node.Mark().line << ":" << node.Mark().column
             << ": Cannot link radio id: Expected object with one child.";
    return false;
  }

  return ConfigObject::link(node.begin()->second, ctx);
}


/* ********************************************************************************************* *
 * Implementation of DefaultRadioID
 * ********************************************************************************************* */
DefaultRadioID *DefaultRadioID::_instance = nullptr;

DefaultRadioID::DefaultRadioID(QObject *parent)
  : RadioID(tr("[Default]"),0,parent)
{
  // pass...
}

DefaultRadioID *
DefaultRadioID::get() {
  if (nullptr == _instance)
    _instance = new DefaultRadioID();
  return _instance;
}


/* ********************************************************************************************* *
 * Implementation of RadioIDList
 * ********************************************************************************************* */
RadioIDList::RadioIDList(QObject *parent)
  : ConfigObjectList(RadioID::staticMetaObject, parent), _default(nullptr)
{
  // pass...
}

void
RadioIDList::clear() {
  ConfigObjectList::clear();
  // default will be removed through deleted signal.
}

RadioID *
RadioIDList::getId(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<RadioID>();
  return nullptr;
}

RadioID *
RadioIDList::defaultId() const {
  return _default;
}

RadioID *
RadioIDList::find(uint32_t id) const {
  for (int i=0; i<count(); i++) {
    if (id == getId(i)->number())
      return getId(i);
  }
  return nullptr;
}

int
RadioIDList::add(ConfigObject *obj, int row) {
  if ((nullptr == obj) || (! obj->is<RadioID>()))
    return -1;

  bool was_empty = (0 == count());
  int idx = ConfigObjectList::add(obj, row);
  if (0 > idx)
    return idx;
  // automatically select first added ID as default
  if (was_empty && (nullptr == _default)) {
    logDebug() << "Automatically set default radio id to " << obj->as<RadioID>()->name() << ".";
    setDefaultId(idx);
  }
  return idx;
}

int
RadioIDList::addId(const QString &name, uint32_t id) {
  return add(new RadioID(name, id, this));
}

bool
RadioIDList::setDefaultId(int idx) {
  if (_default) {
    disconnect(_default, SIGNAL(destroyed(QObject*)), this, SLOT(onDefaultIdDeleted()));
    if (0 <= indexOf(_default))
      emit elementModified(indexOf(_default));
  }

  if (0 > idx) {
    _default = nullptr;
    return true;
  }

  _default = getId(idx);
  if (nullptr == _default)
    return false;
  connect(_default, SIGNAL(destroyed(QObject*)), this, SLOT(onDefaultIdDeleted()));
  emit elementModified(idx);
  return true;
}

bool
RadioIDList::delId(uint32_t id) {
  return del(find(id));
}


ConfigItem *
RadioIDList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx) {
  Q_UNUSED(ctx)

  if (! node)
    return nullptr;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg() << node.Mark().line << ":" << node.Mark().column
             << ": Cannot create radio id: Expected object with one child.";
    return nullptr;
  }

  QString type = QString::fromStdString(node.begin()->first.as<std::string>());
  if ("dmr" == type) {
    return new RadioID();
  }

  errMsg() << node.Mark().line << ":" << node.Mark().column
           << ": Cannot create radio id: Unknown type '" << type << "'.";

  return nullptr;
}

void
RadioIDList::onDefaultIdDeleted() {
  _default = nullptr;
}
