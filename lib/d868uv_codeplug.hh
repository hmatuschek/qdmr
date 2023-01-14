#ifndef D868UV_CODEPLUG_HH
#define D868UV_CODEPLUG_HH

#include <QDateTime>

#include "anytone_codeplug.hh"
#include "signaling.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D868UV radios.
 *
 * In contrast to many other code-plugs, the code-plug for Anytone radios are spread over a large
 * memory area. The amount of fragmentation of the codeplug is overwhelming.
 * For example, while channels are organized more or less nicely in continuous banks, zones are
 * distributed throughout the entire code-plug. That is, the names of zones are located in a
 * different memory section that the channel lists. Some lists are defined though bit-masks others
 * by byte-masks. All bit-masks are positive, that is 1 indicates an enabled item while the
 * bit-mask for contacts is inverted.
 *
 * In general the code-plug is huge and complex. Moreover, the radio provides a huge amount of
 * options and features. To this end, reverse-engineering this code-plug was a nightmare.
 *
 * More over, the binary code-plug file generate by the windows CPS does not directly relate to
 * the data being written to the radio. To this end the code-plug has been reverse-engineered
 * using wireshark to monitor the USB communication between the windows CPS (running in a virtual
 * box) and the device. The latter makes the reverse-engineering particularly cumbersome.
 *
 * @section d868uvcpl Codeplug structure within radio
 * <table>
 *  <tr><th colspan="3">Channels</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1500</td> <td>000200</td>      <td>Bitmap of 4000 channels, default 0x00, 0x00 padded.</td></tr>
 *  <tr><td>00800000</td> <td>max. 002000</td> <td>Channel bank 0 of up to 128 channels,
 *   see @c AnytoneCodeplug::ChannelElement 64 b each. </td></tr>
 *  <tr><td>00840000</td> <td>max. 002000</td> <td>Channel bank 1 of up to 128 channels.</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>00FC0000</td> <td>max. 000800</td> <td>Channel bank 32, up to 32 channels.</td></tr>
 *  <tr><td>00FC0800</td> <td>000040</td>      <td>VFO A settings,
 *    see @c AnytoneCodeplug::ChannelElement.</td></tr>
 *  <tr><td>00FC0840</td> <td>000040</td>      <td>VFO B settings,
 *    see @c AnytoneCodeplug::ChannelElement.</td></tr>
 *
 *  <tr><th colspan="3">Zones</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1300</td> <td>000020</td>      <td>Bitmap of 250 zones.</td></tr>
 *  <tr><td>01000000</td> <td>max. 01f400</td> <td>250 zones channel lists of 250 16bit indices each.
 *    0-based, little endian, default/padded=0xffff. Offset between channel lists 0x200, size of each list 0x1f4.</td></tr>
 *  <tr><td>02540000</td> <td>max. 001f40</td> <td>250 Zone names.
 *    Each zone name is up to 16 ASCII chars long and gets 0-padded to 32b.</td></tr>
 *
 *  <tr><th colspan="3">Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02600000</td> <td>max. 009C40</td> <td>Index list of valid contacts.
 *    10000 32bit indices, little endian, default 0xffffffff</td></tr>
 *  <tr><td>02640000</td> <td>000500</td>      <td>Contact bitmap, 10000 bit, inverted, default 0xff, 0x00 padded.</td></tr>
 *  <tr><td>02680000</td> <td>max. 0186a0</td> <td>Bank 1 of 1000 contacts,
 *    see @c AnytoneCodeplug::ContactElement. As each contact is 100b, they do not align with the
 *    16b blocks being transferred to the device. Hence contacts are organized internally in groups
 *    of 4 contacts. There are 10 banks, each containing 1000 contacts. The offset between banks is 0x40000. </td></tr>
 *  <tr><td>04340000</td> <td>max. 013880</td> <td>DMR ID to contact index map,
 *    see @c AnytoneCodeplug::ContactMapElement. Sorted by ID, empty entries set to
 *    @c 0xffffffffffffffff.</td>
 *
 *  <tr><th colspan="3">Analog Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02900000</td> <td>000080</td>      <td>Index list of valid analog contacts.</td></tr>
 *  <tr><td>02900100</td> <td>000080</td>      <td>Bytemap for 128 analog contacts.</td></tr>
 *  <tr><td>02940000</td> <td>max. 000180</td> <td>128 analog contacts.
 *    See @c AnytoneCodeplug::DTMFContactElement. As each analog contact is 24b, they do not align
 *    with the 16b transfer block-size. Hence analog contacts are internally organized in groups
 *    of 2. </td></tr>
 *
 *  <tr><th colspan="3">RX Group Lists</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>025C0B10</td> <td>000020</td>      <td>Bitmap of 250 RX group lists, default/padding 0x00.</td></tr>
 *  <tr><td>02980000</td> <td>max. 000120</td> <td>Grouplist 0,
 *    see @c AnytoneCodeplug::GroupListElement.</td></tr>
 *  <tr><td>02980200</td> <td>max. 000120</td> <td>Grouplist 1</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>0299f200</td> <td>max. 000120</td> <td>Grouplist 250</td></tr>
 *
 *  <tr><th colspan="3">Scan lists</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1340</td> <td>000020</td> <td>Bitmap of 250 scan lists.</td></tr>
 *  <tr><td>01080000</td> <td>000090</td> <td>Bank 0, Scanlist 1,
 *    see @c AnytoneCodeplug::ScanListElement. </td></tr>
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
 *  <tr><td>02580000</td> <td>max. 001f40</td> <td>250 Radio IDs.
 *    See @c AnytoneCodeplug::RadioIDElement.</td></tr>
 *
 *  <tr><th colspan="3">GPS</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02501000</td> <td>000030</td> <td>GPS settings,
 *    see @c AnytoneCodeplug::DMRAPRSSettingsElement.</td>
 *  <tr><td>02501100</td> <td>000030</td> <td>GPS message.</td>
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>0000D0</td> <td>General settings,
 *    see @c D868UVCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>02500100</td> <td>000400</td> <td>Zone A & B channel list.</td></tr>
 *  <tr><td>02500500</td> <td>000100</td> <td>DTMF list</td></tr>
 *  <tr><td>02500600</td> <td>000030</td> <td>Power on settings,
 *    see @c AnytoneCodeplug::BootSettingsElement.</td></tr>
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
 *   See @c AnytoneCodeplug::AnalogQuickCallElement.</td>
 *  <tr><td>025C0B00</td> <td>000010</td> <td>Status message bitmap.</td>
 *  <tr><td>025C0100</td> <td>000400</td> <td>Up to 32 status messages.
 *    Length unknown, offset 0x20. ASCII 0x00 terminated and padded.</td>
 *  <tr><td>025C0500</td> <td>000360</td> <td>18 hot-key settings,
 *    see @c AnytoneCodeplug::HotKeyElement</td></tr>
 *
 *  <tr><th colspan="3">Misc</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1400</td> <td>000020</td> <td>Alarm setting,
 *    see @c AnytoneCodeplug::AlarmSettingElement.</td></tr>
 *  <tr><td>024C1440</td> <td>000030</td> <td>Digital alarm settings extension,
 *   see @c AnytoneCodeplug::DigitalAlarmExtensionElement. </td></tr>
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
 *  <tr><td>024C0C80</td> <td>000010</td> <td>5-tone encoding bitmap for 100 IDs.</td></tr>
 *  <tr><td>024C0000</td> <td>000020</td> <td>List of 100 5-tone IDs,
 *    see @c AnytoneCodeplug::FiveToneIDElement</td></tr>
 *  <tr><td>024C0D00</td> <td>000200</td> <td>List of 16 5-tone functions,
 *    see @c AnytoneCodeplug::FiveToneFunctionElement.</td></tr>
 *  <tr><td>024C1000</td> <td>000080</td> <td>5-tone settings,
 *    see @c AnytoneCodeplug::FiveToneSettingsElement. </td></tr>
 *  <tr><td>024C1080</td> <td>000050</td> <td>DTMF settings,
 *    see @c AnytoneCodeplug::DTMFSettingsElement.</td></tr>
 *  <tr><td>024C1280</td> <td>000010</td> <td>2-tone encoding bitmap.</td></tr>
 *  <tr><td>024C1100</td> <td>000010</td> <td>2-tone encoding.</td></tr>
 *  <tr><td>024C1290</td> <td>000010</td> <td>2-tone settings.</td></tr>
 *  <tr><td>024C2600</td> <td>000010</td> <td>2-tone decoding bitmap.</td></tr>
 *  <tr><td>024C2400</td> <td>000030</td> <td>2-tone decoding.</td></tr>
 *
 *  <tr><th colspan="3">Encryption</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1700</td> <td>000040</td> <td>32 Encryption IDs, 0-based, 16bit big-endian.</td></tr>
 *  <tr><td>024C1800</td> <td>000500</td> <td>32 DMR-Encryption keys, see @c dmr_encryption_key_t,
 *    40b each.</td></tr>
 * </table>
 *
 * @ingroup d868uv */
