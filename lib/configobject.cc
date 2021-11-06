#include "configobject.hh"
#include "configreference.hh"
#include "logger.hh"

#include <QMetaProperty>
#include <QMetaEnum>


/* ********************************************************************************************* *
 * Implementation of ConfigObject::Context
 * ********************************************************************************************* */
QHash<QString, QHash<QString, ConfigObject *>> ConfigObject::Context::_tagObjects =
    QHash<QString, QHash<QString, ConfigObject *>>();
QHash<QString, QHash<ConfigObject *, QString>> ConfigObject::Context::_tagNames =
    QHash<QString, QHash<ConfigObject *, QString>>();

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

bool
ConfigObject::Context::hasTag(const QString &className, const QString &property, const QString &tag) {
  QString qname = className+"::"+property;
  return _tagObjects.contains(qname) && _tagObjects[qname].contains(tag);
}

bool
ConfigObject::Context::hasTag(const QString &className, const QString &property, ConfigObject *obj) {
  QString qname = className+"::"+property;
  return _tagNames.contains(qname) && _tagNames[qname].contains(obj);
}

ConfigObject *
ConfigObject::Context::getTag(const QString &className, const QString &property, const QString &tag) {
  //logDebug() << "Request " << tag << " for " << property << " in " << className << ".";
  QString qname = className+"::"+property;
  if (! _tagObjects.contains(qname))
    return nullptr;
  return _tagObjects[qname].value(tag, nullptr);
}

QString
ConfigObject::Context::getTag(const QString &className, const QString &property, ConfigObject *obj) {
  //logDebug() << "Request tag for " << property << " in " << className << ".";
  QString qname = className+"::"+property;
  if (! _tagNames.contains(qname))
    return nullptr;
  return _tagNames[qname].value(obj);
}

void
ConfigObject::Context::setTag(const QString &className, const QString &property, const QString &tag, ConfigObject *obj) {
  //logDebug() << "Register tag " << tag << " for " << property << " in " << className << ".";
  QString qname = className+"::"+property;
  if (! _tagObjects.contains(qname))
    _tagObjects[qname] = QHash<QString, ConfigObject*>();
  _tagObjects[qname].insert(tag, obj);
  if (! _tagNames.contains(qname))
    _tagNames[qname] = QHash<ConfigObject*, QString>();
  _tagNames[qname].insert(obj, tag);
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

  // With empty ID base, skip labeling.
  if (_idBase.isEmpty())
    return true;

  unsigned n=1;
  QString id=QString("%1%2").arg(_idBase).arg(n);
  while (context.contains(id)) {
    id=QString("%1%2").arg(_idBase).arg(++n);
  }

  return context.add(id, this);
}

YAML::Node
ConfigObject::serialize(const Context &context) {
  YAML::Node node;
  if (! populate(node, context))
    return YAML::Node();
  return node;
}

void
ConfigObject::clear() {
  foreach (ConfigObject *extension, _extensions)
    extension->deleteLater();
  _extensions.clear();
}

