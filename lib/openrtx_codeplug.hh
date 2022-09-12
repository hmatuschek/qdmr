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
 * @section ortxcpl Codeplug structure within radio
 * The binary codeplug does not use fixed offsets. It is just a concatenation arrays of codeplug
 * elements. This codeplug implements the revision 0.1.
 * <table>
 *  <tr><th>Content</th></tr>
 *  <tr><td>Header see @c HeaderElement.</td></tr>
 *  <tr><td>Max. 65536 contacts as specified in the header. See @c ContactElement.</td></tr>
 *  <tr><td>Max. 65536 channels as specified in the header. See @c ChannelElement.</td></tr>
 *  <tr><td>Max. 65536 bank (zone) offsets as specified in the header. Each offset is stored as a
 *      ??? and specifies the offset to the bank w.r.t. ??? in ???.</td></tr>
 *  <tr><td>Max. 65536 banks (zones). See @c ZoneElement.</td></tr>
 * </table>
 *
 * @ingroup ortx */
class OpenRTXCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements the binary rerpesentation of a channel.
   *
   * Binary representation (size 0058h bytes):
   * @verbinclude openrtx_channel.txt */
  class ChannelElement: public Codeplug::Element
  {
  public:
    /** Possible modes for a channel. */
    enum Mode {
      Mode_None = 0, ///< Disabled?
      Mode_FM   = 1, ///< FM Channel.
      Mode_DMR  = 2, ///< DMR Channel.
      Mode_M17  = 3  ///< M17 Channel.
    };

    enum Bandwidth {
      BW_12_5kHz = 0,
      BW_20kHz   = 1,
      BW_25kHz   = 2
    };

    enum Timeslot {
      Timeslot1 = 1,
      Timeslot2 = 2
    };

    enum ChannelMode {
      M17Voice = 1,
      M17Data = 2,
      M17VoiceData = 3
    };

    enum EncryptionMode {
      EncrNone      = 0,
      EncrAES256    = 1,
      EncrScrambler = 2
    };

  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    explicit ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    bool isValid() const;

    /** Resets the channel. */
    virtual void clear();

    /** Returns the channel mode. */
    virtual Mode mode() const;
    /** Sets the channel mode. */
    virtual void setMode(Mode mode);

    /** Returns @c true, if the channel is RX only. */
    virtual bool rxOnly() const;
    /** Enables/disables RX only for the channel. */
    virtual void setRXOnly(bool enable);

    /** Retunrs the bandwidth of the channel. */
    virtual Bandwidth bandwidth() const;
    /** Sets the bandwidth of the channel. */
    virtual void setBandwidth(Bandwidth bw);

    /** Power in dBm. */
    virtual float power() const;
    /** Set power in dBm. */
    virtual void setPower(float dBm);

    /** Returns the RX frequency in MHz. */
    virtual double rxFrequency() const;
    /** Sets the RX frequency in MHz. */
    virtual void setRXFrequency(double MHz);
    /** Returns the TX frequency in MHz. */
    virtual double txFrequency() const;
    /** Sets the TX frequency in MHz. */
    virtual void setTXFrequency(double MHz);

    /** Retrusn @c true if the scan list is set. */
    virtual bool hasScanListIndex() const;
    /** Retunrs the scan list index. */
    virtual unsigned int scanListIndex() const;
    /** Sets the scan list index. */
    virtual void setScanListIndex(unsigned int index);
    /** Clears the scan list index. */
    virtual void clearScanListIndex();

    /** Retrusn @c true if the group list is set. */
    virtual bool hasGroupListIndex() const;
    /** Retunrs the group list index. */
    virtual unsigned int groupListIndex() const;
    /** Sets the group list index. */
    virtual void setGroupListIndex(unsigned int index);
    /** Clears the group list index. */
    virtual void clearGroupListIndex();

    /** Returns the channel name. */
    virtual QString name() const;
    /** Sets the channel name. */
    virtual void setName(const QString &name);
    /** Returns the channel description. */
    virtual QString description() const;
    /** Sets the channel description. */
    virtual void setDescription(const QString &description);

    /** Returns the channel latitude. */
    virtual float latitude() const;
    /** Sets the latitude. */
    virtual void setLatitude(float lat);
    /** Returns the channel longitude. */
    virtual float longitude() const;
    /** Sets the longitude. */
    virtual void setLongitude(float lat);
    /** Returns the height in meters. */
    virtual unsigned int altitude() const;
    /** Sets the height in meters. */
    virtual void setAltitude(unsigned int alt);

    /** Returns the CTCSS RX sub-tone. Only valid for FM channels. */
    virtual Signaling::Code rxTone() const;
    /** Sets the CTCSS RX sub-tone. Only valid for FM channels. */
    virtual void setRXTone(Signaling::Code code, const ErrorStack &err=ErrorStack());
    /** Returns the CTCSS TX sub-tone. Only valid for FM channels. */
    virtual Signaling::Code txTone() const;
    /** Sets the CTCSS TX sub-tone. Only valid for FM channels. */
    virtual void setTXTone(Signaling::Code code, const ErrorStack &err=ErrorStack());

    /** Returns the RX color code. Only valid for DMR channels. */
    virtual unsigned int rxColorCode() const;
    /** Sets the RX color code. Only valid for DMR channels. */
    virtual void setRXColorCode(unsigned int cc);
    /** Returns the TX color code. Only valid for DMR channels. */
    virtual unsigned int txColorCode() const;
    /** Sets the TX color code. Only valid for DMR channels. */
    virtual void setTXColorCode(unsigned int cc);

    /** Returns the times slot for the channel. Only valid for DMR channels. */
    virtual DigitalChannel::TimeSlot timeslot() const;
    /** Sets the timeslot for the channel. Only valid for DMR channels. */
    virtual void setTimeslot(DigitalChannel::TimeSlot ts);

    /** Returns @c true if the DMR contact index is set. */
    virtual bool hasDMRContactIndex() const;
    /** Returns the contact index for the DMR contact. Only valid for DMR channels. */
    virtual unsigned int dmrContactIndex() const;
    /** Sets the DMR contact index. Only valid for DMR channels. */
    virtual void setDMRContactIndex(unsigned int idx);
    /** Clears the DMR contact index. */
    virtual void clearDMRContactIndex();

    /** Returns the RX channel access number. Only valid for M17 channels. */
    virtual unsigned int rxChannelAccessNumber() const;
    /** Sets the RX channel access number. Only valid for M17 channels. */
    virtual void setRXChannelAccessNumber(unsigned int cc);
    /** Returns the TX color code. Only valid for M17 channels. */
    virtual unsigned int txChannelAccessNumber() const;
    /** Sets the TX color code. Only valid for M17 channels. */
    virtual void setTXChannelAccessNumber(unsigned int cc);

    /** Returns the encryption mode of the channel. Only valid for M17 channels. */
    virtual EncryptionMode encryptionMode() const;
    /** Sets the encryption mode for the channel. Only valid for M17 channels. */
    virtual void setEncryptionMode(EncryptionMode mode);

    /** Returns the channe mode. Only valid for M17 channels. */
    virtual ChannelMode channelMode() const;
    /** Sets the channel mode. Only valid for M17 channels. */
    virtual void setChannelMode(ChannelMode mode);

    /** Returns @c true if GPS position is send as meta-data. Only valid for M17 channels. */
    virtual bool gpsDataEnabled() const;
    /** Enables/disables sending of GPS position as meta-data. Only valid for M17 channels. */
    virtual void enableGPSData(bool enable);

    /** Returns @c true if the M17 contact index is set. */
    virtual bool hasM17ContactIndex() const;
    /** Returns the M17 contact index. Only valid for M17 channels. */
    virtual unsigned int m17ContactIndex() const;
    /** Sets the M17 contact index. Only valid for M17 channels. */
    virtual void setM17ContactIndex(unsigned int idx);
    /** Clears the M17 contact index. */
    virtual void clearM17ContactIndex();

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Just contains the offsets within the channel element. */
    enum Offsets {
      OffsetMode = 0x00, OffsetBandwidth = 0x01, BitBandwidth = 0x00, OffsetRXOnly = 0x01,
      BitRXOnly = 0x02, OffsetPower = 0x02, OffsetRXFrequency = 0x03, OffsetTXFrequency = 0x07,
      OffsetScanList = 0x0b, OffsetGroupList = 0x0c, OffsetName = 0x0d, OffsetDescription = 0x2d,
      OffsetChLatInt = 0x4d, OffsetChLatDec = 0x4e, OffsetChLonInt = 0x50, OffsetChLonDec = 0x51,
      OffsetChAltitude = 0x53, OffsetRXTone = 0x55, OffsetTXTone = 0x56, OffsetRXColorCode = 0x55,
      BitRXColorCode = 0x00, OffsetTXColorCode = 0x55, BitTXColorCode = 0x04, OffsetTimeSlot = 0x56,
      OffsetDMRContact = 0x57, OffsetRXCAN = 0x55, BitRXCAN = 0x00, OffsetTXCAN = 0x55,
      BitTXCAN = 0x04, OffsetEncrMode = 0x56, BitEncrMode = 0x04, OffsetM17ChMode = 0x56,
      BitM17ChMode = 0x00, OffsetM17GPSMode = 0x57, OffsetM17Contact = 0x58, StringLength = 0x20
    };
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

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config, const ErrorStack &err=ErrorStack());
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags(), const ErrorStack &err=ErrorStack());

