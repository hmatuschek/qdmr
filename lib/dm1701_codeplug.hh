#ifndef DM1701_CODEPLUG_HH
#define DM1701_CODEPLUG_HH

#include "tyt_codeplug.hh"

/** Device specific implementation of the codeplug for the Baofeng DM-1701.
 *
 * The codeplug consists of two segments. The first segment starts at address @c 0x002000 and ends at
 * address @c 0x040000. The second segment starts at address @c 0x110000 and ends at @c 0x1a0000. The
 * segments must align with @c 0x400 (1024 bytes).
 *
 * @section dm1701cpl Codeplug structure within radio
 * The codeplug structure is reverse engineered almost completely and can be programmed from
 * scratch. That is, it is not neccessary to update an existing codeplug on the radio.
 * <table>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x002000-0x040000</th></tr>
 *  <tr><td>0x002000</td> <td>0x00200c</td> <td>0x0000c</td> <td>Timestamp see @c TyTCodeplug::TimestampElement.</td></tr>
 *  <tr><td>0x00200c</td> <td>0x002040</td> <td>0x00034</td> <td>Reserved, filled with 0xff. </td></tr>
 *  <tr><td>0x002040</td> <td>0x0020f0</td> <td>0x000b0</td> <td>General settings see @c DM1701Codeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>0x0020f0</td> <td>0x002100</td> <td>0x00010</td> <td>Menu settings, see @c TyTCodeplug::MenuSettingsElement</td></tr>
 *  <tr><td>0x002100</td> <td>0x002140</td> <td>0x00040</td> <td>Button config, see @c DM1701Codeplug::ButtonSettingsElement.</td></tr>
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
 *  <tr><td>0x02ef00</td> <td>0x02ef40</td> <td>0x00040</td> <td>VFO A channel, see @c DM1701Codeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef40</td> <td>0x02ef80</td> <td>0x00040</td> <td>VFO B channel, see @c DM1701Codeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef80</td> <td>0x031000</td> <td>0x02080</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x031000</td> <td>0x03eac0</td> <td>0x0dac0</td> <td>250 Zone-extensions @ 0xe0 bytes each, see @c DM1701Codeplug::ZoneExtElement.</td></tr>
 *  <tr><td>0x03eac0</td> <td>0x03ec40</td> <td>0x00180</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x03ec40</td> <td>0x03ed40</td> <td>0x00100</td> <td>16 GPS systems @ 0x10 bytes each, see @c TyTCodeplug::GPSSystemElement.</td></tr>
 *  <tr><td>0x03ed40</td> <td>0x040000</td> <td>0x012c0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><th colspan="4">Second segment 0x110000-0x1a0000</th></tr>
 *  <tr><td>0x110000</td> <td>0x13ee00</td> <td>0x2ee00</td> <td>3000 Channels @ 0x40 bytes each, see @c DM1701Codeplug::ChannelElement.</td></tr>
 *  <tr><td>0x13ee00</td> <td>0x140000</td> <td>0x01200</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x140000</td> <td>0x197e40</td> <td>0x57e40</td> <td>10000 Contacts @ 0x24 bytes each, see @c TyTCodeplug::ContactElement.</td></tr>
 *  <tr><td>0x197e40</td> <td>0x1a0000</td> <td>0x081c0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 * </table>
 *
 * @ingroup dm1701 */
class DM1701Codeplug : public TyTCodeplug
{
  Q_OBJECT

public:
  /** Extends the common @c TyTCodeplug::ChannelElement to implement the DM-1701 specific settings.
   *
   * Memory layout of the channel (size 0x0040 bytes):
   * @verbinclude dm1701_channel.txt */
  class ChannelElement: public TyTCodeplug::ChannelElement
  {
  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    void clear();

    /** Returns @c true if the squelch is 'tight'. */
    virtual bool tightSquelchEnabled() const;
    /** Enables/disables tight squelch. */
    virtual void enableTightSquelch(bool enable);

    /** Returns @c true if the reversed burst is enabled. */
    virtual bool reverseBurst() const;
    /** Enables/disables reverse burst. */
    virtual void enableReverseBurst(bool enable);

    /** Returns the power of this channel. */
    virtual Channel::Power power() const;
    /** Sets the power of this channel. */
    virtual void setPower(Channel::Power pwr);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj() const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual void fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Extends the @c ChannelElement to implement the VFO channel settings for the DM-1701.
   * This class is an extension of the normal @c ChannelElement that only implements the step-size
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

