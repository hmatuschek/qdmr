#ifndef CONTACT_HH
#define CONTACT_HH

#include "configobject.hh"
#include <QVector>
#include <QAbstractTableModel>


class Config;

/** Represents the base-class for all contact types, Analog (DTMF) or Digital (DMR).
 *
 * @ingroup conf */
class Contact: public ConfigObject
{
	Q_OBJECT

  /** The name of the contact. */
  Q_PROPERTY(QString name READ name WRITE setName)
  /** If @c true and supported by radio, ring on call from this contact. */
  Q_PROPERTY(bool ring READ ring WRITE setRing)

protected:
  /** Default constructor. */
  explicit Contact(QObject *parent=nullptr);
  /** Hidden constructor.
   * @param name   Specifies the name of the contact.
   * @param ring Specifies whether a ring-tone for this contact is used.
   * @param parent Specifies the QObject parent. */
  Contact(const QString &name, bool ring=true, QObject *parent=nullptr);

public:
  /** Returns the name of the contact. */
	const QString &name() const;
  /** (Re)Sets the name of the contact. */
	bool setName(const QString &name);
  /** Returns @c true if the ring-tone is enabled for this contact. */
  bool ring() const;
  /** Enables/disables the ring-tone for this contact. */
  void setRing(bool enable);

  /** Typecheck contact.
   * For example, @c contact->is<DigitalContact>() returns @c true if @c contact is a
   * @c DigitalContact. */
	template <class T>
	bool is() const {
		return 0 != dynamic_cast<const T *>(this);
	}
  /** Typecast contact. */
	template<class T>
	T *as() {
		return dynamic_cast<T *>(this);
	}
  /** Typecast contact. */
	template<class T>
	const T *as() const {
		return dynamic_cast<const T *>(this);
	}

public:
  ConfigObject *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx);
  bool parse(const YAML::Node &node, Context &ctx);

protected:
  /** Contact name. */
	QString _name;
  /** Ringtone enabled? */
  bool _ring;
};


/** Represents an analog contact, that is a DTMF number.
 * @ingroup conf */
class DTMFContact: public Contact
{
	Q_OBJECT

  /** The contact number. */
  Q_PROPERTY(QString number READ number WRITE setNumber)

public:
  /** Default constructor. */
  explicit DTMFContact(QObject *parent=nullptr);
  /** Constructs a DTMF (analog) contact.
   * @param name   Specifies the contact name.
   * @param number Specifies the DTMF number (0-9,A,B,C,D,*,#).
   * @param ring Specifies whether the ring-tone is enabled for this contact.
   * @param parent Specifies the QObject parent. */
  DTMFContact(const QString &name, const QString &number, bool ring=false, QObject *parent=nullptr);

  /** Returns the DTMF number of this contact.
   * The number must consist of 0-9, a-f, * or #. */
	const QString &number() const;
  /** (Re-)Sets the DTMF number of this contact. */
	bool setNumber(const QString &number);

public:
  YAML::Node serialize(const Context &context);

protected:
  /** The DTMF number. */
	QString _number;
};


/** Represents a digital contact, that is a DMR number.
 * @ingroup conf */
class DigitalContact: public Contact
{
	Q_OBJECT

  /** The type of the contact. */
  Q_PROPERTY(Type type READ type WRITE setType)
  /** The number of the contact. */
  Q_PROPERTY(unsigned number READ number WRITE setNumber)

public:
  /** Possible call types for a contact. */
	typedef enum {
		PrivateCall,   ///< A private call.
    GroupCall,     ///< A group call.
    AllCall        ///< An all-call.
	} Type;
  Q_ENUM(Type)

public:
  /** Default constructor. */
  explicit DigitalContact(QObject *parent=nullptr);
  /** Constructs a DMR (digital) contact.
   * @param type   Specifies the call type (private, group, all-call).
   * @param name   Specifies the contact name.
   * @param number Specifies the DMR number for this contact.
   * @param ring Specifies whether the ring-tone is enabled for this contact.
   * @param parent Specifies the QObject parent. */
  DigitalContact(Type type, const QString &name, unsigned number, bool ring=false, QObject *parent=nullptr);

  /** Returns the call-type. */
	Type type() const;
  /** (Re-)Sets the call-type. */
  void setType(Type type);
  /** Returns the DMR number. */
	unsigned number() const;
  /** (Re-)Sets the DMR number of the contact. */
	bool setNumber(unsigned number);

public:
  YAML::Node serialize(const Context &context);

protected:
  /** The call type. */
	Type _type;
  /** The DMR number of the contact. */
	unsigned _number;
};


/** Represents the list of contacts within the abstract radio configuration.
 *
 * A special feature of this list, is that DTMF and digital contacts can be accessed by their own
 * unique index altough they are held within this single list. Most radios manage digital and
 * DTMF contacts in separate lists, hence a means to iterate over and get indices of digital and
 * DTMF contacts only is needed.
 *
 * This class implements the @c QAbstractTableModel, such that the list can be shown with the
 * @c QTableView widget.
 *
 * @ingroup conf */
class ContactList: public ConfigObjectList
{
	Q_OBJECT

public:
  /** Constructs an empty contact list. */
	explicit ContactList(QObject *parent=nullptr);

  int add(ConfigObject *obj, int row=-1);

  /** Returns the number of digital contacts. */
	int digitalCount() const;
  /** Returns the number of DTMF contacts. */
	int dtmfCount() const;

  /** Returns the contact at index @c idx. */
  Contact *contact(int idx) const;
  /** Returns the digital contact at index @c idx among digital contacts. */
  DigitalContact *digitalContact(int idx) const;
  /** Searches for a digital contact with the given number. */
  DigitalContact *findDigitalContact(unsigned number) const;
  /** Returns the DTMF contact at index @c idx among DTMF contacts. */
  DTMFContact *dtmfContact(int idx) const;

  /** Returns the index of the given digital contact within digital contacts. */
  int indexOfDigital(DigitalContact *contact) const;
  /** Returns the index of the given DTMF contact within DTMF contacts. */
  int indexOfDTMF(DTMFContact *contact) const;

public:
  ConfigObject *allocateChild(const YAML::Node &node, ConfigObject::Context &ctx);
};

#endif // CONTACT_HH
