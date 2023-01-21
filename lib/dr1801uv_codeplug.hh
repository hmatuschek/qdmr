#ifndef DR1801UVCODEPLUG_HH
#define DR1801UVCODEPLUG_HH

#include "codeplug.hh"

/** Implements the binary codeplug representation of a BTECH DR-1801UV (a.k.a, BF-1801-A6).
 *
 * This device is kind of a re-release of the BTECH DM-1801. However, the hardware and firmware
 * is completely different. It is just the chassis that remained the same. This version uses the
 * Auctus A6 radio-on-a-chip as its main component.
 *
 * @section dr1801ver Matching firmware versions
 * This class implements the codeplug for the firmware version @c 1.10. The codeplug format usually
 * does not change much with firmware revisions, in particular not with older radios.
 *
 * @section dr1801cpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is simply a single large block of
 * data, read and written entirely. The total size of the codeplug is 01dd90h bytes.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><td>0x00000</td> <td>0x00000</td>  <td>0x0000</td>  <td></td></tr>
 *  <tr><td>0x04330</td> <td>0x00008</td>  <td>0x0008</td>  <td>Number of contacts.</td></tr>
 *  <tr><td>0x04338</td> <td>0x0a338</td>  <td>0x6000</td>  <td>1024 contacts, see
 *      @c DR1801UVCodeplug::ContactElement. </td></tr>
 *  <tr><td>0x0a660</td> <td>0x17660</td>  <td>0xd000</td>  <td>1024 Channel settings.
 *      34h bytes each. Names are stored separately. See @c DR1801UVCodeplug::ChannelElement for
 *      details.</td></tr>
 *  <tr><td>0x17660</td> <td>0x1c660</td>  <td>0x5000</td>  <td>1024 Channel names, 20 ASCII chars,
 *      0-terminated. Unset/unused names are filled with 0.</td></tr>
 * </table>
 *
 * @ingroup dr1801uv */
