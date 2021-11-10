#include "configobject.hh"
#include "configreference.hh"
#include "logger.hh"

#include <QMetaProperty>
#include <QMetaEnum>

// Helper function to extract key names for a QMetaEnum
inline QStringList enumKeys(const QMetaEnum &e) {
  QStringList lst;
  for (int i=0; i<e.keyCount(); i++)
    lst.push_back(e.key(i));
  return lst;
}

template <class T>
bool propIsInstance(QMetaProperty &prop) {
  if (QMetaType::UnknownType == prop.userType())
    return false;
  QMetaType type(prop.userType());
  if (! (QMetaType::PointerToQObject & type.flags()))
    return false;
  const QMetaObject *propType = type.metaObject();
  for (; nullptr != propType; propType = propType->superClass()) {
    if (0==strcmp(T::staticMetaObject.className(), propType->className()))
      return true;
  }
  return false;
}

/* ********************************************************************************************* *
 * Implementation of ConfigObject::Context
 * ********************************************************************************************* */
QHash<QString, QHash<QString, ConfigObject *>> ConfigObject::Context::_tagObjects =
    QHash<QString, QHash<QString, ConfigObject *>>();
QHash<QString, QHash<ConfigObject *, QString>> ConfigObject::Context::_tagNames =
    QHash<QString, QHash<ConfigObject *, QString>>();

ConfigItem::Context::Context()
  : _version(), _objects(), _ids()
{
  // pass...
}

ConfigItem::Context::~Context() {
  // pass...
}

const QString &
ConfigItem::Context::version() const {
  return _version;
}
void
ConfigItem::Context::setVersion(const QString &ver) {
  _version = ver;
}

bool
ConfigItem::Context::contains(ConfigObject *obj) const {
  return _ids.contains(obj);
}

bool
ConfigItem::Context::contains(const QString &id) const {
  return _objects.contains(id);
}

QString
ConfigItem::Context::getId(ConfigObject *obj) const {
  return _ids.value(obj, "");
}

ConfigObject *
ConfigItem::Context::getObj(const QString &id) const {
  return _objects.value(id, nullptr);
}

bool
ConfigItem::Context::add(const QString &id, ConfigObject *obj) {
  if (_objects.contains(id) || _ids.contains(obj))
    return false;
  _objects.insert(id, obj);
  _ids.insert(obj, id);
  return true;
}

bool
ConfigItem::Context::hasTag(const QString &className, const QString &property, const QString &tag) {
  QString qname = className+"::"+property;
  return _tagObjects.contains(qname) && _tagObjects[qname].contains(tag);
}

bool
ConfigItem::Context::hasTag(const QString &className, const QString &property, ConfigObject *obj) {
  QString qname = className+"::"+property;
  return _tagNames.contains(qname) && _tagNames[qname].contains(obj);
}

ConfigObject *
ConfigItem::Context::getTag(const QString &className, const QString &property, const QString &tag) {
  //logDebug() << "Request " << tag << " for " << property << " in " << className << ".";
  QString qname = className+"::"+property;
  if (! _tagObjects.contains(qname))
    return nullptr;
  return _tagObjects[qname].value(tag, nullptr);
}

QString
ConfigItem::Context::getTag(const QString &className, const QString &property, ConfigObject *obj) {
  //logDebug() << "Request tag for " << property << " in " << className << ".";
  QString qname = className+"::"+property;
  if (! _tagNames.contains(qname))
    return nullptr;
  return _tagNames[qname].value(obj);
}

void
ConfigItem::Context::setTag(const QString &className, const QString &property, const QString &tag, ConfigObject *obj) {
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
 * Implementation of ConfigItem
 * ********************************************************************************************* */
ConfigItem::ConfigItem(QObject *parent)
  : QObject(parent), ErrorStack(), _extensions()
{
  // pass...
}

bool
ConfigItem::label(ConfigObject::Context &context) {
  foreach(ConfigItem *ext, _extensions) {
    ext->label(context);
  }

  // Label properties owning config objects, that is of type ConfigObject or ConfigObjectList
  const QMetaObject *meta = metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (prop.read(this).value<ConfigObjectList *>()) {
      ConfigObjectList *lst = prop.read(this).value<ConfigObjectList *>();
      if (! lst->label(context))
        return false;
    } else if (prop.read(this).value<ConfigItem *>()) {
      ConfigItem *obj = prop.read(this).value<ConfigItem *>();
      if (! obj->label(context))
        return false;
    }
  }

  return true;
}


YAML::Node
ConfigItem::serialize(const Context &context) {
  YAML::Node node;
  if (! populate(node, context))
    return YAML::Node();
  return node;
}