class D868UVCodeplug : public AnytoneCodeplug
{
  Q_OBJECT

public:
  /** Represents the channel element for AnyTone D868UV devices.
   *  This class derives from @c AnytoneCodeplug::ChannelElement and implements the device-specific
   *  encoding of channels for the AnyTone D868UV.
   *
   *  Memory layout of the encoded channel element (size 0x0040 bytes):
   *  @verbinclude d868uv_channel.txt */
  class ChannelElement: public AnytoneCodeplug::ChannelElement
  {
  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns @c true if ranging is enabled. */
    virtual bool ranging() const;
    /** Enables/disables ranging. */
    virtual void enableRanging(bool enable);
    /** Returns @c true if through mode is enabled. */
    virtual bool throughMode() const;
    /** Enables/disables though mode. */
    virtual void enableThroughMode(bool enable);
    /** Returns @c true if data ACK is enabled. */
    virtual bool dataACK() const;
    /** Enables/disables data ACK. */
    virtual void enableDataACK(bool enable);

    /** Returns @c true if TX APRS is enabled. */
    virtual bool txDigitalAPRS() const;
    /** Enables/disables TX APRS. */
    virtual void enableTXDigitalAPRS(bool enable);
    /** Returns the DMR APRS system index. */
    virtual unsigned digitalAPRSSystemIndex() const;
    /** Sets the DMR APRS system index. */
    virtual void setDigitalAPRSSystemIndex(unsigned idx);

