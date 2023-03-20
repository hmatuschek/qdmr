#ifndef D578UV_CODEPLUG_HH
#define D578UV_CODEPLUG_HH

#include <QDateTime>

#include "d878uv_codeplug.hh"
#include "signaling.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D578UV radios.
 *
 * @section d578uvcpl Codeplug structure within radio
 * <table>
 *  <tr><th colspan="3">Channels</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1500</td> <td>000200</td>      <td>Bitmap of 4000 channels, default 0x00, 0x00 padded.</td></tr>
 *  <tr><td>00800000</td> <td>max. 002000</td> <td>Channel bank 0 of up to 128 channels, see @c D578UVCodeplug::ChannelElement 64 b each. </td></tr>
 *  <tr><td>00802000</td> <td>max, 002000</td> <td>Unknown data, Maybe extended channel information for channel bank 0?
 *    It is of exactly the same size as the channel bank 0. Mostly 0x00, a few 0xff.</td></tr>
 *  <tr><td>00840000</td> <td>max. 002000</td> <td>Channel bank 1 of up to 128 channels.</td></tr>
 *  <tr><td>00842000</td> <td>max. 002000</td> <td>Unknown data, related to CH bank 1?</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>00FC0000</td> <td>max. 000800</td> <td>Channel bank 32, up to 32 channels.</td></tr>
 *  <tr><td>00FC2000</td> <td>max. 000800</td> <td>Unknown data, related to CH bank 32.</td></tr>
 *  <tr><td>00FC0800</td> <td>000040</td>      <td>VFO A settings, see @c D578UVCodeplug::ChannelElement.</td></tr>
 *  <tr><td>00FC0840</td> <td>000040</td>      <td>VFO B settings, see @c D578UVCodeplug::ChannelElement.</td></tr>
 *  <tr><td>00FC2800</td> <td>000080</td>      <td>Unknown data, related to VFO A+B?
 *    It is of exactly the same size as the two VFO channels. Mostly 0x00, a few 0xff. Same pattern as
 *    the unknown data associated with channel banks.</td></tr>
 *
 *  <tr><th colspan="3">Zones</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1300</td> <td>000020</td>      <td>Bitmap of 250 zones.</td></tr>
 *  <tr><td>024C1360</td> <td>000020</td>      <td>Hidden zone bitmap of 250 zones.</td></tr>
 *  <tr><td>01000000</td> <td>max. 01f400</td> <td>250 zones channel lists of 250 16bit indices each.
 *    0-based, little endian, default/padded=0xffff. Offset between channel lists 0x200, size of each list 0x1f4.</td></tr>
 *  <tr><td>02540000</td> <td>max. 001f40</td> <td>250 Zone names.
 *    Each zone name is up to 16 ASCII chars long and gets 0-padded to 32b.</td></tr>
 *
 *  <tr><th colspan="3">Roaming</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>01042000</td> <td>000020</td>      <td>Roaming channel bitmask, up to 250 bits, 0-padded, default 0.</td></tr>
 *  <tr><td>01040000</td> <td>max. 0x1f40</td> <td>Optional up to 250 roaming channels, of 32b each.
 *    See @c D878UVCodeplug::RoamingChannelElement for details.</td></tr>
 *  <tr><td>01042080</td> <td>000010</td>      <td>Roaming zone bitmask, up to 64 bits, 0-padded, default 0.</td></tr>
 *  <tr><td>01043000</td> <td>max. 0x2000</td> <td>Optional up to 64 roaming zones, of 128b each.
 *    See @c D878UVCodeplug::RoamingZoneElement for details.</td></tr>
 *
 *  <tr><th colspan="3">Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02600000</td> <td>max. 009C40</td> <td>Index list of valid contacts.
 *    10000 32bit indices, little endian, default 0xffffffff</td></tr>
 *  <tr><td>02640000</td> <td>000500</td>      <td>Contact bitmap, 10000 bit, inverted, default 0xff, 0x00 padded.</td></tr>
 *  <tr><td>02680000</td> <td>max. 0f4240</td> <td>10000 contacts, see @c D868UVCodeplug::ContactElement.
 *    As each contact is 100b, they do not align with the 16b blocks being transferred to the device.
 *    Hence contacts are organized internally in groups of 4 contacts forming a "bank". </td></tr>
 *  <tr><td>04800000</td> <td>max. 013880</td> <td>DMR ID to contact index map, see @c AnytoneCodeplug::ContactMapElement.
 *    Sorted by ID, empty entries set to 0xffffffffffffffff.</td>
 *
 *  <tr><th colspan="3">Analog Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02900000</td> <td>000080</td>      <td>Index list of valid analog contacts.</td></tr>
 *  <tr><td>02900100</td> <td>000080</td>      <td>Bytemap for 128 analog contacts.</td></tr>
 *  <tr><td>02940000</td> <td>max. 000180</td> <td>128 analog contacts. See @c AnytoneCodeplug::DTMFContactElement.
 *    As each analog contact is 24b, they do not align with the 16b transfer block-size. Hence
 *    analog contacts are internally organized in groups of 2. </td></tr>
 *
 *  <tr><th colspan="3">RX Group Lists</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>025C0B10</td> <td>000020</td>      <td>Bitmap of 250 RX group lists, default/padding 0x00.</td></tr>
 *  <tr><td>02980000</td> <td>max. 000120</td> <td>Grouplist 0, see @c AnytoneCodeplug::GroupListElement.</td></tr>
 *  <tr><td>02980200</td> <td>max. 000120</td> <td>Grouplist 1</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>0299f200</td> <td>max. 000120</td> <td>Grouplist 250</td></tr>
 *
 *  <tr><th colspan="3">Scan lists</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1340</td> <td>000020</td> <td>Bitmap of 250 scan lists.</td></tr>
 *  <tr><td>01080000</td> <td>000090</td> <td>Bank 0, Scanlist 1, see @c AnytoneCodeplug::ScanListElement. </td></tr>
 *  <tr><td>01080200</td> <td>000090</td> <td>Bank 0, Scanlist 2</td></tr>
 *  <tr><td>...</td>      <td>...</td>    <td>...</td></tr>
 *  <tr><td>01081E00</td> <td>000090</td> <td>Bank 0, Scanlist 16</td></tr>
 *  <tr><td>010C0000</td> <td>000090</td> <td>Bank 1, Scanlist 17</td></tr>
 *  <tr><td>...</td>      <td>...</td>    <td>...</td></tr>
 *  <tr><td>01440000</td> <td>000090</td> <td>Bank 15, Scanlist 241</td></tr>
 *  <tr><td>...</td>      <td>...</td>    <td>...</td></tr>
 *  <tr><td>01441400</td> <td>000090</td> <td>Bank 15, Scanlist 250</td></tr>
 *
 *  <tr><th colspan="3">Radio IDs</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1320</td> <td>000020</td>      <td>Bitmap of 250 radio IDs.</td></tr>
 *  <tr><td>02580000</td> <td>max. 001f40</td> <td>250 Radio IDs. See @c AnytoneCodeplug::RadioIDElement.</td></tr>
 *
 *  <tr><th colspan="3">GPS/APRS</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02501000</td> <td>000040</td> <td>APRS settings, see @c D878UVCodeplug::AnalogAPRSSettingsElement.</td>
 *  <tr><td>02501040</td> <td>000060</td> <td>APRS settings, see @c D878UVCodeplug::DMRAPRSSystemsElement.</td>
 *  <tr><td>025010A0</td> <td>000060</td> <td>Extended APRS settings, see @c D878UVCodeplug::AnalogAPRSSettingsExtensionElement.</tr>
 *  <tr><td>02501200</td> <td>000040</td> <td>APRS Text, up to 60 chars ASCII, 0-padded.</td>
 *  <tr><td>02501800</td> <td>000100</td> <td>APRS-RX settings list up to 32 entries, 8b each.
 *    See @c D878UVCodeplug::AnalogAPRSRXEntryElement.</td></tr>
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>0000f0</td> <td>General settings, see @c D578UVCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>02500100</td> <td>000400</td> <td>Zone A & B channel list.</td></tr>
 *  <tr><td>02500500</td> <td>000100</td> <td>DTMF list</td></tr>
 *  <tr><td>02500600</td> <td>000030</td> <td>Power on settings, see @c AnytoneCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>02501280</td> <td>000030</td> <td>DMR ARPS (GPS) messages, see @c D878UVCodeplug::GPSMessageElement.</td></tr>
 *  <tr><td>02501400</td> <td>000200</td> <td>General settings extension 2, see @c D878UVCodeplug::GeneralSettingsExtensionElement.</td></tr>
 *  <tr><td>024C2000</td> <td>0003F0</td> <td>List of 250 auto-repeater offset frequencies.
 *    32bit little endian frequency in 10Hz. I.e., 600kHz = 60000. Default 0x00000000, 0x00 padded.</td></tr>
 *
 *  <tr><th colspan="3">Messages</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>01640000</td> <td>max. 000100</td> <td>Some kind of linked list of messages.
 *    See @c AnytoneCodeplug::MessageListElement. Each entry has a size of 0x10.</td></tr>
 *  <tr><td>01640800</td> <td>000090</td>      <td>Bytemap of up to 100 valid messages.
 *    0x00=valid, 0xff=invalid, remaining 46b set to 0x00.</td></tr>
 *  <tr><td>02140000</td> <td>max. 000800</td> <td>Bank 0, Messages 1-8.
 *    Each message consumes 0x100b. See @c AnytoneCodeplug::MessageElement. </td></tr>
 *  <tr><td>02180000</td> <td>max. 000800</td> <td>Bank 1, Messages 9-16</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>02440000</td> <td>max. 000800</td> <td>Bank 12, Messages 97-100</td></tr>
 *
 *  <tr><th colspan="3">Hot Keys</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>025C0000</td> <td>000100</td> <td>4 analog quick-call settings.
 *    See @c AnytoneCodeplug::AnalogQuickCallElement.</td>
 *  <tr><td>025C0B00</td> <td>000010</td> <td>Status message bitmap.</td>
 *  <tr><td>025C0100</td> <td>000400</td> <td>Up to 32 status messages.
 *    Length unknown, offset 0x20. ASCII 0x00 terminated and padded.</td>
 *  <tr><td>025C0500</td> <td>000470</td> <td>24 hot-key settings,
 *    see @c AnytoneCodeplug::HotKeyElement</td></tr>
 *
 *  <tr><th colspan="3">Encryption keys</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1700</td> <td>000040</td> <td>32 Encryption IDs, 0-based, 16bit big-endian.</td></tr>
 *  <tr><td>024C1800</td> <td>000500</td> <td>32 DMR-Encryption keys,
 *    see @c D868UVCodeplug::dmr_encryption_key_t,
 *    40b each.</td></tr>
 *  <tr><td>024C4000</td> <td>004000</td> <td>Up to 256 AES encryption keys.
 *    See @c D878UVCodeplug::AESEncryptionKeyElement.</td></tr>
 *
 *  <tr><th colspan="3">Misc</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1400</td> <td>000020</td> <td>Alarm setting,
 *    see @c AnytoneCodeplug::AlarmSettingElement.</td></tr>
 *  <tr><td>024C1440</td> <td>000030</td> <td>Digital alarm settings extension,
 *    see @c AnytoneCodeplug::DigitalAlarmExtensionElement. </td></tr>
 *
 *  <tr><th colspan="3">FM Broadcast</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02480210</td> <td>000020</td>      <td>Bitmap of 100 FM broadcast channels.</td></tr>
 *  <tr><td>02480000</td> <td>max. 000200</td> <td>100 FM broadcast channels. Encoded
 *    as 8-digit BCD little-endian in 100Hz. Filled with 0x00.</td></tr>
 *  <tr><td>02480200</td> <td>000010</td>      <td>FM broadcast VFO frequency. Encoded
 *    as 8-digit BCD little-endian in 100Hz. Filled with 0x00.</td></tr>
 *
 *  <tr><th colspan="3">DTMF, 2-tone & 5-tone signaling.</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C0C80</td> <td>000010</td> <td>5-tone encoding bitmap.</td></tr>
 *  <tr><td>024C0000</td> <td>000020</td> <td>5-tone encoding.</td></tr>
 *  <tr><td>024C0D00</td> <td>000200</td> <td>5-tone ID list.</td></tr>
 *  <tr><td>024C1000</td> <td>000080</td> <td>5-tone settings.</td></tr>
 *  <tr><td>024C1080</td> <td>000050</td> <td>DTMF settings.</td></tr>
 *  <tr><td>024C1280</td> <td>000010</td> <td>2-tone encoding bitmap.</td></tr>
 *  <tr><td>024C1100</td> <td>000010</td> <td>2-tone encoding.</td></tr>
 *  <tr><td>024C1290</td> <td>000010</td> <td>2-tone settings.</td></tr>
 *  <tr><td>024C2600</td> <td>000010</td> <td>2-tone decoding bitmap.</td></tr>
 *  <tr><td>024C2400</td> <td>000030</td> <td>2-tone decoding.</td></tr>
 *
 *  <tr><th colspan="3">Air-band settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02BC0000</td> <td>000c80</td> <td>Up to 100 air band channels, 0x20 bytes each. See
 *      @c AirBandChannelElement.</td></tr>
 *  <tr><td>02BC1000</td> <td>000020</td> <td>Air band VFO channel, see
 *      @c AirBandChannelElement.</td></tr>
 *  <tr><td>02BC1020</td> <td>000020</td> <td>Air band channel bitmap</td></tr>
 *  <tr><td>02BC1040</td> <td>000020</td> <td>Air band scan enable bitmap</td></tr>
 *
 *  <tr><th colspan="3">Still unknown</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1090</td> <td>000040</td> <td>Unknown, set to 0xff</td></tr>
 *  <tr><td>024C1440</td> <td>000030</td> <td>Unknown data.</td></tr>
 *  <tr><td>02504000</td> <td>000400</td> <td>Unknown data.</td></tr>
 * </table>
 *
 * @ingroup d578uv */
