#ifndef D878UV_CODEPLUG_HH
#define D878UV_CODEPLUG_HH

#include <QDateTime>

#include "d868uv_codeplug.hh"
#include "signaling.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D878UV radios.
 *
 * In contrast to many other code-plugs, the code-plug for Anytone radios are spread over a large
 * memory area. In principle, this is a good idea, as it allows one to upload only the portion of the
 * codeplug that is actually configured. For example, if only a small portion of the available
 * contacts and channels are used, the amount of data that is written to the device can be
 * reduced.
 *
 * However, the implementation of this idea in this device is utter shit. The amount of
 * fragmentation of the codeplug is overwhelming. For example, while channels are organized more or
 * less nicely in continuous banks, zones are distributed throughout the entire code-plug. That is,
 * the names of zones are located in a different memory section that the channel lists. Some lists
 * are defined though bit-masks others by byte-masks. All bit-masks are positive, that is 1
 * indicates an enabled item while the bit-mask for contacts is inverted.
 *
 * In general the code-plug is huge and complex. Moreover, the radio provides a huge amount of
 * options and features. To this end, reverse-engineering this code-plug was a nightmare.
 *
 * More over, the binary code-plug file generate by the windows CPS does not directly relate to
 * the data being written to the radio. To this end the code-plug has been reverse-engineered
 * using wireshark to monitor the USB communication between the windows CPS (running in a virtual
 * box) and the device. The latter makes the reverse-engineering particularly cumbersome.
 *
 * @section d878uvcpl Codeplug structure within radio
 * <table>
 *  <tr><th colspan="3">Channels</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1500</td> <td>000200</td>      <td>Bitmap of 4000 channels, default 0x00,
 *    0x00 padded.</td></tr>
 *  <tr><td>00800000</td> <td>max. 002000</td> <td>Channel bank 0 of up to 128 channels,
 *    see @c D878UVCodeplug::ChannelElement 64 b each. </td></tr>
 *  <tr><td>00802000</td> <td>max, 002000</td> <td>Unknown data, Maybe extended channel information
 *    for channel bank 0? It is of exactly the same size as the channel bank 0. Mostly 0x00, a
 *    few 0xff.</td></tr>
 *  <tr><td>00840000</td> <td>max. 002000</td> <td>Channel bank 1 of up to 128 channels.</td></tr>
 *  <tr><td>00842000</td> <td>max. 002000</td> <td>Unknown data, related to CH bank 1?</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>00FC0000</td> <td>max. 000800</td> <td>Channel bank 32, up to 32 channels.</td></tr>
 *  <tr><td>00FC2000</td> <td>max. 000800</td> <td>Unknown data, related to CH bank 32.</td></tr>
 *  <tr><td>00FC0800</td> <td>000040</td>      <td>VFO A settings,
 *    see @c D878UVCodeplug::ChannelElement.</td></tr>
 *  <tr><td>00FC0840</td> <td>000040</td>      <td>VFO B settings,
 *    see @c D878UVCodeplug::ChannelElement.</td></tr>
 *  <tr><td>00FC2800</td> <td>000080</td>      <td>Unknown data, related to VFO A+B?
 *    It is of exactly the same size as the two VFO channels. Mostly 0x00, a few 0xff. Same pattern
 *    as the unknown data associated with channel banks.</td></tr>
 *
 *  <tr><th colspan="3">Zones</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1300</td> <td>000020</td>      <td>Bitmap of 250 zones.</td></tr>
 *  <tr><td>024C1360</td> <td>000020</td>      <td>Hidden zone bitmap of 250 zones.</td></tr>
 *  <tr><td>01000000</td> <td>max. 01f400</td> <td>250 zones channel lists of 250 16bit indices each.
 *    0-based, little endian, default/padded=0xffff. Offset between channel lists 0x200,
 *    size of each list 0x1f4.</td></tr>
 *  <tr><td>02540000</td> <td>max. 001f40</td> <td>250 Zone names.
 *    Each zone name is up to 16 ASCII chars long and gets 0-padded to 32b.</td></tr>
 *
 *  <tr><th colspan="3">Roaming</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>01042000</td> <td>000020</td>      <td>Roaming channel bitmask, up to 250 bits,
 *    0-padded, default 0.</td></tr>
 *  <tr><td>01040000</td> <td>max. 0x1f40</td> <td>Optional up to 250 roaming channels, of 32b each.
 *    See @c D878UVCodeplug::RoamingChannelElement for details.</td></tr>
 *  <tr><td>01042080</td> <td>000010</td>      <td>Roaming zone bitmask, up to 64 bits, 0-padded,
 *    default 0.</td></tr>
 *  <tr><td>01043000</td> <td>max. 0x2000</td> <td>Optional up to 64 roaming zones, of 128b each.
 *    See @c D878UVCodeplug::RoamingZoneElement for details.</td></tr>
 *
 *  <tr><th colspan="3">Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02600000</td> <td>max. 009C40</td> <td>Index list of valid contacts.
 *    10000 32bit indices, little endian, default 0xffffffff</td></tr>
 *  <tr><td>02640000</td> <td>000500</td>      <td>Contact bitmap, 10000 bit, inverted,
 *    default 0xff, 0x00 padded.</td></tr>
 *  <tr><td>02680000</td> <td>max. 0f4240</td> <td>10000 contacts,
 *    see @c AnytoneCodeplug::ContactElement. As each contact is 100b, they do not align with the
 *    16b blocks being transferred to the device. Hence contacts are organized internally in groups
 *    of 4 contacts forming a "bank". </td></tr>
 *  <tr><td>04340000</td> <td>max. 013880</td> <td>DMR ID to contact index map,
 *    see @c AnytoneCodeplug::ContactMapElement. Sorted by ID, empty entries set to
 *    @c 0xffffffffffffffff.</td>
 *
 *  <tr><th colspan="3">Analog Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02900000</td> <td>000080</td>      <td>Index list of valid analog contacts.</td></tr>
 *  <tr><td>02900100</td> <td>000080</td>      <td>Bytemap for 128 analog contacts.</td></tr>
 *  <tr><td>02940000</td> <td>max. 000180</td> <td>128 analog contacts.
 *    See @c AnytoneCodeplug::DTMFContactElement. As each analog contact is 24b, they do not align with
 *    the 16b transfer block-size. Hence analog contacts are internally organized in groups of 2. </td></tr>
 *
 *  <tr><th colspan="3">RX Group Lists</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>025C0B10</td> <td>000020</td>      <td>Bitmap of 250 RX group lists,
 *    default/padding 0x00.</td></tr>
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
 *  <tr><th colspan="3">GPS/APRS</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02501000</td> <td>0000f0</td> <td>APRS settings,
 *    see @c D878UVCodeplug::AnalogAPRSSettingsElement.</td>
 *  <tr><td>02501040</td> <td>000060</td> <td>APRS settings,
 *    see @c D878UVCodeplug::DMRAPRSSystemsElement.</td>
 *  <tr><td>025010A0</td> <td>000060</td> <td>Extended APRS settings,
 *    see @c D878UVCodeplug::AnalogAPRSSettingsExtensionElement.</tr>
 *  <tr><td>02501200</td> <td>000040</td> <td>APRS Text, up to 60 chars ASCII, 0-padded.</td>
 *  <tr><td>02501280</td> <td>000030</td> <td>Unknown settigs.</td></tr>
 *  <tr><td>02501800</td> <td>000100</td> <td>APRS-RX settings list up to 32 entries, 8b each.
 *    See @c D878UVCodeplug::AnalogAPRSRXEntryElement.</td></tr>
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>0000f0</td> <td>General settings,
 *    see @c D878UVCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>02500100</td> <td>000400</td> <td>Zone A & B channel list.</td></tr>
 *  <tr><td>02500500</td> <td>000100</td> <td>DTMF list</td></tr>
 *  <tr><td>02500600</td> <td>000030</td> <td>Power on settings,
 *    see @c AnytoneCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>02501400</td> <td>000200</td> <td>General settings extension,
 *    see @c D878UVCodeplug::GeneralSettingsExtensionElement.</td></tr>
 *  <tr><td>024C2000</td> <td>0003F0</td> <td>List of 250 auto-repeater offset frequencies.
 *    32bit little endian frequency in 10Hz. I.e., 600kHz = 60000.
 *    Default 0x00000000, 0x00 padded.</td></tr>
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
 *  <tr><td>025C0500</td> <td>000360</td> <td>18 hot-key settings, see
 *    @c AnytoneCodeplug::HotKeyElement</td></tr>
 *
 *  <tr><th colspan="3">Encryption</th></tr>
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
 *  <tr><th colspan="3">Unknown settings.</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1090</td> <td>000040</td> <td>Uknown, filled with 0xff.</td></tr>
 *  <tr><td>02504000</td> <td>000400</td> <td>Uknown, filled with 0xff.</td></tr>
 * </table>
 *
 * @ingroup d878uv */