    /** Returns the DMR encryption key index (+1), 0=Off. */
    virtual unsigned dmrEncryptionKeyIndex() const;
    /** Sets the DMR encryption key index (+1), 0=Off. */
    virtual void setDMREncryptionKeyIndex(unsigned idx);
    /** Returns @c true if multiple key encryption is enabled. */
    virtual bool multipleKeyEncryption() const;
    /** Enables/disables multiple key encryption. */
    virtual void enableMultipleKeyEncryption(bool enable);
    /** Returns @c true if random key is enabled. */
    virtual bool randomKey() const;
    /** Enables/disables random key. */
    virtual void enableRandomKey(bool enable);
    /** Returns @c true if SMS is enabled. */
    virtual bool sms() const;
    /** Enables/disables SMS. */
    virtual void enableSMS(bool enable);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj(Context &ctx) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Represents the general config of the radio within the D868UV binary codeplug.
   *
   * This class only implements the differences to the generic
   * @c AnytoneCodeplug::GeneralSettingsElement.
   *
   * Memory layout of encoded general settings (size 0x00d0 bytes):
   * @verbinclude d868uv_generalsettings.txt
   */
  class GeneralSettingsElement: public AnytoneCodeplug::GeneralSettingsElement
  {
  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** Resets the general settings. */
    void clear();