class D578UVCodeplug : public D878UVCodeplug
{
  Q_OBJECT

public:
  /** Represents the actual channel encoded within the binary code-plug.
   *
   * Memory layout of encoded channel (size 0x40 bytes):
   * @verbinclude d578uv_channel.txt
   */
  class ChannelElement: public D878UVCodeplug::ChannelElement
  {
  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns @c true if bluetooth hands-free is enabled. */
    virtual bool handsFree() const;
    /** Enables/disables hands-free. */
    virtual void enableHandsFree(bool enable);

    // moved to a different bit
    bool roamingEnabled() const;
    void enableRoaming(bool enable);
    // moved to a different bit
    /** Returns @c true if the data ACK is enabled. */
    bool dataACK() const;
    /** Enables data ACK. */
    void enableDataACK(bool enable);

    // Replaced by analog scrambler settings
    /** The D578UV does not support the weak DMR encryption, hence this function returns always
     * 0. */
    unsigned dmrEncryptionKeyIndex() const;
    /** The D578UV does not support the weak DMR encryption, hence this function has not effect. */
    void setDMREncryptionKeyIndex(unsigned idx);

    /** Returns @c true if the analog scambler is enabled. */
    virtual bool analogScambler() const;
    /** Enables/disables the analog scambler. */
    virtual void enableAnalogScamber(bool enable);