bool
ConfigObject::populate(YAML::Node &node, const Context &context){
  if (context.contains(this))
    node["id"] = context.getId(this).toStdString();

  // Serialize all properties
  const QMetaObject *meta = metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (prop.isEnumType()) {
      QMetaEnum e = prop.enumerator();
      QVariant value = prop.read(this);
      const char *key = e.valueToKey(value.toInt());
      if (nullptr == key) {
        logError() << "Cannot map value " << value.toUInt()
                   << " to enum " << e.name()
                   << ". Ignore attribute but this points to an incompatibility in some codeplug. "
                   << "Consider reporting it to https://github.com/hmatuschek/qdmr/issues.";
        continue;
      }
      node[prop.name()] = key;
    } else if (QString("bool") == prop.typeName()) {
      node[prop.name()] = this->property(prop.name()).toBool();
    } else if (QString("int") == prop.typeName()) {
      node[prop.name()] = this->property(prop.name()).toInt();
    } else if (QString("uint") == prop.typeName()) {
      node[prop.name()] = this->property(prop.name()).toUInt();
    } else if (QString("double") == prop.typeName()) {
      node[prop.name()] = this->property(prop.name()).toDouble();
    } else if (QString("QString") == prop.typeName()) {
      node[prop.name()] = this->property(prop.name()).toString().toStdString();
    } else if (prop.read(this).value<ConfigObjectReference *>()) {
      ConfigObjectReference *ref = prop.read(this).value<ConfigObjectReference *>();
      ConfigObject *obj = ref->as<ConfigObject>();
      if (nullptr == obj)
        continue;
      if (context.hasTag(prop.enclosingMetaObject()->className(), prop.name(), obj)) {
        YAML::Node tag(YAML::NodeType::Scalar);
        tag.SetTag(context.getTag(prop.enclosingMetaObject()->className(), prop.name(), obj).toStdString());
        node[prop.name()] = tag;
        continue;
      } else if (! context.contains(obj)) {
        logError() << "Cannot reference object of type " << obj->metaObject()->className()
                   << " object not labeled.";
        return false;
      }
      node[prop.name()] = context.getId(obj).toStdString();
    } else if (prop.read(this).value<ConfigObjectRefList *>()) {
      ConfigObjectRefList *refs = prop.read(this).value<ConfigObjectRefList *>();
      //logDebug() << "Serialize obj list w/ " << refs->count() << " elements." ;
      YAML::Node list = YAML::Node(YAML::NodeType::Sequence);
      list.SetStyle(YAML::EmitterStyle::Flow);
      for (int i=0; i<refs->count(); i++) {
        ConfigObject *obj = refs->get(i);
        if (context.hasTag(prop.enclosingMetaObject()->className(), prop.name(), obj)) {
          YAML::Node tag(YAML::NodeType::Scalar);
          tag.SetTag(context.getTag(prop.enclosingMetaObject()->className(), prop.name(), obj).toStdString());
          tag = tag.Tag().substr(1);
          list.push_back(tag);
          continue;
        } else if (! context.contains(obj)) {
          logError() << "Cannot reference object of type " << obj->metaObject()->className()
                     << " object not labeled.";
          return false;
        }
        list.push_back(context.getId(obj).toStdString());
      }
      node[prop.name()] = list;
    } else {
      logDebug() << "Unhandled property " << prop.name()
                 << " of unknown type " << prop.typeName() << ".";
    }
  }

  // Serialize extensions
  foreach (QString name, _extensions.keys()) {
    YAML::Node extNode = _extensions[name]->serialize(context);
    if (extNode.IsNull())
      return false;
    node[name.toStdString()] = extNode;
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
 * Implementation of ConfigExtension
 * ********************************************************************************************* */
ConfigExtension::ConfigExtension(const QString &idBase, QObject *parent)
  : ConfigObject(idBase, parent)
{
  // pass...
}

bool
ConfigExtension::populate(YAML::Node &node, const Context &context) {
  // Call parent method
  if (! ConfigObject::populate(node, context))
    return false;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AbstractConfigObjectList
 * ********************************************************************************************* */
AbstractConfigObjectList::AbstractConfigObjectList(const QMetaObject &elementType, QObject *parent)
  : QObject(parent), _elementType(elementType), _items()
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
  for (int i=(count()-1); i>=0; i--) {
    _items.pop_back();
    emit elementRemoved(i);
  }
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
  // Check type
  if (! obj->inherits(_elementType.className())) {
    logError() << "Cannot add element of type " << obj->metaObject()->className()
               << " to list, expected instances of " << _elementType.className();
    return false;
  }
  _items.insert(row, obj);
  // Otherwise connect to object
  connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(onElementDeleted(QObject*)));
  connect(obj, SIGNAL(modified(ConfigObject*)), this, SLOT(onElementModified(ConfigObject*)));
  emit elementAdded(row);
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
  return true;
}

bool
AbstractConfigObjectList::moveUp(int first, int last) {
  if ((first <= 0) || (last>=count()))
    return false;
  for (int row=first; row<=last; row++)
    std::swap(_items[row-1], _items[row]);
  return true;
}

bool
AbstractConfigObjectList::moveDown(int row) {
  if ((row >= (count()-1)) || (0 > row))
    return false;
  std::swap(_items[row+1], _items[row]);
  return true;
}

bool
AbstractConfigObjectList::moveDown(int first, int last) {
  if ((last >= (count()-1)) || (0 > first))
    return false;
  for (int row=last; row>=first; row--)
    std::swap(_items[row+1], _items[row]);
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
  if (0 <= idx)
    emit elementRemoved(idx);
}


/* ********************************************************************************************* *
 * Implementation of ConfigObjectList
 * ********************************************************************************************* */
ConfigObjectList::ConfigObjectList(const QMetaObject &elementType, QObject *parent)
  : AbstractConfigObjectList(elementType, parent)
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

void
ConfigObjectList::clear() {
  QVector<ConfigObject *> items = _items;
  AbstractConfigObjectList::clear();
  for (int i=0; i<items.count(); i++)
    items[i]->deleteLater();
}


/* ********************************************************************************************* *
 * Implementation of ConfigObjectRefList
 * ********************************************************************************************* */
ConfigObjectRefList::ConfigObjectRefList(const QMetaObject &elementType, QObject *parent)
  : AbstractConfigObjectList(elementType, parent)
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