class DR1801UVCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements the binary encoding of the channels settings.
   *
   * Memory representation of the channel settings (0034h bytes):
   * @verbinclude dr1801uv_channelelement.txt */
  class ChannelElement: public Element
  {
  public:
    /** Possible channel types. */
    enum class Type {
      FM = 1, DMR = 3
    };
    /** Possible power settings. */
    enum class Power {
      Low = 0, High = 1
    };
    /** Possible values for the admid criterion. */
    enum class Admit {
      Always = 0, ColorCode_or_Tone = 1, ChannelFree = 2
    };
    /** Possible time-slot values. */
    enum class TimeSlot {
      TS1 = 1, TS2 = 2
    };
    /** Possible FM signaling modes. */
    enum class SignalingMode {
      None = 0, DTMF = 1
    };
    /** Possible band width settings. */
    enum class Bandwidth {
      Narrow = 1, Wide = 2
    };
    /** Possible subtone types. */
    enum class SubToneType {
      None = 0, CTCSS = 1, DCS = 2
    };
    /** Possible DCS modes. */
    enum class DCSMode {
      Normal = 0, Inverted = 1
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor from pointer. */
    ChannelElement(uint8_t *ptr);

    bool isValid() const;
    void clear();

    /** Retunrs the 0-based index of the channel. */
    virtual uint16_t index() const;
    /** Sets the index. */
    virtual void setIndex(uint16_t idx);

    /** Returns the channel type. */
    virtual Type channelType() const;
    /** Sets the channel type. */
    virtual void setChannelType(Type type);

    /** Returns the power settings of the channel. */
    virtual Channel::Power power() const;
    /** Sets the power for the channel. */
    virtual void setPower(Channel::Power pwr);

    /** Retunrs the RX frequency in MHz. */
    virtual double rxFrequency() const;
    /** Sets the RX frequency in MHz. */
    virtual void setRXFrequency(double MHz);
    /** Retunrs the TX frequency in MHz. */
    virtual double txFrequency() const;
    /** Sets the TX frequency in MHz. */
    virtual void setTXFrequency(double MHz);

    /** Returns @c true if a contact index is set. */
    virtual bool hasTransmitContact() const;
    /** Retunrs the contact index. */
    virtual unsigned int transmitContactIndex() const;
    /** Sets the transmit contact index. */
    virtual void setTransmitContactIndex(unsigned int index);
    /** Clears the contact index. */
    virtual void clearTransmitContactIndex();

    /** Returns the admit criterion. */
    virtual Admit admitCriterion() const;
    /** Sets the admit criterion. */
    virtual void setAdmitCriterion(Admit admit);

    /** Returns the color code (0-15). */
    virtual unsigned int colorCode() const;
    /** Sets the color code (0-15). */
    virtual void setColorCode(unsigned int cc);
    /** Returns the time slot. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns @c true if an ecryption key index is set. */
    virtual bool hasEncryptionKey() const;
    /** Returns the encryption key index. */
    virtual uint8_t encryptionKeyIndex() const;
    /** Sets the encryption key index. */
    virtual void setEncryptionKeyIndex(uint8_t index);
    /** Clears the encryption key index. */
    virtual void clearEncryptionKeyIndex();

    /** Returns @c true if dual-capacity direct mode is enabled. */
    virtual bool dcdm() const;
    /** Enables/disables dual-capacity direct mode. */
    virtual void enableDCDM(bool enable);
    /** Returns @c true if private-call confirmation is enabled. */
    virtual bool confirmPrivateCall() const;
    /** Enables/disables private-call confirmation. */
    virtual void enablePrivateCallConfirmation(bool enable);

    /** Returns the FM signaling mode. */
    virtual SignalingMode signalingMode() const;
    /** Sets the FM signaling mode. */
    virtual void setSignalingMode(SignalingMode mode);

    /** Returns @c true if the alarm system index is set. */
    virtual bool hasAlarmSystem() const;
    /** Returns the index of the alarm system. */
    virtual uint8_t alarmSystemIndex() const;
    /** Sets the alarm system index. */
    virtual void setAlarmSystemIndex(uint8_t index);
    /** Clears the alarm system index. */
    virtual void clearAlarmSystemIndex();

    /** Retunrs the band width for FM channels. */
    virtual FMChannel::Bandwidth bandwidth() const;
    /** Sets the band width for FM channels. */
    virtual void setBandwidth(FMChannel::Bandwidth bw);

    /** Retunrs @c true, if the auto-scan is enabled. */
    virtual bool autoScanEnabled() const;
    /** Enables/disables auto-scan. */
    virtual void enableAutoScan(bool enable);
    /** Retunrs @c true if a scan list is assigned. */
    virtual bool hasScanList() const;
    /** Returns the scan list index. */
    virtual uint8_t scanListIndex() const;
    /** Sets the scan list index. */
    virtual void setScanListIndex(uint8_t index);
    /** Clears the scan list index. */
    virtual void clearScanListIndex();

    /** Retunrs the RX CTCSS/DCS signaling. */
    virtual Signaling::Code rxTone() const;
    /** Sets the RX CTCSS/DCS signaling. */
    virtual void setRXTone(Signaling::Code code);
    /** Retunrs the TX CTCSS/DCS signaling. */
    virtual Signaling::Code txTone() const;
    /** Sets the TX CTCSS/DCS signaling. */
    virtual void setTXTone(Signaling::Code code);

    /** Returns @c true if talkaround is enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround. */
    virtual void enableTalkaround(bool enable);

    /** Returns @c true if a PTT ID is set. */
    virtual bool hasPTTID() const;
    /** Returns the PTT-ID index. */
    virtual uint8_t pttIDIndex() const;
    /** Sets the PTT-ID index. */
    virtual void setPTTIDIndex(uint8_t idx);
    /** Clears the PTT ID. */
    virtual void clearPTTID();

    /** Retruns @c true if a group list is assigned. */
    virtual bool hasGroupList() const;
    /** Retunrs the group-list index. */
    virtual uint8_t groupListIndex() const;
    /** Sets the group-list index. */
    virtual void setGroupListIndex(uint8_t index);
    /** Clears the group list index. */
    virtual void clearGroupListIndex();

    /** Returns @c true if lone-worker is enabled. */
    virtual bool loneWorker() const;
    /** Enables/disables lone-worker. */
    virtual void enableLoneWorker(bool enable);

    /** Constructs a channel object. */
    virtual Channel *toChannelObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the channel object. */
    virtual bool linkChannelObj(Channel *channel, Context &ctx, const ErrorStack &err=ErrorStack()) const;
  };

  /** Implements the binary encoding of a contact.
   *
   * Memory representation of contact (0018h bytes):
   * @verbinclude dr1801uv_contactelement.txt */
  class ContactElement: public Element
  {
  public:
    /** Possible call types. */
    enum class CallType {
      AllCall = 0x20, PrivateCall = 0x40, GroupCall = 0x80
    };

  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);

    bool isValid() const;
    void clear();

    /** Returns @c true if the contact has a successor. */
    virtual bool hasSuccessor() const;
    /** Returns the index of the next element. */
    virtual uint16_t successorIndex() const;
    /** Sets successor index. */
    virtual void setSuccessorIndex(uint16_t index);
    /** Clears the successor index. */
    virtual void clearSuccessorIndex();

    /** Returns the number. */
    virtual uint32_t dmrID() const;
    /** Sets the number. */
    virtual void setDMRID(uint32_t id);

    /** Returns the call type. */
    virtual DMRContact::Type type() const;
    /** Sets the call type. */
    virtual void setCallType(DMRContact::Type type);

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &name);

    virtual DMRContact *toContactObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    virtual bool linkContactObj(DMRContact *contact, Context &ctx, const ErrorStack &err=ErrorStack());
  };

public:
  /** Default constructor. */
  explicit DR1801UVCodeplug(QObject *parent = nullptr);

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;
  bool encode(Config *config, const Flags &flags, const ErrorStack &err=ErrorStack());
  bool decode(Config *config, const ErrorStack &err=ErrorStack());

protected:
  /** Decode codeplug elements. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link decoded elements. */
  virtual bool linkElements(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode channel elements. */
  virtual bool decodeChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link channel elements. */
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Decode contact elements. */
  virtual bool decodeContacts(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link contact elements. */
  virtual bool linkContacts(Context &ctx, const ErrorStack &err=ErrorStack());

};

#endif // DR1801UVCODEPLUG_HH