    Channel *toChannelObj(Context &ctx) const;
  };

  /** Represents the general config of the radio within the D578UV binary codeplug.
   *
   * This class implements only the differences to the D878UV general settings
   * @c D878UVCodeplug::GeneralSettingsElement.
   *
   * Binary encoding of the general settings (size 0x00f0 bytes):
   * @verbinclude d578uv_generalsettings.txt */
  class GeneralSettingsElement: public D878UVCodeplug::GeneralSettingsElement
  {
  protected:
    /** Device specific key functions. */
    enum class KeyFunction {
      Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
      Encryption = 0x05, Call = 0x06, ToggleVFO = 0x07, Scan = 0x08, WFM = 0x09, Alarm = 0x0a,
      RecordSwitch = 0x0b, Record = 0x0c, SMS = 0x0d, Dial = 0x0e, Monitor = 0x10,
      ToggleMainChannel = 0x11, HotKey1 = 0x12, HotKey2 = 0x13, HotKey3 = 0x14, HotKey4 = 0x15,
      HotKey5 = 0x16, HotKey6 = 0x17, WorkAlone = 0x18, SkipChannel = 0x19, DMRMonitor = 0x1a,
      SubChannel = 0x1b, PriorityZone = 0x1c, VFOScan = 0x1d, MICSoundQuality = 0x1e,
      LastCallReply = 0x1f, ChannelType = 0x20, Roaming = 0x22, MaxVolume = 0x24, Slot = 0x25,
      Zone = 0x26, MuteA = 0x27, MuteB = 0x28, RoamingSet = 0x2a, APRSSet = 0x2b, ZoneUp = 0x2c,
      ZoneDown = 0x2d, XBandRepeater = 0x30, Speaker = 0x31, ChannelName = 0x32, Bluetooth = 0x33,
      GPS = 0x34, CDTScan = 0x35, TBSTSend = 0x36, APRSSend = 0x37, APRSInfo = 0x38,
      GPSRoaming = 0x39
    };

