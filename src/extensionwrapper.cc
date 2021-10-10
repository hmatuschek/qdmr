#include "extensionwrapper.hh"

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
  ConfigObject *grant = qobject_cast<ConfigObject*>(pobj->parent());
  if (nullptr == grant)
    return QModelIndex();

}
