#ifndef OPENGD77_CODEPLUG_HH
#define OPENGD77_CODEPLUG_HH

#include "gd77_codeplug.hh"
#include "opengd77_extension.hh"


/** Represents, encodes and decodes the device specific codeplug for Open GD-77 firmware.
 * This codeplug is almost identical to the original GD77 codeplug.
 *
 * @section ogd77cpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is divided into two images
 * (EEPROM and Flash) and each image again into two sections.

 * The first segment of the EEPROM image starts at the address 0x000e0 and ends at 0x06000, while
 * the second EEPROM section starts at 0x07500 and ends at 0x0b000.
 *
 * The first segment of the Flash image starts at the address 0x00000 and ends at 0x011a0, while the
 * second Flash section starts at 0x7b000 and ends at 0x8ee60.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First EEPROM segment 0x000e0-0x06000</th></tr>
 *  <tr><td>0x000e0</td> <td>0x000ec</td> <td>0x000c</td> <td>General settings, see @c RadioddityCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>0x000ec</td> <td>0x00128</td> <td>0x003c</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 message texts, see @c RadioddityCodeplug::MessageBankElement.</td></tr>
 *  <tr><td>0x01370</td> <td>0x01790</td> <td>0x0420</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01790</td> <td>0x02dd0</td> <td>0x1640</td> <td>64 scan lists, see @c GD77Codeplug::ScanListBankElement, GD77Codeplug::ScanListElement.</td></tr>
 *  <tr><td>0x02dd0</td> <td>0x02f88</td> <td>0x01b8</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x02f88</td> <td>0x03388</td> <td>0x0400</td> <td>DTMF contacts, see RadioddityCodeplug::DTMFContactElement.</td></tr>
 *  <tr><td>0x03388</td> <td>0x03780</td> <td>0x03f8</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 channels (bank 0), see @c RadioddityCodeplug::ChannelBankElement, @c OpenGD77Codeplug::ChannelElement.</td></tr>
 *  <tr><td>0x05390</td> <td>0x06000</td> <td>0x0c70</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second EEPROM segment 0x07500-0x13000</th></tr>
 *  <tr><td>0x07500</td> <td>0x07518</td> <td>0x0018</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07518</td> <td>0x07538</td> <td>0x0020</td> <td>Boot settings, see @c RadioddityCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>0x07538</td> <td>0x07540</td> <td>0x0008</td> <td>Menu settings, see @c RadioddityCodeplug::MenuSettingsElement.</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c RadioddityCodeplug::BootTextElement.</td></tr>
 *  <tr><td>0x07560</td> <td>0x07590</td> <td>0x0030</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07590</td> <td>0x075c8</td> <td>0x0038</td> <td>VFO A settings @c OpenGD77Codeplug::VFOChannelElement</td></tr>
 *  <tr><td>0x075c8</td> <td>0x07600</td> <td>0x0038</td> <td>VFO B settings @c OpenGD77Codeplug::VFOChannelElement</td></tr>
 *  <tr><td>0x07600</td> <td>0x08010</td> <td>0x0a10</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x12c10</td> <td>0xac00</td> <td>250 zones, see @c OpenGD77Codeplug::ZoneBankElement, @c OpenGD77Codeplug::ZoneElement.</td></tr>
 *  <tr><td>0x12c10</td> <td>0x13000</td> <td>0x03f0</td> <td>??? Unknown ???</td></tr>
 *
 *  <tr><th colspan="4">First Flash segment 0x00000-0x011a0</th></tr>
 *  <tr><td>0x00000</td> <td>0x011a0</td> <td>0x11a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second Flash segment 0x7b000-0x8ee60</th></tr>
 *  <tr><td>0x7b000</td> <td>0x7b1b0</td> <td>0x01b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x7b1b0</td> <td>0x87620</td> <td>0xc470</td> <td>Remaining 896 channels (bank 1-7), see @c RadioddityCodeplug::ChannelBankElement and @c OpenGD77Codeplug::ChannelElement.</td></tr>
 *  <tr><td>0x87620</td> <td>0x8d620</td> <td>0x6000</td> <td>1024 contacts, see @c OpenGD77Codeplug::ContactElement.</td></tr>
 *  <tr><td>0x8d620</td> <td>0x8e2a0</td> <td>0x0c80</td> <td>76 RX group lists, see @c GD77Codeplug::GroupListBankElement, @c GD77Codeplug::GroupListElement.</td></tr>
 *  <tr><td>0x8e2a0</td> <td>0x8ee60</td> <td>0x0bc0</td> <td>??? Unknown ???</td></tr>
 * </table>
 * @ingroup ogd77 */
class OpenGD77Codeplug: public GD77Codeplug
{
	Q_OBJECT

public:
  /** EEPROM memory bank. */
  static const uint32_t EEPROM = 0;
  /** Flash memory bank. */
  static const uint32_t FLASH  = 1;

public:
  /** Implements the OpenGD77 specific channel.
   *
   * Encoding of channel:
   * @verbinclude opengd77_channel.txt */
  class ChannelElement: public GD77Codeplug::ChannelElement
  {
  public:
    /** Reuse Power enum from extension. */
    typedef OpenGD77ChannelExtension::Power Power;

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    void clear();

    Channel::Power power() const;
    void setPower(Channel::Power power);

    /** If @c true, the scan zone skip is enabled. */
    virtual bool scanZoneSkip() const;
    /** Enables/disables scan zone skip. */
    virtual void enableScanZoneSkip(bool enable);
    /** If @c true, the scan all skip is enabled. */
    virtual bool scanAllSkip() const;
    /** Enables/disables scan all skip. */
    virtual void enableScanAllSkip(bool enable);