    AnytoneKeySettingsExtension::KeyFunction mapCodeToKeyFunction(uint8_t code) const;
    uint8_t mapKeyFunctionToCode(AnytoneKeySettingsExtension::KeyFunction func) const;

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);
};

  /** Represents the hot-key settings of the radio within the D578UV binary codeplug.
   *
   * This class extends the common @c AnytoneCodeplug::HotKeySettings element, encoding 24 instead
   * of 17 @c HotKeySettingsElement.
   *
   * Memory layout of the hot-key settings (size 0x0370 bytes):
   * @verbinclude d578uv_hotkeysettings.txt */
  class HotKeySettingsElement: public AnytoneCodeplug::HotKeySettingsElement
  {
  protected:
    /** Hidden constructor. */
    HotKeySettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    HotKeySettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0970; }

    uint8_t *hotKeySetting(unsigned int n) const;

  public:
    /** Some limits for this element. */
    struct Limit {
      static constexpr unsigned int numEntries() { return 24; }   ///< Maximum number of hot-key entries.
    };

  };

  /** Implements the air-band receiver channel.
   *
   * Memory layout of the air-band channel list (size 0x0020 bytes):
   * @verbinclude d578uv_airbandchannel.txt */
  class AirBandChannelElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AirBandChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AirBandChannelElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }

    void clear();

    /** The channel frequency. */
    virtual Frequency frequency() const;
    /** Sets the channel frequency. */
    virtual void setFrequency(Frequency freq);

    /** The name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &name);

  public:
    /** Some limits of the channel. */
    struct Limit {
      static constexpr unsigned int nameLength() { return 16; }     ///< Maximum name length.
    };

  public:
    /** Internal used offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int frequency() { return 0x0000; }
      static constexpr unsigned int name()      { return 0x0004; }
      /// @endcond
    };
  };

  /** Represents the bitmap indicating which channels are valid and which are included in the
   *  air-band scan. */
  class AirBandBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    AirBandBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AirBandBitmapElement(uint8_t *ptr);

    /** The element size. */
    static constexpr unsigned int size() { return 0x0020; }
  };