class D878UVCodeplug : public D868UVCodeplug
{
  Q_OBJECT

public:
  /** Represents the actual channel encoded within the binary D878UV codeplug.
   *
   * This class implements only the differences to the generic @c AnytoneCodeplug::ChannelElement
   * (i.e. D868UVII).
   *
   * Memory layout of encoded channel (size 0x40 bytes):
   * @verbinclude d878uv_channel.txt
   */
  class ChannelElement: public D868UVCodeplug::ChannelElement
  {
  public:
    /** Possible PTT ID settings. */
    enum class PTTId {
      Off = 0,                    ///< Never send PTT-ID.
      Start = 1,                  ///< Send PTT-ID at start.
      End = 2,                    ///< Send PTT-ID at end.
      Both = 3                    ///< Send PTT-ID at start and end.
    };

    /** Defines all possible APRS PTT settings. */
    enum class APRSPTT {
      Off   = 0,                  ///< Do not send APRS on PTT.
      Start = 1,                  ///< Send APRS at start of transmission.
      End   = 2                   ///< Send APRS at end of transmission.
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    /** Resets the channel. */
    void clear();

    /** Returns the PTT ID settings. */
    virtual PTTId pttIDSetting() const;
    /** Sets the PTT ID setting. */
    virtual void setPTTIDSetting(PTTId ptt);

    /** Returns @c true if roaming is enabled. */
    virtual bool roamingEnabled() const;
    /** Enables/disables roaming. */
    virtual void enableRoaming(bool enable);
    // Moved
    /** Returns @c true if the data ACK is enabled. */
    bool dataACK() const;
    /** Enables/disables data ACK. */
    void enableDataACK(bool enable);

    /** Returns @c true if digital APRS transmission is enabled. */
    bool txDigitalAPRS() const;
    /** Enables/disables digital APRS transmission. */
    void enableTXDigitalAPRS(bool enable);
    /** Returns @c true if the analog APRS reporting (TX) is enabled. */
    virtual bool txAnalogAPRS() const;
    /** Enables/disables analog APRS reporting. */
    virtual void enableTXAnalogAPRS(bool enable);

    /** Returns the analog APRS PTT setting. */
    virtual APRSPTT analogAPRSPTTSetting() const;
    /** Sets the analog APRS PTT setting. */
    virtual void setAnalogAPRSPTTSetting(APRSPTT ptt);
    /** Returns the digital APRS PTT setting. */
    virtual APRSPTT digitalAPRSPTTSetting() const;
    /** Sets the digital APRS PTT setting. */
    virtual void setDigitalAPRSPTTSetting(APRSPTT ptt);

    /** Returns the DMR APRS system index. */
    virtual unsigned digitalAPRSSystemIndex() const;
    /** Sets the DMR APRS system index. */
    virtual void setDigitalAPRSSystemIndex(unsigned idx);

    /** Returns the frequency correction in ???. */
    virtual int frequenyCorrection() const;
    /** Sets the frequency correction in ???. */
    virtual void setFrequencyCorrection(int corr);

    /** Constructs a Channel object from this element. */
    Channel *toChannelObj(Context &ctx) const;
    /** Links a previously created channel object. */
    bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Encodes the given channel object. */
    bool fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Represents the general config of the radio within the D878UV binary codeplug.
   *
   * This class implements only the differences to the generic
   * @c AnytonCodeplug::GeneralSettingsElement.
   *
   * Binary encoding of the general settings (size 0x00f0 bytes):
   * @verbinclude d878uv_generalsettings.txt */
  class GeneralSettingsElement: public D868UVCodeplug::GeneralSettingsElement
  {
  protected:
    /** Device specific key functions. */
    enum class KeyFunction {
      Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
      Encryption = 0x05, Call = 0x06, VOX = 0x07, ToggleVFO = 0x08, SubPTT = 0x09,
      Scan = 0x0a, WFM = 0x0b, Alarm = 0x0c, RecordSwitch = 0x0d, Record = 0x0e, SMS = 0x0f,
      Dial = 0x10, Monitor = 0x12, ToggleMainChannel = 0x13, HotKey1 = 0x14,
      HotKey2 = 0x15, HotKey3 = 0x16, HotKey4 = 0x17, HotKey5 = 0x18, HotKey6 = 0x19,
      WorkAlone = 0x1a, SkipChannel = 0x1b, DMRMonitor = 0x1c, SubChannel = 0x1d,
      PriorityZone = 0x1e, VFOScan = 0x1f, MICSoundQuality = 0x20, LastCallReply = 0x21,
      ChannelType = 0x22, Roaming = 0x24, MaxVolume = 0x25, Slot = 0x26, Zone = 0x29,
      RoamingSet = 0x2a, Mute=0x2c, CtcssDcsSet=0x2d, TBSTSend = 0x2e, Bluetooth = 0x2f, GPS = 0x30,
      ChannelName = 0x31, CDTScan = 0x32, APRSSend = 0x33, APRSInfo = 0x34
    };

    AnytoneKeySettingsExtension::KeyFunction mapCodeToKeyFunction(uint8_t code) const;
    uint8_t mapKeyFunctionToCode(AnytoneKeySettingsExtension::KeyFunction func) const;

  public:
    /** Possible VFO frequency steps. */
    enum FreqStep {
      FREQ_STEP_2_5kHz = 0,             ///< 2.5kHz
      FREQ_STEP_5kHz = 1,               ///< 5kHz
      FREQ_STEP_6_25kHz = 2,            ///< 6.25kHz
      FREQ_STEP_10kHz = 3,              ///< 10kHz
      FREQ_STEP_12_5kHz = 4,            ///< 12.5kHz
      FREQ_STEP_20kHz = 5,              ///< 20kHz
      FREQ_STEP_25kHz = 6,              ///< 25kHz
      FREQ_STEP_50kHz = 7               ///< 50kHz
    };

    /** DTMF signalling durations. */
    enum DTMFDuration {
      DTMF_DUR_50ms = 0, DTMF_DUR_100ms = 1, DTMF_DUR_200ms = 2, DTMF_DUR_300ms = 3, DTMF_DUR_500ms = 4
    };

    /** TBST (open repeater) frequencies. */
    enum class TBSTFrequency {
      Hz1000 = 0, Hz1450 = 1, Hz1750 = 2, Hz2100 = 3
    };

    /** All possible STE (squelch tail eliminate) frequencies. */
    enum class STEFrequency {
      Off = 0, Hz55_2  = 1, Hz259_2 = 2
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00f0; }

    /** Resets the general settings. */
    void clear();

    /** Returns the transmit timeout in seconds. */
    virtual unsigned transmitTimeout() const;
    /** Sets the transmit timeout in seconds. */
    virtual void setTransmitTimeout(unsigned tot);

    QTimeZone gpsTimeZone() const;
    void setGPSTimeZone(const QTimeZone &zone);

    /** Returns the UI language. */
    virtual AnytoneDisplaySettingsExtension::Language language() const;
    /** Sets the UI language. */
    virtual void setLanguage(AnytoneDisplaySettingsExtension::Language lang);