void
ConfigItem::clear() {
  foreach (ConfigItem *extension, _extensions)
    extension->deleteLater();
  _extensions.clear();

  // Delete or clear all object owned by properites, that is ConfigObjectList and ConfigObject
  const QMetaObject *meta = metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (ConfigObjectList *lst = prop.read(this).value<ConfigObjectList *>()) {
      lst->clear();
    } else if (ConfigItem *obj = prop.read(this).value<ConfigItem *>()) {
      obj->deleteLater();
    }
  }

}

bool
ConfigItem::populate(YAML::Node &node, const Context &context){
  // Serialize all properties
  const QMetaObject *meta = metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (! prop.isScriptable()) {
      logDebug() << "Do not serialize property '"
                 << prop.name() << "': Marked as not scriptable.";
      continue;
    }
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
    } else if (ConfigObjectReference *ref = prop.read(this).value<ConfigObjectReference *>()) {
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
    } else if (ConfigObjectRefList *refs = prop.read(this).value<ConfigObjectRefList *>()) {
      //logDebug() << "Serialize obj ref list w/ " << refs->count() << " elements." ;
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
    } else if (ConfigItem *obj = prop.read(this).value<ConfigItem *>()) {
      // Serialize config objects in-place.
      node[prop.name()] = obj->serialize(context);
    } else if (ConfigObjectList *lst = prop.read(this).value<ConfigObjectList *>()) {
      // Serialize config object lists in-place.
      node[prop.name()] = lst->serialize(context);
    } else {
      logDebug() << "Unhandled property " << prop.name()
                 << " of unknown type " << prop.typeName() << ".";
    }
  }

  // Serialize extensions
  foreach (QString name, _extensionTable.keys()) {
    YAML::Node extNode = _extensionTable[name]->serialize(context);
    if (extNode.IsNull())
      return false;
    node[name.toStdString()] = extNode;
  }
  return true;
}

bool
ConfigItem::parse(const YAML::Node &node, ConfigItem::Context &ctx) {
  Q_UNUSED(ctx)

  if (! node)
    return false;

  if (! node.IsMap()) {
    errMsg() << node.Mark().line << ":" << node.Mark().column
             << ": Cannot parse element: Expected object.";
    return false;
  }

  const QMetaObject *meta = this->metaObject();
  for (int p=QObject::staticMetaObject.propertyOffset(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);

    if (! prop.isValid())
      continue;
    // If marked as non-scriptable, skip that property.
    // It is handled separately or not at all.
    if (! prop.isScriptable())
      continue;

    if (prop.isEnumType()) {
      // If property is not set -> skip
      if (! node[prop.name()])
        continue;
      // parse & check enum key
      if (! node[prop.name()].IsScalar()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className()
                 << ": Expected enum key.";
        return false;
      }
      QMetaEnum e = prop.enumerator();
      std::string key = node[prop.name()].as<std::string>();
      bool ok=true; int value = e.keyToValue(key.c_str(), &ok);
      if (! ok) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Unknown key '" << key.c_str() << "' for enum '" << prop.name()
                 << "'. Expected one of " << enumKeys(e).join(", ") << ".";
        return false;
      }
      // finally set property
      prop.write(this, value);
    } else if (QString("bool") == prop.typeName()) {
      // If property is not set -> skip
      if (! node[prop.name()])
        continue;
      // parse & check type
      if (! node[prop.name()].IsScalar()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className()
                 << ": Expected boolean value.";
        return false;
      }
      prop.write(this, node[prop.name()].as<bool>());
    } else if (QString("int") == prop.typeName()) {
      // If property is not set -> skip
      if (! node[prop.name()])
        continue;
      // parse & check type
      if (! node[prop.name()].IsScalar()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className()
                 << ": Expected integer value.";
        return false;
      }
      prop.write(this, node[prop.name()].as<int>());
    } else if (QString("uint") == prop.typeName()) {
      // If property is not set -> skip
      if (! node[prop.name()])
        continue;
      // parse & check type
      if (! node[prop.name()].IsScalar()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className()
                 << ": Expected unsigned integer value.";
        return false;
      }
      prop.write(this, node[prop.name()].as<unsigned>());
    } else if (QString("double") == prop.typeName()) {
      // If property is not set -> skip
      if (! node[prop.name()])
        continue;
      // parse & check type
      if (! node[prop.name()].IsScalar()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className()
                 << ": Expected floating point value.";
        return false;
      }
      prop.write(this, node[prop.name()].as<double>());
    } else if (QString("QString") == prop.typeName()) {
      // If property is not set -> skip
      if (! node[prop.name()])
        continue;
      // parse & check type
      if (! node[prop.name()].IsScalar()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className()
                 << ": Expected string.";
        return false;
      }
      prop.write(this, QString::fromStdString(node[prop.name()].as<std::string>()));
    } else if (prop.read(this).value<ConfigObjectReference *>()) {
      // references are linked later
      continue;
    } else if (prop.read(this).value<ConfigObjectRefList *>()) {
      // reference lists are linked later
      continue;
    } else if (propIsInstance<ConfigItem>(prop)) {
      if (! node[prop.name()])
        continue;
      // check type
      if (! node[prop.name()].IsMap()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className()
                 << ": Expected instance of '"
                 << QMetaType::metaObjectForType(prop.userType())->className() << "'.";
        return false;
      }
      // allocate instance
      ConfigItem *obj = this->allocateChild(prop, node[prop.name()], ctx);
      if (nullptr == obj) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot allocate " << prop.name() << " of " << meta->className() << ".";
        return false;
      }
      // parse instance
      if (! obj->parse(node[prop.name()], ctx)) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot parse " << prop.name() << " of " << meta->className() << ".";
        obj->deleteLater();
        return false;
      }
      // Set property
      prop.write(this, QVariant::fromValue(obj));
    }
  }

  return true;
}

