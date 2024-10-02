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

protected:
  /** Colors supported by the D868UVE. */
  struct Color {
  public:
    /** Maps code -> color. */
    static AnytoneDisplaySettingsExtension::Color decode(uint8_t code);
    /** Maps color -> code. */
    static uint8_t encode(AnytoneDisplaySettingsExtension::Color color);

  protected:
    /** Encoding of the supported colors. */
    typedef enum {
      White = 0, Red=1
    } CodedColor;
  };


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

  protected:
    /** Internal used offsets within the channel element. */
    struct Offset: public AnytoneCodeplug::ChannelElement::Offset {
      /// @todo Implement
    };
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
    /** Device specific key functions. */
    struct KeyFunction {
    public:
      /** Encodes key function. */
      static uint8_t encode(AnytoneKeySettingsExtension::KeyFunction tone);
      /** Decodes key function. */
      static AnytoneKeySettingsExtension::KeyFunction decode(uint8_t code);

    protected:
      /** Encoded key functions. */
      typedef enum {
        Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
        Encryption = 0x05, Call = 0x06, VOX = 0x07, ToggleVFO = 0x08, SubPTT = 0x09,
        Scan = 0x0a, WFM = 0x0b, Alarm = 0x0c, RecordSwitch = 0x0d, Record = 0x0e, SMS = 0x0f,
        Dial = 0x10, GPSInformation = 0x11, Monitor = 0x12, ToggleMainChannel = 0x13, HotKey1 = 0x14,
        HotKey2 = 0x15, HotKey3 = 0x16, HotKey4 = 0x17, HotKey5 = 0x18, HotKey6 = 0x19,
        WorkAlone = 0x1a, SkipChannel = 0x1b, DMRMonitor = 0x1c, SubChannel = 0x1d,
        PriorityZone = 0x1e, VFOScan = 0x1f, MICSoundQuality = 0x20, LastCallReply = 0x21,
        ChannelType = 0x22, Ranging = 0x23, ChannelRanging = 0x24, MaxVolume = 0x25, Slot = 0x26
      } KeyFunctionCode;
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00d0; }

    /** Resets the general settings. */
    void clear();

    /** Returns the power-save mode. */
    virtual AnytonePowerSaveSettingsExtension::PowerSave powerSave() const;
    /** Sets the power-save mode. */
    virtual void setPowerSave(AnytonePowerSaveSettingsExtension::PowerSave mode);

    /** Returns the VOX level. */
    virtual unsigned voxLevel() const;
    /** Sets the VOX level. */
    virtual void setVOXLevel(unsigned level);
    /** Returns the VOX delay in ms. */
    virtual Interval voxDelay() const;
    /** Sets the VOX delay in ms. */
    virtual void setVOXDelay(Interval ms);
    /** Returns the VOX source. */
    virtual AnytoneAudioSettingsExtension::VoxSource voxSource() const;
    /** Sets the VOX source. */
    virtual void setVOXSource(AnytoneAudioSettingsExtension::VoxSource source);
    unsigned int dmrMicGain() const;
    void setDMRMicGain(unsigned int gain);
    unsigned maxSpeakerVolume() const;
    void setMaxSpeakerVolume(unsigned level);
    /** Returns the maximum headphone volume. */
    virtual unsigned maxHeadPhoneVolume() const;
    /** Sets the maximum headphone volume. */
    virtual void setMaxHeadPhoneVolume(unsigned max);
    bool enhanceAudio() const;
    void enableEnhancedAudio(bool enable);
    bool recording() const;
    void enableRecording(bool enable);
    /** Returns the recording delay in ms. */
    virtual unsigned recordingDelay() const;
    /** Sets the recording delay in ms. */
    virtual void setRecodringDelay(unsigned ms);


    AnytoneSettingsExtension::VFOScanType vfoScanType() const;
    void setVFOScanType(AnytoneSettingsExtension::VFOScanType type);
    Frequency minVFOScanFrequencyUHF() const;
    void setMinVFOScanFrequencyUHF(Frequency hz);
    Frequency maxVFOScanFrequencyUHF() const;
    void setMaxVFOScanFrequencyUHF(Frequency hz);
    Frequency minVFOScanFrequencyVHF() const;
    void setMinVFOScanFrequencyVHF(Frequency hz);
    Frequency maxVFOScanFrequencyVHF() const;
    void setMaxVFOScanFrequencyVHF(Frequency hz);

    AnytoneKeySettingsExtension::KeyFunction funcKeyAShort() const;
    void setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBShort() const;
    void setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCShort() const;
    void setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey1Short() const;
    void setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Short() const;
    void setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyALong() const;
    void setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBLong() const;
    void setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCLong() const;
    void setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey1Long() const;
    void setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Long() const;
    void setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func);
    Interval longPressDuration() const;
    void setLongPressDuration(Interval ms);

    bool vfoModeA() const;
    void enableVFOModeA(bool enable);
    bool vfoModeB() const;
    void enableVFOModeB(bool enable);

    unsigned memoryZoneA() const;
    void setMemoryZoneA(unsigned zone);
    unsigned memoryZoneB() const;
    void setMemoryZoneB(unsigned zone);

    bool gps() const;
    void enableGPS(bool enable);
    QTimeZone gpsTimeZone() const;
    void setGPSTimeZone(const QTimeZone &zone);
    bool getGPSPosition() const;
    void enableGetGPSPosition(bool enable);
    bool gpsUnitsImperial() const ;
    void enableGPSUnitsImperial(bool enable);
    /** Returns the GPS update period in seconds. */
    virtual Interval gpsUpdatePeriod() const;
    /** Sets the GPS update period in seconds. */
    virtual void setGPSUpdatePeriod(Interval sec);

    bool keyToneEnabled() const;
    void enableKeyTone(bool enable);
    bool smsAlert() const;
    void enableSMSAlert(bool enable);
    bool callAlert() const;
    void enableCallAlert(bool enable);
    bool dmrTalkPermit() const;
    void enableDMRTalkPermit(bool enable);
    bool fmTalkPermit() const;
    void enableFMTalkPermit(bool enable);
    bool dmrResetTone() const;
    void enableDMRResetTone(bool enable);
    bool idleChannelTone() const;
    void enableIdleChannelTone(bool enable);
    bool startupTone() const;
    void enableStartupTone(bool enable);
    void callToneMelody(Melody &melody) const;
    void setCallToneMelody(const Melody &melody);
    void idleToneMelody(Melody &melody) const;
    void setIdleToneMelody(const Melody &melody);
    void resetToneMelody(Melody &melody) const;
    void setResetToneMelody(const Melody &melody);

    bool activeChannelB() const;
    void enableActiveChannelB(bool enable);

    bool subChannel() const;
    void enableSubChannel(bool enable);

    Interval menuExitTime() const;
    void setMenuExitTime(Interval intv);

    bool callEndPrompt() const;
    void enableCallEndPrompt(bool enable);
    bool volumeChangePrompt() const;
    void enableVolumeChangePrompt(bool enable);
    AnytoneDisplaySettingsExtension::LastCallerDisplayMode lastCallerDisplayMode() const;
    void setLastCallerDisplayMode(AnytoneDisplaySettingsExtension::LastCallerDisplayMode mode);
    bool displayClock() const;
    void enableDisplayClock(bool enable);
    bool displayCall() const;
    void enableDisplayCall(bool enable);
    AnytoneDisplaySettingsExtension::Color callDisplayColor() const;
    void setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color);
    bool showLastHeard() const;
    void enableShowLastHeard(bool enable);
    unsigned brightness() const;
    void setBrightness(unsigned level);
    /** Returns @c true if the backlight is always on. */
    virtual bool backlightPermanent() const;
    /** Returns the backlight duration in seconds. */
    virtual Interval rxBacklightDuration() const;
    /** Sets the backlight duration in seconds. */
    virtual void setRXBacklightDuration(Interval sec);
    /** Sets the backlight to permanent (always on). */
    virtual void enableBacklightPermanent();

    AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionA() const;
    void setAutoRepeaterDirectionA(AnytoneAutoRepeaterSettingsExtension::Direction dir);
    AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionB() const;
    void setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir);
    bool hasAutoRepeaterOffsetFrequencyIndexUHF() const;
    unsigned autoRepeaterOffsetFrequencyIndexUHF() const;
    void setAutoRepeaterOffsetFrequenyIndexUHF(unsigned idx);
    void clearAutoRepeaterOffsetFrequencyIndexUHF();
    bool hasAutoRepeaterOffsetFrequencyIndexVHF() const;
    unsigned autoRepeaterOffsetFrequencyIndexVHF() const;
    void setAutoRepeaterOffsetFrequenyIndexVHF(unsigned idx);
    void clearAutoRepeaterOffsetFrequencyIndexVHF();
    Frequency autoRepeaterMinFrequencyVHF() const;
    void setAutoRepeaterMinFrequencyVHF(Frequency Hz);
    Frequency autoRepeaterMaxFrequencyVHF() const;
    void setAutoRepeaterMaxFrequencyVHF(Frequency Hz);
    Frequency autoRepeaterMinFrequencyUHF() const;
    void setAutoRepeaterMinFrequencyUHF(Frequency Hz);
    Frequency autoRepeaterMaxFrequencyUHF() const;
    void setAutoRepeaterMaxFrequencyUHF(Frequency Hz);

    bool showCurrentContact() const;
    void enableShowCurrentContact(bool enable);

    /** Returns @c true if the key-tone level is adjustable. */
    virtual bool keyToneLevelAdjustable() const;
    /** Returns the key-tone level (0=adjustable). */
    virtual unsigned keyToneLevel() const;
    /** Sets the key-tone level. */
    virtual void setKeyToneLevel(unsigned level);
    /** Sets the key-tone level adjustable. */
    virtual void setKeyToneLevelAdjustable();

    bool knobLock() const;
    void enableKnobLock(bool enable);
    bool keypadLock() const;
    void enableKeypadLock(bool enable);
    bool sidekeysLock() const;
    void enableSidekeysLock(bool enable);
    bool keyLockForced() const;
    void enableKeyLockForced(bool enable);

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

    bool keepLastCaller() const;
    void enableKeepLastCaller(bool enable);

    bool fromConfig(const Flags &flags, Context &ctx);
    bool updateConfig(Context &ctx);
    bool linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some internal used offsets within the element. */
    struct Offset: public AnytoneCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int enableKeyTone()     { return 0x0000; }
      static constexpr unsigned int powerSaveMode()     { return 0x000b; }
      static constexpr unsigned int voxLevel()          { return 0x000c; }
      static constexpr unsigned int voxDelay()          { return 0x000d; }
      static constexpr unsigned int vfoScanType()       { return 0x000e; }
      static constexpr unsigned int dmrMicGain()        { return 0x000f; }
      static constexpr unsigned int progFuncKeyAShort() { return 0x0010; }
      static constexpr unsigned int progFuncKeyBShort() { return 0x0011; }
      static constexpr unsigned int progFuncKeyCShort() { return 0x0012; }
      static constexpr unsigned int progFuncKey1Short() { return 0x0013; }
      static constexpr unsigned int progFuncKey2Short() { return 0x0014; }
      static constexpr unsigned int vfoModeA()          { return 0x0015; }
      static constexpr unsigned int vfoModeB()          { return 0x0016; }
      static constexpr unsigned int memZoneA()          { return 0x001f; }
      static constexpr unsigned int memZoneB()          { return 0x0020; }
      static constexpr unsigned int enableRecoding()    { return 0x0022; }
      static constexpr unsigned int displayBrightness() { return 0x0026; }
      static constexpr unsigned int backlightDuration() { return 0x0027; }
      static constexpr unsigned int gpsEnable()         { return 0x0028; }
      static constexpr unsigned int smsAlert()          { return 0x0029; }
      static constexpr unsigned int activeChannelB()    { return 0x002c; }
      static constexpr unsigned int subChannel()        { return 0x002d; }
      static constexpr unsigned int callAlert()         { return 0x002f; }
      static constexpr unsigned int gpsTimeZone()       { return 0x0030; }
      static constexpr unsigned int talkPermit()        { return 0x0031; }
      static constexpr unsigned int dmrResetTone()      { return 0x0032; }
      static constexpr unsigned int voxSource()         { return 0x0033; }
      static constexpr unsigned int idleChannelTone()   { return 0x0036; }
      static constexpr unsigned int menuExitTime()      { return 0x0037; }
      static constexpr unsigned int startupTone()       { return 0x0039; }
      static constexpr unsigned int callEndPrompt()     { return 0x003a; }
      static constexpr unsigned int maxSpeakerVolume()  { return 0x003b; }
      static constexpr unsigned int getGPSPosition()    { return 0x003f; }
      static constexpr unsigned int progFuncKeyALong()  { return 0x0041; }
      static constexpr unsigned int progFuncKeyBLong()  { return 0x0042; }
      static constexpr unsigned int progFuncKeyCLong()  { return 0x0043; }
      static constexpr unsigned int progFuncKey1Long()  { return 0x0044; }
      static constexpr unsigned int progFuncKey2Long()  { return 0x0045; }
      static constexpr unsigned int longPressDuration() { return 0x0046; }
      static constexpr unsigned int volumeChangePrompt(){ return 0x0047; }
      static constexpr unsigned int autoRepeaterDirA()  { return 0x0048; }
      static constexpr unsigned int lastCallerDisplay() { return 0x004d; }
      static constexpr unsigned int showClock()         { return 0x0051; }
      static constexpr unsigned int maxHeadPhoneVolume(){ return 0x0052; }
      static constexpr unsigned int enhanceAudio()      { return 0x0057; }
      static constexpr unsigned int minVFOScanUHF()     { return 0x0058; }
      static constexpr unsigned int maxVFOScanUHF()     { return 0x005c; }
      static constexpr unsigned int minVFOScanVHF()     { return 0x0060; }
      static constexpr unsigned int maxVFOScanVHF()     { return 0x0064; }
      static constexpr unsigned int autoRepOffsetUHF()  { return 0x0068; }
      static constexpr unsigned int autoRepOffsetVHF()  { return 0x0069; }
      static constexpr unsigned int callToneTones()     { return 0x0072; }
      static constexpr unsigned int callToneDurations() { return 0x007c; }
      static constexpr unsigned int idleToneTones()     { return 0x0086; }
      static constexpr unsigned int idleToneDurations() { return 0x0090; }
      static constexpr unsigned int resetToneTones()    { return 0x009a; }
      static constexpr unsigned int resetToneDurations(){ return 0x00a4; }
      static constexpr unsigned int recordingDelay()    { return 0x00ae; }
      static constexpr unsigned int callDisplayMode()   { return 0x00af; }
      static constexpr unsigned int callColor()         { return 0x00b0; }
      static constexpr unsigned int gpsPeriod()         { return 0x00b1; }
      static constexpr unsigned int showCurrentContact(){ return 0x00b2; }
      static constexpr unsigned int keyToneLevel()      { return 0x00b3; }
      static constexpr unsigned int gpsUnits()          { return 0x00b4; }
      static constexpr unsigned int knobLock()          { return 0x00b5; }
      static constexpr unsigned int keypadLock()        { return 0x00b5; }
      static constexpr unsigned int sideKeyLock()       { return 0x00b5; }
      static constexpr unsigned int forceKeyLock()      { return 0x00b5; }
      static constexpr unsigned int showLastHeard()     { return 0x00b6; }
      static constexpr unsigned int autoRepMinVHF()     { return 0x00b8; }
      static constexpr unsigned int autoRepMaxVHF()     { return 0x00bc; }
      static constexpr unsigned int autoRepMinUHF()     { return 0x00c0; }
      static constexpr unsigned int autoRepMaxUHF()     { return 0x00c4; }
      static constexpr unsigned int autoRepeaterDirB()  { return 0x00c8; }
      static constexpr unsigned int defaultChannels()   { return 0x00ca; }
      static constexpr unsigned int defaultZoneA()      { return 0x00cb; }
      static constexpr unsigned int defaultZoneB()      { return 0x00cc; }
      static constexpr unsigned int defaultChannelA()   { return 0x00cd; }
      static constexpr unsigned int defaultChannelB()   { return 0x00ce; }
      static constexpr unsigned int keepLastCaller()    { return 0x00cf; }
      /// @endcond
    };
  };

