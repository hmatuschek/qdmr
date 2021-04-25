#ifndef RXGROUPLIST_HH
#define RXGROUPLIST_HH

#include <QObject>
#include <QAbstractListModel>
#include "contact.hh"

class Config;

/** Generic representation of a RX group list.
 * @ingroup conf */
class RXGroupList: public QAbstractListModel
{
	Q_OBJECT

public:
  /** Constructor.
   * @param name Specifies the name of the group list.
   * @param parent @c QObject parent instance. */
	explicit RXGroupList(const QString &name, QObject *parent=nullptr);

  /** Returns the number of contacts within the group list. */
  int count() const;
  /** Resets & clears this group list. */
  void clear();
  /** Returns the name of this group list. */
	const QString &name() const;
  /** Sets the name of this group list. */
	bool setName(const QString &name);

  /** Returns the contact at the given list index. */
	DigitalContact *contact(int idx) const;
  /** Adds a contect to the list. */
	int addContact(DigitalContact *contact);
  /** Removes the given contact from the list. */
	bool remContact(DigitalContact *contact);
  /** Removes the contact from the list at the given position. */
	bool remContact(int idx);


  /** Implementation of QAbstractListModel, returns the number of rows. */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /** Implementation of QAbstractListModel, returns the table data at the given index. */
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  /** Implementation of QAbstractListModel, returns the header data at the given section. */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
  /** Gets emitted whenever the group list gets modified. */
	void modified();

protected slots:
  /** Internal used callback to handle deleted contacts. */
	void onContactDeleted(QObject *obj);

protected:
  /** The group list name. */
	QString _name;
  /** The list of contacts. */
	QVector<DigitalContact *> _contacts;
};


/** Represents the list of RX group lists within the generic configuration.
 * @ingroup conf */
class RXGroupLists: public QAbstractListModel
{
	Q_OBJECT

public:
  /** Constructor. */
	explicit RXGroupLists(QObject *parent=nullptr);

  /** Returns the number of RX group lists. */
  int count() const;
  /** Clears the list. */
  void clear();

  /** Returns the index of the given RX group list. */
  int indexOf(RXGroupList *list) const;
  /** Returns the list at the given index. */
	RXGroupList *list(int idx) const;

  /** Adds a rx group list at the given position. If row<0, the list gets appended. */
	int addList(RXGroupList *list, int row=-1);
  /** Removes the given rx group list. */
	bool remList(RXGroupList *list);
  /** Removes the rx group list at the specified index. */
	bool remList(int idx);
  /** Moves the group list at the given row one up. */
  bool moveUp(int row);
  /** Moves the group lists at the given rows one up. */
  bool moveUp(int first, int last);
  /** Moves the group list at the given row one down. */
  bool moveDown(int row);
  /** Moves the group lists at the given rows one down. */
  bool moveDown(int first, int last);

  /** Implements the QAbstractListModel, returns the number of rows. */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /** Implements the QAbstractListModel, returns entry data at the given index. */
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  /** Implements the QAbstractListModel, returns header data at the given section. */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
  /** Gets emitted whenever a RX group list gets modified. */
	void modified();

protected slots:
  /** Internal used callback to handle deleted RX group lists. */
  void onListDeleted(QObject *);

protected:
  /** Just the list of RX group lists. */
	QVector<RXGroupList *> _lists;
};

#endif // RXGROUPLIST_HH