bool
ConfigItem::link(const YAML::Node &node, const ConfigItem::Context &ctx) {
  Q_UNUSED(ctx)

  const QMetaObject *meta = this->metaObject();

  for (int p=QObject::staticMetaObject.propertyOffset(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (! prop.isScriptable()) {
      //logDebug() << "Do not link property '" << prop.name() << "': Marked as not scriptable.";
      continue;
    }
    if ((prop.isEnumType()) || (QString("bool") == prop.typeName()) || (QString("int") == prop.typeName()) ||
        (QString("uint") == prop.typeName()) || (QString("double") == prop.typeName()) || (QString("QString") == prop.typeName()) ) {
      continue;
    } else if (ConfigObjectReference *ref = prop.read(this).value<ConfigObjectReference *>()) {
      // If not set -> skip
      if (! node[prop.name()])
        continue;
      // check type
      if (! node[prop.name()].IsScalar()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot link " << prop.name() << " of " << meta->className()
                 << ": Expected id.";
        return false;
      }
      // handle tags
      QString tag = QString::fromStdString(node[prop.name()].Tag());
      if ((!node[prop.name()].Scalar().size()) && (!tag.isEmpty())) {
        if (! ref->set(ctx.getTag(prop.enclosingMetaObject()->className(), prop.name(), tag))) {
          errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                   << ": Cannot link " << prop.name() << " of " << meta->className()
                   << ": Uknown tag " << tag << ".";
          return false;
        }
        continue;
      }
      // set reference
      QString id = QString::fromStdString(node[prop.name()].as<std::string>());
      if (! ref->set(ctx.getObj(id))) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot link " << prop.name() << " of " << meta->className()
                 << ": Cannot set reference.";
        return false;
      }
    } else if (ConfigObjectRefList *lst = prop.read(this).value<ConfigObjectRefList *>()) {
      // If not set -> skip
      if (! node[prop.name()])
        continue;
      // check type
      if (! node[prop.name()].IsSequence()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot link " << prop.name() << " of " << meta->className()
                 << ": Expected sequence.";
        return false;
      }
      for (YAML::const_iterator it=node[prop.name()].begin(); it!=node[prop.name()].end(); it++) {
        if (! it->IsScalar()) {
          errMsg() << it->Mark().line << ":" << it->Mark().column
                   << ": Cannot link " << prop.name() << " of " << meta->className()
                   << ": Expected ID string.";
          return false;
        }
        // check for tags
        QString tag = QString::fromStdString(it->Tag());
        if ((!it->Scalar().size()) && (!tag.isEmpty())) {
          if (0 > lst->add(ctx.getTag(prop.enclosingMetaObject()->className(), prop.name(), tag))) {
            errMsg() << it->Mark().line << ":" << it->Mark().column
                     << ": Cannot link " << prop.name() << " of " << meta->className()
                     << ": Cannot add referece for tag '" << tag << "'.";
            return false;
          }
          continue;
        }
        QString id = QString::fromStdString(it->as<std::string>());
        if (! ctx.contains(id)) {
          errMsg() << it->Mark().line << ":" << it->Mark().column
                   << ": Cannot link " << prop.name() << " of " << meta->className()
                   << ": Reference '" << id << "' not defined.";
          return false;
        }
        if (0 > lst->add(ctx.getObj(id))) {
          errMsg() << it->Mark().line << ":" << it->Mark().column
                   << ": Cannot link " << prop.name() << " of " << meta->className()
                   << ": Cannot add reference to '" << id << "' to list.";
          return false;
        }
      }

    } else if (ConfigItem *obj = prop.read(this).value<ConfigItem *>()) {
      // If not set -> skip
      if (! node[prop.name()])
        continue;

      // check type
      if (! node[prop.name()].IsMap()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot link " << prop.name() << " of " << meta->className()
                 << ": Expected object.";
        return false;
      }

      if (! obj->link(node[prop.name()], ctx)) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot link " << prop.name() << " of " << meta->className() << ".";
        return false;
      }
    } else if (ConfigObjectList *lst = prop.read(this).value<ConfigObjectList *>()) {
      // If not set -> skip
      if (! node[prop.name()])
        continue;

      // check type
      if (! node[prop.name()].IsSequence()) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot link " << prop.name() << " of " << meta->className()
                 << ": Expected sequence.";
        return false;
      }

      if (! lst->link(node[prop.name()], ctx)) {
        errMsg() << node[prop.name()].Mark().line << ":" << node[prop.name()].Mark().column
                 << ": Cannot link " << prop.name() << " of " << meta->className() << ".";
        return false;
      }
    }
  }

  return true;
}

