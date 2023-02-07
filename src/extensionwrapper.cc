#include "extensionwrapper.hh"
#include <QMetaProperty>
#include "logger.hh"
#include "configreference.hh"
#include "configobjecttypeselectiondialog.hh"


/* ******************************************************************************************** *
 * Implementation of ExtensionProxy
 * ******************************************************************************************** */
ExtensionProxy::ExtensionProxy(QObject *parent)
  : QIdentityProxyModel(parent)
{
  // pass...
}

void
ExtensionProxy::setSourceModel(QAbstractItemModel *sourceModel) {
  QIdentityProxyModel::setSourceModel(sourceModel);

  _indexP2S.clear(); _indexS2P.clear();
  if (PropertyWrapper *model = qobject_cast<PropertyWrapper *>(sourceModel)) {
    // Create root element maps
    for (int s=0,p=0; s<model->rowCount(QModelIndex()); s++) {
      if (model->isExtension(model->index(s,0, QModelIndex()))) {
        _indexP2S[p] = s;
        _indexS2P[s] = p;
        p++;
      }
    }
  }
}

int
ExtensionProxy::rowCount(const QModelIndex &parent) const {
  if (nullptr == sourceModel())
    return 0;

  // Non root elements
  if (parent.isValid())
    return sourceModel()->rowCount(mapToSource(parent));

  // root element
  return _indexP2S.count();
}

int
ExtensionProxy::columnCount(const QModelIndex &parent) const {
  if (nullptr == sourceModel())
    return 0;
  return sourceModel()->columnCount(mapToSource(parent));
}

QModelIndex
ExtensionProxy::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid())
    return mapFromSource(sourceModel()->index(row, column, mapToSource(parent)));
  return createIndex(row, column, qobject_cast<PropertyWrapper *>(sourceModel())->root());
}

QModelIndex
ExtensionProxy::parent(const QModelIndex &child) const {
  if (! child.isValid())
    return QModelIndex();

  ConfigItem *root = qobject_cast<PropertyWrapper*>(sourceModel())->root();
  QObject *pptr = reinterpret_cast<QObject*>(child.internalPointer());
  if ((root == pptr) || (nullptr == pptr))
    return QModelIndex();

  QObject *gpptr = pptr->parent();
  if (nullptr == gpptr)
    return QModelIndex();

  if (root == gpptr) {
    // Search for index of parent root:
    const QMetaObject *meta = root->metaObject();
    for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
      QMetaProperty prop = meta->property(p);
      if (! prop.isValid())
        continue;
      if (prop.read(root).value<ConfigItem *>() == pptr) {
        return createIndex(_indexS2P[p-QObject::staticMetaObject.propertyCount()], 0,
            reinterpret_cast<quintptr>(root));
      }
    }
    return QModelIndex();
  }

  return mapFromSource(sourceModel()->parent(child));
}

QModelIndex
ExtensionProxy::mapToSource(const QModelIndex &proxyIndex) const {
  // root maps to root
  if (! proxyIndex.isValid())
    return QModelIndex();
  // If no source -> blub.
  if (nullptr == sourceModel())
    return QModelIndex();

  // If index is not an immediate child of root -> map 1:1
  if (proxyIndex.parent().isValid()) {
    return sourceModel()->index(
          proxyIndex.row(), proxyIndex.column(), mapToSource(proxyIndex.parent()));
  }

  // If index is an immediate child of root -> map only extensions
  if (_indexP2S.contains(proxyIndex.row()))
    return sourceModel()->index(
          _indexP2S[proxyIndex.row()], proxyIndex.column(), QModelIndex());

  return QModelIndex();
}

QModelIndex
ExtensionProxy::mapFromSource(const QModelIndex &sourceIndex) const {
  // root maps to root
  if (! sourceIndex.isValid())
    return QModelIndex();
  // If no source -> blub.
  if (nullptr == sourceModel())
    return QModelIndex();

  // If index is not an immediate child of root -> map 1:1
  if (sourceModel()->parent(sourceIndex).isValid()) {
    return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
  }

  if (_indexS2P.contains(sourceIndex.row()))
    return createIndex(_indexS2P[sourceIndex.row()], sourceIndex.column(), sourceIndex.internalPointer());

  return QModelIndex();
}


