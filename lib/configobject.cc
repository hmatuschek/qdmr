#include "configobject.hh"

/* ********************************************************************************************* *
 * Implementation of ConfigObject::Context
 * ********************************************************************************************* */
ConfigObject::Context::Context()
  : _version(), _objects(), _ids()
{
  // pass...
}

ConfigObject::Context::~Context() {
  // pass...
}

const QString &
ConfigObject::Context::version() const {
  return _version;
}
void
ConfigObject::Context::setVersion(const QString &ver) {
  _version = ver;
}

bool
ConfigObject::Context::contains(ConfigObject *obj) const {
  return _ids.contains(obj);
}

bool
ConfigObject::Context::contains(const QString &id) const {
  return _objects.contains(id);
}

QString
ConfigObject::Context::getId(ConfigObject *obj) const {
  return _ids.value(obj, "");
}

ConfigObject *
ConfigObject::Context::getObj(const QString &id) const {
  return _objects.value(id, nullptr);
}

bool
ConfigObject::Context::add(const QString &id, ConfigObject *obj) {
  if (_objects.contains(id) || _ids.contains(obj))
    return false;
  _objects.insert(id, obj);
  _ids.insert(obj, id);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigObject
 * ********************************************************************************************* */
ConfigObject::ConfigObject(const QString &idBase, QObject *parent)
  : QObject(parent), _idBase(idBase), _extensions()
{
  // pass...
}

bool
ConfigObject::label(Context &context) {
  foreach(QString name, _extensions.keys()) {
    _extensions[name]->label(context);
  }

  uint n=1;
  QString id=QString("%1%2").arg(_idBase).arg(n);
  while (context.contains(id)) {
    id=QString("%1%2").arg(_idBase).arg(++n);
  }

  return context.add(id, this);
}

YAML::Node
ConfigObject::serialize(const Context &context) {
  YAML::Node node;
  if (! serialize(node, context))
    return YAML::Node();
  return node;
}

bool
ConfigObject::serialize(YAML::Node &node, const Context &context){
  if (context.contains(this))
    node["id"] = context.getId(this).toStdString();

  foreach (QString name, _extensions.keys()) {
    YAML::Node extNode = _extensions[name]->serialize(context);
    if (extNode.IsNull())
      return false;
    node["name"] = extNode;
  }
  return true;
}

bool
ConfigObject::hasExtension(const QString &name) const {
  return _extensions.contains(name);
}

QList<QString>
ConfigObject::extensionNames() const {
  return _extensions.keys();
}

const ConfigObject *
ConfigObject::extension(const QString &name) const {
  return _extensions.value(name, nullptr);
}

ConfigObject *
ConfigObject::extension(const QString &name) {
  return _extensions.value(name, nullptr);
}

void
ConfigObject::addExtension(const QString &name, ConfigObject *ext) {
  if (nullptr == ext)
    return;
  if (hasExtension(name))
    _extensions[name]->deleteLater();
  _extensions.insert(name, ext);
  ext->setParent(this);
}


/* ********************************************************************************************* *
 * Implementation of AbstractConfigObjectList
 * ********************************************************************************************* */
AbstractConfigObjectList::AbstractConfigObjectList(QObject *parent)
  : QObject(parent), _items()
{
  // pass...
}

int
AbstractConfigObjectList::count() const {
  return _items.count();
}

int
AbstractConfigObjectList::indexOf(ConfigObject *obj) const {
  return _items.indexOf(obj);
}

void
AbstractConfigObjectList::clear() {
  _items.clear();
  emit modified();
}

ConfigObject *
AbstractConfigObjectList::get(int idx) const {
  return _items.value(idx, nullptr);
}

int AbstractConfigObjectList::add(ConfigObject *obj, int row) {
  // Ignore nullptr
  if (nullptr == obj)
    return -1;
  // If alread in list -> ignore
  if (0 <= indexOf(obj))
    return -1;
  if (-1 == row)
    row = _items.size();
  _items.insert(row, obj);
  // Otherwise connect to object
  connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
  connect(obj, SIGNAL(modified(ConfigObject*)), this, SLOT(onElementModified(ConfigObject*)));
  connect(obj, SIGNAL(modified(ConfigObject*)), this, SIGNAL(modified()));
  emit modified();
  return row;
}

bool
AbstractConfigObjectList::take(ConfigObject *obj) {
  // Ignore nullptr
  if (nullptr == obj)
    return false;
  int idx = indexOf(obj);
  if (0 > idx)
    return false;
  _items.remove(idx, 1);
  emit elementRemoved(idx);
  // Otherwise disconnect from
  disconnect(obj, nullptr, this, nullptr);
  emit modified();
  return true;
}

bool
AbstractConfigObjectList::del(ConfigObject *obj) {
  return take(obj);
}

bool
AbstractConfigObjectList::moveUp(int row) {
  if ((row <= 0) || (row>=count()))
    return false;
  std::swap(_items[row-1], _items[row]);
  emit modified();
  return true;
}

bool
AbstractConfigObjectList::moveUp(int first, int last) {
  if ((first <= 0) || (last>=count()))
    return false;
  for (int row=first; row<=last; row++)
    std::swap(_items[row-1], _items[row]);
  emit modified();
  return true;
}

bool
AbstractConfigObjectList::moveDown(int row) {
  if ((row >= (count()-1)) || (0 > row))
    return false;
  std::swap(_items[row+1], _items[row]);
  emit modified();
  return true;
}

bool
AbstractConfigObjectList::moveDown(int first, int last) {
  if ((last >= (count()-1)) || (0 > first))
    return false;
  for (int row=last; row>=first; row--)
    std::swap(_items[row+1], _items[row]);
  emit modified();
  return true;
}

void
AbstractConfigObjectList::onElementModified(ConfigObject *obj) {
  int idx = indexOf(obj);
  if (0 >= idx)
    emit elementModified(idx);
}

void
AbstractConfigObjectList::onElementDeleted(QObject *obj) {
  int idx = indexOf(qobject_cast<ConfigObject *>(obj));
  if (0 >= idx)
    emit elementRemoved(idx);
}


/* ********************************************************************************************* *
 * Implementation of ConfigObjectList
 * ********************************************************************************************* */
ConfigObjectList::ConfigObjectList(QObject *parent)
  : AbstractConfigObjectList(parent)
{
  // pass...
}

bool
ConfigObjectList::label(ConfigObject::Context &context) {
  foreach (ConfigObject *obj, _items) {
    if (! obj->label(context))
      return false;
  }
  return true;
}

YAML::Node
ConfigObjectList::serialize(const ConfigObject::Context &context) {
  YAML::Node list(YAML::NodeType::Sequence);
  foreach (ConfigObject *obj, _items) {
    YAML::Node node = obj->serialize(context);
    if (node.IsNull())
      return node;
    list.push_back(node);
  }
  return list;
}

int ConfigObjectList::add(ConfigObject *obj, int row) {
  if (0 <= (row = AbstractConfigObjectList::add(obj, row)))
    obj->setParent(this);
  return row;
}

bool
ConfigObjectList::take(ConfigObject *obj) {
  if (AbstractConfigObjectList::take(obj))
    obj->setParent(nullptr);
  return true;
}

bool
ConfigObjectList::del(ConfigObject *obj) {
  if (AbstractConfigObjectList::del(obj))
    obj->deleteLater();
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigObjectRefList
 * ********************************************************************************************* */
ConfigObjectRefList::ConfigObjectRefList(QObject *parent)
  : AbstractConfigObjectList(parent)
{
  // pass...
}

bool
ConfigObjectRefList::label(ConfigObject::Context &context) {
  // pass...
  return true;
}

YAML::Node
ConfigObjectRefList::serialize(const ConfigObject::Context &context) {
  YAML::Node list(YAML::NodeType::Sequence);
  foreach (ConfigObject *obj, _items) {
    if (! context.contains(obj))
      return YAML::Node();
    list.push_back(context.getId(obj).toStdString());
  }
  return list;
}