unsigned
ConfigItem::extensionCount() const {
  return _extensions.count();
}

bool
ConfigItem::hasExtension(const QString &name) const {
  return _extensionTable.contains(name);
}

QList<QString>
ConfigItem::extensionNames() const {
  return _extensionTable.keys();
}

const ConfigItem *
ConfigItem::extension(const QString &name) const {
  return _extensionTable.value(name, nullptr);
}

ConfigItem *
ConfigItem::extension(const QString &name) {
  return _extensionTable.value(name, nullptr);
}

const ConfigItem *
ConfigItem::extension(unsigned n) const {
  return _extensions.value(n, nullptr);
}

ConfigItem *
ConfigItem::extension(unsigned n) {
  return _extensions.value(n, nullptr);
}

QString
ConfigItem::extensionName(unsigned n) const {
  if (n >= unsigned(_extensions.count()))
    return QString();
  return _extensionTable.key(_extensions.value(n));
}

void
ConfigItem::addExtension(const QString &name, ConfigItem *ext) {
  if (nullptr == ext)
    return;
  if (hasExtension(name)) {
    ConfigItem *obj = _extensionTable[name];
    _extensions.removeAll(obj);
    _extensionTable.remove(name);
    obj->deleteLater();
  }
  _extensions.append(ext);
  _extensionTable.insert(name, ext);
  ext->setParent(this);
}

void
ConfigItem::delExtension(const QString &name) {
  if (! hasExtension(name))
    return;
  ConfigItem *obj = _extensionTable[name];
  _extensions.removeAll(obj);
  _extensionTable.remove(name);
  obj->deleteLater();
}

void
ConfigItem::delExtension(unsigned n) {
  if (n >= unsigned(_extensions.count()))
    return;
  delExtension(extensionName(n));
}


/* ********************************************************************************************* *
 * Implementation of ConfigObject
 * ********************************************************************************************* */
ConfigObject::ConfigObject(const QString &idBase, QObject *parent)
  : ConfigItem(parent), _idBase(idBase)
{
  // pass...
}

bool
ConfigObject::label(ConfigObject::Context &context) {
  if (! ConfigItem::label(context))
    return false;

  // With empty ID base, skip labeling.
  if (_idBase.isEmpty())
    return true;

  unsigned n=1;
  QString id=QString("%1%2").arg(_idBase).arg(n);
  while (context.contains(id)) {
    id=QString("%1%2").arg(_idBase).arg(++n);
  }

  if (! context.add(id, this))
    return false;

  // Label properties owning config objects, that is of type ConfigObject or ConfigObjectList
  const QMetaObject *meta = metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (prop.read(this).value<ConfigObjectList *>()) {
      ConfigObjectList *lst = prop.read(this).value<ConfigObjectList *>();
      if (! lst->label(context))
        return false;
    } else if (prop.read(this).value<ConfigItem *>()) {
      ConfigItem *obj = prop.read(this).value<ConfigItem *>();
      if (! obj->label(context))
        return false;
    }
  }

  return true;
}

