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
  /** Possible modes for a channel or contact. */
  enum Mode {
    Mode_None = 0, ///< Disabled?
    Mode_FM   = 1, ///< FM Channel.
    Mode_DMR  = 2, ///< DMR Channel.
    Mode_M17  = 3  ///< M17 Channel.
  };

public:
  /** Implements the codeplug header element.
   *
   * Binary representation of the header (size 0058h bytes):
   * @verbinclude openrtx_header.txt */
  class HeaderElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    HeaderElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    HeaderElement(uint8_t *ptr);

    void clear();
    bool isValid() const;

    /** Returns the version number (MAJOR<<8)|MINOR. */
    virtual uint16_t version() const;
    /** Sets the version number. This number is fixed to 0.1, the supported version. */
    virtual void setVersion();

    /** Returns the author name. */
    virtual QString author() const;
    /** Sets the author name. */
    virtual void setAuthor(const QString &name);
    /** Returns the description. */
    virtual QString description() const;
    /** Sets the description. */
    virtual void setDescription(const QString description);

    /** Returns the timestamp. */
    virtual QDateTime timestamp() const;
    /** Sets the timestamp. */
    virtual void setTimestamp(const QDateTime timestamp=QDateTime::currentDateTime());

    /** Returns the contact count. */
    virtual unsigned int contactCount() const;
    /** Sets the contact count. */
    virtual void setContactCount(unsigned int n);
    /** Returns the channel count. */
    virtual unsigned int channelCount() const;
    /** Sets the channel count. */
    virtual void setChannelCount(unsigned int n);
    /** Returns the zone count. */
    virtual unsigned int zoneCount() const;
    /** Sets the zone count. */
    virtual void setZoneCount(unsigned int n);

  protected:
    enum Offsets {
      OffsetMagic = 0x00, MagicNumber = 0x43585452, OffsetVersion = 0x08, OffsetAuthor = 0x0a,
      OffsetDescription = 0x2a, OffsetTimestamp = 0x4a, OffsetContactCount = 0x52,
      OffsetChannelCount = 0x54, OffsetZoneCount = 0x56, StringLength = 0x20,
      SupportedVersion = ((0<<8)|1)
    };
  };

  /** Implements the binary rerpesentation of a channel.
   *
   * Binary representation (size 005ah bytes):
   * @verbinclude openrtx_channel.txt */
  class ChannelElement: public Codeplug::Element
  {
  public:
    /** Specifies the possible bandwidth settings. */
    enum Bandwidth {
      BW_12_5kHz = 0,
      BW_20kHz   = 1,
      BW_25kHz   = 2
    };

    /** Specifies the DMR time slot settings. */
    enum Timeslot {
      Timeslot1 = 1,
      Timeslot2 = 2
    };

    /** Specifies the M17 channel mode. */
    enum ChannelMode {
      M17Voice = 1,
      M17Data = 2,
      M17VoiceData = 3
    };

    /** Specifies the M17 encryption modes. */
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

  /** Implements the digital contact for the OpenRTX firmware.
   *
   * Binary representation (size 0027h bytes):
   * @verbinclude openrtx_contact.txt */
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

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &name);

    /** Returns the mode of the contact (either DMR or M17). */
    virtual Mode mode() const;
    /** Sets the mode of the contact. */
    virtual void setMode(Mode mode);

    /** Returns the DMR ID. Only valid for DMR contacts. */
    virtual unsigned int dmrId() const;
    /** Sets the DMR ID. Only valid for DMR contacts. */
    virtual void setDMRId(unsigned int id);

    /** Retruns @c true if the RX tone is enabled (ring). Only valid for DMR contacts. */
    virtual bool dmrRing() const;
    /** Enables/disables RX tone (ring). Only valid for DMR contacts. */
    virtual void enableDMRRing(bool enable);

    /** Returns the contact type. Only valid for DMR contacts. */
    virtual DigitalContact::Type dmrContactType() const;
    /** Sets the contact type. */
    virtual void setDMRContactType(DigitalContact::Type type);

    /** Returns the contact call. */
    virtual QString m17Call() const;
    /** Sets the M17 call. */
    virtual bool setM17Call(const QString &call, const ErrorStack &err=ErrorStack());

    /** Constructs a @c DigitalContact instance from this codeplug contact. */
    virtual DigitalContact *toContactObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Resets this codeplug contact from the given @c DigitalContact. */
    virtual void fromContactObj(const DigitalContact *obj, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Just holds the offsets within the codeplug. */
    enum Offsets {
      OffsetName = 0x00, OffsetMode = 0x20, OffsetDMRId = 0x21, OffsetDMRCallType = 0x25,
      BitDMRCallType = 0, OffsetDMRRing = 0x25, BitDMRRing = 0x02, OffsetM17Address = 0x21,
      StringLength = 0x20
    };

  };


  /** The binary encoding of a zone.
   *
   * Binary representation (variable size):
   * @verbinclude openrtx_zone.txt */
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

    /** Returns the zone name. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Returns the number of channels in zone. */
    virtual unsigned int channelCount() const;
    /** Sets the number of channels in zone. */
    virtual void setChannelCount(unsigned int n);

    /** Returns the n-th channel index. */
    virtual unsigned int channelIndex(unsigned int n) const;
    /** Sets the n-th channel index. */
    virtual void setChannelIndex(unsigned int n, unsigned int idx);

    /** Constructs a generic @c Zone object from this codeplug zone. */
    virtual Zone *toZoneObj(Context &ctx) const;
    /** Links a previously constructed @c Zone object to the rest of the configuration. That is
     * linking to the referred channels. */
    virtual bool linkZoneObj(Zone *zone, Context &ctx, bool putInB, const ErrorStack &err=ErrorStack()) const;
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    virtual void fromZoneObjA(const Zone *zone, Context &ctx);
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    virtual void fromZoneObjB(const Zone *zone, Context &ctx);

  protected:
    /** Just defines the offsets with the element. */
    enum Offsets {
      OffsetName = 0x00, OffsetCount = 0x20, OffsetChannel = 0x22, StringLength = 0x20
    };
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

  /** Returns the number of stored contacts. */
  virtual unsigned int numContacts();
  /** Returns the offset to the n-th contact element. */
  virtual unsigned int offsetContact(unsigned int n);
  /** Encodes all digital contacts in the configuration into the codeplug. */
  virtual bool encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Adds a digital contact to the configuration for each one in the codeplug. */
  virtual bool createContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Returns the number of stored channes. */
  virtual unsigned int numChannels();
  /** Returns the offset to the n-th channel element. */
  virtual unsigned int offsetChannel(unsigned int n);
  /** Encode all channels. */
  virtual bool encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Adds all defined channels to the configuration. */
  virtual bool createChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all channels. */
  virtual bool linkChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Returns the number of stored zones. */
  virtual unsigned int numZones();
  /** Returns the offset to the zone offset array. */
  virtual unsigned int offsetZoneOffsets();
  /** Returns the offset to the n-th zone element. */
  virtual unsigned int offsetZone(unsigned int n);
  /** Encodes zones. */
  virtual bool encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Adds zones to the configuration. */
  virtual bool createZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all zones within the configuration. */
  virtual bool linkZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Just stores some sizes. */
  enum Offsets {
    HeaderSize = 0x58, ChannelSize = 0x5a, ContactSize = 0x27, ZoneHeaderSize=0x22
  };
};

#endif // OPENRTX_CODEPLUG_HH
