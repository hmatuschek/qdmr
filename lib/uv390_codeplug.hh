#ifndef UV390_CODEPLUG_HH
#define UV390_CODEPLUG_HH

#include "tyt_codeplug.hh"

/** Device specific implementation of the codeplug for the TyT MD-UV390.
 *
 * The codeplug consists of two segments. The first segment starts at address @c 0x002000 and ends at
 * address @c 0x040000. The second segment starts at address @c 0x110000 and ends at @c 0x1a0000. The
 * segments must align with @c 0x400 (1024 bytes).
 *
 * @section uv390cpl Codeplug structure within radio
 * The codeplug structure is reverse engineered almost completely and can be programmed from
 * scratch. That is, it is not neccessary to update an existing codeplug on the radio.
 * <table>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x002000-0x040000</th></tr>
 *  <tr><td>0x002000</td> <td>0x00200c</td> <td>0x0000c</td> <td>Timestamp see @c TyTCodeplug::TimestampElement.</td></tr>
 *  <tr><td>0x00200c</td> <td>0x002040</td> <td>0x00034</td> <td>Reserved, filled with 0xff. </td></tr>
 *  <tr><td>0x002040</td> <td>0x0020f0</td> <td>0x000b0</td> <td>General settings see @c TyTCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>0x0020f0</td> <td>0x002100</td> <td>0x00010</td> <td>Menu settings, see @c TyTCodeplug::MenuSettingsElement.</td></tr>
 *  <tr><td>0x002100</td> <td>0x002140</td> <td>0x00040</td> <td>Button config, see @c TyTCodeplug::ButtonSettingsElement.</td></tr>
 *  <tr><td>0x002140</td> <td>0x002180</td> <td>0x00040</td> <td>Reserved, filled with 0xff.</td></tr>
 *  <tr><td>0x002180</td> <td>0x0059c0</td> <td>0x03840</td> <td>50 Text messages @ 0x120 bytes each, see @c UV390Codeplug::message_t.</td></tr>
 *  <tr><td>0x0059c0</td> <td>0x005a70</td> <td>0x000b0</td> <td>Privacy keys, see @c TyTCodeplug::EncryptionElement.</td></tr>
 *  <tr><td>0x005a50</td> <td>0x005f60</td> <td>0x00510</td> <td>Emergency Systems, see @c TyTCodeplug::EmergencySystemElement.</td></td>
 *  <tr><td>0x005f60</td> <td>0x00ec20</td> <td>0x08cc0</td> <td>Reserved, filled with 0xff.</td></td>
 *  <tr><td>0x00ec20</td> <td>0x0149e0</td> <td>0x05dc0</td> <td>250 RX Group lists @ 0x60 bytes each, see @c TyTCodeplug::GroupListElement.</td></tr>
 *  <tr><td>0x0149e0</td> <td>0x018860</td> <td>0x03e80</td> <td>250 Zones @ 0x40 bytes each, see @c TyTCodeplug::ZoneElement.</td></tr>
 *  <tr><td>0x018860</td> <td>0x01edf0</td> <td>0x06590</td> <td>250 Scanlists @ 0x68 bytes each, see @c TyTCodeplug::ScanListElement.</td></tr>
 *  <tr><td>0x01edf0</td> <td>0x02ef00</td> <td>0x10110</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x02ef00</td> <td>0x02ef40</td> <td>0x00040</td> <td>VFO A channel, see @c TyTCodeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef40</td> <td>0x02ef80</td> <td>0x00040</td> <td>VFO B channel, see @c TyTCodeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef80</td> <td>0x02f000</td> <td>0x00080</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x02f000</td> <td>0x02f010</td> <td>0x00010</td> <td>Boot settings, see @c TyTCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>0x02f010</td> <td>0x031000</td> <td>0x01ff0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x031000</td> <td>0x03eac0</td> <td>0x0dac0</td> <td>250 Zone-extensions @ 0xe0 bytes each, see @c TyTCodeplug::ZoneExtElement.</td></tr>
 *  <tr><td>0x03eac0</td> <td>0x03ec40</td> <td>0x00180</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x03ec40</td> <td>0x03ed40</td> <td>0x00100</td> <td>16 GPS systems @ 0x10 bytes each, see @c TyTCodeplug::GPSSystemElement.</td></tr>
 *  <tr><td>0x03ed40</td> <td>0x040000</td> <td>0x012c0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><th colspan="4">Second segment 0x110000-0x1a0000</th></tr>
 *  <tr><td>0x110000</td> <td>0x13ee00</td> <td>0x2ee00</td> <td>3000 Channels @ 0x40 bytes each, see @c TyTCodeplug::ChannelElement.</td></tr>
 *  <tr><td>0x13ee00</td> <td>0x140000</td> <td>0x01200</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x140000</td> <td>0x197e40</td> <td>0x57e40</td> <td>10000 Contacts @ 0x24 bytes each, see @c TyTCodeplug::ContactElement.</td></tr>
 *  <tr><td>0x197e40</td> <td>0x1a0000</td> <td>0x081c0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 * </table>
 *
 * @ingroup uv390 */
