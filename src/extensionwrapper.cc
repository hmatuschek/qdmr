#include "extensionwrapper.hh"
#include <QMetaProperty>

ExtensionWrapper::ExtensionWrapper(ConfigObject *obj, QObject *parent)
  : QAbstractItemModel(parent), _object(obj)
{
  // pass...
}

QModelIndex
ExtensionWrapper::index(int row, int column, const QModelIndex &parent) const {
  ConfigObject *pobj = _object;
  if (parent.isValid())
    pobj = reinterpret_cast<ConfigObject *>(parent.internalId());
  return createIndex(row, column, reinterpret_cast<quintptr>(pobj));
}

QModelIndex
ExtensionWrapper::parent(const QModelIndex &child) const {
  ConfigObject *pobj = reinterpret_cast<ConfigObject *>(child.internalId());
  if (pobj == _object)
    return QModelIndex();

  ConfigObject *gp = qobject_cast<ConfigObject*>(pobj->parent());
  if (nullptr == gp)
    return QModelIndex();

  // Search for parent in grand-parent's properties
  const QMetaObject *meta = gp->metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid())
      continue;
    if (prop.read(gp).value<ConfigObject *>() == pobj) {
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
  ConfigObject *pobj = reinterpret_cast<ConfigObject *>(parent.internalId());
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
  ConfigObject *pobj = reinterpret_cast<ConfigObject *>(index.internalId());
  const QMetaObject *meta = pobj->metaObject();
  int nprop = meta->propertyCount()-QObject::staticMetaObject.propertyCount();
  if (index.row() < nprop) {
    // if is property
    // check if property is a config object or atomic (or reference)
    QMetaProperty prop = meta->property(index.row()+QObject::staticMetaObject.propertyCount());
    if (! prop.read(pobj).value<ConfigObject *>())
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  } else if (index.row() < int(nprop + pobj->extensionCount())) {
    // If is extension
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }
  return Qt::NoItemFlags;
}

bool
ExtensionWrapper::hasChildren(const QModelIndex &parent) const {
  if (0 != parent.column())
    return false;
  ConfigObject *pobj = reinterpret_cast<ConfigObject *>(parent.internalId());
  const QMetaObject *meta = pobj->metaObject();
  int nprop = meta->propertyCount()-QObject::staticMetaObject.propertyCount();
  if (parent.row() < nprop) {
    // if is property
    // check if property is a config object or atomic (or reference)
    QMetaProperty prop = meta->property(parent.row()+QObject::staticMetaObject.propertyCount());
    if (! prop.read(pobj).value<ConfigObject *>())
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
  ConfigObject *pobj = reinterpret_cast<ConfigObject *>(index.internalId());
  const QMetaObject *meta = pobj->metaObject();
  int nprop = meta->propertyCount()-QObject::staticMetaObject.propertyCount();
  if (index.row() < nprop) {
    QMetaProperty prop = meta->property(index.row());
    if (0 == index.column()) {
      return prop.name();
    } else {
      return prop.read(pobj);
    }
  } else if (index.row() < int(nprop+pobj->extensionCount())) {
    if (0 == index.column()) {
      return pobj->extensionName(index.row()-nprop);
    }
  }
  return QVariant();
}