    /** Returns the VFO frequency step in kHz. */
    virtual Frequency vfoFrequencyStep() const;
    /** Sets the VFO frequency step in kHz. */
    virtual void setVFOFrequencyStep(Frequency kHz);

    /** Returns the STE (squelch tail eliminate) type. */
    virtual AnytoneSettingsExtension::STEType steType() const;
    /** Sets the STE (squelch tail eliminate) type. */
    virtual void setSTEType(AnytoneSettingsExtension::STEType type);
    /** Returns the STE (squelch tail eliminate) frequency setting in Hz.
     * A value of 0 disables the STE. Possible values are 55.2 and 259.2 Hz. */
    virtual double steFrequency() const;
    /** Sets the STE (squelch tail eliminate) frequency setting.
     * A value of 0 disables the STE. Possible values are 55.2 and 259.2 Hz. */
    virtual void setSTEFrequency(double freq);

    /** Returns the group call hang time in seconds. */
    virtual Interval groupCallHangTime() const;
    /** Sets the group call hang time in seconds. */
    virtual void setGroupCallHangTime(Interval sec);
    /** Returns the private call hang time in seconds. */
    virtual Interval privateCallHangTime() const;
    /** Sets the private call hang time in seconds. */
    virtual void setPrivateCallHangTime(Interval sec);

    /** Returns the pre-wave time in ms. */
    virtual Interval preWaveDelay() const;
    /** Sets the pre-wave time in ms. */
    virtual void setPreWaveDelay(Interval ms);
    /** Returns the wake head-period in ms. */
    virtual Interval wakeHeadPeriod() const;
    /** Sets the wake head-period in ms. */
    virtual void setWakeHeadPeriod(Interval ms);

    /** Returns the wide-FM (broadcast) channel index. */
    virtual unsigned wfmChannelIndex() const;
    /** Sets the wide-FM (broadcast) channel index. */
    virtual void setWFMChannelIndex(unsigned idx);
    /** Returns @c true if the WFM RX is in VFO mode. */
    virtual bool wfmVFOEnabled() const;
    /** Enables/disables VFO mode for WFM RX. */
    virtual void enableWFMVFO(bool enable);

    /** Returns the DTMF tone duration in ms. */
    virtual unsigned dtmfToneDuration() const;
    /** Sets the DTMF tone duration in ms. */
    virtual void setDTMFToneDuration(unsigned ms);

    /** Returns @c true if "man down" is enabled. */
    virtual bool manDown() const;
    /** Enables/disables "man down". */
    virtual void enableManDown(bool enable);

    /** Returns @c true if WFM monitor is enabled. */
    virtual bool wfmMonitor() const;
    /** Enables/disables WFM monitor. */
    virtual void enableWFMMonitor(bool enable);

    /** Returns the TBST frequency. */
    virtual Frequency tbstFrequency() const;
    /** Sets the TBST frequency. */
    virtual void setTBSTFrequency(Frequency freq);

    /** Returns @c true if the "pro mode" is enabled. */
    virtual bool proMode() const;
    /** Enables/disables the "pro mode". */
    virtual void enableProMode(bool enable);

    /** Returns @c true if the own ID is filtered in call lists. */
    virtual bool filterOwnID() const;
    /** Enables/disables filter of own ID in call lists. */
    virtual void enableFilterOwnID(bool enable);
    /** Returns @c true remote stun/kill is enabled. */
    virtual bool remoteStunKill() const;
    /** Enables/disables remote stun/kill. */
    virtual void enableRemoteStunKill(bool enable);
    /** Returns @c true remote monitor is enabled. */
    virtual bool remoteMonitor() const;
    /** Enables/disables remote monitor. */
    virtual void enableRemoteMonitor(bool enable);

    /** Returns the monitor slot match. */
    virtual AnytoneDMRSettingsExtension::SlotMatch monitorSlotMatch() const;
    /** Sets the monitor slot match. */
    virtual void setMonitorSlotMatch(AnytoneDMRSettingsExtension::SlotMatch match);
    /** Returns @c true if the monitor matches color code. */
    virtual bool monitorColorCodeMatch() const;
    /** Enables/disables monitor color code match. */
    virtual void enableMonitorColorCodeMatch(bool enable);
    /** Returns @c true if the monitor matches ID. */
    virtual bool monitorIDMatch() const;
    /** Enables/disables monitor ID match. */
    virtual void enableMonitorIDMatch(bool enable);
    /** Returns @c true if the monitor holds the time slot. */
    virtual bool monitorTimeSlotHold() const;
    /** Enables/disables monitor time slot hold. */
    virtual void enableMonitorTimeSlotHold(bool enable);

    /** Returns the "man down" delay in seconds. */
    virtual Interval manDownDelay() const;
    /** Sets the "man down" delay in seconds. */
    virtual void setManDownDelay(Interval sec);
    /** Returns the analog call hold in seconds. */
    virtual unsigned fmCallHold() const;
    /** Sets the analog call hold in seconds. */
    virtual void setFMCallHold(unsigned sec);

    /** Returns @c true if the GPS range reporting is enabled. */
    virtual bool gpsMessageEnabled() const;
    /** Enables/disables GPS range reporting. */
    virtual void enableGPSMessage(bool enable);

    /** Returns @c true if the call channel is maintained. */
    virtual bool maintainCallChannel() const;
    /** Enables/disables maintaining the call channel. */
    virtual void enableMaintainCallChannel(bool enable);

    /** Returns the priority Zone A index. */
    virtual unsigned priorityZoneAIndex() const;
    /** Sets the priority zone A index. */
    virtual void setPriorityZoneAIndex(unsigned idx);
    /** Returns the priority Zone B index. */
    virtual unsigned priorityZoneBIndex() const;
    /** Sets the priority zone B index. */
    virtual void setPriorityZoneBIndex(unsigned idx);

    /** Returns @c true if bluetooth is enabled. */
    virtual bool bluetooth() const;
    /** Enables/disables bluetooth. */
    virtual void enableBluetooth(bool enable);

    /** Returns @c true if the internal mic is addionally active when BT is active. */
    virtual bool btAndInternalMic() const;
    /** Enables/disables the internal mic when BT is active. */
    virtual void enableBTAndInternalMic(bool enable);

    /** Returns @c true if the internal speaker is addionally active when BT is active. */
    virtual bool btAndInternalSpeaker() const;
    /** Enables/disables the internal speaker when BT is active. */
    virtual void enableBTAndInternalSpeaker(bool enable);

    /** Returns @c true if the plug-in record tone is enabled. */
    virtual bool pluginRecTone() const;
    /** Enables/disables the plug-in record tone. */
    virtual void enablePluginRecTone(bool enable);

    /** Returns the GPS ranging interval in seconds. */
    virtual Interval gpsUpdatePeriod() const;
    /** Sets the GPS ranging interval in seconds. */
    virtual void setGPSUpdatePeriod(Interval sec);

    /** Retunrs the bluetooth microphone gain [1,10]. */
    virtual unsigned int btMicGain() const;
    /** Sets the bluetooth microphone gain [1,10]. */
    virtual void setBTMicGain(unsigned int gain);
    /** Retunrs the bluetooth speaker gain [1,10]. */
    virtual unsigned int btSpeakerGain() const;
    /** Sets the bluetooth speaker gain [1,10]. */
    virtual void setBTSpeakerGain(unsigned int gain);

    /** Returns @c true if the channel number is displayed. */
    virtual bool displayChannelNumber() const;
    /** Enables/disables display of channel number. */
    virtual void enableDisplayChannelNumber(bool enable);

    bool showCurrentContact() const;
    void enableShowCurrentContact(bool enable);

    /** Returns the auto roaming period in minutes. */
    virtual Interval autoRoamPeriod() const;
    /** Sets the auto roaming period in minutes. */
    virtual void setAutoRoamPeriod(Interval min);