class UV390Codeplug : public TyTCodeplug
{
  Q_OBJECT

public:
  /** Extends the @c TyTCodeplug::ChannelElement for the TyT MD-UV390 and Retevis RT3S.
   *
   * Memory layout of encoded channel:
   * @verbinclude uv390_channel.txt */
  class ChannelElement: public TyTCodeplug::ChannelElement
  {
  public:
    /** Again, I have no idea. */
    enum InCall {
      INCALL_ALWAYS = 0,
      INCALL_ADMIT = 1,
      INCALL_TXINT = 2
    };

    /** Turn-off tone frequency.
     * This radio has a feature that allows to disable radios remotely by sending a specific tone.
     * Certainly not a feature used in ham-radio. */
    enum TurnOffFreq {
      TURNOFF_NONE = 3,             ///< Turn-off disabled. Default!
      TURNOFF_259_2HZ = 0,          ///< Turn-off on 259.2Hz tone.
      TURNOFF_55_2HZ = 1            ///< Turn-off on 55.2Hz tone.
    };


  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    explicit ChannelElement(uint8_t *ptr);

    /** Clears/resets the channel and therefore disables it. */
    void clear();

    /** Returns the in-call criterion for this channel. */
    virtual InCall inCallCriteria() const;
    /** Sets the in-call criterion for this channel. */
    virtual void setInCallCriteria(InCall crit);

    /** Returns the remote turn-off/kill frequency for this channel. */
    virtual TurnOffFreq turnOffFreq() const;
    /** Sets the remote turn-off/kill frequency for this channel. */
    virtual void setTurnOffFreq(TurnOffFreq freq);

    /** Returns the squelch level [0-10]. */
    virtual unsigned squelch() const;
    /** Sets the squelch level [0-10]. */
    virtual void setSquelch(unsigned value);

    /** Returns the power of this channel. */
    virtual Channel::Power power() const;
    /** Sets the power of this channel. */
    virtual void setPower(Channel::Power pwr);

    /** Returns @c true if the channel allows interruption enabled. */
    virtual bool allowInterrupt() const;
    /** Enables/disables interruption for this channel. */
    virtual void enableAllowInterrupt(bool enable);

    /** Returns @c true if the channel has dual-capacity direct mode enabled. */
    virtual bool dualCapacityDirectMode() const;
    /** Enables/disables dual-capacity direct mode for this channel. */
    virtual void enableDualCapacityDirectMode(bool enable);

    /** Retruns @c true if the radio acts as the leader for this DCDM channel. */
    virtual bool leaderOrMS() const;
    /** Enables/disables this radio to be the leader for this DCDM channel. */
    virtual void enableLeaderOrMS(bool enable);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj() const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual void fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Extends the common @c TyTCodeplug::GeneralSettings to implement the MD-UV390 specific
   * settings.
   *
   * Memory layout of the settings (size 0x???? bytes):
   * @verbinclude uv390_settings.txt */
  class GeneralSettingsElement: public TyTCodeplug::GeneralSettingsElement
  {
  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    void clear();

    /** Defines all possible transmit modes. */
    enum TransmitMode {
      LAST_CALL_CH = 0,
      LAST_CALL_AND_HAND_CH = 1,
      DESIGNED_CH = 2,
      DESIGNED_AND_HAND_CH = 3,
    };

    /** Returns the transmit mode. */
    virtual TransmitMode transmitMode() const;
    /** Sets the transmit mode. */
    virtual void setTransmitMode(TransmitMode mode);

    /** Returns @c true, if the speech synthesis is enabled. */
    virtual bool channelVoiceAnnounce() const;
    /** Enables/disables the speech synthesis. */
    virtual void enableChannelVoiceAnnounce(bool enable);

