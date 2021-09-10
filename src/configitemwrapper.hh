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
  /** Moves the channel at index @c idx one step up. */
  virtual bool moveUp(int idx);
  /** Moves the channels at one step up. */
  virtual bool moveUp(int first, int last);
  /** Moves the channel at index @c idx one step down. */
  virtual bool moveDown(int idx);
  /** Moves the channels one step down. */
  virtual bool moveDown(int first, int last);

  // QAbstractListModel interface
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
  /** Moves the channel at index @c idx one step up. */
  virtual bool moveUp(int idx);
  /** Moves the channels at one step up. */
  virtual bool moveUp(int first, int last);
  /** Moves the channel at index @c idx one step down. */
  virtual bool moveDown(int idx);
  /** Moves the channels one step down. */
  virtual bool moveDown(int first, int last);

  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of rows. */
  int rowCount(const QModelIndex &index) const;

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
  /** Holds a weak reference to the list object. */
  AbstractConfigObjectList *_list;
};


class ChannelListWrapper: public GenericTableWrapper
{
  Q_OBJECT

public:
  ChannelListWrapper(ChannelList *list, QObject *parent=nullptr);

public:
  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of colums. */
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
