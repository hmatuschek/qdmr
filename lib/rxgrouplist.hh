#ifndef RXGROUPLIST_HH
#define RXGROUPLIST_HH

#include <QObject>
#include <QAbstractListModel>
#include "contact.hh"

class Config;

/** Generic representation of a RX group list.
 * @ingroup conf */
class RXGroupList: public ConfigObject
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

protected:
  bool serialize(YAML::Node &node, const Context &context);

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
class RXGroupLists: public ConfigObjectList
{
	Q_OBJECT

public:
  /** Constructor. */
	explicit RXGroupLists(QObject *parent=nullptr);

  /** Returns the list at the given index. */
	RXGroupList *list(int idx) const;

  int add(ConfigObject *obj, int row=-1);
};

#endif // RXGROUPLIST_HH
