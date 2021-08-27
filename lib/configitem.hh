#ifndef CONFIG_ITEM_HH
#define CONFIG_ITEM_HH

#include <QObject>
#include <QVector>
#include <QSet>
#include <QHash>
#include <yaml-cpp/yaml.h>

#include "channel.hh"
#include "signaling.hh"
#include "logger.hh"

#include "genericconfigitem.hh"


namespace Configuration {

class ScanList;
class GroupList;
class Positioning;
class RoamingZone;
class RadioId;
class APRSPositioning;
class Channel;
class DigitalChannel;
class AnalogChannel;
class DigitalContact;
class GroupCall;


/** Represents a radio ID and name definition. *
 * @ingroup conf */
class RadioId: public Object
{
  Q_OBJECT

public:
  /** Declares a radio ID. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

  protected:
    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit RadioId(QObject *parent=nullptr);

public:
  /** Returns the name for this radio id. */
  const QString &name() const;
  /** Sets the name for this radio id. */
  void setName(const QString &nm);

  /** Returns the radio id. */
  uint32_t number() const;
  /** Sets the radio id. */
  void setNumber(uint32_t id);
};


/** Base class for all contact definitions.
 * @ingroup conf */
class Contact: public Object
{
Q_OBJECT

public:
  /** Base class of all contact declarations. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration(const QString &description="");
  };

protected:
  /** Hidden constructor. */
  Contact(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the contact. */
  const QString &name() const;
  /** Sets the name of the contact. */
  void setName(const QString &nm);

  /** Returns @c true if the contact rings. */
  bool ring() const;
  /** Enables/disables ring tone for contact. */
  void setRing(bool enable);
};


/** Base class for all digitial (ie, DMR) contacts.
 * @ingroup conf */
class DigitalContact: public Contact
{
Q_OBJECT

public:
  /** Base class of all digital contact declarations. */
  class Declaration: public Contact::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration(const QString &description="");
  };

protected:
  /** Hidden constructor. */
  DigitalContact(Declaration *declaraion, QObject *parent=nullptr);
};


/** Represents a private call conact.
 * @ingroup conf */
class PrivateCall: public DigitalContact
{
Q_OBJECT

public:
  /** Declares a private call contact. */
  class Declaration: public DigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit PrivateCall(QObject *parent=nullptr);

  /** Returns the number of the contact. */
  uint32_t number() const;
  /** Sets the number of the contact. */
  void setNumber(uint32_t num);
};


/** Represents a group call contact.
 * @ingroup conf */
class GroupCall: public DigitalContact
{
Q_OBJECT

public:
  /** Declares a group call contact. */
  class Declaration: public DigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit GroupCall(QObject *parent=nullptr);

  /** Returns the number of the contact. */
  uint32_t number() const;
  /** Sets the number of the contact. */
  void setNumber(uint32_t num);
};


/** Represets an all-call contact.
 * @ingroup conf */
class AllCall: public DigitalContact
{
Q_OBJECT

public:
  /** Declares a all call contact. */
  class Declaration: public DigitalContact::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit AllCall(QObject *parent=nullptr);
};


/** Represents a list of references to digital (DMR) contacts.
 * @ingroup conf */
class DigitalContactRefList: public RefList
{
  Q_OBJECT

public:
  /** The type declaration for the DigitalContactList. */
  class Declaration: public RefList::Declaration
  {
  protected:
    /** Hidden constructor. */
    Declaration();

  public:
    /** Factory method to get the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Constructs an empty digital contact list. */
  explicit DigitalContactRefList(QObject *parent=nullptr);

  /** Returns the i-th contact. */
  virtual DigitalContact *getContact(int i) const;

  /** Adds a contact to the list. */
  bool add(Item *item) override;
};


/** Represents an RX group list.
 * @ingroup conf */
class GroupList: public Object
{
  Q_OBJECT

public:
  /** Declares a all call contact. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
     Declaration();

     Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
     /** Returns/creates the singleton instance. */
     static Declaration *get();

  private:
     /** The singleton instance. */
     static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit GroupList(QObject *parent=nullptr);

