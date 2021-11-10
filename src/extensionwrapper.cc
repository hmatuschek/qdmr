#include "extensionwrapper.hh"
#include <QMetaProperty>
#include "logger.hh"
#include "configreference.hh"


ExtensionWrapper::ExtensionWrapper(ConfigItem *obj, QObject *parent)
  : QAbstractItemModel(parent), _object(obj)
{
  logDebug() << "Create extension wrapper...";
}

ConfigItem *
ExtensionWrapper::item(const QModelIndex &item) const {
  if (! item.isValid())
    return nullptr;

  ConfigItem *pobj = _object;
  if (nullptr != item.internalPointer())
    pobj = reinterpret_cast<ConfigItem *>(item.internalPointer());

  logDebug() << "Find item for " << item.row()
             << "@" << QString::number(reinterpret_cast<qintptr>(pobj), 16);

  const QMetaObject *meta = pobj->metaObject();
  int pcount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  if (item.row() < pcount) {
    QMetaProperty prop = meta->property(QObject::staticMetaObject.propertyCount() + item.row());
    if (ConfigItem *item = prop.read(pobj).value<ConfigItem *>())
      return item;
    else
      return nullptr;
  } else if (item.row() < int(pcount + pobj->extensionCount())) {
    return pobj->extension(item.row()-pcount);
  }

  return nullptr;
}

ConfigItem *
ExtensionWrapper::parentObject(const QModelIndex &index) const {
  ConfigItem *pobj = _object;
  if (index.isValid())
     pobj = reinterpret_cast<ConfigItem *>(index.internalId());
  return pobj;
}

bool
ExtensionWrapper::isProperty(const QModelIndex &index) const {
  if (! index.isValid())
    return false;

  ConfigItem *pobj = parentObject(index);
  const QMetaObject *meta = pobj->metaObject();
  int propCount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  return index.row() < propCount;
}

QMetaProperty
ExtensionWrapper::propertyAt(const QModelIndex &index) const {
  if (! isProperty(index))
    return QMetaProperty();
  ConfigItem *pobj = parentObject(index);
  const QMetaObject *meta = pobj->metaObject();
  int propCount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  if (index.row() < propCount)
    return meta->property(index.row()+QObject::staticMetaObject.propertyCount());
  return QMetaProperty();
}

bool
ExtensionWrapper::isExtension(const QModelIndex &index) const {
  if (! index.isValid())
    return false;

  ConfigItem *pobj = parentObject(index);
  const QMetaObject *meta = pobj->metaObject();
  int propCount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  int extCount = pobj->extensionCount();
  return (index.row() <= propCount) && (index.row() < (propCount+extCount));
}

ConfigItem *
ExtensionWrapper::extensionAt(const QModelIndex &index) const {
  if (! isExtension(index))
    return nullptr;
  ConfigItem *pobj = parentObject(index);
  const QMetaObject *meta = pobj->metaObject();
  int propCount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  return pobj->extension(index.row()-propCount);
}

QModelIndex
ExtensionWrapper::index(int row, int column, const QModelIndex &parent) const {
  ConfigItem *pobj = _object;
  if (parent.isValid())
    pobj = item(parent);

  const QMetaObject *meta = pobj->metaObject();
  int pcount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  if (row < int(pcount+pobj->extensionCount()))
    return createIndex(row, column, pobj);

  return QModelIndex();
}

QModelIndex
ExtensionWrapper::parent(const QModelIndex &child) const {
  if (! child.isValid())
    return QModelIndex();

  ConfigItem *pobj = reinterpret_cast<ConfigItem*>(child.internalPointer());
  if (_object == pobj)
    return QModelIndex();

  ConfigItem *gp = qobject_cast<ConfigItem*>(pobj->parent());
  if (nullptr == gp) {
    logError() << "Parent of object #" << QString::number(reinterpret_cast<quintptr>(pobj))
               << " not set!";
    return QModelIndex();
  }

  logDebug() << "Get parent for " << child.row() << "," << child.column()
             << "@" << QString::number(reinterpret_cast<qintptr>(pobj));

  // Search for parent in grand-parent's properties
  const QMetaObject *meta = gp->metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (prop.read(gp).value<ConfigItem *>() == pobj) {
      return createIndex(p-QObject::staticMetaObject.propertyCount(), 0, reinterpret_cast<quintptr>(gp));
    }
  }

  // Search for parent in grand-parent's extensions
  int nprop = meta->propertyCount()-QObject::staticMetaObject.propertyCount();
  for (int e=0; e<int(gp->extensionCount()); e++) {
    if (gp->extension(e) == pobj)
      return createIndex(nprop+e, 0, reinterpret_cast<quintptr>(gp));
  }

  return QModelIndex();
}

