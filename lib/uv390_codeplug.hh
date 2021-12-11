#ifndef UV390_CODEPLUG_HH
#define UV390_CODEPLUG_HH

#include "dm1701_codeplug.hh"

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
 *  <tr><td>0x002180</td> <td>0x0059c0</td> <td>0x03840</td> <td>50 Text messages @ 0x120 bytes each.</td></tr>
 *  <tr><td>0x0059c0</td> <td>0x005a70</td> <td>0x000b0</td> <td>Privacy keys, see @c TyTCodeplug::EncryptionElement.</td></tr>
 *  <tr><td>0x005a70</td> <td>0x005a80</td> <td>0x00010</td> <td>Emergency system settings, see @c TyTCodeplug::EmergencySettingsElement.</td></td>
 *  <tr><td>0x005a80</td> <td>0x005f80</td> <td>0x00500</td> <td>Emergency systems, see @c TyTCodeplug::EmergencySystemElement.</td></td>
 *  <tr><td>0x005f80</td> <td>0x00ec20</td> <td>0x08ca0</td> <td>Reserved, filled with 0xff.</td></td>
 *  <tr><td>0x00ec20</td> <td>0x0149e0</td> <td>0x05dc0</td> <td>250 RX Group lists @ 0x60 bytes each, see @c TyTCodeplug::GroupListElement.</td></tr>
 *  <tr><td>0x0149e0</td> <td>0x018860</td> <td>0x03e80</td> <td>250 Zones @ 0x40 bytes each, see @c TyTCodeplug::ZoneElement.</td></tr>
 *  <tr><td>0x018860</td> <td>0x01edf0</td> <td>0x06590</td> <td>250 Scanlists @ 0x68 bytes each, see @c TyTCodeplug::ScanListElement.</td></tr>
 *  <tr><td>0x01edf0</td> <td>0x02ef00</td> <td>0x10110</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x02ef00</td> <td>0x02ef40</td> <td>0x00040</td> <td>VFO A channel, see @c TyTCodeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef40</td> <td>0x02ef80</td> <td>0x00040</td> <td>VFO B channel, see @c TyTCodeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef80</td> <td>0x02f000</td> <td>0x00080</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x02f000</td> <td>0x02f010</td> <td>0x00010</td> <td>Boot settings, see @c TyTCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>0x02f010</td> <td>0x031000</td> <td>0x01ff0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x031000</td> <td>0x03eac0</td> <td>0x0dac0</td> <td>250 Zone-extensions @ 0xe0 bytes each, see @c DM1701::ZoneExtElement.</td></tr>
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
  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    explicit ChannelElement(uint8_t *ptr);

    /** Clears/resets the channel and therefore disables it. */
    void clear();

    /** Returns the in-call criterion for this channel. */
    virtual TyTChannelExtension::InCallCriterion inCallCriteria() const;
    /** Sets the in-call criterion for this channel. */
    virtual void setInCallCriteria(TyTChannelExtension::InCallCriterion crit);

    /** Returns the remote turn-off/kill frequency for this channel. */
    virtual TyTChannelExtension::KillTone turnOffFreq() const;
    /** Sets the remote turn-off/kill frequency for this channel. */
    virtual void setTurnOffFreq(TyTChannelExtension::KillTone freq);

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
    virtual bool dcdmLeader() const;
    /** Enables/disables this radio to be the leader for this DCDM channel. */
    virtual void enableDCDMLeader(bool enable);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj() const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual void fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Implements a VFO channel for TyT radios.
   * This class is an extension of the normal ChannelElement that only implements the step-size
   * feature and encodes it where the name used to be. Thus the memory layout and size is identical
   * to the normal channel. */
  class VFOChannelElement: public ChannelElement
  {
  protected:
    /** Constructor from pointer to memory. */
    VFOChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor from pointer to memory. */
    VFOChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~VFOChannelElement();

    QString name() const;
    void setName(const QString &txt);

    /** Returns the step-size for the VFO channel. */
    virtual unsigned stepSize() const;
    /** Sets the step-size for the VFO channel in Hz. */
    virtual void setStepSize(unsigned ss_hz);
  };

  /** Reuse zone extension from DM1701. */
  typedef DM1701Codeplug::ZoneExtElement ZoneExtElement;

  /** Extends the common @c TyTCodeplug::GeneralSettings to implement the MD-UV390 specific
   * settings.
   *
   * Memory layout of the settings (size 0x00b0 bytes):
   * @verbinclude uv390_settings.txt */
  class GeneralSettingsElement: public DM1701Codeplug::GeneralSettingsElement
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

  /** Represents the boot-time settings (selected zone and channels) within the UV390 code-plug.
   *
   * Memory layout of encoded boot settings:
   * @verbinclude uv390_bootsettings.txt */
  class BootSettingsElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    BootSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit BootSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~BootSettingsElement();

    void clear();

    /** Returns the boot zone index. */
    virtual unsigned zoneIndex() const;
    /** Sets the boot zone index. */
    virtual void setZoneIndex(unsigned idx);
    /** Returns the channel index (within zone) for VFO A. */
    virtual unsigned channelIndexA() const;
    /** Sets the channel index (within zone) for VFO A. */
    virtual void setChannelIndexA(unsigned idx);
    /** Returns the channel index (within zone) for VFO B. */
    virtual unsigned channelIndexB() const;
    /** Sets the channel index (within zone) for VFO B. */
    virtual void setChannelIndexB(unsigned idx);
  };

  /** Represents the menu settings (selected zone and channels) within the UV390 code-plug.
   *
   * Memory layout of encoded boot settings:
   * @verbinclude uv390_menusettings.txt */
  class MenuSettingsElement: public TyTCodeplug::MenuSettingsElement
  {
  protected:
    /** Hidden constructor. */
    MenuSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit MenuSettingsElement(uint8_t *ptr);

    void clear();

    /** Returns @c true if GPS settings menu is enabled. */
    virtual bool gpsSettings() const;
    /** Enables/disables GPS settings menu. */
    virtual void enableGPSSettings(bool enable);
    /** Returns @c true if recording menu is enabled. */
    virtual bool recording() const;
    /** Enables/disables recording menu. */
    virtual void enableRecording(bool enable);

    /** Returns @c true if group call match menu is enabled. */
    virtual bool groupCallMatch() const;
    /** Enables/disables group call match menu. */
    virtual void enableGroupCallMatch(bool enable);
    /** Returns @c true if private call match menu is enabled. */
    virtual bool privateCallMatch() const;
    /** Enables/disables private call match menu. */
    virtual void enablePrivateCallMatch(bool enable);
    /** Returns @c true if menu hang time item is enabled. */
    virtual bool menuHangtimeItem() const;
    /** Enables/disables menu hang time item. */
    virtual void enableMenuHangtimeItem(bool enable);
    /** Returns @c true if TX mode menu is enabled. */
    virtual bool txMode() const;
    /** Enables/disables TX mode menu. */
    virtual void enableTXMode(bool enable);
    /** Returns @c true if zone settings menu is enabled. */
    virtual bool zoneSettings() const;
    /** Enables/disables zone settings menu. */
    virtual void enableZoneSettings(bool enable);
    /** Returns @c true if new zone menu is enabled. */
    virtual bool newZone() const;
    /** Enables/disables new zone menu. */
    virtual void enableNewZone(bool enable);

    /** Returns @c true if edit zone menu is enabled. */
    virtual bool editZone() const;
    /** Enables/disables edit zone menu. */
    virtual void enableEditZone(bool enable);
    /** Returns @c true if new scan list menu is enabled. */
    virtual bool newScanList() const;
    /** Enables/disables new scan list menu. */
    virtual void enableNewScanList(bool enable);
  };


public:
  /** Constructor. */
  explicit UV390Codeplug(QObject *parent = nullptr);
  /** Destructor. */
  virtual ~UV390Codeplug();

  void clear();

public:
  void clearTimestamp();
  bool encodeTimestamp();

  void clearGeneralSettings();
  bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Config *config, const ErrorStack &err=ErrorStack());

  void clearChannels();
  bool encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearContacts();
  bool encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearZones();
  bool encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearGroupLists();
  bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearScanLists();
  bool encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearPositioningSystems();
  bool encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkPositioningSystems(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearButtonSettings();
  bool encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeButtonSetttings(Config *config, const ErrorStack &err=ErrorStack());

  /** Resets the boot setting, e.g. initial channels and zone at bootup. */
  virtual void clearBootSettings();
  void clearMenuSettings();
  void clearTextMessages();
  void clearPrivacyKeys();
  void clearEmergencySystems();
  /** Clears the VFO A & B. */
  virtual void clearVFOSettings();

};

#endif // UV390CODEPLUG_HH