    bool keyToneLevelAdjustable() const;
    unsigned keyToneLevel() const;
    void setKeyToneLevel(unsigned level);
    void setKeyToneLevelAdjustable();

    AnytoneDisplaySettingsExtension::Color callDisplayColor() const;
    void setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color);

    bool gpsUnitsImperial() const;
    void enableGPSUnitsImperial(bool enable);

    bool knobLock() const;
    void enableKnobLock(bool enable);
    bool keypadLock() const;
    void enableKeypadLock(bool enable);
    bool sidekeysLock() const;
    void enableSidekeysLock(bool enable);
    bool keyLockForced() const;
    void enableKeyLockForced(bool enable);

    /** Returns the auto-roam delay in seconds. */
    virtual Interval autoRoamDelay() const;
    /** Sets the auto-roam delay in seconds. */
    virtual void setAutoRoamDelay(Interval sec);

    /** Returns the standby text color. */
    virtual AnytoneDisplaySettingsExtension::Color standbyTextColor() const;
    /** Sets the standby text color. */
    virtual void setStandbyTextColor(AnytoneDisplaySettingsExtension::Color color);
    /** Returns the standby image color. */
    virtual AnytoneDisplaySettingsExtension::Color standbyBackgroundColor() const;
    /** Sets the standby image color. */
    virtual void setStandbyBackgroundColor(AnytoneDisplaySettingsExtension::Color color);

    bool showLastHeard() const;
    void enableShowLastHeard(bool enable);

    /** Returns the SMS format. */
    virtual AnytoneDMRSettingsExtension::SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat fmt);

    /** Returns the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual Frequency autoRepeaterMinFrequencyVHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMinFrequencyVHF(Frequency Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual Frequency autoRepeaterMaxFrequencyVHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMaxFrequencyVHF(Frequency Hz);

    /** Returns the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual Frequency autoRepeaterMinFrequencyUHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMinFrequencyUHF(Frequency Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual Frequency autoRepeaterMaxFrequencyUHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMaxFrequencyUHF(Frequency Hz);

    /** Returns the auto-repeater direction for VFO B. */
    virtual AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionB() const;
    /** Sets the auto-repeater direction for VFO B. */
    virtual void setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir);

    /** If enabled, the FM ID is send together with selected contact. */
    virtual bool fmSendIDAndContact() const;
    /** Enables/disables sending contact with FM ID. */
    virtual void enableFMSendIDAndContact(bool enable);

    bool defaultChannel() const;
    void enableDefaultChannel(bool enable);
    unsigned defaultZoneIndexA() const;
    void setDefaultZoneIndexA(unsigned idx);
    unsigned defaultZoneIndexB() const;
    void setDefaultZoneIndexB(unsigned idx);
    bool defaultChannelAIsVFO() const;
    unsigned defaultChannelAIndex() const;
    void setDefaultChannelAIndex(unsigned idx);
    void setDefaultChannelAToVFO();
    bool defaultChannelBIsVFO() const;
    unsigned defaultChannelBIndex() const;
    void setDefaultChannelBIndex(unsigned idx);
    void setDefaultChannelBToVFO();

    /** Returns the default roaming zone index. */
    virtual unsigned defaultRoamingZoneIndex() const;
    /** Sets the default roaming zone index. */
    virtual void setDefaultRoamingZoneIndex(unsigned idx);

    /** Returns @c true if repeater range check is enabled. */
    virtual bool repeaterRangeCheck() const;
    /** Enables/disables repeater range check. */
    virtual void enableRepeaterRangeCheck(bool enable);
    /** Returns the repeater range check period in seconds. */
    virtual Interval repeaterRangeCheckInterval() const;
    /** Sets the repeater range check interval in seconds. */
    virtual void setRepeaterRangeCheckInterval(Interval sec);
    /** Returns the number of repeater range checks. */
    virtual unsigned repeaterRangeCheckCount() const;
    /** Sets the number of repeater range checks. */
    virtual void setRepeaterRangeCheckCount(unsigned n);

    /** Returns the roaming start condition. */
    virtual AnytoneRangingSettingsExtension::RoamStart roamingStartCondition() const;
    /** Sets the roaming start condition. */
    virtual void setRoamingStartCondition(AnytoneRangingSettingsExtension::RoamStart cond);

    /** Returns the backlight duration during TX in seconds. */
    virtual Interval txBacklightDuration() const;
    /** Sets the backlight duration during TX in seconds. */
    virtual void setTXBacklightDuration(Interval sec);

    /** Returns @c true if the "separate display" is enabled. */
    virtual bool separateDisplay() const;
    /** Enables/disables "separate display. */
    virtual void enableSeparateDisplay(bool enable);

    bool keepLastCaller() const;
    void enableKeepLastCaller(bool enable);

    /** Returns the channel name color. */
    virtual AnytoneDisplaySettingsExtension::Color channelNameColor() const;
    /** Sets the channel name color. */
    virtual void setChannelNameColor(AnytoneDisplaySettingsExtension::Color color);

    /** Returns @c true if repeater check notification is enabled. */
    virtual bool repeaterCheckNotification() const;
    /** Enables/disables repeater check notification. */
    virtual void enableRepeaterCheckNotification(bool enable);

    /** Returns the backlight duration during RX in seconds. */
    Interval rxBacklightDuration() const;
    /** Sets the backlight duration during RX in seconds. */
    void setRXBacklightDuration(Interval sec);

    /** Returns @c true if roaming is enabled. */
    virtual bool roaming() const;
    /** Enables/disables repeater check notification. */
    virtual void enableRoaming(bool enable);

    /** Returns the mute delay in minutes. */
    virtual Interval muteDelay() const;
    /** Sets the mute delay in minutes. */
    virtual void setMuteDelay(Interval min);

    /** Returns the number of repeater check notifications. */
    virtual unsigned repeaterCheckNumNotifications() const;
    /** Sets the number of repeater check notifications. */
    virtual void setRepeaterCheckNumNotifications(unsigned num);

    /** Returns @c true if boot GPS check is enabled. */
    virtual bool bootGPSCheck() const;
    /** Enables/disables boot GPS check. */
    virtual void enableBootGPSCheck(bool enable);
    /** Returns @c true if boot reset is enabled. */
    virtual bool bootReset() const;
    /** Enables/disables boot reset. */
    virtual void enableBootReset(bool enable);

    /** Returns @c true, if the bluetooth hold time is enabled. */
    virtual bool btHoldTimeEnabled() const;
    /** Returns @c true, if the bluetooth hold time is infinite. */
    virtual bool btHoldTimeInfinite() const;
    /** Returns the bluetooth hold time. */
    virtual Interval btHoldTime() const;
    /** Sets the bluethooth hold time (1-120s). */
    virtual void setBTHoldTime(Interval interval);
    /** Sets the bluethooth hold time to infinite. */
    virtual void setBTHoldTimeInfinite();
    /** Sets the bluethooth hold time to infinite. */
    virtual void disableBTHoldTime();

    /** Returns the bluetooth RX delay in ms. */
    virtual Interval btRXDelay() const;
    /** Sets the bluetooth RX delay in ms. */
    virtual void setBTRXDelay(Interval delay);

