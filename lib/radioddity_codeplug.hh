#ifndef RADIODDITYCODEPLUG_HH
#define RADIODDITYCODEPLUG_HH

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"
#include "contact.hh"

class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;


/** Base class of all Radioddity codeplugs. This class implements the majority of all codeplug
 * elements present in all Radioddity codeplugs (also some derivatives like OpenGD77). This eases
 * the support of several Radioddity radios, as only the differences in the codeplug to this base
 * class must be implemented.
 *
 * @ingroup radioddity */
class RadioddityCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Implements the base for all Radioddity channel encodings.
   *
   * Memory layout of encoded channel:
   * @verbinclude radioddity_channel.txt */
  class ChannelElement: public CodePlug::Element
  {
  public:
    /** Possible channel types. */
    enum Mode {
      MODE_ANALOG = 0,   ///< Analog channel, aka FM.
      MODE_DIGITAL = 1   ///< Digital channel, aka DMR.
    };

    /** Possible admit criteria. */
    enum Admit {
      ADMIT_ALWAYS = 0,  ///< Allow always.
      ADMIT_CH_FREE = 1, ///< Allow TX on channel free.
      ADMIT_COLOR = 2    ///< Allow TX on matching color-code.
    };

    /** Possible privacy groups, not used in ham radio. */
    enum PrivacyGroup {
      PRIVGR_NONE = 0,     ///< No privacy group, default.
      PRIVGR_53474C39 = 1  ///< Privacy group 53474C39 (wtf?).
    };

  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Resets the channel. */
    virtual void clear();

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &n);

    /** Returns the RX frequency of the channel. */
    virtual uint32_t rxFrequency() const;
    /** Sets the RX frequency of the channel. */
    virtual void setRXFrequency(uint32_t freq);
    /** Returns the TX frequency of the channel. */
    virtual uint32_t txFrequency() const;
    /** Sets the TX frequency of the channel. */
    virtual void setTXFrequency(uint32_t freq);

    /** Returns the channel mode. */
    virtual Mode mode() const;
    /** Sets the channel mode. */
    virtual void setMode(Mode mode);

    /** Returns the TX timeout in seconds. A value of 0 means disabled. */
    virtual uint txTimeOut() const;
    /** Sets the TX timeout in seconds. Setting it to 0 disables the timeout. */
    virtual void setTXTimeOut(uint tot);
    /** Returns the transmit time-out re-key delay in seconds. */
    virtual uint txTimeOutRekeyDelay() const;
    /** Sets the transmit time-out re-key delay in seconds. */
    virtual void setTXTimeOutRekeyDelay(uint delay);

    /** Retunrs the admit criterion. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion. */
    virtual void setAdmitCriterion(Admit admit);

    /** Returns the scan list index (+1). */
    virtual uint scanListIndex() const;
    /** Sets the scan list index (+1). */
    virtual void setScanListIndex(uint index);

    /** Returns the RX subtone. */
    virtual Signaling::Code rxTone() const;
    /** Sets the RX subtone. */
    virtual void setRXTone(Signaling::Code code);
    /** Returns the TX subtone. */
    virtual Signaling::Code txTone() const;
    /** Sets the TX subtone. */
    virtual void setTXTone(Signaling::Code code);

    /** Returns TX signaling index (+1). */
    virtual uint txSignalingIndex() const;
    /** Sets TX signaling index (+1). */
    virtual void setTXSignalingIndex(uint index);
    /** Returns RX signaling index (+1). */
    virtual uint rxSignalingIndex() const;
    /** Sets RX signaling index (+1). */
    virtual void setRXSignalingIndex(uint index);

    /** Returns the privacy group. */
    virtual PrivacyGroup privacyGroup() const;
    /** Sets the privacy group. */
    virtual void setPrivacyGroup(PrivacyGroup grp);

    /** Returns the TX color code. */
    virtual uint txColorCode() const;
    /** Sets the TX color code. */
    virtual void setTXColorCode(uint cc);

    /** Returns the group-list index (+1). */
    virtual uint groupListIndex() const;
    /** Sets the group-list index (+1). */
    virtual void setGroupListIndex(uint index);

    /** Returns the RX color code. */
    virtual uint rxColorCode() const;
    /** Sets the RX color code. */
    virtual void setRXColorCode(uint cc);

    /** Returns the emergency system index (+1). */
    virtual uint emergencySystemIndex() const;
    /** Sets the emergency system index (+1). */
    virtual void setEmergencySystemIndex(uint index);

    /** Returns the transmit contact index (+1). */
    virtual uint contactIndex() const;
    /** Sets the transmit contact index (+1). */
    virtual void setContactIndex(uint index);

    /** Returns @c true if data-call-confirm is enabled. */
    virtual bool dataCallConfirm() const;
    /** Enables/disables data-call-confirm. */
    virtual void enableDataCallConfirm(bool enable);
    /** Returns @c true if emergency alarm ACK is enabled. */
    virtual bool emergencyAlarmACK() const;
    /** Enables/disables emergency alarm ACK. */
    virtual void enableEmergencyAlarmACK(bool enable);
    /** Returns @c true if private-call-confirm is enabled. */
    virtual bool privateCallConfirm() const;
    /** Enables/disables private-call-confirm. */
    virtual void enablePrivateCallConfirm(bool enable);
    /** Returns @c true if privacy is enabled. */
    virtual bool privacyEnabled() const;
    /** Enables/disables privacy. */
    virtual void enablePrivacy(bool enable);

    /** Returns the time slot of the channel. */
    virtual DigitalChannel::TimeSlot timeSlot() const;
    /** Sets the time slot of the channel. */
    virtual void setTimeSlot(DigitalChannel::TimeSlot ts);

    /** Returns @c true if the dual-capacity direct mode is enabled. */
    virtual bool dualCapacityDirectMode() const;
    /** Enables/disables the dual-capacity direct mode. */
    virtual void enableDualCapacityDirectMode(bool enable);
    /** Returns @c true if non-STE is frequency (?!). */
    virtual bool nonSTEFrequency() const;
    /** Enables/disables non-STE is frequency (?!). */
    virtual void enableNonSTEFrequency(bool enable);

    /** Returns the bandwidth. */
    virtual AnalogChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth. */
    virtual void setBandwidth(AnalogChannel::Bandwidth bw);

    /** Returns @c true if RX only is enabled. */
    virtual bool rxOnly() const;
    /** Enables/disables RX only. */
    virtual void enableRXOnly(bool enable);
    /** Returns @c true if talkaround is enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround. */
    virtual void enableTalkaround(bool enable);
    /** Returns @c true if VOX is enabled. */
    virtual bool vox() const;
    /** Enables/disables VOX. */
    virtual void enableVOX(bool enable);

    /** Returns the power setting of the channel. */
    virtual Channel::Power power() const;
    /** Sets the power setting of the channel. */
    virtual void setPower(Channel::Power pwr);

    /** Returns the squelch level. */
    virtual uint squelch() const;
    /** Sets the squelch level. */
    virtual void setSquelch(uint level);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj(Context &ctx) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx);
  };


protected:
  /** Hidden constructor, use a device specific class to instantiate. */
  explicit RadioddityCodeplug(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~RadioddityCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

public:

};

#endif // RADIODDITYCODEPLUG_HH
