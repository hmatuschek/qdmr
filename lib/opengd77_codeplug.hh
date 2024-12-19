#ifndef OPENGD77_CODEPLUG_HH
#define OPENGD77_CODEPLUG_HH

#include "opengd77base_codeplug.hh"
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
class OpenGD77Codeplug: public OpenGD77BaseCodeplug
{
  Q_OBJECT

public:
  /** Possible image types. */
  enum ImageType { EEPROM = 0, FLASH = 1 };

public:
  /** Constructs an empty codeplug for the GD-77. */
  explicit OpenGD77Codeplug(QObject *parent=nullptr);

public:
  void clearGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearDTMFSettings();
  bool encodeDTMFSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeDTMFSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearAPRSSettings();
  bool encodeAPRSSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeAPRSSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearContacts();
  bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearDTMFContacts();
  bool encodeDTMFContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createDTMFContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearChannels();
  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearBootSettings();
  bool encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeBootSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearVFOSettings();

  void clearZones();
  bool encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createZones(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearGroupLists();
  bool encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

public:
  /** Some Limits for this codeplug. */
  struct Limit: public Element::Limit {
    /** Number of channel banks. */
    static constexpr unsigned int channelBanks() { return 8; }
  };

protected:
  /** Internal used image indices. */
  struct ImageIndex {
    /// @cond DO_NOT_DOCUEMNT
    static constexpr unsigned int settings()     { return EEPROM; }
    static constexpr unsigned int dtmfSettings() { return EEPROM; }
    static constexpr unsigned int aprsSettings() { return EEPROM; }
    static constexpr unsigned int dtmfContacts() { return EEPROM; }
    static constexpr unsigned int channelBank0() { return EEPROM; }
    static constexpr unsigned int bootSettings() { return EEPROM; }
    static constexpr unsigned int vfoA()         { return EEPROM; }
    static constexpr unsigned int vfoB()         { return EEPROM; }
    static constexpr unsigned int zoneBank()     { return EEPROM; }
    static constexpr unsigned int channelBank1() { return FLASH; }
    static constexpr unsigned int contacts()     { return FLASH; }
    static constexpr unsigned int groupLists()   { return FLASH; }
    /// @endcond
  };

  /** Some offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUEMNT
    static constexpr unsigned int settings()     { return 0x000080; }
    static constexpr unsigned int dtmfSettings() { return 0x001470; }
    static constexpr unsigned int aprsSettings() { return 0x001588; }
    static constexpr unsigned int dtmfContacts() { return 0x002f88; }
    static constexpr unsigned int channelBank0() { return 0x003780; } // Channels 1-128
    static constexpr unsigned int bootSettings() { return 0x007518; }
    static constexpr unsigned int vfoA()         { return 0x007590; }
    static constexpr unsigned int vfoB()         { return 0x0075c8; }
    static constexpr unsigned int zoneBank()     { return 0x008010; }
    static constexpr unsigned int channelBank1() { return 0x07b1b0; } // Channels 129-1024
    static constexpr unsigned int contacts()     { return 0x087620; }
    static constexpr unsigned int groupLists()   { return 0x08d620; }
    /// @endcond
  };
};

#endif // OPENGD77_CODEPLUG_HH