public:
  /** Decodes the binary codeplug and stores its content in the given generic configuration using
   * the given context. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encodes the given generic configuration as a binary codeplug using the given context. */
  virtual bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears the general settings in the codeplug. */
  virtual void clearGeneralSettings(const ErrorStack &err=ErrorStack());
  /** Updates the general settings from the given configuration. */
  virtual bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Updates the given configuration from the general settings. */
  virtual bool decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears all contacts in the codeplug. */
  virtual void clearContacts(const ErrorStack &err=ErrorStack());
  /** Encodes all digital contacts in the configuration into the codeplug. */
  virtual bool encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Adds a digital contact to the configuration for each one in the codeplug. */
  virtual bool createContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clear all channels. */
  virtual void clearChannels(const ErrorStack &err=ErrorStack());
  /** Encode all channels. */
  virtual bool encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Adds all defined channels to the configuration. */
  virtual bool createChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all channels. */
  virtual bool linkChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears all zones. */
  virtual void clearZones(const ErrorStack &err=ErrorStack());
  /** Encodes zones. */
  virtual bool encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Adds zones to the configuration. */
  virtual bool createZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all zones within the configuration. */
  virtual bool linkZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears all scan lists. */
  virtual void clearScanLists(const ErrorStack &err=ErrorStack());
  /** Encodes all scan lists. */
  virtual bool encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Creates all scan lists. */
  virtual bool createScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all scan lists. */
  virtual bool linkScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears all group lists. */
  virtual void clearGroupLists(const ErrorStack &err=ErrorStack());
  /** Encodes all group lists. */
  virtual bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Creates all group lists. */
  virtual bool createGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all group lists. */
  virtual bool linkGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // OPENRTX_CODEPLUG_HH
