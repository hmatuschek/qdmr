#include "radioid.hh"
#include "logger.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of RadioID
 * ********************************************************************************************* */
RadioID::RadioID(QObject *parent)
  : ConfigObject(parent)
{
  // pass...
}

RadioID::RadioID(const QString &name, QObject *parent)
  : ConfigObject(name, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DMRRadioID
 * ********************************************************************************************* */
DMRRadioID::DMRRadioID(QObject *parent)
  : RadioID(parent), _number(0)
{
  // pass...
}

DMRRadioID::DMRRadioID(const QString &name, uint32_t id, QObject *parent)
  : RadioID(name, parent), _number(id)
{
  // pass...
}

ConfigItem *
DMRRadioID::clone() const {
  DMRRadioID *id = new DMRRadioID();
  if (! id->copy(*this)) {
    id->deleteLater();
    return nullptr;
  }
  return id;
}

uint32_t
DMRRadioID::number() const {
  return _number;
}

void
DMRRadioID::setNumber(uint32_t id) {
  if (id == _number)
    return;
  _number = id;
  emit modified(this);
}

YAML::Node
DMRRadioID::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = RadioID::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  node.SetStyle(YAML::EmitterStyle::Flow);
  type["dmr"] = node;

  return type;
}

bool
DMRRadioID::parse(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse radio id: Expected object with one child.";
    return false;
  }

  return ConfigObject::parse(node.begin()->second, ctx, err);
}

bool
DMRRadioID::link(const YAML::Node &node, const ConfigItem::Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot link radio id: Expected object with one child.";
    return false;
  }

  return ConfigObject::link(node.begin()->second, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of DefaultRadioID
 * ********************************************************************************************* */
DefaultRadioID *DefaultRadioID::_instance = nullptr;

DefaultRadioID::DefaultRadioID(QObject *parent)
  : DMRRadioID(tr("[Default]"),0,parent)
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
 * Implementation of DTMFRadioID
 * ********************************************************************************************* */
DTMFRadioID::DTMFRadioID(QObject *parent)
  : RadioID(parent)
{
  // pass...
}

DTMFRadioID::DTMFRadioID(const QString &name, const QString &number, QObject *parent)
  : RadioID(name, parent), _number()
{
  setNumber(number.simplified());
}

ConfigItem *
DTMFRadioID::clone() const {
  DTMFRadioID *newId = new DTMFRadioID();
  if (! newId->copy(*this)) {
    newId->deleteLater();
    return nullptr;
  }
  return newId;
}

const QString &
DTMFRadioID::number() const {
  return _number;
}
void
DTMFRadioID::setNumber(const QString &number) {
  if (! validDTMFNumber(number))
    return;
  _number = number.simplified();
  emit modified(this);
  return;
}


/* ********************************************************************************************* *
 * Implementation of RadioIDList
 * ********************************************************************************************* */
RadioIDList::RadioIDList(QObject *parent)
  : ConfigObjectList(DMRRadioID::staticMetaObject, parent), _default(nullptr)
{
  // pass...
}

void
RadioIDList::clear() {
  setDefaultId(-1);
  ConfigObjectList::clear();
}

DMRRadioID *
RadioIDList::getId(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<DMRRadioID>();
  return nullptr;
}

DMRRadioID *
RadioIDList::defaultId() const {
  return _default;
}

DMRRadioID *
RadioIDList::find(uint32_t id) const {
  for (int i=0; i<count(); i++) {
    if (id == getId(i)->number())
      return getId(i);
  }
  return nullptr;
}

int
RadioIDList::add(ConfigObject *obj, int row) {
  if ((nullptr == obj) || (! obj->is<DMRRadioID>()))
    return -1;

  bool was_empty = (0 == count());
  int idx = ConfigObjectList::add(obj, row);
  if (0 > idx)
    return idx;
  // automatically select first added ID as default
  if (was_empty && (nullptr == _default)) {
    logDebug() << "Automatically set default radio id to " << obj->as<DMRRadioID>()->name() << ".";
    setDefaultId(idx);
  }
  return idx;
}

int
RadioIDList::addId(const QString &name, uint32_t id) {
  return add(new DMRRadioID(name, id, this));
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
RadioIDList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)

  if (! node)
    return nullptr;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create radio id: Expected object with one child.";
    return nullptr;
  }

  QString type = QString::fromStdString(node.begin()->first.as<std::string>());
  if ("dmr" == type) {
    return new DMRRadioID();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create radio id: Unknown type '" << type << "'.";

  return nullptr;
}

void
RadioIDList::onDefaultIdDeleted() {
  _default = nullptr;
}
