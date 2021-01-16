#ifndef CONTACT_HH
#define CONTACT_HH

#include <QObject>
#include <QVector>
#include <QAbstractTableModel>


class Config;

/** Represents the base-class for all contact types, Analog (DTMF) or Digital (DMR).
 *
 * @ingroup conf */
class Contact: public QObject
{
	Q_OBJECT

protected:
  /** Hidden constructor.
   * @param name   Specifies the name of the contact.
   * @param rxTone Specifies whether a ring-tone for this contact is used.
   * @param parent Specifies the QObject parent. */
	Contact(const QString &name, bool rxTone=true, QObject *parent=nullptr);

public:
  /** Returns the name of the contact. */
	const QString &name() const;
  /** (Re)Sets the name of the contact. */
	bool setName(const QString &name);
  /** Returns @c true if the ring-tone is enabled for this contact. */
  bool rxTone() const;
  /** Enables/disables the ring-tone for this contact. */
  void setRXTone(bool enable);

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

signals:
  /** Gets emitted if the contact is modified. */
	void modified();

protected:
  /** Contact name. */
	QString _name;
  /** Ringtone enabled? */
  bool _rxTone;
};


/** Represents an analog contact, that is a DTMF number.
 * @ingroup conf */
class DTMFContact: public Contact
{
	Q_OBJECT

public:
  /** Constructs a DTMF (analog) contact.
   * @param name   Specifies the contact name.
   * @param number Specifies the DTMF number (0-9,A,B,C,D,*,#).
   * @param rxTone Specifies whether the ring-tone is enabled for this contact.
   * @param parent Specifies the QObject parent. */
	DTMFContact(const QString &name, const QString &number, bool rxTone=false, QObject *parent=nullptr);

  /** Retruns @c true if the DTMF contact is valid (has a valid DTMF number). */
  bool isValid() const;

  /** Returns the DTMF number of this contact.
   * The number must consist of 0-9, a-f, * or #. */
	const QString &number() const;
  /** (Re-)Sets the DTMF number of this contact. */
	bool setNumber(const QString &number);

protected:
  /** The DTMF number. */
	QString _number;
};


/** Represents a digital contact, that is a DMR number.
 * @ingroup conf */
class DigitalContact: public Contact
{
	Q_OBJECT

public:
  /** Possible call types for a contact. */
	typedef enum {
		PrivateCall,   ///< A private call.
    GroupCall,     ///< A group call.
    AllCall        ///< An all-call.
	} Type;

public:
  /** Constructs a DMR (digital) contact.
   * @param type   Specifies the call type (private, group, all-call).
   * @param name   Specifies the contact name.
   * @param number Specifies the DMR number for this contact.
   * @param rxTone Specifies whether the ring-tone is enabled for this contact.
   * @param parent Specifies the QObject parent. */
	DigitalContact(Type type, const QString &name, uint number, bool rxTone=false, QObject *parent=nullptr);

  /** Returns the call-type. */
	Type type() const;
  /** (Re-)Sets the call-type. */
  void setType(Type type);
  /** Returns the DMR number. */
	uint number() const;
  /** (Re-)Sets the DMR number of the contact. */
	bool setNumber(uint number);

protected:
  /** The call type. */
	Type _type;
  /** The DMR number of the contact. */
	uint _number;
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
class ContactList: public QAbstractTableModel
{
	Q_OBJECT

public:
  /** Constructs an empty contact list. */
	explicit ContactList(QObject *parent=nullptr);

  /** Returns the number of contacts. */
	int count() const;
  /** Returns the number of digital contacts. */
	int digitalCount() const;
  /** Returns the number of DTMF contacts. */
	int dtmfCount() const;

  /** Clears the contact list. */
  void clear();

  /** Returns the contact at index @c idx. */
  Contact *contact(int idx) const;
  /** Returns the digital contact at index @c idx among digital contacts. */
  DigitalContact *digitalContact(int idx) const;
  /** Searches for a digital contact with the given number. */
  DigitalContact *findDigitalContact(uint number) const;
  /** Returns the DTMF contact at index @c idx among DTMF contacts. */
  DTMFContact *dtmfContact(int idx) const;

  /** Returns the index of the given contact. */
  int indexOf(Contact *contact) const;
  /** Returns the index of the given digital contact within digital contacts. */
  int indexOfDigital(DigitalContact *contact) const;
  /** Returns the index of the given DTMF contact within DTMF contacts. */
  int indexOfDTMF(DTMFContact *contact) const;

  /** Adds a contact to the list at the given @ç row.
   * If row < 0, the contact gets appended to the list. */
  int addContact(Contact *contact, int row=-1);
  /** Removes the contact at the given index. */
  bool remContact(int idx);
  /** Removes the given contact from the list. */
	bool remContact(Contact *contact);

  /** Moves the contact at the given @c row one up. */
  bool moveUp(int row);
  /** Moves the contact at the given @c row one down. */
  bool moveDown(int row);

	// Implementation of QAbstractTableModel
  /** Returns the number of rows, implements the QAbstractTableModel. */
	int rowCount(const QModelIndex &index) const;
  /** Returns the number of columns, implements the QAbstractTableModel. */
	int columnCount(const QModelIndex &index) const;
  /** Returns the cell data at given index, implements the QAbstractTableModel. */
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Returns the header at given section, implements the QAbstractTableModel. */
	QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

signals:
  /** Gets emitted if the list has changed or any contact in it. */
	void modified();

protected slots:
  /** Internal callback on deleted contacts. */
	void onContactDeleted(QObject *contact);
  /** Internal callback on modified contacts. */
  void onContactEdited();

protected:
  /** Just the vector of contacts. */
	QVector<Contact *> _contacts;
};

#endif // CONTACT_HH
