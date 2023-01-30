#ifndef CONTACT_HH
#define CONTACT_HH

#include "configobject.hh"
#include "anytone_extension.hh"
#include "opengd77_extension.hh"
#include <QVector>
#include <QAbstractTableModel>


class Config;


/** Represents the base-class for all contact types, analog (DTMF) or digital (DMR, M17).
 *
 * @ingroup conf */
class Contact: public ConfigObject
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "cont");

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
  void clear();

public:
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
  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());
  bool link(const YAML::Node &node, const Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Ringtone enabled? */
  bool _ring;
};


/** Base class for all analog contacts.
 * @ingroup conf */
class AnalogContact: public Contact
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AnalogContact(QObject *parent=nullptr);

  /** Constructor. */
  AnalogContact(const QString &name, bool rxTone, QObject *parent=nullptr);
};


/** Represents an analog contact, that is a DTMF number.
 * @ingroup conf */
class DTMFContact: public AnalogContact
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

  ConfigItem *clone() const;
  void clear();

  /** Returns the DTMF number of this contact.
   * The number must consist of 0-9, a-f, * or #. */
	const QString &number() const;
  /** (Re-)Sets the DTMF number of this contact. */
	bool setNumber(const QString &number);

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());

protected:
  /** The DTMF number. */
	QString _number;
};


/** Base class for all digital contacts.
 * @ingroup conf */
class DigitalContact: public Contact
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit DigitalContact(QObject *parent=nullptr);

  /** Hidden constructor. */
  DigitalContact(const QString &name, bool ring, QObject *parent=nullptr);
};


/** Represents a digital contact, that is a DMR number.
 * @ingroup conf */
class DMRContact: public DigitalContact
{
	Q_OBJECT

  /** The type of the contact. */
  Q_PROPERTY(Type type READ type WRITE setType)
  /** The number of the contact. */
  Q_PROPERTY(unsigned number READ number WRITE setNumber)
  /** The AnyTone extension to the digital contact. */
  Q_PROPERTY(AnytoneContactExtension* anytone READ anytoneExtension WRITE setAnytoneExtension)
  /** The OpenGD77 extension to the digital contact. */
  Q_PROPERTY(OpenGD77ContactExtension* openGD77 READ openGD77ContactExtension WRITE setOpenGD77ContactExtension)

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
  explicit DMRContact(QObject *parent=nullptr);

  /** Constructs a DMR (digital) contact.
   * @param type   Specifies the call type (private, group, all-call).
   * @param name   Specifies the contact name.
   * @param number Specifies the DMR number for this contact.
   * @param ring Specifies whether the ring-tone is enabled for this contact.
   * @param parent Specifies the QObject parent. */
  DMRContact(Type type, const QString &name, unsigned number, bool ring=false, QObject *parent=nullptr);

  ConfigItem *clone() const;
  void clear();

  /** Returns the call-type. */
	Type type() const;
  /** (Re-)Sets the call-type. */
  void setType(Type type);
  /** Returns the DMR number. */
	unsigned number() const;
  /** (Re-)Sets the DMR number of the contact. */
	bool setNumber(unsigned number);

  /** Returns the OpenGD77 extension, or @c nullptr if not set. */
  OpenGD77ContactExtension *openGD77ContactExtension() const;
  /** Sets the OpenGD77 extension. */
  void setOpenGD77ContactExtension(OpenGD77ContactExtension *ext);

  /** Returns the AnyTone extension, or @c nullptr if not set. */
  AnytoneContactExtension *anytoneExtension() const;
  /** Sets the AnyTone extension. */
  void setAnytoneExtension(AnytoneContactExtension *ext);

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());

protected:
  /** The call type. */
	Type _type;
  /** The DMR number of the contact. */
	unsigned _number;
  /** Owns the AnytoneContactextension to the digital contacts. */
  AnytoneContactExtension *_anytone;
  /** Owns the OpenGD77 extensions to the digital contacts. */
  OpenGD77ContactExtension *_openGD77;
};


/** Represents the list of contacts within the abstract radio configuration.
 *
 * A special feature of this list, is that DTMF and digital contacts can be accessed by their own
 * unique index although they are held within this single list. Most radios manage digital and
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
  DMRContact *digitalContact(int idx) const;
  /** Searches for a digital contact with the given number. */
  DMRContact *findDigitalContact(unsigned number) const;
  /** Returns the DTMF contact at index @c idx among DTMF contacts. */
  DTMFContact *dtmfContact(int idx) const;

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // CONTACT_HH