protected:
  /** Hidden constructor. */
  explicit D578UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D578UVCodeplug(QObject *parent = nullptr);

protected:
  bool allocateBitmaps();

  void allocateUpdated();

  void allocateHotKeySettings();

  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateContacts();
  bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates the air-band channels und VFO settings. */
  virtual void allocateAirBand();

public:
  /** Some limtis for the codeplug. */
  struct Limit: D878UVCodeplug::Limit {
    static constexpr unsigned int airBandChannels() { return 100; }     ///< Maximum number of air-band channels.
  };

protected:
  /** Internal used offsets within the codeplug. */
  struct Offset: D878UVCodeplug::Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int contactIdTable()       { return 0x04800000; }
    static constexpr unsigned int settings()             { return 0x02500000; }
    static constexpr unsigned int gpsMessages()          { return 0x02501280; }
    static constexpr unsigned int settingsExtension()    { return 0x02501400; }
    static constexpr unsigned int airBandChannels()      { return 0x02BC0000; }
    static constexpr unsigned int airBandVFO()           { return 0x02BC1000; }
    static constexpr unsigned int airBandChannelBitmap() { return 0x02BC1020; }
    static constexpr unsigned int airBandScanBitmap()    { return 0x02BC1040; }
    /// @endcond
  };
};

#endif // D578UV_CODEPLUG_HH