    /** Returns extended power settings. */
    virtual Power extendedPower() const;
    /** Sets extended power. */
    virtual void setExtendedPower(Power power);

    /** Returns @c true if the squelch is set to radio wide default. */
    virtual bool squelchIsDefault() const;
    /** Returns the quelch value [0-10]. 0=open, 10=closed. */
    virtual unsigned squelch() const;
    /** Sets the squelch value. */
    virtual void setSquelch(unsigned squelch);
    /** Sets the squelch to radio wide default. */
    virtual void setSquelchDefault();

    /** Returns @c true if the channel specific radio ID is set. */
    virtual bool hasRadioId() const;
    /** Returns the radio ID for the channel. Only valid if @c hasRadioId() returns @c true. */
    virtual unsigned radioId() const;
    /** Sets and enables the radio ID for the channel. */
    virtual void setRadioId(unsigned id);
    /** Clears the radio ID. */
    virtual void clearRadioId();

    Channel *toChannelObj(Context &ctx) const;
    bool linkChannelObj(Channel *c, Context &ctx) const;
    bool fromChannelObj(const Channel *c, Context &ctx);

    /* Reused fields in OpenGD77.
     * The following properties are reused in the OpenGD77 firmware for other purposes.*/
    /** Overridden, reused in OpenGD77. */
    bool autoscan() const;
    /** Overridden, reused in OpenGD77. */
    void enableAutoscan(bool enable);
    /** Overridden, reused in OpenGD77. */
    bool loneWorker() const;
    /** Overridden, reused in OpenGD77. */
    void enableLoneWorker(bool enable);
    /** Overridden, reused in OpenGD77. */
    unsigned rxSignalingIndex() const;
    /** Overridden, reused in OpenGD77. */
    void setRXSignalingIndex(unsigned idx);
    /** Overridden, reused in OpenGD77. */
    PrivacyGroup privacyGroup() const ;
    /** Overridden, reused in OpenGD77. */
    void setPrivacyGroup(PrivacyGroup grp);
  };

  /** Implements the OpenGD77 specific zone.
   *
   * Encoding of zone:
   * @verbinclude opengd77_zone.txt */
  class ZoneElement: public RadioddityCodeplug::ZoneElement
  {
  public:
    /** Constructor. */
    explicit ZoneElement(uint8_t *ptr);

    /** Clears the zone. */
    void clear();
    bool linkZoneObj(Zone *zone, Context &ctx, bool putInB) const;
    void fromZoneObjA(const Zone *zone, Context &ctx);
    void fromZoneObjB(const Zone *zone, Context &ctx);
  };

  /** Implements the OpenGD77 specific zone bank.
   *
   * Encoding of the entire zone bank:
   * @verbinclude opengd77_zonebank.txt */
  class ZoneBankElement: public RadioddityCodeplug::ZoneBankElement
  {
  public:
    /** Constructor. */
    explicit ZoneBankElement(uint8_t *ptr);

    uint8_t *get(unsigned n) const;
  };

  /** Implements the OpenGD77 specific DMR contact.
   *
   * Encoding of the contact:
   * @verbinclude opengd77_contact.txt
   */
  class ContactElement: public GD77Codeplug::ContactElement
  {
  public:
    /** Reuse enum from extension. */
    typedef OpenGD77ContactExtension::TimeSlotOverride TimeSlotOverride;

    /** Holds some offsets within the element. */
    struct Offset {
      enum {
        TimeSlotOverride = 0x0017
      };
    };

  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);

    /** Resets the contact. */
    void clear();
    /** Returns @c true if the contact is valid. I.e., has a name. */
    bool isValid() const;
    void markValid(bool valid=true);

    /** Returns @c true if this contact overrides the channel time slot. */
    virtual bool overridesTimeSlot() const;
    /** Returns the time slot associated with the contact.
     * Only valid if @c overridesTimeSlot returns @c true. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Associates the given time slot with this contact.
     * This will cause the use this time slot whenever this contact is chosen as the TX contact.
     * Irrespective of the selected time slot of the channel. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);
    /** Disables time slot override feature. */
    virtual void disableTimeSlotOverride();

    DMRContact *toContactObj(Context &ctx) const;
    void fromContactObj(const DMRContact *c, Context &ctx);
  };

  /** Implements the OpenGD77 specific group list.
   *
   * This class is identical to the GD77 one, but allows for private calls to be added to
   * the group list. */
  class GroupListElement: public GD77Codeplug::GroupListElement
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    void fromRXGroupListObj(const RXGroupList *lst, Context &ctx);
  };

public:
  /** Constructs an empty codeplug for the GD-77. */
  explicit OpenGD77Codeplug(QObject *parent=nullptr);

public:
  void clearGeneralSettings();
  bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearButtonSettings();
  void clearMessages();

  void clearScanLists();
  bool encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearContacts();
  bool encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearDTMFContacts();
  bool encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createDTMFContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearChannels();
  bool encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearBootSettings();
  void clearMenuSettings();

  void clearBootText();
  bool encodeBootText(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeBootText(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearVFOSettings();

  void clearZones();
  bool encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearGroupLists();
  bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearEncryption();
  bool encodeEncryption(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err);
  bool createEncryption(Config *config, Context &ctx, const ErrorStack &err);
  bool linkEncryption(Config *config, Context &ctx, const ErrorStack &err);
};

#endif // OPENGD77_CODEPLUG_HH
