#ifndef OPENRTX_CODEPLUG_HH
#define OPENRTX_CODEPLUG_HH

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"
#include "contact.hh"

class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;

/** Implements the binary encoding and decoding of the OpenRTX codeplug.
 *
 * @ingroup ortx */
class OpenRTXCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements the channel encoding.*/
  class ChannelElement: public Codeplug::Element
  {
  public:

  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    explicit ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Resets the channel. */
    virtual void clear();

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj(Context &ctx) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Implements the digital contact encoding. */
  class ContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ContactElement();

    /** Resets the contact. */
    void clear();
    /** Returns @c true if the contact is valid. */
    bool isValid() const;

    /** Constructs a @c DigitalContact instance from this codeplug contact. */
    virtual DigitalContact *toContactObj(Context &ctx) const;
    /** Resets this codeplug contact from the given @c DigitalContact. */
    virtual void fromContactObj(const DigitalContact *obj, Context &ctx);
  };

  /** Represents the zone encoding. */
  class ZoneElement: Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ZoneElement(uint8_t *ptr);
    virtual ~ZoneElement();

    /** Resets the zone. */
    void clear();
    /** Returns @c true if the zone is valid. */
    bool isValid() const;

    /** Constructs a generic @c Zone object from this codeplug zone. */
    virtual Zone *toZoneObj(Context &ctx) const;
    /** Links a previously constructed @c Zone object to the rest of the configuration. That is
     * linking to the referred channels. */
    virtual bool linkZoneObj(Zone *zone, Context &ctx, bool putInB) const;
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    virtual void fromZoneObjA(const Zone *zone, Context &ctx);
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    virtual void fromZoneObjB(const Zone *zone, Context &ctx);
  };

  /** Implements the group list encoding. */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GroupListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GroupListElement();

    /** Resets the group list. */
    void clear();

    /** Constructs a @c RXGroupList object from the codeplug representation. */
    virtual RXGroupList *toRXGroupListObj(Context &ctx);
    /** Links a previously constructed @c RXGroupList to the rest of the generic configuration. */
    virtual bool linkRXGroupListObj(int ncnt, RXGroupList *lst, Context &ctx) const;
    /** Reset this codeplug representation from a @c RXGroupList object. */
    virtual void fromRXGroupListObj(const RXGroupList *lst, Context &ctx);
  };

  /** Implements the scan list encoding. */
  class ScanListElement: public Element
  {
  public:

  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ScanListElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ScanListElement();

    /** Resets the scan list. */
    void clear();

    /** Constrcuts a @c ScanList object from this codeplug representation. */
    virtual ScanList *toScanListObj(Context &ctx) const;
    /** Links a previously constructed @c ScanList object to the rest of the generic configuration. */
    virtual bool linkScanListObj(ScanList *lst, Context &ctx) const;
    /** Initializes this codeplug representation from the given @c ScanList object. */
    virtual void fromScanListObj(const ScanList *lst, Context &ctx);
  };

  /** Implements the general settings encoding. */
  class GeneralSettingsElement: public Element
  {
  public:

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~GeneralSettingsElement();

    /** Resets the general settings. */
    void clear();


    /** Encodes the general setting from the given config. */
    virtual bool fromConfig(const Config *conf, Context &ctx);
    /** Updates the given config from this settings. */
    virtual bool updateConfig(Config *conf, Context &ctx);
  };

public:
  /** Hidden constructor, use a device specific class to instantiate. */
  explicit OpenRTXCodeplug(QObject *parent=nullptr);

  /** Destructor. */
  virtual ~OpenRTXCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  bool index(Config *config, Context &ctx) const;

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

public:
  /** Decodes the binary codeplug and stores its content in the given generic configuration using
   * the given context. */
  virtual bool decodeElements(Context &ctx);
  /** Encodes the given generic configuration as a binary codeplug using the given context. */
  virtual bool encodeElements(const Flags &flags, Context &ctx);

  /** Clears the general settings in the codeplug. */
  virtual void clearGeneralSettings();
  /** Updates the general settings from the given configuration. */
  virtual bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx);
  /** Updates the given configuration from the general settings. */
  virtual bool decodeGeneralSettings(Config *config, Context &ctx);

  /** Clears all contacts in the codeplug. */
  virtual void clearContacts();
  /** Encodes all digital contacts in the configuration into the codeplug. */
  virtual bool encodeContacts(Config *config, const Flags &flags, Context &ctx);
  /** Adds a digital contact to the configuration for each one in the codeplug. */
  virtual bool createContacts(Config *config, Context &ctx);

  /** Clear all channels. */
  virtual void clearChannels();
  /** Encode all channels. */
  virtual bool encodeChannels(Config *config, const Flags &flags, Context &ctx);
  /** Adds all defined channels to the configuration. */
  virtual bool createChannels(Config *config, Context &ctx);
  /** Links all channels. */
  virtual bool linkChannels(Config *config, Context &ctx);

  /** Clears all zones. */
  virtual void clearZones();
  /** Encodes zones. */
  virtual bool encodeZones(Config *config, const Flags &flags, Context &ctx);
  /** Adds zones to the configuration. */
  virtual bool createZones(Config *config, Context &ctx);
  /** Links all zones within the configuration. */
  virtual bool linkZones(Config *config, Context &ctx);

  /** Clears all scan lists. */
  virtual void clearScanLists();
  /** Encodes all scan lists. */
  virtual bool encodeScanLists(Config *config, const Flags &flags, Context &ctx);
  /** Creates all scan lists. */
  virtual bool createScanLists(Config *config, Context &ctx);
  /** Links all scan lists. */
  virtual bool linkScanLists(Config *config, Context &ctx);

  /** Clears all group lists. */
  virtual void clearGroupLists();
  /** Encodes all group lists. */
  virtual bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx);
  /** Creates all group lists. */
  virtual bool createGroupLists(Config *config, Context &ctx);
  /** Links all group lists. */
  virtual bool linkGroupLists(Config *config, Context &ctx);
};

#endif // OPENRTX_CODEPLUG_HH