    bool fromConfig(const Flags &flags, Context &ctx);
    bool updateConfig(Context &ctx);
    bool linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err);

  protected:
    /** Some internal used offsets within the element. */
    struct Offset: public D868UVCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int transmitTimeout()     { return 0x0004; }
      static constexpr unsigned int language()            { return 0x0005; }
      static constexpr unsigned int vfoFrequencyStep()    { return 0x0008; }
      static constexpr unsigned int steType()             { return 0x0017; }
      static constexpr unsigned int steFrequency()        { return 0x0018; }
      static constexpr unsigned int groupCallHangTime()   { return 0x0019; }
      static constexpr unsigned int privateCallHangTime() { return 0x001a; }
      static constexpr unsigned int preWaveDelay()        { return 0x001b; }
      static constexpr unsigned int wakeHeadPeriod()      { return 0x001c; }
      static constexpr unsigned int wfmChannelIndex()     { return 0x001d; }
      static constexpr unsigned int wfmVFOEnabled()       { return 0x001e; }
      static constexpr unsigned int dtmfToneDuration()    { return 0x0023; }
      static constexpr unsigned int manDown()             { return 0x0024; }
      static constexpr unsigned int wfmMonitor()          { return 0x002b; }
      static constexpr unsigned int tbstFrequency()       { return 0x002e; }
      static constexpr unsigned int proMode()             { return 0x0034; }
      static constexpr unsigned int filterOwnID()         { return 0x0038; }
      static constexpr unsigned int remoteStunKill()      { return 0x003c; }
      static constexpr unsigned int remoteMonitor()       { return 0x003e; }
      static constexpr unsigned int monSlotMatch()        { return 0x0049; }
      static constexpr unsigned int monColorCodeMatch()   { return 0x004a; }
      static constexpr unsigned int monIDMatch()          { return 0x004b; }
      static constexpr unsigned int monTimeSlotHold()     { return 0x004c; }
      static constexpr unsigned int manDownDelay()        { return 0x004f; }
      static constexpr unsigned int fmCallHold()          { return 0x0050; }
      static constexpr unsigned int enableGPSMessage()    { return 0x0053; }
      static constexpr unsigned int maintainCallChannel() { return 0x006e; }
      static constexpr unsigned int priorityZoneA()       { return 0x006f; }
      static constexpr unsigned int priorityZoneB()       { return 0x0070; }
      static constexpr unsigned int bluetooth()           { return 0x00b1; }
      static constexpr unsigned int btAndInternalMic()    { return 0x00b2; }
      static constexpr unsigned int btAndInternalSpeaker(){ return 0x00b3; }
      static constexpr unsigned int pluginRecTone()       { return 0x00b4; }
      static constexpr unsigned int gpsRangingInterval()  { return 0x00b5; }
      static constexpr unsigned int btMicGain()           { return 0x00b6; }
      static constexpr unsigned int btSpeakerGain()       { return 0x00b7; }
      static constexpr unsigned int showChannelNumber()   { return 0x00b8; }
      static constexpr unsigned int showCurrentContact()  { return 0x00b9; }
      static constexpr unsigned int autoRoamPeriod()      { return 0x00ba; }
      static constexpr unsigned int keyToneLevel()        { return 0x00bb; }
      static constexpr unsigned int callColor()           { return 0x00bc; }
      static constexpr unsigned int gpsUnits()            { return 0x00bd; }
      static constexpr unsigned int knobLock()            { return 0x00be; }
      static constexpr unsigned int keypadLock()          { return 0x00be; }
      static constexpr unsigned int sideKeyLock()         { return 0x00be; }
      static constexpr unsigned int forceKeyLock()        { return 0x00be; }
      static constexpr unsigned int autoRoamDelay()       { return 0x00bf; }
      static constexpr unsigned int standbyTextColor()    { return 0x00c0; }
      static constexpr unsigned int standbyBackground()   { return 0x00c1; }
      static constexpr unsigned int showLastHeard()       { return 0x00c2; }
      static constexpr unsigned int smsFormat()           { return 0x00c3; }
      static constexpr unsigned int autoRepMinVHF()       { return 0x00c4; }
      static constexpr unsigned int autoRepMaxVHF()       { return 0x00c8; }
      static constexpr unsigned int autoRepMinUHF()       { return 0x00cc; }
      static constexpr unsigned int autoRepMaxUHF()       { return 0x00d0; }
      static constexpr unsigned int autoRepeaterDirB()    { return 0x00d4; }
      static constexpr unsigned int fmSendIDAndContact()  { return 0x00d5; }
      static constexpr unsigned int defaultChannels()     { return 0x00d7; }
      static constexpr unsigned int defaultZoneA()        { return 0x00d8; }
      static constexpr unsigned int defaultZoneB()        { return 0x00d9; }
      static constexpr unsigned int defaultChannelA()     { return 0x00da; }
      static constexpr unsigned int defaultChannelB()     { return 0x00db; }
      static constexpr unsigned int defaultRoamingZone()  { return 0x00dc; }
      static constexpr unsigned int repRangeCheck()       { return 0x00dd; }
      static constexpr unsigned int rangeCheckInterval()  { return 0x00de; }
      static constexpr unsigned int rangeCheckCount()     { return 0x00df; }
      static constexpr unsigned int roamStartCondition()  { return 0x00e0; }
      static constexpr unsigned int txBacklightDuration() { return 0x00e1; }
      static constexpr unsigned int displaySeparator()    { return 0x00e2; }
      static constexpr unsigned int keepLastCaller()      { return 0x00e3; }
      static constexpr unsigned int channelNameColor()    { return 0x00e4; }
      static constexpr unsigned int repCheckNotify()      { return 0x00e5; }
      static constexpr unsigned int rxBacklightDuration() { return 0x00e6; }
      static constexpr unsigned int roaming()             { return 0x00e7; }
      static constexpr unsigned int muteDelay()           { return 0x00e9; }
      static constexpr unsigned int repCheckNumNotify()   { return 0x00ea; }
      static constexpr unsigned int bootGPSCheck()        { return 0x00eb; }
      static constexpr unsigned int bootReset()           { return 0x00ec; }
      static constexpr unsigned int btHoldTime()          { return 0x00ed; }
      static constexpr unsigned int btRXDelay()           { return 0x00ee; }
      /// @endcond
    };
  };

  /** General settings extension element for the D878UV.
   *
   * Memory representation of the encoded settings element (size 0x200 bytes):
   * @verbinclude d878uv_generalsettingsextension.txt */
  class GeneralSettingsExtensionElement: public Element
  {
  public:
    /** Talker alias display preference. */
    enum class TalkerAliasDisplay {
      Off = 0, Conctact = 1, Air = 2
    };

    /** Talker alias encoding. */
    enum class TalkerAliasEncoding {
      ISO8 = 0, ISO7 = 1, Unicode = 2,
    };

    /** Possible GPS modes. */
    enum class GPSMode {
      GPS=0, BDS=1, Both=2
    };

  protected:
    /** Hidden Constructor. */
    GeneralSettingsExtensionElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GeneralSettingsExtensionElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00000200; }

    /** Resets the settings. */
    void clear();

    /** Returns @c true if talker alias is send. */
    virtual bool sendTalkerAlias() const;
    /** Enables/disables sending talker alias. */
    virtual void enableSendTalkerAlias(bool enable);

    /** Returns the talker alias display mode. */
    virtual TalkerAliasDisplay talkerAliasDisplay() const;
    /** Sets the talker alias display mode. */
    virtual void setTalkerAliasDisplay(TalkerAliasDisplay mode);

    /** Returns the talker alias encoding. */
    virtual TalkerAliasEncoding talkerAliasEncoding() const;
    /** Sets the talker alias encoding. */
    virtual void setTalkerAliasEncoding(TalkerAliasEncoding encoding);

    /** Returns @c true if the auto repeater UHF 2 offset index is set. */
    virtual bool hasAutoRepeaterUHF2OffsetIndex() const;
    /** Returns the index of the UHF 2 offset frequency. */
    virtual unsigned autoRepeaterUHF2OffsetIndex() const;
    /** Sets the index of the UHF 2 offset frequency. */
    virtual void setAutoRepeaterUHF2OffsetIndex(unsigned idx);
    /** Clears the auto repeater UHF 2 offset frequency index. */
    virtual void clearAutoRepeaterUHF2OffsetIndex();

    /** Returns @c true if the auto repeater VHF 2 offset index is set. */
    virtual bool hasAutoRepeaterVHF2OffsetIndex() const;
    /** Returns the index of the VHF 2 offset frequency. */
    virtual unsigned autoRepeaterVHF2OffsetIndex() const;
    /** Sets the index of the VHF 2 offset frequency. */
    virtual void setAutoRepeaterVHF2OffsetIndex(unsigned idx);
    /** Clears the auto repeater VHF 2 offset frequency index. */
    virtual void clearAutoRepeaterVHF2OffsetIndex();

    /** Returns the minimum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual Frequency autoRepeaterVHF2MinFrequency() const;
    /** Sets the minimum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual void setAutoRepeaterVHF2MinFrequency(Frequency hz);
    /** Returns the maximum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual Frequency autoRepeaterVHF2MaxFrequency() const;
    /** Sets the maximum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual void setAutoRepeaterVHF2MaxFrequency(Frequency hz);
    /** Returns the minimum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual Frequency autoRepeaterUHF2MinFrequency() const;
    /** Sets the minimum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual void setAutoRepeaterUHF2MinFrequency(Frequency hz);
    /** Returns the maximum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual Frequency autoRepeaterUHF2MaxFrequency() const;
    /** Sets the maximum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual void setAutoRepeaterUHF2MaxFrequency(Frequency hz);

    /** Returns the GPS mode. */
    virtual GPSMode gpsMode() const;
    /** Sets the GPS mode. */
    virtual void setGPSMode(GPSMode mode);

    /** Returns the FM Mic gain [1,10]. */
    virtual unsigned int analogMicGain() const;
    /** Sets the analog mic gain [1,10]. */
    virtual void setAnalogMicGain(unsigned int gain);

    /** Encodes the settings from the config. */
    virtual bool fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Update config from settings. */
    virtual bool updateConfig(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Link config from settings extension. */
    virtual bool linkConfig(Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Internal used offset within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int sendTalkerAlias()              { return 0x0000; }
      static constexpr unsigned int talkerAliasDisplay()           { return 0x001e; }
      static constexpr unsigned int talkerAliasEncoding()          { return 0x001f; }
      static constexpr unsigned int autoRepeaterUHF2OffsetIndex()  { return 0x0022; }
      static constexpr unsigned int autoRepeaterVHF2OffsetIndex()  { return 0x0023; }
      static constexpr unsigned int autoRepeaterVHF2MinFrequency() { return 0x0024; }
      static constexpr unsigned int autoRepeaterVHF2MaxFrequency() { return 0x0028; }
      static constexpr unsigned int autoRepeaterUHF2MinFrequency() { return 0x002c; }
      static constexpr unsigned int autoRepeaterUHF2MaxFrequency() { return 0x0030; }
      static constexpr unsigned int gpsMode()                      { return 0x0035; }
      static constexpr unsigned int analogMicGain()                { return 0x0043; }
      /// @endcond
    };
  };

  /** Represents the APRS settings within the binary D878UV codeplug.
   *
   * Memory layout of APRS settings (size 0x00f0 bytes):
   * @verbinclude d878uv_aprssetting.txt
   */
  class AnalogAPRSSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit AnalogAPRSSettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00f0; }

    /** Resets the settings. */
    void clear();
    bool isValid() const;

    /** Returns the transmit frequency in Hz. */
    virtual unsigned frequency() const;
    /** Sets the transmit frequency in Hz. */
    virtual void setFrequency(unsigned hz);

    /** Returns the TX delay in ms. */
    virtual unsigned txDelay() const;
    /** Sets the TX delay in ms. */
    virtual void setTXDelay(unsigned ms);

    /** Returns the sub tone settings. */
    virtual Signaling::Code txTone() const;
    /** Sets the sub tone settings. */
    virtual void setTXTone(Signaling::Code code);

    /** Returns the manual TX interval in seconds. */
    virtual unsigned manualTXInterval() const;
    /** Sets the manual TX interval in seconds. */
    virtual void setManualTXInterval(unsigned sec);

    /** Returns @c true if the auto transmit is enabled. */
    virtual bool autoTX() const;
    /** Returns the auto TX interval in seconds. */
    virtual unsigned autoTXInterval() const;
    /** Sets the auto TX interval in seconds. */
    virtual void setAutoTXInterval(unsigned sec);
    /** Disables auto tx. */
    virtual void disableAutoTX();

    /** Returns @c true if a fixed location is send. */
    virtual bool fixedLocationEnabled() const;
    /** Returns the fixed location send. */
    virtual QGeoCoordinate fixedLocation() const;
    /** Sets the fixed location to send. */
    virtual void setFixedLocation(QGeoCoordinate &loc);
    /** Disables sending a fixed location. */
    virtual void disableFixedLocation();

    /** Returns the destination call. */
    virtual QString destination() const;
    /** Returns the destination SSID. */
    virtual unsigned destinationSSID() const;
    /** Sets the destination call & SSID. */
    virtual void setDestination(const QString &call, unsigned ssid);
    /** Returns the source call. */
    virtual QString source() const;
    /** Returns the source SSID. */
    virtual unsigned sourceSSID() const;
    /** Sets the source call & SSID. */
    virtual void setSource(const QString &call, unsigned ssid);

    /** Returns the path string. */
    virtual QString path() const;
    /** Sets the path string. */
    virtual void setPath(const QString &path);

    /** Returns the APRS icon. */
    virtual APRSSystem::Icon icon() const;
    /** Sets the APRS icon. */
    virtual void setIcon(APRSSystem::Icon icon);

    /** Returns the transmit power. */
    virtual Channel::Power power() const;
    /** Sets the transmit power. */
    virtual void setPower(Channel::Power power);

    /** Returns the pre-wave delay in ms. */
    virtual unsigned preWaveDelay() const;
    /** Sets the pre-wave delay in ms. */
    virtual void setPreWaveDelay(unsigned ms);

    /** Configures this APRS system from the given generic config. */
    virtual bool fromAPRSSystem(const APRSSystem *sys, Context &ctx,
                                const ErrorStack &err=ErrorStack());
    /** Constructs a generic APRS system configuration from this APRS system. */
    virtual APRSSystem *toAPRSSystem();
    /** Links the transmit channel within the generic APRS system based on the transmit frequency
     * defined within this APRS system. */
    virtual bool linkAPRSSystem(APRSSystem *sys, Context &ctx);

  };

  /** Represents an extension to the APRS settings.
   *
   * Memory layout of APRS settings (0x60byte):
   * @verbinclude d878uv_aprssettingext.txt */
  class AnalogAPRSSettingsExtensionElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSSettingsExtensionElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    AnalogAPRSSettingsExtensionElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0060; }

    /** Resets the settings. */
    void clear();

    /** Returns the fixed altitude in meter. */
    virtual unsigned fixedAltitude() const;
    /** Sets the fixed altitude in meter. */
    virtual void setFixedAltitude(unsigned m);

    /** Returns @c true if the report position flag is set. */
    virtual bool reportPosition() const;
    /** Enables/disables report position flag. */
    virtual void enableReportPosition(bool enable);
    /** Returns @c true if the report Mic-E flag is set. */
    virtual bool reportMicE() const;
    /** Enables/disables report Mic-E flag. */
    virtual void enableReportMicE(bool enable);
    /** Returns @c true if the report object flag is set. */
    virtual bool reportObject() const;
    /** Enables/disables report object flag. */
    virtual void enableReportObject(bool enable);
    /** Returns @c true if the report item flag is set. */
    virtual bool reportItem() const;
    /** Enables/disables report item flag. */
    virtual void enableReportItem(bool enable);
    /** Returns @c true if the report message flag is set. */
    virtual bool reportMessage() const;
    /** Enables/disables report message flag. */
    virtual void enableReportMessage(bool enable);
    /** Returns @c true if the report weather flag is set. */
    virtual bool reportWeather() const;
    /** Enables/disables report weather flag. */
    virtual void enableReportWeather(bool enable);
    /** Returns @c true if the report NMEA flag is set. */
    virtual bool reportNMEA() const;
    /** Enables/disables report NMEA flag. */
    virtual void enableReportNMEA(bool enable);
    /** Returns @c true if the report status flag is set. */
    virtual bool reportStatus() const;
    /** Enables/disables report status flag. */
    virtual void enableReportStatus(bool enable);
    /** Returns @c true if the report other flag is set. */
    virtual bool reportOther() const;
    /** Enables/disables report other flag. */
    virtual void enableReportOther(bool enable);
  };

  /** Represents an (analog/FM) APRS message. */
  class AnalogAPRSMessageElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSMessageElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    AnalogAPRSMessageElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0040; }

    void clear();

    /** Returns the message. */
    virtual QString message() const;
    /** Sets the message. */
    virtual void setMessage(const QString &msg);

  public:
    /** Some limits. */
    struct Limit {
      static constexpr unsigned int length() { return 60; }    ///< Maximum message length.
    };
  };

  /** Represents an analog APRS RX entry.
   *
   * Memory layout of analog APRS RX entry (size 0x0008 bytes):
   * @verbinclude d878uv_aprsrxentry.txt */
  class AnalogAPRSRXEntryElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSRXEntryElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    AnalogAPRSRXEntryElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0008; }

    /** Resets the entry. */
    void clear();
    /** Returns @c true if the APRS RX entry is valid. */
    bool isValid() const;

    /** Returns the call sign. */
    virtual QString call() const;
    /** Returns the SSID. */
    virtual unsigned ssid() const;
    /** Sets the call, SSID and enables the entry. */
    virtual void setCall(const QString &call, unsigned ssid);
  };

  /** Represents the 8 DMR-APRS systems within the binary codeplug.
   *
   * Memory layout of encoded DMR-APRS systems (size 0x0060 bytes):
   * @verbinclude d878uv_dmraprssettings.txt */
  class DMRAPRSSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DMRAPRSSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    DMRAPRSSettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0060; }

    /** Resets the systems. */
    void clear();

    /** Returns @c true if the channel points to the current/selected channel. */
    virtual bool channelIsSelected(unsigned n) const;
    /** Returns the digital channel index for the n-th system. */
    virtual unsigned channelIndex(unsigned n) const;
    /** Sets the digital channel index for the n-th system. */
    virtual void setChannelIndex(unsigned n, unsigned idx);
    /** Sets the channel to the current/selected channel. */
    virtual void setChannelSelected(unsigned n);

    /** Returns the destination contact for the n-th system. */
    virtual unsigned destination(unsigned n) const;
    /** Sets the destination contact for the n-th system. */
    virtual void setDestination(unsigned n, unsigned idx);

    /** Returns the call type for the n-th system. */
    virtual DMRContact::Type callType(unsigned n) const;
    /** Sets the call type for the n-th system. */
    virtual void setCallType(unsigned n, DMRContact::Type type);

    /** Returns @c true if the n-th system overrides the channel time-slot. */
    virtual bool timeSlotOverride(unsigned n);
    /** Returns the time slot if overridden (only valid if @c timeSlot returns true). */
    virtual DMRChannel::TimeSlot timeSlot(unsigned n) const;
    /** Overrides the time slot of the n-th selected channel. */
    virtual void setTimeSlot(unsigned n, DMRChannel::TimeSlot ts);
    /** Clears the time-slot override. */
    virtual void clearTimeSlotOverride(unsigned n);

    /** Returns @c true if the roaming is enabled. */
    virtual bool roaming() const;
    /** Enables/disables roaming. */
    virtual void enableRoaming(bool enable);

    /** Returns the the repeater activation delay in ms. */
    virtual Interval repeaterActivationDelay() const;
    /** Sets the repeater activation delay in ms. */
    virtual void setRepeaterActivationDelay(Interval ms);

    /** Constructs all GPS system from the generic configuration. */
    virtual bool fromGPSSystems(Context &ctx);
    /** Encodes the given GPS system. */
    virtual bool fromGPSSystemObj(unsigned int idx, GPSSystem *sys, Context &ctx);
    /** Constructs a generic GPS system from the idx-th encoded GPS system. */
    virtual GPSSystem *toGPSSystemObj(int idx) const;
    /** Links the specified generic GPS system. */
    virtual bool linkGPSSystem(int idx, GPSSystem *sys, Context &ctx) const;

  public:
    /** Some limits. */
    struct Limit {
      /// Maximum number of channels, destinations, call types and time slots.
      static constexpr unsigned int systemCount() { return 8; }
    };

  protected:
    /** Internal used offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int channels()                { return 0x0000; }
      static constexpr unsigned int betweenChannels()         { return 0x0002; }
      static constexpr unsigned int destinations()            { return 0x0010; }
      static constexpr unsigned int betweenDestinations()     { return 0x0004; }
      static constexpr unsigned int callTypes()               { return 0x0030; }
      static constexpr unsigned int betweenCallTypes()        { return 0x0001; }
      static constexpr unsigned int timeSlots()               { return 0x0039; }
      static constexpr unsigned int betweenTimeSlots()        { return 0x0001; }
      static constexpr unsigned int roaming()                 { return 0x0038; }
      static constexpr unsigned int repeaterActivationDelay() { return 0x0041; }
      /// @endcond
    };
  };

  /** Implements the binary representation of a roaming channel within the codeplug.
   *
   * Memory layout of roaming channel (size 0x0020 bytes):
   * @verbinclude d878uv_roamingchannel.txt */
  class RoamingChannelElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RoamingChannelElement(uint8_t *ptr, unsigned size);

  protected:
    /** Special values for the color code. */
    enum ColorCodeValue {
      Disabled = 16
    };

    /** Encoded values for the time slot. */
    enum TimeSlotValue {
      TS1 = 0,
      TS2 = 1
    };

  public:
    /** Constructor. */
    RoamingChannelElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }

    /** Resets the roaming channel. */
    void clear();

    /** Returns the RX frequency in Hz. */
    virtual unsigned rxFrequency() const;
    /** Sets the RX frequency in Hz. */
    virtual void setRXFrequency(unsigned hz);
    /** Returns the TX frequency in Hz. */
    virtual unsigned txFrequency() const;
    /** Sets the TX frequency in Hz. */
    virtual void setTXFrequency(unsigned hz);

    /** Returns @c true if the color code is set. */
    virtual bool hasColorCode() const;
    /** Returns the color code. */
    virtual unsigned colorCode() const;
    /** Sets the color code. */
    virtual void setColorCode(unsigned cc);
    /** Disables the color code for the roaming channel. */
    virtual void disableColorCode();

    /** Returns the time slot. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &name);

    /** Constructs a roaming channel from the given digital channel. */
    virtual bool fromChannel(const RoamingChannel *ch);
    /** Constructs a @c RoamingChannel instance for this roaming channel. */
    virtual RoamingChannel *toChannel(Context &ctx);

  public:
    /** Some limits. */
    struct Limit {
      static constexpr unsigned int nameLength() { return 16; }       ///< Maximum name length.
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int rxFrequency() { return 0x0000; }
      static constexpr unsigned int txFrequency() { return 0x0004; }
      static constexpr unsigned int colorCode()   { return 0x0008; }
      static constexpr unsigned int timeSlot()    { return 0x0009; }
      static constexpr unsigned int name()        { return 0x000a; }
      /// @endcond
    };
  };

  /** Represents the bitmap, indicating which roaming channel is valid. */
  class RoamingChannelBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    RoamingChannelBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    RoamingChannelBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }
  };

  /** Represents a roaming zone within the binary codeplug.
   *
   * Memory layout of roaming zone (0x80byte):
   * @verbinclude d878uv_roamingzone.txt */
  class RoamingZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RoamingZoneElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    RoamingZoneElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0080; }

    /** Clears the roaming zone. */
    void clear();

    /** Returns @c true if the n-th member is set. */
    virtual bool hasMember(unsigned n) const;
    /** Returns the n-th member index. */
    virtual unsigned member(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMember(unsigned n, unsigned idx);
    /** Clears the n-th member. */
    virtual void clearMember(unsigned n);

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Assembles a binary representation of the given RoamingZone instance.*/
    virtual bool fromRoamingZone(RoamingZone *zone, Context& ctx, const ErrorStack &err=ErrorStack());
    /** Constructs a @c RoamingZone instance from this configuration. */
    virtual RoamingZone *toRoamingZone(Context& ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the given RoamingZone. */
    virtual bool linkRoamingZone(RoamingZone *zone, Context& ctx, const ErrorStack& err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      static constexpr unsigned int nameLength() { return 16; }          ///< Maximum name length.
      static constexpr unsigned int numMembers() { return 64; }          ///< Maximum number of roaming channel in zone.
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()        { return 0x0000; }
      static constexpr unsigned int betweenMembers() { return 0x0001; }
      static constexpr unsigned int name()           { return 0x0040; }
      /// @endcond
    };
  };

  /** Represents the bitmap, indicating which roaming zone is valid. */
  class RoamingZoneBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    RoamingZoneBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    RoamingZoneBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0010; }
  };

  /** Represents an AES encryption key.
   *
   * Binary representation of the key (size 0x0040 bytes):
   * @verbinclude d878uv_aeskey.txt */
  class AESEncryptionKeyElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AESEncryptionKeyElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    AESEncryptionKeyElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0040; }

    /** Resets the key. */
    void clear();

    /** Returns @c true if the key is set. */
    bool isValid() const;

    /** Returns the key index. */
    virtual unsigned index() const;
    /** Sets the key index. */
    virtual void setIndex(unsigned idx);

    /** Returns the actual key. */
    virtual QByteArray key() const;
    /** Sets the key. */
    virtual void setKey(const QByteArray &key);
  };

  /** Encodes the bitmap, indicating which zone is hidden. */
  class HiddenZoneBitmapElement: public BitmapElement
  {
  protected:
    /** Hidden constructor. */
    HiddenZoneBitmapElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    HiddenZoneBitmapElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0020; }
  };

  /** Encodes some information about the radio and firmware.
   *
   * Binary encoding of the info, size 0x0100 bytes:
   * @verbinclude d878uv_radioinfo.txt */
  class RadioInfoElement: public Element
  {
  public:
    /** Possible frequency ranges for the AT-D878UV. */
    enum FrequencyRange {
      RX_400_480_136_174_TX_400_480_136_174 = 0,
      RX_400_480_136_174_TX_400_480_136_174_STEP_12_5kHz = 1,
      RX_430_440_136_174_TX_430_440_136_174 = 2,
      RX_400_480_136_174_TX_430_440_144_146 = 3,
      RX_440_480_136_174_TX_440_480_136_174 = 4,
      RX_440_480_144_146_TX_440_480_144_146 = 5,
      RX_446_447_136_174_TX_446_447_136_174 = 6,
      RX_400_480_136_174_TX_420_450_136_174 = 7,
      RX_400_470_136_174_TX_400_470_136_174 = 8,
      RX_430_432_144_146_TX_430_432_144_146 = 9,
      RX_400_480_136_174_TX_430_450_144_148 = 10,
      RX_400_520_136_174_TX_400_520_136_174 = 11,
      RX_400_490_136_174_TX_400_490_136_174 = 12,
      RX_400_480_136_174_TX_403_470_136_174 = 13,
      RX_400_520_220_225_136_174_TX_400_520_220_225_136_174 = 14,
      RX_420_520_144_148_TX_420_520_144_148 = 15,
      RX_430_440_144_147_TX_430_440_144_147 = 16,
      RX_430_440_136_174_TX_136_174 = 17
    };

  protected:
    /** Hidden constructor. */
    RadioInfoElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit RadioInfoElement(uint8_t *ptr);

    /** Resets the info. */
    void clear();

    /** Returns @c true if full test is enabled.
     * @warning Do not enable, may brick device! */
    virtual bool fullTest() const;

    /** Returns the frequency range. */
    virtual FrequencyRange frequencyRange() const;
    /** Sets the frequency range. */
    virtual void setFrequencyRange(FrequencyRange range);

    /** Returns @c true if "international" is enabled. */
    virtual bool international() const;
    /** Enables/disables "international". */
    virtual void enableInternational(bool enable);

    /** Returns @c true if band select is enabled. */
    virtual bool bandSelect() const;
    /** Enables/disables band select. */
    virtual void enableBandSelect(bool enable);

    /** Returns the band-select password. */
    virtual QString bandSelectPassword() const;
    /** Sets the band-select password. */
    virtual void setBandSelectPassword(const QString &passwd);

    /** Returns the radio type. */
    virtual QString radioType() const;

    /** Returns the program password. */
    virtual QString programPassword() const;
    /** Sets the program password. */
    virtual void setProgramPassword(const QString &passwd);

    /** Returns the area code. */
    virtual QString areaCode() const;
    /** Returns the serial number. */
    virtual QString serialNumber() const;
    /** Returns the production date. */
    virtual QString productionDate() const;
    /** Returns the manufacturer code. */
    virtual QString manufacturerCode() const;
    /** Returns the maintained date. */
    virtual QString maintainedDate() const;
    /** Returns the dealer code. */
    virtual QString dealerCode() const;
    /** Returns the stock date. */
    virtual QString stockDate() const;
    /** Returns the sell date. */
    virtual QString sellDate() const;
    /** Returns the seller. */
    virtual QString seller() const;
    /** Returns the maintainer note. */
    virtual QString maintainerNote() const;
  };

