#ifndef TYT_CODEPLUG_HH
#define TYT_CODEPLUG_HH

#include <QDateTime>

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"
#include "contact.hh"

class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;
class CodeplugContext;


/** Base class of all TyT codeplugs. This class implements the majority of all codeplug elements
 * present in all TyT codeplugs. This eases the support of several TyT radios, as only the
 * differences in the codeplug to this base class must be implemented.
 *
 * @ingroup tyt */
class TyTCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Represents a single channel (analog or digital) within the TyT codeplug.
   *
   * Memmory layout of encoded channel:
   * @verbinclude tytchannel.txt
   */
  class ChannelElement: public CodePlug::Element
  {
  public:
    /** Possible modes for the channel, i.e. analog and digital. */
    typedef enum {
      MODE_ANALOG  = 1,             ///< Analog channel.
      MODE_DIGITAL = 2              ///< Digital channel.
    } Mode;

    /** Bandwidth of the channel. */
    typedef enum {
      BW_12_5_KHZ = 0,              ///< 12.5 kHz narrow, (default for binary channels).
      BW_20_KHZ   = 1,              ///< 20 kHz (really?)
      BW_25_KHZ   = 2               ///< 25kHz wide.
    } Bandwidth;

    /** Possible privacy types. */
    typedef enum {
      PRIV_NONE = 0,                ///< No privacy.
      PRIV_BASIC = 1,               ///< Basic privacy.
      PRIV_ENHANCED = 2             ///< Enhenced privacy.
    } PrivacyType;

    /** I have absolutely no idea what this means. */
    typedef enum {
      REF_LOW = 0,
      REF_MEDIUM = 1,
      REF_HIGH = 2
    } RefFrequency;

    /** TX Admit criterion. */
    typedef enum {
      ADMIT_ALWAYS = 0,             ///< Always allow TX.
      ADMIT_CH_FREE = 1,            ///< Allow TX if channel is free.
      ADMIT_TONE = 2,               ///< Allow TX if CTCSS tone matches.
      ADMIT_COLOR = 3,              ///< Allow TX if color-code matches.
    } Admit;

    /** Again, I have no idea. */
    typedef enum {
      INCALL_ALWAYS = 0,
      INCALL_ADMIT = 1,
      INCALL_TXINT = 2
    } InCall;

    /** Turn-off tone frequency.
     * This radio has a feature that allows to disable radios remotely by sending a specific tone.
     * Certainly not a feature used in ham-radio. */
    typedef enum {
      TURNOFF_NONE = 3,             ///< Turn-off disabled. Default!
      TURNOFF_259_2HZ = 0,          ///< Turn-off on 259.2Hz tone.
      TURNOFF_55_2HZ = 1            ///< Turn-off on 55.2Hz tone.
    } TurnOffFreq;


  public:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Returns @c true if channel is valid/enabled. */
    bool isValid() const;
    /** Clears/resets the channel and therefore disables it. */
    void clear();

    /** Returns the mode of the channel. */
    virtual Mode mode() const;
    /** Sets the mode of the channel. */
    virtual void mode(Mode mode);

    /** Retuns the bandwidth of the (analog) channel. */
    virtual AnalogChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth of the (analog) channel. */
    virtual void bandwidth(AnalogChannel::Bandwidth bw);

    /** Returns @c true if the channel has auto scan enabled. */
    virtual bool autoScan() const;
    /** Enables/disables auto scan for this channel. */
    virtual void autoScan(bool enable);

    /** Returns @c true if the channel has lone worker enabled. */
    virtual bool loneWorker() const;
    /** Enables/disables lone worker for this channel. */
    virtual void loneWorker(bool enable);

    /** Returns @c true if the channel has talkaround enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround for this channel. */
    virtual void talkaround(bool enable);

    /** Returns @c true if the channel has rx only enabled. */
    virtual bool rxOnly() const;
    /** Enables/disables rx only for this channel. */
    virtual void rxOnly(bool enable);

    /** Returns the time slot of this channel. */
    virtual DigitalChannel::TimeSlot timeSlot() const;
    /** Sets the time slot of this channel. */
    virtual void timeSlot(DigitalChannel::TimeSlot ts);

    /** Returns the color code of this channel. */
    virtual uint8_t colorCode() const;
    /** Sets the color code of this channel. */
    virtual void colorCode(uint8_t ts);

    /** Returns the index (+1) of the privacy system (key). */
    virtual uint8_t privacyIndex() const;
    /** Sets the index (+1) of the privacy system (key). */
    virtual void privacyIndex(uint8_t ts);
    /** Returns the type of the privacy system. */
    virtual PrivacyType privacyType() const;
    /** Sets the type of the privacy system. */
    virtual void privacyType(PrivacyType type);

    /** Returns @c true if the channel has private call confirmation enabled. */
    virtual bool privateCallConfirm() const;
    /** Enables/disables private call confirmation for this channel. */
    virtual void privateCallConfirm(bool enable);

    /** Returns @c true if the channel has data call confirmation enabled. */
    virtual bool dataCallConfirm() const;
    /** Enables/disables data call confirmation for this channel. */
    virtual void dataCallConfirm(bool enable);

    /** Returns some weird reference frequency setting for reception. */
    virtual RefFrequency rxRefFrequency() const;
    /** Sets some weird reference frequency setting for reception. */
    virtual void rxRefFrequency(RefFrequency ref);
    /** Returns some weird reference frequency setting for transmission. */
    virtual RefFrequency txRefFrequency() const;
    /** Sets some weird reference frequency setting for transmission. */
    virtual void txRefFrequency(RefFrequency ref);

    /** Returns @c true if the channel has alarm confirmation enabled. */
    virtual bool emergencyAlarmACK() const;
    /** Enables/disables alarm confirmation for this channel. */
    virtual void emergencyAlarmACK(bool enable);

    /** Returns @c true if the channel has display PTT ID enabled. */
    virtual bool displayPTTId() const;
    /** Enables/disables PTT ID display for this channel. */
    virtual void displayPTTId(bool enable);

    /** Returns @c true if the channel has VOX enabled. */
    virtual bool vox() const;
    /** Enables/disables VOX for this channel. */
    virtual void vox(bool enable);

    /** Returns the admit criterion for this channel. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion for this channel. */
    virtual void admitCriterion(Admit admit);

    /** Returns the in-call criterion for this channel. */
    virtual InCall inCallCriteria() const;
    /** Sets the in-call criterion for this channel. */
    virtual void inCallCriteria(InCall crit);

    /** Returns the remote turn-off/kill frequency for this channel. */
    virtual TurnOffFreq turnOffFreq() const;
    /** Sets the remote turn-off/kill frequency for this channel. */
    virtual void turnOffFreq(TurnOffFreq freq);

    /** Returns the transmit contact index (+1) for this channel. */
    virtual uint16_t contactIndex() const;
    /** Sets the transmit contact index (+1) for this channel. */
    virtual void contactIndex(uint16_t idx);

    /** Returns the transmit time-out in seconds. */
    virtual uint txTimeOut() const;
    /** Sets the transmit time-out in seconds. */
    virtual void txTimeOut(uint tot);
    /** Returns the transmit time-out re-key delay in seconds. */
    virtual uint8_t txTimeOutRekeyDelay() const;
    /** Sets the transmit time-out re-key delay in seconds. */
    virtual void txTimeOutRekeyDelay(uint8_t delay);

    /** Returns the emergency system index (+1) for this channel. */
    virtual uint8_t emergencySystemIndex() const;
    /** Sets the emergency system index (+1) for this channel. */
    virtual void emergencySystemIndex(uint8_t idx);

    /** Returns the scan-list index (+1) for this channel. */
    virtual uint8_t scanListIndex() const;
    /** Sets the scan-list index (+1) for this channel. */
    virtual void scanListIndex(uint8_t idx);

    /** Returns the RX group list index (+1) for this channel. */
    virtual uint8_t groupListIndex() const;
    /** Sets the RX group list index (+1) for this channel. */
    virtual void groupListIndex(uint8_t idx);

    /** Returns the positioning system index (+1) for this channel. */
    virtual uint8_t positioningSystemIndex() const;
    /** Sets the positioning system index (+1) for this channel. */
    virtual void positioningSystemIndex(uint8_t idx);

    /** Returns @c true if the channel has DTMF decoding enabled. */
    virtual bool dtmfDecode(uint8_t idx) const;
    /** Enables/disables DTMF decoding this channel. */
    virtual void dtmfDecode(uint8_t idx, bool enable);

    /** Returns the squelch level [0-10]. */
    virtual uint squelch() const;
    /** Sets the squelch level [0-10]. */
    virtual void squelch(uint value);

    /** Returns the RX frequency in Hz. */
    virtual uint32_t rxFrequency() const;
    /** Sets the RX frequency in Hz. */
    virtual void rxFrequency(uint32_t Hz);
    /** Returns the TX frequency in Hz. */
    virtual uint32_t txFrequency() const;
    /** Sets the TX frequency in Hz. */
    virtual void txFrequency(uint32_t Hz);

    /** Returns the CTCSS/DSC signaling for RX. */
    virtual Signaling::Code rxSignaling() const;
    /** Sets the CTCSS/DSC signaling for RX. */
    virtual void rxSignaling(Signaling::Code code);
    /** Returns the CTCSS/DSC signaling for TX. */
    virtual Signaling::Code txSignaling() const;
    /** Sets the CTCSS/DSC signaling for TX. */
    virtual void txSignaling(Signaling::Code code);
    /** Returns the signaling system index (+1) for RX. */
    virtual uint8_t rxSignalingSystemIndex() const;
    /** Sets the signaling system index (+1) for RX. */
    virtual void rxSignalingSystemIndex(uint8_t idx);
    /** Returns the signaling system index (+1) for TX. */
    virtual uint8_t txSignalingSystemIndex() const;
    /** Sets the signaling system index (+1) for TX. */
    virtual void txSignalingSystemIndex(uint8_t idx);

    /** Returns the power of this channel. */
    virtual Channel::Power power() const;
    /** Sets the power of this channel. */
    virtual void power(Channel::Power pwr);

    /** Returns @c true if the channel transmits GPS information enabled. */
    virtual bool txGPSInfo() const;
    /** Enables/disables transmission of GPS information for this channel. */
    virtual void txGPSInfo(bool enable);
    /** Returns @c true if the channel receives GPS information enabled. */
    virtual bool rxGPSInfo() const;
    /** Enables/disables reception of GPS information for this channel. */
    virtual void rxGPSInfo(bool enable);

    /** Returns @c true if the channel allows interruption enabled. */
    virtual bool allowInterrupt() const;
    /** Enables/disables interruption for this channel. */
    virtual void allowInterrupt(bool enable);

    /** Returns @c true if the channel has dual-capacity direct mode enabled. */
    virtual bool dualCapacityDirectMode() const;
    /** Enables/disables dual-capacity direct mode for this channel. */
    virtual void dualCapacityDirectMode(bool enable);

    /** Retruns @c true if the radio acts as the leader for this DCDM channel. */
    virtual bool leaderOrMS() const;
    /** Enables/disables this radio to be the leader for this DCDM channel. */
    virtual void leaderOrMS(bool enable);

    /** Returns the name of this channel. */
    virtual QString name() const;
    /** Sets the name of this channel. */
    virtual void name(const QString &name);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj() const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, const CodeplugContext &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual void fromChannelObj(const Channel *c, const CodeplugContext &ctx);
  };

  /** Implements a VFO channel for TyT radios.
   * This class is an extension of the normal ChannelElement that only implements the step-size
   * feature and encodes it where the name used to be. Thus the memory layout and size is identical
   * to the normal channel. */
  class VFOChannelElement: public ChannelElement
  {
  public:
    /** Constructor from pointer to memory. */
    VFOChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~VFOChannelElement();

    QString name() const;
    void name(const QString &txt);

    /** Returns the step-size for the VFO channel. */
    virtual uint stepSize() const;
    /** Sets the step-size for the VFO channel in Hz. */
    virtual void stepSize(uint ss_hz);
  };

  /** Represents a digital (DMR) contact within the codeplug.
   *
   * Memmory layout of encoded contact:
   * @verbinclude tytcontact.txt */
  class ContactElement: public CodePlug::Element
  {
  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr, uint size=0x24);
    /** Destructor. */
    virtual ~ContactElement();

    void clear();
    bool isValid() const;

    virtual uint32_t dmrId() const;
    virtual void dmrId(uint32_t id);

    virtual DigitalContact::Type callType() const;
    virtual void callType(DigitalContact::Type type);

    virtual bool ringTone() const;
    virtual void ringTone(bool enable);

    virtual QString name() const;
    virtual void name(const QString &nm);

    virtual bool fromContactObj(const DigitalContact *contact);
    virtual DigitalContact *toContactObj() const;
  };

  /** Represents a zone within the codeplug.
   * Please note that a zone consists of two elements the @c ZoneElement and the @c ZoneExtElement.
   * The latter adds additional channels for VFO A and the channels for VFO B.
   *
   * Memmory layout of encoded zone:
   * @verbinclude tytzone.txt
   */
  class ZoneElement: public CodePlug::Element
  {
  public:
    /** Construtor. */
    ZoneElement(uint8_t *ptr, uint size=0x40);
    /** Desturctor. */
    virtual ~ZoneElement();

    void clear();
    bool isValid() const;

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void name(const QString &name);

    /** Returns the index (+1) of the @c n-th member. */
    virtual uint16_t memberIndex(uint n) const;
    /** Sets the index (+1) of the @c n-th member. */
    virtual void memberIndex(uint n, uint16_t idx);

    /** Encodes a given zone object. */
    virtual bool fromZoneObj(const Zone *zone, const CodeplugContext &ctx);
    /** Creates a zone. */
    virtual Zone *toZoneObj() const;
    /** Links the created zone to channels. */
    virtual bool linkZone(Zone *zone, const CodeplugContext &ctx) const;
  };

  /** Extended zone data.
   * The zone definition @c ZoneElement contains only a single set of 16 channels. For each zone
   * definition, there is a zone extension which extends a zone to zwo sets of 64 channels each.
   *
   * Memmory layout of encoded zone extension:
   * @verbinclude tytzoneext.txt
   */
  class ZoneExtElement: public CodePlug::Element
  {
  public:
    /** Constructor. */
    ZoneExtElement(uint8_t *ptr, uint size=0xe0);
    /** Destructor. */
    virtual ~ZoneExtElement();

    void clear();

    /** Returns the n-th member index of the channel list for A. */
    virtual uint16_t memberIndexA(uint n) const;
    /** Sets the n-th member index of the channel list for A. */
    virtual void memberIndexA(uint n, uint16_t idx);
    /** Returns the n-th member index of the channel list for B. */
    virtual uint16_t memberIndexB(uint n) const;
    /** Returns the n-th member index of the channel list for B. */
    virtual void memberIndexB(uint n, uint16_t idx);

    /** Encodes the given zone. */
    virtual bool fromZoneObj(const Zone *zone, const CodeplugContext &ctx);
    /** Links the given zone object.
     * Thant is, extends channel list A and populates channel list B. */
    virtual bool linkZoneObj(Zone *zone, const CodeplugContext &ctx);
  };