int
ExtensionWrapper::rowCount(const QModelIndex &parent) const {
  ConfigItem *pobj = _object;
  if (parent.isValid())
    pobj = item(parent);

  logDebug() << "Get row count for " << parent.row() << "," << parent.column()
             << "@" << QString::number(reinterpret_cast<qintptr>(pobj));

  if (nullptr == pobj)
    return 0;

  const QMetaObject *meta = pobj->metaObject();
  return (meta->propertyCount() - QObject::staticMetaObject.propertyCount())
      + pobj->extensionCount();
}

int
ExtensionWrapper::columnCount(const QModelIndex &parent) const {
  return 2;
}

Qt::ItemFlags
ExtensionWrapper::flags(const QModelIndex &index) const {
  ConfigItem *pobj = parentObject(index);
  if (isProperty(index)) {
    // if is property
    // check if property is a config object or atomic (or reference)
    QMetaProperty prop = propertyAt(index);
    if (prop.read(pobj).value<ConfigItem *>()) {
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    } else {
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;
    }
  } else if (isExtension(index)) {
    // If is extension
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }
  return Qt::NoItemFlags;
}

bool
ExtensionWrapper::hasChildren(const QModelIndex &parent) const {
  ConfigItem *pobj = _object;
  if (parent.isValid())
    pobj = reinterpret_cast<ConfigItem *>(parent.internalId());

  logDebug() << "Get children for " << parent.row() << "," << parent.column()
             << "@" << QString::number(reinterpret_cast<qintptr>(pobj));

  const QMetaObject *meta = pobj->metaObject();
  int nprop = meta->propertyCount()-QObject::staticMetaObject.propertyCount();
  if (! parent.isValid())
    return (0<(nprop+pobj->extensionCount()));
  if (parent.row() < nprop) {
    // if is property
    // check if property is a config object or atomic (or reference)
    QMetaProperty prop = meta->property(parent.row()+QObject::staticMetaObject.propertyCount());
    if (! prop.read(pobj).value<ConfigItem *>())
      return false;
    return true;
  } else if (parent.row() < int(nprop + pobj->extensionCount())) {
    // If is extension
    return true;
  }
  return false;
}

QVariant
ExtensionWrapper::data(const QModelIndex &index, int role) const {
  if (! index.isValid())
    return QVariant();

  ConfigItem *pobj = parentObject(index);

  if (isProperty(index)) {
    QMetaProperty prop =propertyAt(index);
    QVariant value = prop.read(pobj);
    if ((0 == index.column()) && (Qt::DisplayRole == role))
      return prop.name();
    if (prop.isEnumType()) {
      QMetaEnum e = prop.enumerator();
      const char *key = e.valueToKey(value.toInt());
      if (nullptr == key) {
        logError() << "Cannot map value " << value.toUInt()
                   << " to enum " << e.name()
                   << ". Ignore attribute but this points to an incompatibility in some codeplug. "
                   << "Consider reporting it to https://github.com/hmatuschek/qdmr/issues.";
        return QVariant();
      }
      return QString(key);
    } else if ((QString("bool") == prop.typeName()) && (Qt::EditRole == role)) {
      return value;
    } else if ((QString("bool") == prop.typeName()) && (Qt::DisplayRole == role)) {
      if (value.toBool())
        return tr("true");
      return tr("false");
    } else if ( ((QString("int") == prop.typeName()) || (QString("uint") == prop.typeName()) ||
                 (QString("double") == prop.typeName()) || (QString("QString") == prop.typeName()))
                && ((Qt::DisplayRole == role) || (Qt::EditRole==role)) ) {
      return value;
    } else if (value.value<ConfigObjectReference *>() && (Qt::DisplayRole == role)) {
      const QMetaObject *mobj = QMetaType::metaObjectForType(prop.userType());
      ConfigObjectReference *ref = value.value<ConfigObjectReference *>();
      ConfigItem *obj = ref->as<ConfigItem>();
      if (nullptr == obj)
        return tr("[None]");

      return QString(mobj->className()) + "@"
          + QString::number(reinterpret_cast<quintptr>(obj), 16);
    } else if (value.value<ConfigObjectReference *>() && (Qt::EditRole == role)) {
      return value;
    }
  } else if (isExtension(index)) {
    ConfigItem *extension = extensionAt(index);
    if ((0 == index.column()) && (Qt::DisplayRole==role)) {
      return extension->metaObject()->className();
    }
  }

  return QVariant();
}