/* ******************************************************************************************** *
 * Implementation of PropertyWrapper
 * ******************************************************************************************** */
PropertyWrapper::PropertyWrapper(ConfigItem *obj, QObject *parent)
  : QAbstractItemModel(parent), _object(obj)
{
  if (_object) {
    connect(_object, SIGNAL(beginClear()), this, SLOT(onItemClearing()));
    connect(_object, SIGNAL(endClear()), this, SLOT(onItemCleared()));
  }
}

ConfigItem *
PropertyWrapper::root() const {
  return _object;
}

ConfigItem *
PropertyWrapper::item(const QModelIndex &item) const {
  if (! item.isValid())
    return nullptr;

  // Get pointer to parent (either list or item)
  QObject *pptr = _object;
  if (nullptr != item.internalPointer())
    pptr = reinterpret_cast<QObject *>(item.internalPointer());

  if (ConfigItem *pobj = qobject_cast<ConfigItem*>(pptr)) {
    // If parent is item find corresponding property
    const QMetaObject *meta = pobj->metaObject();
    int pcount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
    if (item.row() < pcount) {
      QMetaProperty prop = meta->property(QObject::staticMetaObject.propertyCount() + item.row());
      return prop.read(pobj).value<ConfigItem *>();
    }
  } else if (ConfigObjectList *plst = qobject_cast<ConfigObjectList*>(pptr)) {
    // If parent is list
    if (item.row() < plst->count())
      return plst->get(item.row());
  }

  return nullptr;
}

ConfigObjectList *
PropertyWrapper::list(const QModelIndex &item) const {
  if (! item.isValid())
    return nullptr;

  QObject *pptr = reinterpret_cast<QObject*>(item.internalPointer());
  ConfigItem *pobj = qobject_cast<ConfigItem*>(pptr);
  if (nullptr == pobj)
    return nullptr;

  const QMetaObject *meta = pobj->metaObject();
  int pcount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  if (item.row() < pcount) {
    QMetaProperty prop = meta->property(QObject::staticMetaObject.propertyCount() + item.row());
    return prop.read(pobj).value<ConfigObjectList *>();
  }

  return nullptr;
}

ConfigObjectList *
PropertyWrapper::parentList(const QModelIndex &index) const {
  if (! index.isValid())
    return nullptr;

  QObject *pptr = reinterpret_cast<QObject *>(index.internalId());
  if (nullptr == pptr)
    return nullptr;

  if (ConfigObjectList *lst = qobject_cast<ConfigObjectList*>(pptr))
    return lst;

  return nullptr;
}

ConfigItem *
PropertyWrapper::parentObject(const QModelIndex &index) const {
  if (! index.isValid())
     return _object;

  QObject *pptr = reinterpret_cast<QObject*>(index.internalId());
  return qobject_cast<ConfigItem*>(pptr);
}

QMetaProperty
PropertyWrapper::propertyAt(const QModelIndex &index) const {
  ConfigItem *pobj = parentObject(index);
  const QMetaObject *meta = pobj->metaObject();

  int propCount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
  if (index.row() < propCount)
    return meta->property(index.row()+QObject::staticMetaObject.propertyCount());
  return QMetaProperty();
}


bool
PropertyWrapper::isExtension(const QModelIndex &index) const {
  if (! index.isValid())
    return false;
  QMetaProperty prop = propertyAt(index);
  return propIsInstance<ConfigExtension>(prop);
}

bool
PropertyWrapper::isProperty(const QModelIndex &index) const {
  if (! index.isValid())
    return true;
  // Index is property if parent is an ConfigItem
  QObject *pptr = reinterpret_cast<QObject *>(index.internalPointer());
  return nullptr != qobject_cast<ConfigItem*>(pptr);
}

bool
PropertyWrapper::isListElement(const QModelIndex &index) const {
  if (! index.isValid())
    return false;
  // Index is list element if parent is ConfigObjectList
  QObject *pptr = reinterpret_cast<QObject *>(index.internalPointer());
  if (nullptr == pptr)
    return false;
  return nullptr != qobject_cast<ConfigObjectList*>(pptr);
}

