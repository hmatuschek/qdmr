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
  : QObject(parent)
{
  // pass...
}

bool
AbstractConfigObjectList::add(ConfigObject *obj) {
  // Ignore nullptr
  if (nullptr == obj)
    return false;
  // If alread in list -> ignore
  if (0 <= indexOf(obj))
    return false;
  // Otherwise connect to object
  connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
  connect(obj, SIGNAL(modified(ConfigObject*)), this, SLOT(onElementModified(ConfigObject*)));
  return true;
}

bool
AbstractConfigObjectList::take(ConfigObject *obj) {
  // Ignore nullptr
  if (nullptr == obj)
    return false;
  // Otherwise disconnect from
  disconnect(obj, nullptr, this, nullptr);
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
    emit elementDeleted(idx);
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
ConfigObjectList::add(ConfigObject *obj) {
  if (AbstractConfigObjectList::add(obj))
    obj->setParent(this);
  return true;
}

bool
ConfigObjectList::take(ConfigObject *obj) {
  if (AbstractConfigObjectList::take(obj))
    obj->setParent(nullptr);
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