protected:
  /** Hidden constructor. */
  explicit D878UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D878UVCodeplug(QObject *parent = nullptr);

protected:
  bool allocateBitmaps();
  void setBitmaps(Context &ctx);
  void allocateForDecoding();
  void allocateUpdated();
  void allocateForEncoding();

  bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateChannels();
  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  virtual void allocateZones();
  virtual bool encodeZone(int i, Zone *zone, bool isB, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  virtual bool decodeZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateGPSSystems();
  bool encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGPSSystems(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGPSSystems(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Allocates memory to store all roaming channels and zones. */
  virtual void allocateRoaming();
  /** Encodes the roaming channels and zones. */
  virtual bool encodeRoaming(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Creates roaming channels and zones from codeplug. */
  virtual bool createRoaming(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links roaming channels and zones. */
  virtual bool linkRoaming(Context &ctx, const ErrorStack &err=ErrorStack());

public:
  /** Some limits. */
  struct Limit: public D868UVCodeplug::Limit {
    static constexpr unsigned int analogAPRSRXEntries() { return 32; }   ///< Maximum number of analog APRS RX entries.
    static constexpr unsigned int roamingChannels()     { return 250; }  ///< Maximum number of roaming channels.
    static constexpr unsigned int roamingZones()        { return 64; }   ///< Maximum number of roaming zones.
    static constexpr unsigned int aesKeys()             { return 256; }  ///< Maximum number of AES keys.
  };

protected:
  /** Internal offsets within the codeplug. */
  struct Offset: public D868UVCodeplug::Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int settingsExtension()           { return 0x02501400; }
    static constexpr unsigned int analogAPRSSettings()          { return 0x02501000; }
    static constexpr unsigned int analogAPRSSettingsExtension() { return 0x025010A0; }
    static constexpr unsigned int analogAPRSMessage()           { return 0x02501200; }
    static constexpr unsigned int analogAPRSRXEntries()         { return 0x02501800; }
    static constexpr unsigned int dmrAPRSSettings()             { return 0x02501040; }
    static constexpr unsigned int dmrAPRSMessage()              { return 0x02501280; }
    static constexpr unsigned int hiddenZoneBitmap()            { return 0x024c1360; }
    static constexpr unsigned int roamingChannelBitmap()        { return 0x01042000; }
    static constexpr unsigned int roamingChannels()             { return 0x01040000; }
    static constexpr unsigned int roamingZoneBitmap()           { return 0x01042080; }
    static constexpr unsigned int roamingZones()                { return 0x01043000; }
    static constexpr unsigned int aesKeys()                     { return 0x024C4000; }
    /// @endcond
  };
};

#endif // D878UVCODEPLUG_HH