bool
PropertyWrapper::createInstanceAt(const QModelIndex &item) {
  if (! isProperty(item))
    return false;

  ConfigItem *obj = parentObject(item);
  QMetaProperty prop = propertyAt(item);
  if ((nullptr == obj) || (! prop.isValid()))
    return false;
  // Check type of property
  if (! propIsInstance<ConfigItem>(prop))
    return false;
  // If property is already set -> abort
  if (prop.read(obj).value<ConfigItem*>())
    return false;
  // Get TypeObject of property
  if (QMetaType::UnknownType == prop.userType())
    return false;
  QMetaType type(prop.userType());
  if (! (QMetaType::PointerToQObject & type.flags()))
    return false;
  const QMetaObject *propType = type.metaObject();
  // Instantiate extension
  ConfigItem *ext = qobject_cast<ConfigItem *>(
        propType->newInstance(QGenericArgument(nullptr, obj)));
  if (nullptr == ext)
    return false;
  // store item
  beginInsertRows(item, 0, ext->metaObject()->propertyCount());
  prop.write(obj, QVariant::fromValue(ext));
  endInsertRows();
  emit dataChanged(index(item.row(), 0, item.parent()),
                   index(item.row(), 2, item.parent()));
  emit dataChanged(index(0, 0, item),
                   index(ext->metaObject()->propertyCount(),2, item));
  return true;
}

bool
PropertyWrapper::deleteInstanceAt(const QModelIndex &item) {
  ConfigItem *obj = parentObject(item);
  QMetaProperty prop = propertyAt(item);
  if ((nullptr == obj) || (! prop.isValid()))
    return false;
  // Check type of property
  if (! propIsInstance<ConfigItem>(prop))
    return false;
  // If property is set -> delete
  if (ConfigItem *ext = prop.read(obj).value<ConfigItem*>()) {
    if (! prop.isWritable())
      return false;
    beginRemoveRows(item, 0, rowCount(item));
    prop.write(obj, QVariant::fromValue<ConfigItem*>(nullptr));
    endRemoveRows();
    ext->deleteLater();
    return true;
  }
  return false;
}

bool
PropertyWrapper::createElementAt(const QModelIndex &item) {
  ConfigItem *obj = parentObject(item);
  QMetaProperty prop = propertyAt(item);
  if ((nullptr == obj) || (! prop.isValid()))
    return false;

  ConfigObjectList *lst = prop.read(obj).value<ConfigObjectList*>();
  if (nullptr == lst)
    return false;

  ConfigObjectTypeSelectionDialog dialog(lst->elementTypes());
  if (QDialog::Accepted != dialog.exec())
    return true;

  QMetaObject type= dialog.selectedType();

  // Instantiate element
  ConfigObject *element = qobject_cast<ConfigObject *>(
        type.newInstance(QGenericArgument(nullptr, lst)));
  if (nullptr == element)
    return false;
  element->setName(tr("new element"));

  // store item
  beginInsertRows(item,lst->count(), lst->count()+1);
  lst->add(element);
  endInsertRows();
  emit dataChanged(index(item.row(), 0, item.parent()),
                   index(item.row(), 2, item.parent()));

  QModelIndex elementIndex = createIndex(lst->count(), 0, lst);
  beginInsertRows(elementIndex, 0, element->metaObject()->propertyCount());
  endInsertRows();
  emit dataChanged(index(0, 0, item),
                   index(lst->count(), 2, item));
  return true;
}

bool
PropertyWrapper::deleteElementAt(const QModelIndex &item) {
  ConfigObjectList *lst = parentList(item);
  if (nullptr == lst)
    return false;
  if (item.row() >= lst->count())
    return false;

  beginRemoveRows(item, 0, rowCount(item));
  lst->del(lst->get(item.row()));
  endRemoveRows();

  return true;
}

