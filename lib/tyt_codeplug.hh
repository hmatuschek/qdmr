#ifndef RT3S_GPS_CODEPLUG_HH
#define RT3S_GPS_CODEPLUG_HH

#include <QDateTime>

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"

class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;
class CodeplugContext;


/**
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
    ChannelElement(uint8_t *ptr);
    virtual ~ChannelElement();

    bool isValid() const;
    void clear();

    virtual Mode mode() const;
    virtual void mode(Mode mode);

    virtual AnalogChannel::Bandwidth bandwidth() const;
    virtual void bandwidth(AnalogChannel::Bandwidth bw);

    virtual bool autoScan() const;
    virtual void autoScan(bool enable);

    virtual bool loneWorker() const;
    virtual void loneWorker(bool enable);

    virtual bool talkaround() const;
    virtual void talkaround(bool enable);

    virtual bool rxOnly() const;
    virtual void rxOnly(bool enable);

    virtual DigitalChannel::TimeSlot timeSlot() const;
    virtual void timeSlot(DigitalChannel::TimeSlot ts);

    virtual uint8_t colorCode() const;
    virtual void colorCode(uint8_t ts);

    virtual uint8_t privacyIndex() const;
    virtual void privacyIndex(uint8_t ts);
    virtual PrivacyType privacyType() const;
    virtual void privacyType(PrivacyType type);

    virtual bool privateCallConfirm() const;
    virtual void privateCallConfirm(bool enable);
    virtual bool dataCallConfirm() const;
    virtual void dataCallConfirm(bool enable);

    virtual RefFrequency rxRefFrequency() const;
    virtual void rxRefFrequency(RefFrequency ref);
    virtual RefFrequency txRefFrequency() const;
    virtual void txRefFrequency(RefFrequency ref);

    virtual bool emergencyAlarmACK() const;
    virtual void emergencyAlarmACK(bool enable);

    virtual bool displayPTTId() const;
    virtual void displayPTTId(bool enable);

    virtual bool vox() const;
    virtual void vox(bool enable);

    virtual Admit admitCriterion() const;
    virtual void admitCriterion(Admit admit);

    virtual InCall inCallCriteria() const;
    virtual void inCallCriteria(InCall crit);

    virtual TurnOffFreq turnOffFreq() const;
    virtual void turnOffFreq(TurnOffFreq freq);

    virtual uint16_t contactIndex() const;
    virtual void contactIndex(uint16_t idx);

    virtual uint txTimeOut() const;
    virtual void txTimeOut(uint tot);
    virtual uint8_t txTimeOutRekeyDelay() const;
    virtual void txTimeOutRekeyDelay(uint8_t delay);

    virtual uint8_t emergencySystemIndex() const;
    virtual void emergencySystemIndex(uint8_t idx);
    virtual uint8_t scanListIndex() const;
    virtual void scanListIndex(uint8_t idx);
    virtual uint8_t groupListIndex() const;
    virtual void groupListIndex(uint8_t idx);
    virtual uint8_t positioningSystemIndex() const;
    virtual void positioningSystemIndex(uint8_t idx);

    virtual bool dtmfDecode(uint8_t idx) const;
    virtual void dtmfDecode(uint8_t idx, bool enable);

    virtual uint squelch() const;
    virtual void squelch(uint value);

    virtual uint32_t rxFrequency() const;
    virtual void rxFrequency(uint32_t Hz);
    virtual uint32_t txFrequency() const;
    virtual void txFrequency(uint32_t Hz);

    virtual Signaling::Code rxSignaling() const;
    virtual void rxSignaling(Signaling::Code code);
    virtual Signaling::Code txSignaling() const;
    virtual void txSignaling(Signaling::Code code);
    virtual uint8_t rxSignalingSystemIndex() const;
    virtual void rxSignalingSystemIndex(uint8_t idx);
    virtual uint8_t txSignalingSystemIndex() const;
    virtual void txSignalingSystemIndex(uint8_t idx);

    virtual Channel::Power power() const;
    virtual void power(Channel::Power pwr);

    virtual bool txGPSInfo() const;
    virtual void txGPSInfo(bool enable);
    virtual bool rxGPSInfo() const;
    virtual void rxGPSInfo(bool enable);

    virtual bool allowInterrupt() const;
    virtual void allowInterrupt(bool enable);

    virtual bool dualCapacityDirectMode() const;
    virtual void dualCapacityDirectMode(bool enable);

    virtual bool leaderOrMS() const;
    virtual void leaderOrMS(bool enable);

    virtual QString name() const;
    virtual void name(const QString &name);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj() const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, const CodeplugContext &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual void fromChannelObj(const Channel *c, const CodeplugContext &ctx);
  };


  class VFOChannelElement: public ChannelElement
  {
  public:
    VFOChannelElement(uint8_t *ptr);
    virtual ~VFOChannelElement();

    virtual uint8_t stepSize() const;
    virtual void stepSize(uint8_t ss);
  };


public:
  /** Empty constructor. */
  explicit TyTCodeplug(QObject *parent = nullptr);
  /** Destructor. */
  virtual ~TyTCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

public:
  virtual void clearTimestamp();
  virtual bool encodeTimestamp(Config *config, const Flags &flags);
  virtual bool decodeTimestamp(Config *config);

  virtual void clearGeneralSettings();
  virtual bool encodeGeneralSettings(Config *config, const Flags &flags);
  virtual bool decodeGeneralSettings(Config *config);

  virtual void clearBootSettings();
  virtual bool encodeBootSettings(Config *config, const Flags &flags);
  virtual bool decodeBootSettings(Config *config);

  virtual void clearContacts();
  virtual bool encodeContacts(Config *config, const Flags &flags);
  virtual bool createContacts(Config *config, CodeplugContext &ctx);

  virtual void clearGroupLists();
  virtual bool encodeGroupLists(Config *config, const Flags &flags);
  virtual bool createGroupLists(Config *config, CodeplugContext &ctx);
  virtual bool linkGroupLists(Config *config, CodeplugContext &ctx);

  virtual void clearChannels();
  virtual bool encodeChannels(Config *config, const Flags &flags);
  virtual bool createChannels(Config *config, CodeplugContext &ctx);
  virtual bool linkChannels(Config *config, CodeplugContext &ctx);

  virtual void clearZones();
  virtual bool encodeZones(Config *config, const Flags &flags);
  virtual bool createZones(Config *config, CodeplugContext &ctx);
  virtual bool linkZones(Config *config, CodeplugContext &ctx);

  virtual void clearScanLists();
  virtual bool encodeScanLists(Config *config, const Flags &flags);
  virtual bool createScanLists(Config *config, CodeplugContext &ctx);
  virtual bool linkScanLists(Config *config, CodeplugContext &ctx);

  virtual void clearPositioningSystems();
  virtual bool encodePositioningSystems(Config *config, const Flags &flags);
  virtual bool createPositioningSystems(Config *config, CodeplugContext &ctx);
  virtual bool linkPositioningSystems(Config *config, CodeplugContext &ctx);

  virtual void clearMenuSettings();
  virtual void clearButtonSettings();
  virtual void clearTextMessages();
  virtual void clearPrivacyKeys();
  virtual void clearEmergencySystems();
  virtual void clearVFOSettings();
};

#endif // UV390_CODEPLUG_HH
