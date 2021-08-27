#ifndef CONFIG_ITEM_WRAPPER_HH
#define CONFIG_ITEM_WRAPPER_HH

#include "configitem.hh"
#include <QAbstractTableModel>

class GenericListWrapper: public QAbstractListModel
{
  Q_OBJECT

protected:
  GenericListWrapper(Configuration::List *list, QObject *parent=nullptr);

public:
  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of rows. */
  int rowCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns number of colums. */
  int columnCount(const QModelIndex &index) const;

signals:
  /** Gets emitted once the table has been changed. */
  void modified();

protected slots:
  /** Internal used callback on deleted config. */
  void onListDeleted();
  /** Internal callback on deleted channels. */
  void onItemRemoved(int idx);
  /** Internal callback on modified channels. */
  void onItemModified(int idx);

protected:
  /** Holds a weak reference to the list object. */
  Configuration::List *_list;
};


class GenericTableWrapper: public QAbstractTableModel
{
  Q_OBJECT

protected:
  GenericTableWrapper(Configuration::List *list, QObject *parent=nullptr);

public:
  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of rows. */
  int rowCount(const QModelIndex &index) const;

signals:
  /** Gets emitted once the table has been changed. */
  void modified();

protected slots:
  /** Internal used callback on deleted config. */
  void onListDeleted();
  /** Internal callback on deleted channels. */
  void onItemRemoved(int idx);
  /** Internal callback on modified channels. */
  void onItemModified(int idx);

protected:
  /** Holds a weak reference to the list object. */
  Configuration::List *_list;
};


class ChannelListWrapper: public GenericTableWrapper
{
  Q_OBJECT

public:
  ChannelListWrapper(Configuration::ChannelList *list, QObject *parent=nullptr);

public:
  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of colums. */
  int columnCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns data at cell. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implements QAbstractTableModel, returns header at section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
};


#endif // CONFIG_ITEM_WRAPPER_HH
