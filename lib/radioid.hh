#ifndef RADIOID_HH
#define RADIOID_HH

#include "configobject.hh"

class DMRRadioIDReference;


/** Abstract base class for all radio IDs.
 *
 * That is, DMR radio IDs as well as M17, DTMF, ZVEI, 5-tone etc PTT-IDs.
 *
 * @ingroup conf */
class RadioID: public ConfigObject
{
  Q_OBJECT

protected:
  /** Hidden default constructor.
   * Use one of the derived classes to instantiate radio IDs. */
  explicit RadioID(QObject *parent=nullptr);
  /** Hidden constructor with name. */
  RadioID(const QString &name, QObject *parent=nullptr);
};


/** Represents a DMR radio ID within the abstract config.
 *
 * This class is used to store the DMR ID(s) of the radio.
 *
 * @ingroup conf */
class DMRRadioID : public RadioID
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "id")

  /** The number of the radio ID. */
  Q_PROPERTY(unsigned number READ number WRITE setNumber)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit DMRRadioID(QObject *parent=nullptr);

  /** Constructor.
   * @param name Specifies the name of the ID.
   * @param number Specifies the DMR ID.
   * @param parent Specifies the parent QObject owning this object. */
  DMRRadioID(const QString &name, uint32_t number, QObject *parent = nullptr);

  ConfigItem *clone() const;

  /** Returns the DMR ID. */
  uint32_t number() const;
  /** Sets the DMR ID. */
  void setNumber(uint32_t number);

  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());
  bool parse(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
  bool link(const YAML::Node &node, const ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Holds the DMR ID. */
  uint32_t _number;
};


/** A singleton radio ID representing the default DMR radio ID within the abstract config.
 * @ingroup conf */
class DefaultRadioID: public DMRRadioID
{
  Q_OBJECT

protected:
  /** Constructor. */
  explicit DefaultRadioID(QObject *parent=nullptr);

public:
  /** Factory method returning the singleton instance. */
  static DefaultRadioID *get();

private:
  /** The singleton instance. */
  static DefaultRadioID *_instance;
};


/** Represents a DTMF radio ID as used for PTT-ID on analog channels.
 *
 * This class just holds the name and DTMF number of the ID.
 * @ingroup conf */
class DTMFRadioID: public RadioID
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "dtmf")

  /** The DTMF number of the radio ID. */
  Q_PROPERTY(QString number READ number WRITE setNumber)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit DTMFRadioID(QObject *parent=nullptr);

  /** Constructor from name and number.
   * @param name Specifies the name of the DTMF radio ID.
   * @param number Specifies the DTMF number of the radio ID.
   * @param parent Specifies the QObject parent, the object that owns this one. */
  explicit DTMFRadioID(const QString &name, const QString &number, QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the DTMF number of the radio ID. */
  const QString &number() const;
  /** Sets the DTMF number of the radio ID. */
  void setNumber(const QString &number);

protected:
  /** Holds the DTMF number of the radio ID. */
  QString _number;
};


/** Represents the list of configured DMR IDs (radio IDs) within the abstract config.
 * @ingroup conf */
class RadioIDList: public ConfigObjectList
{
  Q_OBJECT


public:
  /** Constructor. */
  explicit RadioIDList(QObject *parent=nullptr);

  void clear();

  /** Returns the radio ID at the given index. */
  DMRRadioID *getId(int idx) const;
  /** Searches the DMR ID object associated with the given DMR ID. */
  DMRRadioID *find(uint32_t id) const;

  int add(ConfigObject *obj, int row=-1, bool unique=true);

  /** Adds the given DMR ID. */
  virtual int addId(const QString &name, uint32_t id);
  /** Deletes and removes the given DMR ID. */
  virtual bool delId(uint32_t id);

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};


#endif // RADIOID_HH