  /** Returns the name of the group list. */
  const QString &name() const;
  /** Returns the list of members. */
  DigitalContactRefList *members() const;
};


/** Base class of all channels.
 * @ingroup conf */
class Channel: public Object
{
  Q_OBJECT

public:
  /** Base class for all channel declarations. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration(const QString &description="");
  };

protected:
  /** Hidden constructor. */
  Channel(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the channel. */
  const QString &name() const;
  /** Returns the RX frequency in Mhz. */
  double rx() const;
  /** Returns the TX frequency in Mhz. */
  double tx() const;
  /** Returns the TX power. */
  ::Channel::Power power() const;
  /** Returns the transmit timeout for this channel. */
  qint64 txTimeout() const;
  /** Returns @c true if the channel is RX only. */
  bool rxOnly() const;
  /** Returns a reference to the scan-list or @c nullptr if none is set. */
  ScanList *scanList() const;
};


/** Represents a digital channel (ie, DMR channel).
 * @ingroup conf */
class DigitalChannel: public Channel
{
  Q_OBJECT

public:
  /** Declares a digital channel. */
  class Declaration: public Channel::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit DigitalChannel(QObject *parent=nullptr);

  /** Returns the admit criterion for this channel. */
  ::DigitalChannel::Admit admit() const;
  /** Returns the color-code of the channel. */
  uint8_t colorCode() const;
  /** Returns the time-slot of the channel. */
  ::DigitalChannel::TimeSlot timeSlot() const;
  /** Returns a reference to the RX group list. */
  GroupList *groupList() const;
  /** Returns a reference to the default TX contact or @c nullptr if none is set. */
  DigitalContact *txContact() const;
  /** Returns a reference to the positioning system or @c nullptr if none is set. */
  Positioning *aprs() const;
  /** Returns a reference to the roaming zone or @c nullptr if none is set. */
  RoamingZone *roamingZone() const;
  /** Returns a reference to the radio used by the channel or @c nullptr if the default should be used. */
  RadioId *radioId() const;
};


/** Represents an analog (ie, FM) channel.
 * @ingroup conf */
class AnalogChannel: public Channel
{
  Q_OBJECT

public:
  /** Declares an analog channel. */
  class Declaration: public Channel::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit AnalogChannel(QObject *parent=nullptr);

  /** Returns the admit criterion. */
  ::AnalogChannel::Admit admit() const;
  /** Returns the squelch value in [1,10]. */
  uint squelch() const;
  /** Retunrs the RX signalling. */
  Signaling::Code rxSignalling() const;
  /** Retunrs the TX signalling. */
  Signaling::Code txSignalling() const;
  /** Retunrs the band-width. */
  ::AnalogChannel::Bandwidth bandWidth() const;
  /** Retunrs the APRS positioning system or @c nullptr if none is set. */
  APRSPositioning *aprs() const;
};


/** Represents a zone.
 * @ingroup conf */
class Zone: public Object
{
  Q_OBJECT

public:
  /** Represents the zone declaration. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit Zone(QObject *parent=nullptr);

  /** Returns the name of the zone. */
  const QString &name() const;
  /** Returns the channel list for VFO A. */
  AbstractList *a() const;
  /** Returns the channel list for VFO B. */
  AbstractList *b() const;
};


/** Represents a scan-list.
 * @ingroup conf */
class ScanList: public Object
{
  Q_OBJECT

public:
  /** Represents the scan-list declaration. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };


public:
  /** Empty constructor. */
  explicit ScanList(QObject *parent=nullptr);