protected:
  /** Hidden constructor constructor. */
  explicit D868UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D868UVCodeplug(QObject *parent = nullptr);

protected:
  bool allocateBitmaps();
  virtual void setBitmaps(Context &ctx);
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
  virtual bool encodeZone(int i, Zone *zone, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create zones from codeplug. */
  virtual bool createZones(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Function to decode a single zone. */
  virtual bool decodeZone(int i, Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack());
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
  /** Link the general settings. */
  virtual bool linkGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

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
  /** Encodes prefab SMS messages. */
  virtual bool encodeSMSMessages(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Create SMS messages from codeplug. */
  virtual bool createSMSMessages(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Link SMS messages. */
  virtual bool linkSMSMessages(Context &ctx, const ErrorStack &err=ErrorStack());


  /** Allocates hot key settings memory section. */
  virtual void allocateHotKeySettings();

  /** Allocates repeater offset settings memory section. */
  virtual void allocateRepeaterOffsetFrequencies();
  /** Encodes auto-repeater offset frequencies. */
  virtual bool encodeRepeaterOffsetFrequencies(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  /** Decodes auto-repeater offset frequencies. */
  virtual bool decodeRepeaterOffsetFrequencies(Context &ctx, const ErrorStack &err=ErrorStack());

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

public:
  /** Some limits for the codeplug. */
  struct Limit {
    static constexpr unsigned int channelsPerBank()     { return 128; }   ///< Max number of channels per bank.
    static constexpr unsigned int numChannels()         { return 4000; }  ///< Max number of channels.
    static constexpr unsigned int contactsPerBank()     { return 1000; }  ///< Max number of contacts per bank.
    static constexpr unsigned int contactsPerBlock()    { return 4; }     ///< Max number of contacts per block.
    static constexpr unsigned int numContacts()         { return 10000; } ///< Max number of contacts.
    static constexpr unsigned int numDTMFContacts()     { return 128; }   ///< Max number of DTMF contacts.
    static constexpr unsigned int numGroupLists()       { return 250; }   ///< Max number of group lists.
    static constexpr unsigned int numScanLists()        { return 250; }   ///< Max number of scan lists.
    static constexpr unsigned int numScanListsPerBank() { return 16; }    ///< Max number of scan lists per bank.
    static constexpr unsigned int numRadioIDs()         { return 250; }   ///< Max number of radio IDs.
    // There is no zone element -> hence all attributes must be defied within the codeplug.
    static constexpr unsigned int numZones()            { return 250; }   ///< Max number of zones.
    static constexpr unsigned int numChannelsPerZone()  { return 250; }   ///< Max number of channels per zone.
    static constexpr unsigned int zoneNameLength()      { return 16; }    ///< Max zone name length.
    static constexpr unsigned int dmrAPRSSystems()      { return 8; }     ///< Max number of DMR APRS systems.
    static constexpr unsigned int numMessages()         { return 100; }   ///< Max number of preset SMS.
    static constexpr unsigned int numMessagePerBank()   { return 8; }     ///< Max number of SMS per bank.
    static constexpr unsigned int numTwoToneIDs()       { return 24; }    ///< Max number of two-tone IDs.
    static constexpr unsigned int numTwoToneFunctions() { return 16; }    ///< Max number of two-tone functions.
  };

protected:
  /** Some internal used offsets within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int channelBitmap()        { return 0x024c1500; }
    static constexpr unsigned int channelBanks()         { return 0x00800000; }
    static constexpr unsigned int betweenChannelBanks()  { return 0x00040000; }
    static constexpr unsigned int vfoA()                 { return 0x00fc0800; }
    static constexpr unsigned int vfoB()                 { return 0x00fc0840; }

    static constexpr unsigned int contactBitmap()        { return 0x02640000; }
    static constexpr unsigned int contactBanks()         { return 0x02680000; }
    static constexpr unsigned int betweenContactBanks()  { return 0x00040000; }
    static constexpr unsigned int betweenContactBlocks() { return 0x00000190; }
    static constexpr unsigned int contactIndex()         { return 0x02600000; }
    static constexpr unsigned int contactIdTable()       { return 0x04340000; }

    static constexpr unsigned int dtmfContactBytemap()   { return 0x02900100; }
    static constexpr unsigned int dtmfContacts()         { return 0x02940000; }
    static constexpr unsigned int dtmfIndex()            { return 0x02900000; }
    static constexpr unsigned int dtmfSettings()         { return 0x024C1080; }
    static constexpr unsigned int dtmfIDList()           { return 0x02500500; }

    static constexpr unsigned int groupListBitmap()      { return 0x025C0B10; }
    static constexpr unsigned int groupLists()           { return 0x02980000; }
    static constexpr unsigned int betweenGroupLists()    { return 0x00000200; }

    static constexpr unsigned int scanListBitmap()       { return 0x024c1340; }
    static constexpr unsigned int scanListBanks()        { return 0x01080000; }
    static constexpr unsigned int betweenScanLists()     { return 0x00000200; }
    static constexpr unsigned int betweenScanListBanks() { return 0x00040000; }

    static constexpr unsigned int radioIDBitmap()        { return 0x024c1320; }
    static constexpr unsigned int radioIDs()             { return 0x02580000; }

    static constexpr unsigned int settings()             { return 0x02500000; }
    static constexpr unsigned int bootSettings()         { return 0x02500600; }
    static constexpr unsigned int aprsSettings()         { return 0x02501000; }
    static constexpr unsigned int dmrAPRSMessage()       { return 0x02501100; }
    static constexpr unsigned int zoneChannelList()      { return 0x02500100; }
    static constexpr unsigned int offsetFrequencies()    { return 0x024C2000; }

    static constexpr unsigned int zoneBitmap()           { return 0x024c1300; }
    static constexpr unsigned int zoneChannels()         { return 0x01000000; }
    static constexpr unsigned int betweenZoneChannels()  { return 0x00000200; }
    static constexpr unsigned int zoneNames()            { return 0x02540000; }
    static constexpr unsigned int betweenZoneNames()     { return 0x00000020; }

    static constexpr unsigned int messageBytemap()       { return 0x01640800; }
    static constexpr unsigned int messageBanks()         { return 0x02140000; }
    static constexpr unsigned int betweenMessageBanks()  { return 0x00040000; }
    static constexpr unsigned int messageIndex()         { return 0x01640000; }

    static constexpr unsigned int analogQuickCall()      { return 0x025C0000; }
    static constexpr unsigned int statusMessageBitmap()  { return 0x025C0B00; }
    static constexpr unsigned int statusMessages()       { return 0x025C0100; }
    static constexpr unsigned int hotKeySettings()       { return 0x025C0500; }

    static constexpr unsigned int alarmSettings()        { return 0x024C1400; }
    static constexpr unsigned int alarmSettingsExtension() { return 0x024c1440; }

    static constexpr unsigned int fiveToneIdBitmap()     { return 0x024C0C80; }
    static constexpr unsigned int fiveToneIdList()       { return 0x024C0000; }
    static constexpr unsigned int fiveToneFunctions()    { return 0x024C0D00; }
    static constexpr unsigned int fiveToneSettings()     { return 0x024C1000; }

    static constexpr unsigned int twoToneIdBitmap()      { return 0x024C1280; }
    static constexpr unsigned int twoToneIdList()        { return 0x024C1100; }
    static constexpr unsigned int twoToneFunctionBitmap(){ return 0x024c2600; }
    static constexpr unsigned int twoToneFunctionList()  { return 0x024c2400; }
    static constexpr unsigned int twoToneSettings()      { return 0x024C1290; }

    static constexpr unsigned int wfmChannelBitmap()     { return 0x02480210; }
    static constexpr unsigned int wfmChannels()          { return 0x02480000; }
    static constexpr unsigned int wfmVFO()               { return 0x02480200; }

    static constexpr unsigned int dmrEncryptionIDs()     { return 0x024C1700; }
    static constexpr unsigned int dmrEncryptionKeys()    { return 0x024C1800; }
    /// @endcond
  };

  /** Internal used sizes. Usually sizes are specified by static methods of the element, however,
   * some stuff is not represented by Elements in the AnyTone codeplug, hence this table is needed. */
  struct Size {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int zoneName()             { return 0x0020; }
    static constexpr unsigned int zoneChannels()         { return 0x0200; }
    static constexpr unsigned int messageBank()          { return 0x0800; }
    static constexpr unsigned int messageIndex()         { return 0x0010; }
    /// @endcond
  };
};

#endif // D868UVCODEPLUG_HH