QModelIndex
PropertyWrapper::index(int row, int column, const QModelIndex &parent) const {
  if (! parent.isValid()) {
    // Handle root element
    const QMetaObject *meta = _object->metaObject();
    int pcount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
    if (row < pcount)
      return createIndex(row, column, _object);
  } else if (ConfigItem *pobj = item(parent)) {
    const QMetaObject *meta = pobj->metaObject();
    int pcount = meta->propertyCount() - QObject::staticMetaObject.propertyCount();
    if (row < pcount)
      return createIndex(row, column, pobj);
  } else if (ConfigObjectList *plst = list(parent)) {
    if (row < plst->count())
      return createIndex(row, column, plst);
  }
  return QModelIndex();
}

QModelIndex
PropertyWrapper::parent(const QModelIndex &child) const {
  if (! child.isValid())
    return QModelIndex();

  QObject *pptr = reinterpret_cast<QObject*>(child.internalPointer());
  // Handle root
  if (ConfigItem *pobj = qobject_cast<ConfigItem*>(pptr)) {
    if (_object == pobj)
      return QModelIndex();
  }

  QObject *gpptr = pptr->parent();
  // Should not happen
  if (nullptr == gpptr)
    return QModelIndex();

  if (ConfigItem *gp = qobject_cast<ConfigItem*>(gpptr)) {
    // If grand parent is item:
    // Search for parent in grand-parent's properties
    const QMetaObject *meta = gp->metaObject();
    for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
      QMetaProperty prop = meta->property(p);
      if (! prop.isValid())
        continue;
      if (prop.read(gp).value<QObject *>() == pptr) {
        return createIndex(p-QObject::staticMetaObject.propertyCount(), 0, reinterpret_cast<quintptr>(gp));
      }
    }
  } else if (ConfigObjectList *gp = qobject_cast<ConfigObjectList*>(gpptr)) {
    // If grand parent is item:
    // Search for parent in grand-parent's elements
    for (int i=0; i<gp->count(); i++) {
      if (pptr == gp->get(i))
        return createIndex(i, 0, reinterpret_cast<quintptr>(gp));
    }
  }

  return QModelIndex();
}

int
PropertyWrapper::rowCount(const QModelIndex &parent) const {
  if (! parent.isValid()) {
    // If parent is root -> handle _object
    const QMetaObject *meta = _object->metaObject();
    return (meta->propertyCount() - QObject::staticMetaObject.propertyCount());
  }

  if (ConfigItem *pobj = item(parent)) {
    // If parent is item -> return property count
    const QMetaObject *meta = pobj->metaObject();
    return (meta->propertyCount() - QObject::staticMetaObject.propertyCount());
  } else if (ConfigObjectList *plst = list(parent)) {
    // If parent is list -> return element count.
    return plst->count();
  }

  return 0;
}

int
PropertyWrapper::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return 3;
}

Qt::ItemFlags
PropertyWrapper::flags(const QModelIndex &index) const {
  if (isProperty(index)) {
    // check if property is a config object or atomic (or reference)
    QMetaProperty prop = propertyAt(index);
    if (propIsInstance<ConfigItem>(prop) || propIsInstance<ConfigObjectList>(prop))
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (propIsInstance<ConfigObjectReference>(prop) && prop.isScriptable() && (1 == index.column()))
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;

    if (prop.isWritable() && (1 == index.column()))
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
  } else if (isListElement(index)) {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }
  return Qt::NoItemFlags;
}

bool
PropertyWrapper::hasChildren(const QModelIndex &element) const {
  // If root element
  if (! element.isValid())
    return true;
  // Get parent object (might be item or list)
  QObject *pptr = reinterpret_cast<QObject *>(element.internalId());

  if (ConfigItem* pobj = qobject_cast<ConfigItem *>(pptr)) {
    // If parent is item -> get addressed property
    QMetaProperty prop = propertyAt(element);
    // If property is an item or list (and is set)
    if (propIsInstance<ConfigItem>(prop)) {
      return nullptr != prop.read(pobj).value<ConfigItem*>();
    } else if (propIsInstance<ConfigObjectList>(prop)) {
      // If element and there are elements within the list.
      ConfigObjectList *lst = prop.read(pobj).value<ConfigObjectList*>();
      return (nullptr != lst) && (lst->count()>0);
    }
  } else if (ConfigObjectList *lst = qobject_cast<ConfigObjectList*>(pptr)) {
    // If parent is a list, check row index. List elements are always objects and have children.
    return lst->count() > element.row();
  }

  return false;
}

