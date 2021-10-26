#ifndef EXTENSIONWRAPPER_HH
#define EXTENSIONWRAPPER_HH

#include "configobject.hh"
#include <QAbstractItemModel>
#include <QMetaProperty>

class ExtensionWrapper: public QAbstractItemModel
{
  Q_OBJECT

public:
  ExtensionWrapper(ConfigObject *obj, QObject *parent=nullptr);

  ConfigObject *parentObject(const QModelIndex &index) const;
  bool isProperty(const QModelIndex &index) const;
  QMetaProperty propertyAt(const QModelIndex &index) const;
  bool isExtension(const QModelIndex &index) const;
  ConfigObject *extensionAt(const QModelIndex &index) const;

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool hasChildren(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

protected:
  ConfigObject *item(const QModelIndex &item) const;

protected:
  ConfigObject *_object;
};

#endif // EXTENSIONWRAPPER_HH
