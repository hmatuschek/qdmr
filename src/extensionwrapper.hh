#ifndef EXTENSIONWRAPPER_HH
#define EXTENSIONWRAPPER_HH

#include "configobject.hh"
#include <QAbstractItemModel>

class ExtensionWrapper: public QAbstractItemModel
{
  Q_OBJECT

public:
  ExtensionWrapper(ConfigObject *obj, QObject *parent=nullptr);

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

protected:
  ConfigObject *_object;
};

#endif // EXTENSIONWRAPPER_HH