    /** Returns the display color for callsigns. */
    virtual Color callDisplayColor() const;
    /** Sets the display color for callsigns. */
    virtual void setCallDisplayColor(Color color);
    /** Returns the GPS update period in seconds. */
    virtual unsigned gpsUpdatePeriod() const;
    /** Sets the GPS update period in seconds. */
    virtual void setGPSUpdatePeriod(unsigned sec);
    /** Returns @c true if the zone and contact are shown. */
    virtual bool showZoneAndContact() const;
    /** Enables/disables @c true the zone and contact display. */
    virtual void enableShowZoneAndContact(bool enable);
    /** Returns @c true if the key-tone level is adjustable. */
    virtual bool keyToneLevelAdjustable() const;
    /** Returns the key-tone level (0=adjustable). */
    virtual unsigned keyToneLevel() const;
    /** Sets the key-tone level. */
    virtual void setKeyToneLevel(unsigned level);
    /** Sets the key-tone level adjustable. */
    virtual void setKeyToneLevelAdjustable();
    /** Returns @c true if the GPS units are imperial. */
    virtual bool gpsUnitsImperial() const;
    /** Enables/disables imperial GPS units. */
    virtual void enableGPSUnitsImperial(bool enable);
    /** Returns @c true if the knob is locked. */
    virtual bool knobLock() const;
    /** Enables/disables the knob lock. */
    virtual void enableKnobLock(bool enable);
    /** Returns @c true if the keypad is locked. */
    virtual bool keypadLock() const;
    /** Enables/disables the keypad lock. */
    virtual void enableKeypadLock(bool enable);
    /** Returns @c true if the sidekeys are locked. */
    virtual bool sidekeysLock() const;
    /** Enables/disables the sidekeys lock. */
    virtual void enableSidekeysLock(bool enable);
    /** Returns @c true if the "professional" key is locked. */
    virtual bool keyLockForced() const;
    /** Enables/disables the "professional" key lock. */
    virtual void enableKeyLockForced(bool enable);
    /** Returns @c true if the last heard is shown while pressing PTT. */
    virtual bool showLastHeard() const;
    /** Enables/disables showing last heard. */
    virtual void enableShowLastHeard(bool enable);
    /** Returns the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual unsigned autoRepeaterMinFrequencyVHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMinFrequencyVHF(unsigned Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual unsigned autoRepeaterMaxFrequencyVHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMaxFrequencyVHF(unsigned Hz);

    /** Returns the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual unsigned autoRepeaterMinFrequencyUHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMinFrequencyUHF(unsigned Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual unsigned autoRepeaterMaxFrequencyUHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMaxFrequencyUHF(unsigned Hz);
    /** Returns the auto-repeater direction for VFO B. */
    virtual AutoRepDir autoRepeaterDirectionB() const;
    /** Sets the auto-repeater direction for VFO B. */
    virtual void setAutoRepeaterDirectionB(AutoRepDir dir);
    /** Returns @c true if the default boot channel is enabled. */
    virtual bool defaultChannel() const;
    /** Enables/disables default boot channel. */
    virtual void enableDefaultChannel(bool enable);
    /** Returns the default zone index (0-based) for VFO A. */
    virtual unsigned defaultZoneIndexA() const;
    /** Sets the default zone (0-based) for VFO A. */
    virtual void setDefaultZoneIndexA(unsigned idx);
    /** Returns the default zone index (0-based) for VFO B. */
    virtual unsigned defaultZoneIndexB() const;
    /** Sets the default zone (0-based) for VFO B. */
    virtual void setDefaultZoneIndexB(unsigned idx);
    /** Returns @c true if the default channel for VFO A is VFO. */
    virtual bool defaultChannelAIsVFO() const;
    /** Returns the default channel index for VFO A.
     * Must be within default zone. If 0xff, default channel is VFO. */
    virtual unsigned defaultChannelAIndex() const;
    /** Sets the default channel index for VFO A. */
    virtual void setDefaultChannelAIndex(unsigned idx);
    /** Sets the default channel for VFO A to be VFO. */
    virtual void setDefaultChannelAToVFO();
    /** Returns @c true if the default channel for VFO B is VFO. */
    virtual bool defaultChannelBIsVFO() const;
    /** Returns the default channel index for VFO B.
     * Must be within default zone. If 0xff, default channel is VFO. */
    virtual unsigned defaultChannelBIndex() const;
    /** Sets the default channel index for VFO B. */
    virtual void setDefaultChannelBIndex(unsigned idx);
    /** Sets the default channel for VFO B to be VFO. */
    virtual void setDefaultChannelBToVFO();
    /** Returns @c true if the last caller is kept when changing channel. */
    virtual bool keepLastCaller() const;
    /** Enables/disables keeping the last caller when changing the channel. */
    virtual void enableKeepLastCaller(bool enable);

    virtual bool fromConfig(const Flags &flags, Context &ctx);
    virtual bool updateConfig(Context &ctx);
  };

protected:
  /** Hidden constructor constructor. */
  explicit D868UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D868UVCodeplug(QObject *parent = nullptr);

protected:
  bool allocateBitmaps();
  virtual void setBitmaps(Config *config);
  virtual void allocateUpdated();
  virtual void allocateForDecoding();
  virtual void allocateForEncoding();

