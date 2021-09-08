#ifndef CONFIGREFERENCE_HH
#define CONFIGREFERENCE_HH

#include "configobject.hh"

class Channel;
class DigitalChannel;
class AnalogChannel;
class ScanList;


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

signals:
  /** Gets emitted if the reference is changed.
   * This signal is not emitted if the referenced object is modified. */
  void modified();

protected slots:
  /** Internal call back whenever the referenced object gets deleted. */
  void onReferenceDeleted(QObject *obj);

protected:
  /** Holds the static QMetaObject of the element type. */
  QMetaObject _elementType;
  /** The reference to the object. */
  ConfigObject *_object;
};


/** List of references to digital contacts. */
class DigitalContactRefList: public ConfigObjectRefList
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DigitalContactRefList(QObject *parent=nullptr);
};


/** Represents a reference to a channel.
 * This class is only used to automate the parsing and generation of the YAML codeplug file.
 * @ingroup config */
class ChannelReference: public ConfigObjectReference
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ChannelReference(QObject *parent=nullptr);
};


/** Represents a list of weak references to channels (analog and digital).
 * @ingroup config */
class ChannelRefList: public ConfigObjectRefList
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ChannelRefList(const QMetaObject &elementType, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit ChannelRefList(QObject *parent=nullptr);
};


/** Represents a list of references to some digital channels.
 * @ingroup config */
class DigitalChannelRefList: public ChannelRefList
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit DigitalChannelRefList(QObject *parent=nullptr);
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


#endif // CONFIGREFERENCE_HH