QVariant
PropertyWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  // Only handle horizontal display data
  if ((Qt::Horizontal != orientation) || (Qt::DisplayRole != role))
    return QAbstractItemModel::headerData(section, orientation, role);
  // Dispatch by section index
  switch (section) {
  case 0: return tr("Property");
  case 1: return tr("Value");
  case 2: return tr("Description");
  default: break;
  }
  // default
  return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant
PropertyWrapper::data(const QModelIndex &index, int role) const {
  if (! index.isValid())
    return QVariant();

  if (isProperty(index)) {
    ConfigItem *pobj = parentObject(index);
    QMetaProperty prop = propertyAt(index);
    if (0 == index.column()) {
      if (Qt::DisplayRole == role)
        return prop.name();
    } else if ((2 == index.column()) && (Qt::DisplayRole == role)) {
      if (propIsInstance<ConfigItem>(prop)) {
        ConfigItem *item = prop.read(pobj).value<ConfigItem*>();
        if (item && item->hasDescription())
          return item->description();
      }
      if (pobj->hasDescription(prop))
        return pobj->description(prop);
      return QVariant();
    } else if (Qt::ToolTipRole == role) {
      if (propIsInstance<ConfigItem>(prop)) {
        ConfigItem *item = prop.read(pobj).value<ConfigItem*>();
        if (item && item->hasLongDescription())
          return item->longDescription();
      }
      if (pobj->hasLongDescription(prop))
        return pobj->longDescription(prop);
      if (pobj->hasDescription(prop))
        return pobj->description(prop);
      return QVariant();
    }

    QVariant value = prop.read(pobj);
    if (prop.isEnumType() && ((Qt::DisplayRole == role) || (Qt::EditRole == role))) {
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
    } else if ((QVariant::Bool == prop.type()) && (Qt::EditRole == role)) {
      return value;
    } else if ((QVariant::Bool == prop.type()) && (Qt::DisplayRole == role)) {
      if (value.toBool())
        return tr("true");
      return tr("false");
    } else if ( ((QVariant::Int == prop.type()) || (QVariant::UInt == prop.type()) ||
                 (QVariant::Double == prop.type()) || (QVariant::String == prop.type()))
                && ((Qt::DisplayRole == role) || (Qt::EditRole==role)) ) {
      return value;
    } else if (value.value<ConfigObjectReference *>() && (Qt::DisplayRole == role)) {
      ConfigObjectReference *ref = value.value<ConfigObjectReference *>();
      ConfigObject *obj = ref->as<ConfigObject>();
      if (nullptr == obj)
        return tr("[None]");
      return QString("%1 (%2)").arg(obj->name()).arg(obj->metaObject()->className());
    } else if (value.value<ConfigObjectReference *>() && (Qt::EditRole == role)) {
      return value;
    } else if (propIsInstance<ConfigItem>(prop)) {
      ConfigItem *item = value.value<ConfigItem*>();
      if (Qt::DisplayRole == role) {
        if (nullptr == item)
          return tr("[None]");
        else
          return tr("Instance of %1").arg(item->metaObject()->className());
      }
    } else if (propIsInstance<ConfigObjectList>(prop)) {
      ConfigObjectList *lst = value.value<ConfigObjectList*>();
      if (Qt::DisplayRole == role)
        return tr("List of %1 instances").arg(lst->classNames().join(", "));
    }
  } else if (isListElement(index)) {
    ConfigObjectList *lst = parentList(index);
    if (index.row() >= lst->count())
      return QVariant();

    if ((0 == index.column()) && (Qt::DisplayRole == role))
      return lst->get(index.row())->name();
    else if ((1 == index.column()) && (Qt::DisplayRole == role))
      return lst->get(index.row())->description();
    else if ((2 == index.column()) && (Qt::DisplayRole == role))
      return lst->get(index.row())->longDescription();
  }


  return QVariant();
}

void
PropertyWrapper::onItemClearing() {
  beginResetModel();
}

void
PropertyWrapper::onItemCleared() {
  endResetModel();
}