bool
ConfigObject::parse(const YAML::Node &node, Context &ctx) {
  if (! node["id"]) {
    logWarn() << node.Mark().line << ":" << node.Mark().column
              << ": No id specified for " << metaObject()->className()
              << ". Object cannot be referenced later.";
  } else {
    QString id = QString::fromStdString(node["id"].as<std::string>());
    if (! ctx.add(id, this)) {
      errMsg() << node["id"].Mark().line << ":" << node["id"].Mark().column
               << ": Cannot register ID '" << id << "'.";
      return false;
    }
  }

  return ConfigItem::parse(node, ctx);
}

bool
ConfigObject::populate(YAML::Node &node, const Context &context) {
  if (context.contains(this))
    node["id"] = context.getId(this).toStdString();
  return ConfigItem::populate(node, context);
}


/* ********************************************************************************************* *
 * Implementation of ConfigExtension
 * ********************************************************************************************* */
ConfigExtension::ConfigExtension(QObject *parent)
  : ConfigItem(parent)
{
  // pass...
}

bool
ConfigExtension::populate(YAML::Node &node, const Context &context) {
  // Call parent method
  if (! ConfigItem::populate(node, context))
    return false;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AbstractConfigObjectList
 * ********************************************************************************************* */
AbstractConfigObjectList::AbstractConfigObjectList(const QMetaObject &elementType, QObject *parent)
  : QObject(parent), ErrorStack(), _elementType(elementType), _items()
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

ConfigObject *AbstractConfigObjectList::get(int idx) const {
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
  connect(obj, SIGNAL(modified(ConfigItem*)), this, SLOT(onElementModified(ConfigObject*)));
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
AbstractConfigObjectList::onElementModified(ConfigItem *obj) {
  int idx = indexOf(obj->as<ConfigObject>());
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
ConfigObjectList::label(ConfigItem::Context &context) {
  foreach (ConfigItem *obj, _items) {
    if (! obj->label(context))
      return false;
  }
  return true;
}

YAML::Node
ConfigObjectList::serialize(const ConfigItem::Context &context) {
  YAML::Node list(YAML::NodeType::Sequence);
  foreach (ConfigItem *obj, _items) {
    YAML::Node node = obj->serialize(context);
    if (node.IsNull())
      return node;
    list.push_back(node);
  }
  return list;
}

bool
ConfigObjectList::parse(const YAML::Node &node, ConfigItem::Context &ctx) {
  if (! node)
    return false;

  if (!node.IsSequence()) {
    errMsg() << node.Mark().line << ":" << node.Mark().column
             << ": Cannot parse list: Expected list.";
    return false;
  }

  for (YAML::Node::const_iterator it=node.begin(); it!=node.end(); it++) {
    // Create object for node
    ConfigItem *element = allocateChild(*it, ctx);
    if ((nullptr == element) || (!element->is<ConfigObject>())) {
      errMsg() << it->Mark().line << ":" << it->Mark().column << ": Cannot parse list.";
      return false;
    }
    if (! element->parse(*it, ctx)) {
      pushErrorMessage(*element);
      errMsg() << it->Mark().line << ":" << it->Mark().column << ": Cannot parse list.";
      element->deleteLater();
      return false;
    }
    if (0 > add(element->as<ConfigObject>())) {
      errMsg() << it->Mark().line << ":" << it->Mark().column
               << ": Cannot add element to list.";
      element->deleteLater();
      return false;
    }
  }

  return true;
}

bool
ConfigObjectList::link(const YAML::Node &node, const ConfigItem::Context &ctx) {
  if (! node)
    return false;

  if (!node.IsSequence()) {
    errMsg() << node.Mark().line << ":" << node.Mark().column
             << ": Cannot parse list: Expected list.";
    return false;
  }
  int i = 0;
  YAML::Node::const_iterator it=node.begin();
  for (; it!=node.end(); it++, i++) {
    // Create object for node
    ConfigItem *element = get(i);
    if (nullptr == element) {
      errMsg() << it->Mark().line << ":" << it->Mark().column
               << ": Cannot link list, " << i << "-th element not present.";
      return false;
    }
    if (! element->link(*it, ctx)) {
      errMsg() << it->Mark().line << ":" << it->Mark().column
               << ": Cannot link list.";
      return false;
    }
  }

  return true;
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
ConfigObjectRefList::label(ConfigItem::Context &context) {
  Q_UNUSED(context)
  // pass...
  return true;
}

YAML::Node
ConfigObjectRefList::serialize(const ConfigItem::Context &context) {
  YAML::Node list(YAML::NodeType::Sequence);
  foreach (ConfigObject *obj, _items) {
    if (! context.contains(obj))
      return YAML::Node();
    list.push_back(context.getId(obj).toStdString());
  }
  return list;
}