  /** Returns the name of the scan-list. */
  const QString &name() const;
  /** Returns a reference to the primary channel or @c nullptr if none is set. */
  Channel *primary() const;
  /** Returns a reference to the secondary channel or @c nullptr if none is set. */
  Channel *secondary() const;
  /** Returns a reference to the revert/transmit channel or @c nullptr if none is set. */
  Channel *revert() const;
  /** Returns the channel list */
  RefList *channels() const;
};


/** Base class of all positioning systems.
 * @ingroup conf */
class Positioning: public Object
{
  Q_OBJECT

public:
  /** Base class for all positioning declarations. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param description Specifies the optional field description. */
    explicit Declaration(const QString &description="");
  };

protected:
  /** Hidden constructor. */
  Positioning(Declaration *declaraion, QObject *parent=nullptr);

public:
  /** Returns the name of the positioning system. */
  const QString &name() const;
  /** Returns the update period in seconds. */
  uint32_t period() const;
};


/** Represents an DMR positioning system.
 * @ingroup conf */
class DMRPositioning: public Positioning
{
  Q_OBJECT

public:
  /** Declares a DMR positioning system. */
  class Declaration: public Positioning::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit DMRPositioning(QObject *parent=nullptr);
  /** Returns the destination contact the position is send to. */
  DigitalContact *destination() const;
  /** Returns the transmit channel or @c nullptr if none is set. */
  DigitalChannel *channel() const;
};


/** Represents an APRS positioning system.
 * @ingroup conf */
class APRSPositioning: public Positioning
{
  Q_OBJECT

public:
  /** Declares an APRS positioning system. */
  class Declaration: public Positioning::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  APRSPositioning(QObject *parent=nullptr);

  /** Returns the source call and SSID. */
  const QString &source() const;
  /** Returns the destination call and SSID. */
  const QString &destination() const;
  /** Returns the transmit channel. */
  AnalogChannel *channel() const;
  /** Returns the list of path calls and SSIDs. */
  AbstractList *path() const;
  /** Returns the map icon. */
  const QString &icon() const;
  /** Returns the APRS message. */
  const QString &message() const;
};


/** Represents a roaming zone
 * @ingroup conf */
class RoamingZone: public Object
{
  Q_OBJECT

public:
  /** Declares a roaming zone. */
  class Declaration: public Object::Declaration
  {
  protected:
    /** Constructor.
     * @param mandatory If @c true the field is mandatory.
     * @param description Specifies the optional field description. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Returns/creates the singleton instance. */
    static Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  RoamingZone(QObject *parent=nullptr);

public:
  /** Returns the name of the roaming zone. */
  QString name() const;
  /** Returns the list of channels. */
  RefList *channels() const;
};


/** The list of all channels defined within the codeplug.
 * @ingroup conf */
class ChannelList: public List
{
  Q_OBJECT

public:
  /** Declaration of the channel list. */
  class Declaration: public List::Declaration
  {
  protected:
    /** Hidden constructor, use the @c get method to obtain the declaration. */
    Declaration();

  public:
    /** Factory method to get the channel list declaration. */
    static Declaration *get();

  private:
    /** Singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit ChannelList(QObject *parent=nullptr);

  /** Adds an object to the list.
   * Checks if the item is a channel, returns @c false if not. */
  bool add(Item *item) override;

  /** Returns the channel at index @c i or @c nullptr. */
  virtual Configuration::Channel *getChannel(int i) const;
};


/** Holds the entire abstract codeplug configuration.
 * @ingroup conf */
class Config: public Map
{
  Q_OBJECT

public:
  /** This class represents the complete configuration declaration.
   * That is the codeplug YAML format declaration. */
  class Declaration: public Map::Declaration
  {
  protected:
    /** Constructor. */
    Declaration();

    Item *parseAllocate(const YAML::Node &node, QString &msg) const override;

  public:
    /** Verifies the parsed codeplug.
     * @returns @c true on success and @c false otherwise. On error, the @c message argument
     * will contain the error message. */
    virtual bool verify(const YAML::Node &doc, QString &message);

    /** Assembles configuration from the given YAML docuemnt.
     * Returns @c false on error. */
    bool parse(Config *config, YAML::Node &doc, QString &message);
    /** Assembles configuration from the given YAML document. */
    Config *parse(YAML::Node &doc, QString &message);

    /** Returns/creates the singleton instance. */
    static Config::Declaration *get();

  private:
    /** The singleton instance. */
    static Declaration *_instance;
  };

public:
  /** Empty constructor. */
  explicit Config(QObject *parent = nullptr);

  /** Generates a YAML document from this configuration. */
  YAML::Node generate(QString &message) const;
};


}

#endif // CONFIGITEM_HH
