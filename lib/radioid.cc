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
  if (ConfigObject *obj = get(idx))
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

void
RadioIDList::onDefaultIdDeleted() {
  _default = nullptr;
}