  /** Extends the common @c TyTCodeplug::GeneralSettingsElement to implement the DM-1701 specific
   * settings.
   *
   * Memory layout of the settings (size 0x00b0 bytes):
   * @verbinclude dm1701_settings.txt */
  class GeneralSettingsElement: public TyTCodeplug::GeneralSettingsElement
  {
  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    void clear();

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

    /** Returns the channel hang time in ms. */
    virtual unsigned channelHangTime() const;
    /** Sets the channel hang time in ms. */
    virtual void setChannelHangTime(unsigned dur);

    /** Encodes the general settings. */
    virtual bool fromConfig(const Config *config);
    /** Updates config from general settings. */
    virtual bool updateConfig(Config *config);
  };

  /** Extens the common @c TyTCodeplug::ButtonSettingsElement to implement the DM-1701 specific
   * settings.
   *
   * Memory layout of the setting (size 0x0014 bytes):
   * @verbinclude dm1701_buttonsettings.txt */
  class ButtonSettingsElement: public TyTCodeplug::ButtonSettingsElement
  {
  protected:
    /** Hidden constructor. */
    ButtonSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ButtonSettingsElement(uint8_t *ptr);

    void clear();

    /** Returns the action for a short press on side button 3. */
    virtual ButtonAction sideButton3Short() const;
    /** Sets the action for a short press on side button 3. */
    virtual void setSideButton3Short(ButtonAction action);
    /** Returns the action for a long press on side button 3. */
    virtual ButtonAction sideButton3Long() const;
    /** Sets the action for a long press on side button 3. */
    virtual void setSideButton3Long(ButtonAction action);

    /** Returns the action for a short press on programmable button 1. */
    virtual ButtonAction progButton1Short() const;
    /** Sets the action for a short press on programmable button 1. */
    virtual void setProgButton1Short(ButtonAction action);
    /** Returns the action for a long press on programmable button 1. */
    virtual ButtonAction progButton1Long() const;
    /** Sets the action for a long press on programmable button 1. */
    virtual void setProgButton1Long(ButtonAction action);

    /** Returns the action for a short press on programmable button 2. */
    virtual ButtonAction progButton2Short() const;
    /** Sets the action for a short press on programmable button 2. */
    virtual void setProgButton2Short(ButtonAction action);
    /** Returns the action for a long press on programmable button 2. */
    virtual ButtonAction progButton2Long() const;
    /** Sets the action for a long press on programmable button 2. */
    virtual void setProgButton2Long(ButtonAction action);

    /** Encodes the button settings. */
    virtual bool fromConfig(const Config *config);
    /** Updates config from button settings. */
    virtual bool updateConfig(Config *config);
  };

  /** Extended zone data for the DM-1701.
   * The zone definition @c ZoneElement contains only a single set of 16 channels. For each zone
   * definition, there is a zone extension which extends a zone to zwo sets of 64 channels each.
   *
   * Memory layout of encoded zone extension:
   * @verbinclude dm1701_zoneext.txt */
  class ZoneExtElement: public Codeplug::Element
  {
  protected:
    /** Constructor. */
    ZoneExtElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ZoneExtElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ZoneExtElement();

    void clear();

    /** Returns the n-th member index of the channel list for A. */
    virtual uint16_t memberIndexA(unsigned n) const;
    /** Sets the n-th member index of the channel list for A. */
    virtual void setMemberIndexA(unsigned n, uint16_t idx);
    /** Returns the n-th member index of the channel list for B. */
    virtual uint16_t memberIndexB(unsigned n) const;
    /** Returns the n-th member index of the channel list for B. */
    virtual void setMemberIndexB(unsigned n, uint16_t idx);

    /** Encodes the given zone. */
    virtual bool fromZoneObj(const Zone *zone, Context &ctx);
    /** Links the given zone object.
     * Thant is, extends channel list A and populates channel list B. */
    virtual bool linkZoneObj(Zone *zone, Context &ctx);
  };

public:
  /** Constructor. */
  explicit DM1701Codeplug(QObject *parent = nullptr);
  /** Destructor. */
  virtual ~DM1701Codeplug();

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

  void clearButtonSettings();
  bool encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeButtonSetttings(Config *config, const ErrorStack &err=ErrorStack());

  void clearPositioningSystems();
  bool encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err);
  bool createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err);
  bool linkPositioningSystems(Context &ctx, const ErrorStack &err);

  void clearMenuSettings();
  void clearTextMessages();
  void clearPrivacyKeys();
  void clearEmergencySystems();
  /** Resets VFO settings. */
  virtual void clearVFOSettings();

};

#endif // DM1701_CODEPLUG_HH