  virtual bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocate channels from bitmap. */
  virtual void allocateChannels();
  /** Encode channels into codeplug. */
  virtual bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create channels from codeplug. */
  virtual bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link channels. */
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocate VFO settings. */
  virtual void allocateVFOSettings();

  /** Allocate contacts from bitmaps. */
  virtual void allocateContacts();
  /** Encode contacts into codeplug. */
  virtual bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create contacts from codeplug. */
  virtual bool createContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocate analog contacts from bitmaps. */
  virtual void allocateAnalogContacts();
  /** Encode analog contacts into codeplug. */
  virtual bool encodeAnalogContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create analog contacts from codeplug. */
  virtual bool createAnalogContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocate radio IDs from bitmaps. */
  virtual void allocateRadioIDs();
  /** Encode radio ID into codeplug. */
  virtual bool encodeRadioID(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Set radio ID from codeplug. */
  virtual bool setRadioID(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates RX group lists from bitmaps. */
  virtual void allocateRXGroupLists();
  /** Encode RX group lists into codeplug. */
  virtual bool encodeRXGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create RX group lists from codeplug. */
  virtual bool createRXGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link RX group lists. */
  virtual bool linkRXGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocate zones from bitmaps. */
  virtual void allocateZones();
  /** Encode zones into codeplug. */
  virtual bool encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Function to encode a single zone. */
  virtual bool encodeZone(int i, Zone *zone, bool isB, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create zones from codeplug. */
  virtual bool createZones(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Function to decode a single zone. */
  virtual bool decodeZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link zones. */
  virtual bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Function to link a single zone. */
  virtual bool linkZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocate scanlists from bitmaps. */
  virtual void allocateScanLists();
  /** Encode scan lists into codeplug. */
  virtual bool encodeScanLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create scan lists from codeplug. */
  virtual bool createScanLists(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link scan lists. */
  virtual bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates general settings memory section. */
  virtual void allocateGeneralSettings();
  /** Encodes the general settings section. */
  virtual bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Decodes the general settings section. */
  virtual bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates zone channel list memory section. */
  virtual void allocateZoneChannelList();

  /** Allocates DTMF number list memory section. */
  virtual void allocateDTMFNumbers();

  /** Allocates boot settings memory section. */
  virtual void allocateBootSettings();
  /** Encodes the boot settings section. */
  virtual bool encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Decodes the boot settings section. */
  virtual bool decodeBootSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates GPS settings memory section. */
  virtual void allocateGPSSystems();
  /** Encodes the GPS settings section. */
  virtual bool encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create GPS systems from codeplug. */
  virtual bool createGPSSystems(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link GPS systems. */
  virtual bool linkGPSSystems(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocate prefab SMS messages. */
  virtual void allocateSMSMessages();
  /** Allocates hot key settings memory section. */
  virtual void allocateHotKeySettings();
  /** Allocates repeater offset settings memory section. */
  virtual void allocateRepeaterOffsetSettings();
  /** Allocates alarm settings memory section. */
  virtual void allocateAlarmSettings();
  /** Allocates FM broadcast settings memory section. */
  virtual void allocateFMBroadcastSettings();

  /** Allocates all 5-Tone IDs used. */
  virtual void allocate5ToneIDs();
  /** Allocates 5-Tone functions. */
  virtual void allocate5ToneFunctions();
  /** Allocates 5-Tone settings. */
  virtual void allocate5ToneSettings();

  /** Allocates all 2-Tone IDs used. */
  virtual void allocate2ToneIDs();
  /** Allocates 2-Tone functions. */
  virtual void allocate2ToneFunctions();
  /** Allocates 2-Tone settings. */
  virtual void allocate2ToneSettings();

  /** Allocates DTMF settings. */
  virtual void allocateDTMFSettings();

  /** Internal used function to encode CTCSS frequencies. */
  static uint8_t ctcss_code2num(Signaling::Code code);
  /** Internal used function to decode CTCSS frequencies. */
  static Signaling::Code ctcss_num2code(uint8_t num);
};

#endif // D868UVCODEPLUG_HH
