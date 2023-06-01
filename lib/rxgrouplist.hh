#ifndef RXGROUPLIST_HH
#define RXGROUPLIST_HH

#include <QObject>
#include "configreference.hh"

class Config;
class DMRContact;


/** Generic representation of a RX group list.
 * @ingroup conf */
class RXGroupList: public ConfigObject
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "grp")

  /** The list of contacts. */
  Q_PROPERTY(DMRContactRefList* contacts READ contacts)

public:
  /** Default constructor. */
  explicit RXGroupList(QObject *parent=nullptr);
  /** Constructor.
   * @param name Specifies the name of the group list.
   * @param parent @c QObject parent instance. */
  RXGroupList(const QString &name, QObject *parent=nullptr);

  /** Copy from other group list. */
  RXGroupList &operator =(const RXGroupList &other);
  ConfigItem *clone() const;

  /** Returns the number of contacts within the group list. */
  int count() const;
  /** Resets & clears this group list. */
  void clear();

  /** Returns the contact at the given list index. */
	DMRContact *contact(int idx) const;
  /** Adds a contact to the list. */
  int addContact(DMRContact *contact, int idx=-1);
  /** Removes the given contact from the list. */
	bool remContact(DMRContact *contact);
  /** Removes the contact from the list at the given position. */
	bool remContact(int idx);

  /** Returns the contact list. */
  const DMRContactRefList *contacts() const;
  /** Returns the contact list. */
  DMRContactRefList *contacts();

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());

protected slots:
  /** Internal used callback to handle list modifications. */
  void onModified();

protected:
  /** The list of contacts. */
  DMRContactRefList _contacts;
};


/** Represents the list of RX group lists within the generic configuration.
 * @ingroup conf */
class RXGroupLists: public ConfigObjectList
{
	Q_OBJECT

public:
  /** Constructor. */
	explicit RXGroupLists(QObject *parent=nullptr);

  /** Returns the group list at the given index. */
	RXGroupList *list(int idx) const;

  int add(ConfigObject *obj, int row=-1);

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // RXGROUPLIST_HH