protected:
  /** Empty constructor. */
  explicit TyTCodeplug(QObject *parent = nullptr);

public:
  /** Destructor. */
  virtual ~TyTCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

public:
  /** Clears the time-stamp in the codeplug. */
  virtual void clearTimestamp();
  /** Sets the time-stamp. */
  virtual bool encodeTimestamp(Config *config, const Flags &flags);
  /** Decodes the time-stamp. This function does actually nothing as the time-stamp is of no use. */
  virtual bool decodeTimestamp(Config *config);

  /** Clears the general settings in the codeplug. */
  virtual void clearGeneralSettings();
  /** Updates the general settings from the given configuration. */
  virtual bool encodeGeneralSettings(Config *config, const Flags &flags);
  /** Updates the given configuration from the general settings. */
  virtual bool decodeGeneralSettings(Config *config);

  /** Clears the boot settings in the codeplug. */
  virtual void clearBootSettings();
  /** Updates the boot settings from the given configuration. */
  virtual bool encodeBootSettings(Config *config, const Flags &flags);
  /** Updates the given configuration from the boot settings. */
  virtual bool decodeBootSettings(Config *config);

  /** Clears all contacts in the codeplug. */
  virtual void clearContacts();
  /** Encodes all digital contacts in the configuration into the codeplug. */
  virtual bool encodeContacts(Config *config, const Flags &flags);
  /** Adds a digital contact to the configuration for each one in the codeplug. */
  virtual bool createContacts(CodeplugContext &ctx);

  /** Clears all RX group lists in the codeplug. */
  virtual void clearGroupLists();
  /** Encodes all group lists in the configuration into the codeplug. */
  virtual bool encodeGroupLists(Config *config, const Flags &flags);
  /** Adds a RX group list to the configuration for each one in the codeplug. */
  virtual bool createGroupLists(CodeplugContext &ctx);
  /** Links all added RX group lists within the configuration. */
  virtual bool linkGroupLists(CodeplugContext &ctx);

  /** Clears all channels in the codeplug. */
  virtual void clearChannels();
  /** Encodes all channels in the configuration into the codeplug. */
  virtual bool encodeChannels(Config *config, const Flags &flags);
  /** Adds a channel to the configuration for each one in the codeplug. */
  virtual bool createChannels(CodeplugContext &ctx);
  /** Links all added channels within the configuration. */
  virtual bool linkChannels(CodeplugContext &ctx);

  /** Clears all zones in the codeplug. */
  virtual void clearZones();
  /** Encodes all zones in the configuration into the codeplug. */
  virtual bool encodeZones(Config *config, const Flags &flags);
  /** Adds a zone to the configuration for each one in the codeplug. */
  virtual bool createZones(CodeplugContext &ctx);
  /** Links all added zones within the configuration. */
  virtual bool linkZones(CodeplugContext &ctx);

  /** Clears all scan lists in the codeplug. */
  virtual void clearScanLists();
  /** Encodes all scan lists in the configuration into the codeplug. */
  virtual bool encodeScanLists(Config *config, const Flags &flags);
  /** Adds a scan list to the configuration for each one in the codeplug. */
  virtual bool createScanLists(CodeplugContext &ctx);
  /** Links all added scan lists within the configuration. */
  virtual bool linkScanLists(CodeplugContext &ctx);

  /** Clears all positioning systems in the codeplug. */
  virtual void clearPositioningSystems();
  /** Encodes all DMR positioning systems in the configuration into the codeplug. */
  virtual bool encodePositioningSystems(Config *config, const Flags &flags);
  /** Adds a GPS positioning system to the configuration for each one in the codeplug. */
  virtual bool createPositioningSystems(CodeplugContext &ctx);
  /** Links all added positioning systems within the configuration. */
  virtual bool linkPositioningSystems(CodeplugContext &ctx);

  /** Clears the menu settings in the codeplug. */
  virtual void clearMenuSettings();
  /** Clears the button settings in the codeplug. */
  virtual void clearButtonSettings();
  /** Clears all text messages in the codeplug. */
  virtual void clearTextMessages();
  /** Clears all encryption keys in the codeplug. */
  virtual void clearPrivacyKeys();
  /** Clears all emergency systems in the codeplug. */
  virtual void clearEmergencySystems();
  /** Clears the VFO settings in the codeplug. */
  virtual void clearVFOSettings();
};

#endif // TYT_CODEPLUG_HH
