#ifndef OPENGD77_CODEPLUG_HH
#define OPENGD77_CODEPLUG_HH

#include "gd77_codeplug.hh"

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
 *  <tr><td>0x000e0</td> <td>0x000ec</td> <td>0x000c</td> <td>General settings, see @c GD77Codeplug::general_settings_t.</td></tr>
 *  <tr><td>0x000ec</td> <td>0x00128</td> <td>0x003c</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 message texts, see @c GD77Codeplug::msgtab_t</td></tr>
 *  <tr><td>0x01370</td> <td>0x01790</td> <td>0x0420</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01790</td> <td>0x02dd0</td> <td>0x1640</td> <td>64 scan lists, see @c GD77Codeplug::scanlist_t</td></tr>
 *  <tr><td>0x02dd0</td> <td>0x03780</td> <td>0x09b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 chanels (bank 0), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><td>0x05390</td> <td>0x06000</td> <td>0x0c70</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second EEPROM segment 0x07500-0x13000</th></tr>
 *  <tr><td>0x07500</td> <td>0x07540</td> <td>0x0040</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c GD77Codeplug::intro_text_t</td></tr>
 *  <tr><td>0x07560</td> <td>0x08010</td> <td>0x0ab0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x12c10</td> <td>0xac00</td> <td>250 zones, see @c OpenGD77Codeplug::zonetab_t</td></tr>
 *  <tr><td>0x12c10</td> <td>0x13000</td> <td>0x03f0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">First Flash segment 0x00000-0x011a0</th></tr>
 *  <tr><td>0x00000</td> <td>0x011a0</td> <td>0x11a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second Flash segment 0x7b000-0x8ee60</th></tr>
 *  <tr><td>0x7b000</td> <td>0x7b1b0</td> <td>0x01b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x7b1b0</td> <td>0x87620</td> <td>0xc470</td> <td>Remaining 896 chanels (bank 1-7), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><td>0x87620</td> <td>0x8d620</td> <td>0x6000</td> <td>1024 contacts, see @c GD77Codeplug::contact_t.</td></tr>
 *  <tr><td>0x8d620</td> <td>0x8e2a0</td> <td>0x0c80</td> <td>76 RX group lists, see @c GD77Codeplug::grouptab_t</td></tr>
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
    /** All possible power settings. */
    enum class Power {
      Global  =  0,              ///< Use global power setting.
      P50mW   =  1,              ///< About 50mW.
      P250mW  =  2,              ///< About 250mW.
      P500mW  =  3,              ///< About 500mW.
      P750mW  =  4,              ///< About 750mW.
      P1W     =  5,              ///< About 1W.
      P2W     =  6,              ///< About 2W.
      P3W     =  7,              ///< About 3W.
      P4W     =  8,              ///< About 4W.
      P5W     =  9,              ///< About 5W.
      Max     = 10,              ///< Maximum power (5.5W on UHF, 7W on VHF).
    };

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    void clear();

    Channel::Power power() const;
    void setPower(Channel::Power power);

    /** Returns extended power settings. */
    virtual Power extendedPower() const;
    /** Sets extended power. */
    virtual void setExtendedPower(Power power);

    Channel *toChannelObj(Context &ctx) const;
    bool fromChannelObj(const Channel *c, Context &ctx);
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

    uint8_t *get(uint n) const;
  };

  /** Implements the OpenGD77 specific DMR contact.
   *
   * Encoding of the contact:
   * @verbinclude opengd77_contact.txt
   */
  class ContactElement: public GD77Codeplug::ContactElement
  {
  public:
    /** Possible values for the time-slot override option. */
    enum class TimeSlotOverride {
      None = 0x01,                  ///< Do not override time-slot of channel.
      TS1  = 0x00,                  ///< Force time-slot to TS1.
      TS2  = 0x02                   ///< Force time-slot to TS2.
    };

  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);

    /** Resets the contact. */
    void clear();

    /** Returns @c true if this contact overrides the channel time slot. */
    virtual bool overridesTimeSlot() const;
    /** Returns the time slot associated with the contact.
     * Only valid if @c overridesTimeSlot returns @c true. */
    virtual DigitalChannel::TimeSlot timeSlot() const;
    /** Associates the given time slot with this contact.
     * This will cause the use this time slot whenever this contact is chosen as the TX contact.
     * Irrespective of the selected time slot of the channel. */
    virtual void setTimeSlot(DigitalChannel::TimeSlot ts);
    /** Disables time slot override feature. */
    virtual void disableTimeSlotOverride();
  };

public:
  /** Constructs an empty codeplug for the GD-77. */
  explicit OpenGD77Codeplug(QObject *parent=nullptr);

public:
  void clearGeneralSettings();
  bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx);
  bool decodeGeneralSettings(Config *config, Context &ctx);

  void clearButtonSettings();
  void clearMessages();

  void clearScanLists();
  bool encodeScanLists(Config *config, const Flags &flags, Context &ctx);
  bool createScanLists(Config *config, Context &ctx);
  bool linkScanLists(Config *config, Context &ctx);

  void clearContacts();
  bool encodeContacts(Config *config, const Flags &flags, Context &ctx);
  bool createContacts(Config *config, Context &ctx);

  void clearDTMFContacts();
  bool encodeDTMFContacts(Config *config, const Flags &flags, Context &ctx);
  bool createDTMFContacts(Config *config, Context &ctx);

  void clearChannels();
  bool encodeChannels(Config *config, const Flags &flags, Context &ctx);
  bool createChannels(Config *config, Context &ctx);
  bool linkChannels(Config *config, Context &ctx);

  void clearBootSettings();
  void clearMenuSettings();

  void clearBootText();
  bool encodeBootText(Config *config, const Flags &flags, Context &ctx);
  bool decodeBootText(Config *config, Context &ctx);

  void clearVFOSettings();

  void clearZones();
  bool encodeZones(Config *config, const Flags &flags, Context &ctx);
  bool createZones(Config *config, Context &ctx);
  bool linkZones(Config *config, Context &ctx);

  void clearGroupLists();
  bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx);
  bool createGroupLists(Config *config, Context &ctx);
  bool linkGroupLists(Config *config, Context &ctx);
};

#endif // OPENGD77_CODEPLUG_HH
