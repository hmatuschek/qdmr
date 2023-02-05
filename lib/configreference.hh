#ifndef CONFIGREFERENCE_HH
#define CONFIGREFERENCE_HH

#include "configobject.hh"
#include <QSet>

class Channel;
class DMRChannel;
class FMChannel;
class ScanList;
class EncryptionKey;

/** Implements a reference to a config object.
 * This class is only used to implement the automatic generation/parsing of the YAML codeplug files.
 * @ingroup conf */
class ConfigObjectReference: public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  ConfigObjectReference(const QMetaObject &elementType=ConfigObject::staticMetaObject, QObject *parent = nullptr);

public:
  /** Returns @c true if the reference is null.
   * That is, if there is no object referenced. */
  bool isNull() const;

  /** Resets the reference.
   * Same as @c set(nullptr). */
  virtual void clear();
  /** Sets the reference.
   * If set to @c nullptr, the reference gets cleared. */
  virtual bool set(ConfigObject *object);
  /** Copies the reference from another reference. */
  virtual bool copy(const ConfigObjectReference *ref);
  /** Adds a possible type to this reference. */
  virtual bool allow(const QMetaObject *elementType);

  /** Returns the type names of allowed objects. */
  const QStringList &elementTypeNames() const;

  /** Returns the reference as the specified type. */
  template <class Type>
  Type *as() const {
    if (nullptr == _object)
      return nullptr;
    return _object->as<Type>();
  }

  /** Returns @c true if the reference is of the specified type. */
  template <class Type>
  bool is() const {
    if (nullptr == _object)
      return false;
    return _object->is<Type>();
  }

  /** Compares the references. */
  int compare(const ConfigObjectReference &other) const;

signals:
  /** Gets emitted if the reference is changed.
   * This signal is not emitted if the referenced object is modified. */
  void modified();

protected slots:
  /** Internal call back whenever the referenced object gets deleted. */
  void onReferenceDeleted(QObject *obj);

protected:
  /** Holds the static QMetaObject of the possible element types. */
  QStringList _elementTypes;
  /** The reference to the object. */
  ConfigObject *_object;
};


/** Represents a reference to a contact.
 * This class is only used to automate the parsing and generation of the YAML codeplug file.
 * @ingroup config */
class ContactReference: public ConfigObjectReference
{
  Q_OBJECT

protected:
  /** Constructor. */
  ContactReference(const QMetaObject &elementType, QObject *parent = nullptr);

public:
  /** Constructor. */
  explicit ContactReference(QObject *parent=nullptr);
};


/** Represents a reference to a DMR contact.
 * @ingroup conf*/
class DMRContactReference: public ContactReference
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DMRContactReference(QObject *parent=nullptr);
};


/** List of references to DMR contacts. */
class DMRContactRefList: public ConfigObjectRefList
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DMRContactRefList(QObject *parent=nullptr);
};


/** Represents a reference to a channel.
 * This class is only used to automate the parsing and generation of the YAML codeplug file.
 * @ingroup config */
class ChannelReference: public ConfigObjectReference
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  ChannelReference(const QMetaObject &elementType, QObject *parent = nullptr);

public:
  /** Constructor. */
  explicit ChannelReference(QObject *parent=nullptr);
};


/** Implements a reference to a DMR channel.
 * @ingroup conf */
class DMRChannelReference: public ChannelReference
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DMRChannelReference(QObject *parent=nullptr);
};


/** Implements a reference to a FM channel.
 * @ingroup conf */
class FMChannelReference: public ChannelReference
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit FMChannelReference(QObject *parent=nullptr);
};


/** Represents a list of weak references to channels (analog and digital).
 * @ingroup config */
class ChannelRefList: public ConfigObjectRefList
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ChannelRefList(const QMetaObject &elementTypes, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit ChannelRefList(QObject *parent=nullptr);
};


/** Represents a list of references to some DMR channels.
 * @ingroup config */
class DMRChannelRefList: public ChannelRefList
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit DMRChannelRefList(QObject *parent=nullptr);
};


/** Represents a list of references to some roaming channels.
 * @ingroup config */
class RoamingChannelRefList: public ConfigObjectRefList
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit RoamingChannelRefList(QObject *parent=nullptr);
};


/** Implements a reference to a scan list.
 * @ingroup conf */
class ScanListReference: public ConfigObjectReference
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ScanListReference(QObject *parent=nullptr);
};


/** Implements a reference to a positioning system.
 * @ingroup conf */
class PositioningSystemReference: public ConfigObjectReference {
  Q_OBJECT

protected:
  /** Hidden constructor. */
  PositioningSystemReference(const QMetaObject &elementType, QObject *parent = nullptr);

public:
  /** Constructor. */
  explicit PositioningSystemReference(QObject *parent=nullptr);
};


/** Implements a reference to an APRS system.
 * @ingroup conf */
class APRSSystemReference: public PositioningSystemReference {
  Q_OBJECT

public:
  /** Constructor. */
  explicit APRSSystemReference(QObject *parent=nullptr);
};


/** Implements a reference to a GPS system.
 * @ingroup conf */
class GPSSystemReference: public PositioningSystemReference {
  Q_OBJECT

public:
  /** Constructor. */
  explicit GPSSystemReference(QObject *parent=nullptr);
};


/** Implements a reference to a radio ID.
 * @ingroup conf */
class DMRRadioIDReference: public ConfigObjectReference {
  Q_OBJECT

public:
  /** Constructor. */
  explicit DMRRadioIDReference(QObject *parent=nullptr);
};


/** Implements a reference to a group list.
 * @ingroup conf */
class GroupListReference: public ConfigObjectReference {
  Q_OBJECT

public:
  /** Constructor. */
  explicit GroupListReference(QObject *parent=nullptr);
};


/** Implements a reference to a roaming zone.
 * @ingroup conf */
class RoamingZoneReference: public ConfigObjectReference {
  Q_OBJECT

public:
  /** Constructor. */
  explicit RoamingZoneReference(QObject *parent=nullptr);
};


/** Implements a reference to an encryption key.
 * @ingroup conf */
class EncryptionKeyReference: public ConfigObjectReference {
  Q_OBJECT

public:
  /** Constructor. */
  explicit EncryptionKeyReference(QObject *parent=nullptr);
};


/** Implements a reference to a zone.
 * @ingroup conf */
class ZoneReference: public ConfigObjectReference {
  Q_OBJECT

public:
  /** Constructor. */
  explicit ZoneReference(QObject *parent=nullptr);
};


#endif // CONFIGREFERENCE_HH
