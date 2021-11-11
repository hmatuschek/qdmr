#ifndef EXTENSIONWRAPPER_HH
#define EXTENSIONWRAPPER_HH

#include "configobject.hh"
#include <QAbstractItemModel>
#include <QIdentityProxyModel>
#include <QMetaProperty>


class ExtensionProxy: public QIdentityProxyModel
{
  Q_OBJECT

public:
  explicit ExtensionProxy(QObject *parent=nullptr);

public:
  void setSourceModel(QAbstractItemModel *sourceModel);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
  //QItemSelection mapSelectionFromSource(const QItemSelection &selection) const;
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
  //QItemSelection mapSelectionToSource(const QItemSelection &selection) const;

protected:
  QHash<int, int> _indexS2P, _indexP2S;
};


class PropertyWrapper: public QAbstractItemModel
{
  Q_OBJECT

public:
  PropertyWrapper(ConfigItem *obj, QObject *parent=nullptr);

  ConfigItem *root() const;
  ConfigItem *item(const QModelIndex &item) const;

  ConfigItem *parentObject(const QModelIndex &index) const;
  QMetaProperty propertyAt(const QModelIndex &index) const;
  bool isExtension(const QModelIndex &index) const;

  bool createInstanceAt(const QModelIndex &item);
  bool deleteInstanceAt(const QModelIndex &item);

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool hasChildren(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;


protected:
  ConfigItem *_object;
};

#endif // EXTENSIONWRAPPER_HH
