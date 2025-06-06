#ifndef CONFIG_ITEM_WRAPPER_HH
#define CONFIG_ITEM_WRAPPER_HH

#include "config.hh"
#include <QAbstractTableModel>

class GenericListWrapper: public QAbstractListModel
{
  Q_OBJECT

protected:
  GenericListWrapper(AbstractConfigObjectList *list, QObject *parent=nullptr);

public:
  // QAbstractListModel interface
  /** Implements QAbstractTableModel, returns number of rows. */
  int rowCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns number of columns. */
  int columnCount(const QModelIndex &index) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  Qt::DropActions supportedDropActions() const;

  bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                const QModelIndex &destinationParent, int destinationChild);

signals:
  /** Gets emitted once the table has been changed. */
  void modified();

protected slots:
  /** Internal used callback on deleted config. */
  void onListDeleted();
  /** Internal callback on added items. */
  void onItemAdded(int idx);
  /** Internal callback on deleted channels. */
  void onItemRemoved(int idx);
  /** Internal callback on modified channels. */
  void onItemModified(int idx);

protected:
  /** Holds a weak reference to the list object. */
  AbstractConfigObjectList *_list;
};


class GenericTableWrapper: public QAbstractTableModel
{
  Q_OBJECT

protected:
  GenericTableWrapper(AbstractConfigObjectList *list, QObject *parent=nullptr);

public:
  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of rows. */
  int rowCount(const QModelIndex &index) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  Qt::DropActions supportedDropActions() const;

  bool insertRows(int row, int count, const QModelIndex &parent);
  bool removeRows(int row, int count, const QModelIndex &parent);
  bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                const QModelIndex &destinationParent, int destinationChild);

  bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const;

signals:
  /** Gets emitted once the table has been changed. */
  void modified();

protected slots:
  /** Internal used callback on deleted config. */
  void onListDeleted();
  /** Internal used callback on adding an item. */
  void onItemAdded(int idx);
  /** Internal callback on deleted channels. */
  void onItemRemoved(int idx);
  /** Internal callback on modified channels. */
  void onItemModified(int idx);

protected:
  /** Returns a string containing all extension properties set. */
  QString formatExtensions(int idx) const;

protected:
  /** Holds a weak reference to the list object. */
  AbstractConfigObjectList *_list;
  /** Insert index for drag & drop move. */
  int _insertRow;
};


class ChannelListWrapper: public GenericTableWrapper
{
  Q_OBJECT

public:
  ChannelListWrapper(ChannelList *list, QObject *parent=nullptr);

public:
  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of columns. */
  int columnCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns data at cell. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implements QAbstractTableModel, returns header at section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
};


class ChannelRefListWrapper: public GenericListWrapper
{
  Q_OBJECT

public:
  ChannelRefListWrapper(ChannelRefList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractListModel
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};


class RoamingChannelListWrapper: public GenericTableWrapper
{
  Q_OBJECT

public:
  RoamingChannelListWrapper(RoamingChannelList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractTableModel
  /** Returns the number of columns, implements the QAbstractTableModel. */
  int columnCount(const QModelIndex &index) const;
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};


class RoamingChannelRefListWrapper: public GenericListWrapper
{
  Q_OBJECT

public:
  RoamingChannelRefListWrapper(RoamingChannelRefList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractListModel
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};


class ContactListWrapper: public GenericTableWrapper
{
  Q_OBJECT

public:
  ContactListWrapper(ContactList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractTableModel
  /** Returns the number of columns, implements the QAbstractTableModel. */
  int columnCount(const QModelIndex &index) const;
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Returns the header at given section, implements the QAbstractTableModel. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
};


class ZoneListWrapper: public GenericListWrapper
{
  Q_OBJECT

public:
  ZoneListWrapper(ZoneList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractListModel
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};


class PositioningSystemListWrapper: public GenericTableWrapper
{
  Q_OBJECT

public:
  PositioningSystemListWrapper(PositioningSystems *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractTableModel
  /** Returns the number of columns, implements the QAbstractTableModel. */
  int columnCount(const QModelIndex &index) const;
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Returns the header at given section, implements the QAbstractTableModel. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
};


class ScanListsWrapper: public GenericListWrapper
{
  Q_OBJECT

public:
  ScanListsWrapper(ScanLists *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractListModel
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};


class GroupListsWrapper: public GenericListWrapper
{
  Q_OBJECT

public:
  GroupListsWrapper(RXGroupLists *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractListModel
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};


class GroupListWrapper: public GenericListWrapper
{
  Q_OBJECT

public:
  GroupListWrapper(RXGroupList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractListModel
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};


class RoamingListWrapper: public GenericListWrapper
{
  Q_OBJECT

public:
  RoamingListWrapper(RoamingZoneList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractListModel
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Returns the header at given section, implements the QAbstractTableModel. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
};


class RadioIdListWrapper: public GenericTableWrapper
{
  Q_OBJECT

public:
  RadioIdListWrapper(RadioIDList *list, QObject *parent=nullptr);

public:
  // Implementation of QAbstractTableModel
  /** Returns the number of columns, implements the QAbstractTableModel. */
  int columnCount(const QModelIndex &index) const;
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Returns the header at given section, implements the QAbstractTableModel. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
};



#endif // CONFIG_ITEM_WRAPPER_HH