    /** Returns @c true, if keypad tones are enabled. */
    virtual bool keypadTones() const;
    /** Enables/disables the keypad tones. */
    virtual void enableKeypadTones(bool enable);

    /** Returns @c true, if VFO A is in channel mode. */
    virtual bool channelModeA() const;
    /** Enables/disables the channel mode for VFO A. */
    virtual void enableChannelModeA(bool enable);
    /** Returns @c true, if VFO B is in channel mode. */
    virtual bool channelModeB() const;
    /** Enables/disables the channel mode for VFO B. */
    virtual void enableChannelModeB(bool enable);

    /** Returns @c true, if the radio is in channel (and not VFO) mode. */
    virtual bool channelMode() const;
    /** Enable/disable channel mode. */
    virtual void enableChannelMode(bool enable);

    /** Returns @c true if group-call match is enabled. */
    virtual bool groupCallMatch() const;
    /** Enables/disables group-call match. */
    virtual void enableGroupCallMatch(bool enable);
    /** Returns @c true if private-call match is enabled. */
    virtual bool privateCallMatch() const;
    /** Enables/disables private-call match. */
    virtual void enablePrivateCallMatch(bool enable);

    /** Returns the time-zone. */
    virtual QTimeZone timeZone() const;
    /** Sets the time-zone. */
    virtual void setTimeZone(const QTimeZone &zone);

    /** Returns the channel hang time. */
    virtual unsigned channelHangTime() const;
    /** Sets the channel hang time. */
    virtual void setChannelHangTime(unsigned dur);
    /** Returns @c true, if public zone is enabled. */
    virtual bool publicZone() const;
    /** Enables/disables public zone. */
    virtual void enablePublicZone(bool enable);

    /** Returns the n-th DMR id. */
    virtual uint32_t additionalDMRId(unsigned n) const;
    /** Sets the n-th DMR id. */
    virtual void setAdditionalDMRId(unsigned n, uint32_t id);

    /** Returns the microphone gain. */
    virtual unsigned micLevel() const;
    /** Sets the microphone gain. */
    virtual void setMICLevel(unsigned val);

    /** If @c true, radio ID editing is enabled. */
    virtual bool editRadioID() const;
    /** Enable/disable radio ID editing. */
    virtual void enableEditRadioID(bool enable);

    /** Encodes the general settings. */
    virtual bool fromConfig(const Config *config);
    /** Updates config from general settings. */
    virtual bool updateConfig(Config *config);
  };

public:
  /** Constructor. */
  explicit UV390Codeplug(QObject *parent = nullptr);
  /** Destructor. */
  virtual ~UV390Codeplug();

public:
  void clearTimestamp();
  bool encodeTimestamp();

  void clearGeneralSettings();
  bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx);
  bool decodeGeneralSettings(Config *config);

  void clearChannels();
  bool encodeChannels(Config *config, const Flags &flags, Context &ctx);
  bool createChannels(Config *config, Context &ctx);
  bool linkChannels(Context &ctx);

  void clearContacts();
  bool encodeContacts(Config *config, const Flags &flags, Context &ctx);
  bool createContacts(Config *config, Context &ctx);

  void clearZones();
  bool encodeZones(Config *config, const Flags &flags, Context &ctx);
  bool createZones(Config *config, Context &ctx);
  bool linkZones(Context &ctx);

  void clearGroupLists();
  bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx);
  bool createGroupLists(Config *config, Context &ctx);
  bool linkGroupLists(Context &ctx);

  void clearScanLists();
  bool encodeScanLists(Config *config, const Flags &flags, Context &ctx);
  bool createScanLists(Config *config, Context &ctx);
  bool linkScanLists(Context &ctx);

  void clearPositioningSystems();
  bool encodePositioningSystems(Config *config, const Flags &flags, Context &ctx);
  bool createPositioningSystems(Config *config, Context &ctx);
  bool linkPositioningSystems(Context &ctx);

  void clearButtonSettings();
  bool encodeButtonSettings(Config *config, const Flags &flags, Context &ctx);
  bool decodeButtonSetttings(Config *config);

  void clearBootSettings();
  void clearMenuSettings();
  void clearTextMessages();
  void clearPrivacyKeys();
  void clearEmergencySystems();
  void clearVFOSettings();

};

#endif // MD2017CODEPLUG_HH
