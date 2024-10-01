#ifndef DMR6X2UVCODEPLUG_HH
#define DMR6X2UVCODEPLUG_HH

#include "d878uv_codeplug.hh"
#include "ranges.hh"


/** Represents the device specific binary codeplug for BTECH DMR-6X2UV radios.
 *
 * This codeplug implementation is compatible with firmware revision 2.04.
 *
 * @section dmr6x2uvcpl Codeplug structure within radio
 * <table>
 *  <tr><th colspan="3">Channels</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1500</td> <td>000200</td>      <td>Bitmap of 4000 channels, default 0x00,
 *    0x00 padded.</td></tr>
 *  <tr><td>00800000</td> <td>max. 002000</td> <td>Channel bank 0 of up to 128 channels,
 *    see @c AnytoneCodeplug::ChannelElement 64 b each. </td></tr>
 *  <tr><td>00840000</td> <td>max. 002000</td> <td>Channel bank 1 of up to 128 channels.</td></tr>
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
 *    0-based, little endian, default/padded=0xffff. Offset between channel lists 0x200,
 *    size of each list 0x1f4.</td></tr>
 *  <tr><td>02540000</td> <td>max. 001f40</td> <td>250 Zone names.
 *    Each zone name is up to 16 ASCII chars long and gets 0-padded to 32b.</td></tr>
 *  <tr><td>02500100</td> <td>000400</td> <td>Zone A & B channel list.</td></tr>
 *
 *  <tr><th colspan="3">Roaming</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>01042000</td> <td>000020</td>      <td>Roaming channel bitmask, up to 250 bits,
 *    0-padded, default 0.</td></tr>
 *  <tr><td>01040000</td> <td>max. 0x1f40</td> <td>Optional up to 250 roaming channels, of 32b each.
 *    See @c D868UVCodeplug::RoamingChannelElement for details.</td></tr>
 *  <tr><td>01042080</td> <td>000010</td>      <td>Roaming zone bitmask, up to 64 bits, 0-padded,
 *    default 0.</td></tr>
 *  <tr><td>01043000</td> <td>max. 0x2000</td> <td>Optional up to 64 roaming zones, of 128b each.
 *    See @c D868UVCodeplug::RoamingZoneElement for details.</td></tr>
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
 *  <tr><td>02501000</td> <td>0000A0</td> <td>APRS settings (DMR + FM),
 *    see @c DMR6X2UVCodeplug::APRSSettingsElement.</td>
 *  <tr><td>02501200</td> <td>000040</td> <td>APRS Text, up to 60 chars ASCII, 0-padded.</td>
 *  <tr><td>02501280</td> <td>000030</td> <td>GPS template message, ASCII, 0-padded.</td> </tr>
 *  <tr><td>02502000</td> <td>000080</td> <td>FM APRS frequency names,
 *    see @c FMAPRSFrequencyNamesElement. This element is not part of the manufacturer codeplug.
 *    QDMR uses this memory section to store additional information.</td></tr>
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>0000e0</td> <td>General settings,
 *    see @c DMR6X2UVCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>02500500</td> <td>000100</td> <td>DTMF list</td></tr>
 *  <tr><td>02500600</td> <td>000030</td> <td>Power on settings,
 *    see @c AnytoneCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>02501400</td> <td>000030</td> <td>Settings extension, see @c DMR6X2UVCodeplug::ExtendedSettingsElement.</td> </tr>
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
 *  <tr><td>025C1000</td> <td>004000</td> <td>Up to 256 AES encryption keys.
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
 *  <tr><td>024C2610</td> <td>000020</td> <td>Unknown bitmap.</td> </tr>
 *  <tr><td>024C2630</td> <td>000020</td> <td>Unknown bitmap.</td> </tr>
 *  <tr><td>024C3000</td> <td>000020</td> <td>Unknown settings.</td> </tr>
 *  <tr><td>024C5000</td> <td>000020</td> <td>Unknown settings.</td> </tr>
 * </table>
 *
 * @ingroup dmr6x2uv */
class DMR6X2UVCodeplug : public D868UVCodeplug
{
  Q_OBJECT

protected:
  /** Colors supported by the DMR-6X2UV. */
  struct Color {
  public:
    /** Maps code -> color. */
    static AnytoneDisplaySettingsExtension::Color decode(uint8_t code);
    /** Maps color -> code. */
    static uint8_t encode(AnytoneDisplaySettingsExtension::Color color);

  protected:
    /** Encoding of the supported colors. */
    typedef enum {
      Orange=0, Red=1, Yellow=2, Green=3, Turquoise=4, Blue=5, White = 6, Black = 7
    } CodedColor;
  };

  /** Background colors supported by the DMR-6X2UV. */
  struct BackgroundColor {
  public:
    /** Maps code -> color. */
    static AnytoneDisplaySettingsExtension::Color decode(uint8_t code);
    /** Maps color -> code. */
    static uint8_t encode(AnytoneDisplaySettingsExtension::Color color);

  protected:
    /** Encoding of the supported colors. */
    typedef enum {
      Black = 0, Blue = 1
    } CodedColor;
  };

  /** Font colors supported by the DMR-6X2UV. */
  struct FontColor {
  public:
    /** Maps code -> color. */
    static AnytoneDisplaySettingsExtension::Color decode(uint8_t code);
    /** Maps color -> code. */
    static uint8_t encode(AnytoneDisplaySettingsExtension::Color color);

  protected:
    /** Encoding of the supported colors. */
    typedef enum {
      White = 0, Black = 1, Orange=2, Red=3, Yellow=4, Green=5, Turquoise=6, Blue=7
    } CodedColor;
  };

public:
  /** General settings element for the DMR-6X2UV.
   *
   * Extends the @c AnytoneCodeplug::GeneralSettingsElement by the device specific settings for
   * the BTECH DMR-6X2UV.
   *
   * Memory representation of the encoded settings element (size 0x0e0 bytes):
   * @verbinclude dmr6x2uv_generalsettings.txt */
  class GeneralSettingsElement: public D868UVCodeplug::GeneralSettingsElement
  {
  protected:
    /** Device specific encoding of the key functions. */
    struct KeyFunction {
    public:
      /** Encodes key function. */
      static uint8_t encode(AnytoneKeySettingsExtension::KeyFunction tone);
      /** Decodes key function. */
      static AnytoneKeySettingsExtension::KeyFunction decode(uint8_t code);

    protected:
      /** Device specific key functions. */
      typedef enum {
        Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
        Encryption = 0x05, Call = 0x06, VOX = 0x07, ToggleVFO = 0x08, SubPTT = 0x09,
        Scan = 0x0a, WFM = 0x0b, Alarm = 0x0c, RecordSwitch = 0x0d, Record = 0x0e, SMS = 0x0f,
        Dial = 0x10, GPSInformation = 0x11, Monitor = 0x12, ToggleMainChannel = 0x13, HotKey1 = 0x14,
        HotKey2 = 0x15, HotKey3 = 0x16, HotKey4 = 0x17, HotKey5 = 0x18, HotKey6 = 0x19,
        WorkAlone = 0x1a, SkipChannel = 0x1b, DMRMonitor = 0x1c, SubChannel = 0x1d,
        PriorityZone = 0x1e, VFOScan = 0x1f, MICSoundQuality = 0x20, LastCallReply = 0x21,
        ChannelType = 0x22, SimplexRepeater = 0x23, Ranging = 0x24, ChannelRanging = 0x25,
        MaxVolume = 0x26, Slot = 0x27, Squelch = 0x28, Roaming = 0x29, Zone = 0x2a, RoamingSet = 0x2b,
        Mute = 0x02c, CtcssDcsSet=0x2d, APRSSet = 0x2e, APRSSend = 0x2f
      } KeyFunctionCode;
    };

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
    /** Hidden Constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00e0; }

    bool idleChannelTone() const;
    void enableIdleChannelTone(bool enable);

    /** Returns the transmit timeout in seconds. */
    virtual unsigned transmitTimeout() const;
    /** Sets the transmit timeout in seconds. */
    virtual void setTransmitTimeout(unsigned tot);

    /** Returns the UI language. */
    virtual AnytoneDisplaySettingsExtension::Language language() const;
    /** Sets the UI language. */
    virtual void setLanguage(AnytoneDisplaySettingsExtension::Language lang);

    /** Returns the VFO frequency step in kHz. */
    virtual Frequency vfoFrequencyStep() const;
    /** Sets the VFO frequency step in kHz. */
    virtual void setVFOFrequencyStep(Frequency kHz);

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

    bool vfoModeA() const;
    void enableVFOModeA(bool enable);
    bool vfoModeB() const;
    void enableVFOModeB(bool enable);

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

    bool keyToneEnabled() const;
    void enableKeyTone(bool enable);

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

    /** Returns @c true, if the selection of a TX contact is enabled. */
    virtual bool selectTXContactEnabled() const;
    /** Enables/disables selection of the TX contact. */
    virtual void enableSelectTXContact(bool enable);

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

    /** Returns @c true, if a SMS confirmation is sent. */
    virtual bool smsConfirmEnabled() const;
    /** Enables/disables SMS confirmation. */
    virtual void enableSMSConfirm(bool enable);

    /** Returns @c true if the simplex repeater feature is enabled. */
    virtual bool simplexRepeaterEnabled() const;
    /** Enables disables the simplex repeater feature. */
    virtual void enableSimplexRepeater(bool enable);

    Interval gpsUpdatePeriod() const;
    void setGPSUpdatePeriod(Interval sec);

    /** Returns @c true if the speaker is switched on during RX in simplex repeater mode,
     * see @c simplexRepeaterEnabled. */
    virtual bool monitorSimplexRepeaterEnabled() const;
    /** Enables/disables the speaker during RX in simplex repeater mode. */
    virtual void enableMonitorSimplexRepeater(bool enable);

    bool showCurrentContact() const;
    void enableShowCurrentContact(bool enable);

    bool keyToneLevelAdjustable() const;
    unsigned keyToneLevel() const;
    void setKeyToneLevel(unsigned level);
    void setKeyToneLevelAdjustable();

    bool knobLock() const;
    void enableKnobLock(bool enable);
    bool keypadLock() const;
    void enableKeypadLock(bool enable);
    bool sidekeysLock() const;
    void enableSidekeysLock(bool enable);
    bool keyLockForced() const;
    void enableKeyLockForced(bool enable);

    /** Returns the time-slot in simplex repeater mode. */
    virtual AnytoneSimplexRepeaterSettingsExtension::TimeSlot simplexRepeaterTimeslot() const;
    /** Sets the time-slot in simplex repeater mode. */
    virtual void setSimplexRepeaterTimeslot(AnytoneSimplexRepeaterSettingsExtension::TimeSlot slot);

    bool showLastHeard() const;
    void enableShowLastHeard(bool enable);

    /** Returns the SMS format. */
    virtual AnytoneDMRSettingsExtension::SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat fmt);

    bool gpsUnitsImperial() const;
    void enableGPSUnitsImperial(bool enable);

    Frequency autoRepeaterMinFrequencyVHF() const;
    void setAutoRepeaterMinFrequencyVHF(Frequency Hz);
    Frequency autoRepeaterMaxFrequencyVHF() const;
    void setAutoRepeaterMaxFrequencyVHF(Frequency Hz);
    Frequency autoRepeaterMinFrequencyUHF() const;
    void setAutoRepeaterMinFrequencyUHF(Frequency Hz);
    Frequency autoRepeaterMaxFrequencyUHF() const;
    void setAutoRepeaterMaxFrequencyUHF(Frequency Hz);

    AnytoneAutoRepeaterSettingsExtension::Direction autoRepeaterDirectionB() const;
    void setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir);

    /** If enabled, the FM ID is sent together with selected contact. */
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

    bool keepLastCaller() const;
    void enableKeepLastCaller(bool enable);

    Interval rxBacklightDuration() const;
    void setRXBacklightDuration(Interval sec);

    /** Returns the stand-by background color. */
    virtual AnytoneDisplaySettingsExtension::Color standbyBackgroundColor() const;
    /** Sets the stand-by background color. */
    virtual void setStandbyBackgroundColor(AnytoneDisplaySettingsExtension::Color color);

    /** Returns the group-call hang time, if group call was dialed manually. */
    virtual unsigned int manualDialedGroupCallHangTime() const;
    /** Sets the group-call hang time, if the group call was dialed maually. */
    virtual void setManualDialedGroupCallHangTime(unsigned int dur);
    /** Returns the private-call hang time, if private call was dialed manually. */
    virtual unsigned int manualDialedPrivateCallHangTime() const;
    /** Sets the private-call hang time, if the private call was dialed maually. */
    virtual void setManualDialedPrivateCallHangTime(unsigned int dur);


    bool fromConfig(const Flags &flags, Context &ctx);
    bool updateConfig(Context &ctx);

  protected:
    /** Some internal used offsets within the element. */
    struct Offset: public D868UVCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int idleChannelTone()     { return 0x0000; }
      static constexpr unsigned int transmitTimeout()     { return 0x0004; }
      static constexpr unsigned int language()            { return 0x0005; }
      static constexpr unsigned int vfoFrequencyStep()    { return 0x0008; }
      static constexpr unsigned int vfoModeB()            { return 0x0015; }
      static constexpr unsigned int vfoModeA()            { return 0x0016; }
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
      static constexpr unsigned int enableKeyTone()       { return 0x0036; }
      static constexpr unsigned int filterOwnID()         { return 0x0038; }
      static constexpr unsigned int remoteStunKill()      { return 0x003c; }
      static constexpr unsigned int remoteMonitor()       { return 0x003e; }
      static constexpr unsigned int selectTXContact()     { return 0x0040; }
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
      static constexpr unsigned int smsConfirm()          { return 0x0071; }
      static constexpr unsigned int simplexRepEnable()    { return 0x00b1; }
      static constexpr unsigned int gpsUpdatePeriod()     { return 0x00b2; }
      static constexpr unsigned int simplxRepSpeaker()    { return 0x00b3; }
      static constexpr unsigned int showContact()         { return 0x00b4; }
      static constexpr unsigned int keyToneLevel()        { return 0x00b5; }
      static constexpr unsigned int knobLock()            { return 0x00b6; }
      static constexpr unsigned int keypadLock()          { return 0x00b6; }
      static constexpr unsigned int sideKeyLock()         { return 0x00b6; }
      static constexpr unsigned int forceKeyLock()        { return 0x00b6; }
      static constexpr unsigned int simplxRepSlot()       { return 0x00b7; }
      static constexpr unsigned int showLastHeard()       { return 0x00b8; }
      static constexpr unsigned int smsFormat()           { return 0x00b9; }
      static constexpr unsigned int gpsUnits()            { return 0x00ba; }
      static constexpr unsigned int autoRepMinVHF()       { return 0x00bc; }
      static constexpr unsigned int autoRepMaxVHF()       { return 0x00c0; }
      static constexpr unsigned int autoRepMinUHF()       { return 0x00c4; }
      static constexpr unsigned int autoRepMaxUHF()       { return 0x00c8; }
      static constexpr unsigned int autoRepeaterDirB()    { return 0x00cc; }
      static constexpr unsigned int fmSendIDAndContact()  { return 0x00cd; }
      static constexpr unsigned int defaultChannels()     { return 0x00ce; }
      static constexpr unsigned int defaultZoneA()        { return 0x00cf; }
      static constexpr unsigned int defaultZoneB()        { return 0x00d0; }
      static constexpr unsigned int defaultChannelA()     { return 0x00d1; }
      static constexpr unsigned int defaultChannelB()     { return 0x00d2; }
      static constexpr unsigned int keepLastCaller()      { return 0x00d3; }
      static constexpr unsigned int rxBacklightDuration() { return 0x00d4; }
      static constexpr unsigned int standbyBackground()   { return 0x00d5; }
      static constexpr unsigned int manGrpCallHangTime()  { return 0x00d6; }
      static constexpr unsigned int manPrvCallHangTime()  { return 0x00d7; }
      /// @endcond
    };

  };

  /** Implements some settings extension for the BTECH DMR-6X2UV.
   *
   * Memory representation of the encoded settings element (size 0x0e0 bytes):
   * @verbinclude dmr6x2uv_settingsextension.txt */
  class ExtendedSettingsElement: public AnytoneCodeplug::ExtendedSettingsElement
  {
  protected:
    /** Hidden Constructor. */
    ExtendedSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ExtendedSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0030; }

    /** Resets the general settings. */
    void clear();

    bool sendTalkerAlias() const;
    void enableSendTalkerAlias(bool enable);

    AnytoneDMRSettingsExtension::TalkerAliasSource talkerAliasSource() const;
    void setTalkerAliasSource(AnytoneDMRSettingsExtension::TalkerAliasSource source);
    AnytoneDMRSettingsExtension::TalkerAliasEncoding talkerAliasEncoding() const;
    void setTalkerAliasEncoding(AnytoneDMRSettingsExtension::TalkerAliasEncoding encoding);

    /** Returns the font color. */
    virtual AnytoneDisplaySettingsExtension::Color fontColor() const;
    /** Sets the font color. */
    virtual void setFontColor(AnytoneDisplaySettingsExtension::Color color);

    /** Returns @c true if the custom channel background is enabled. */
    virtual bool customChannelBackgroundEnabled() const;
    /** Enables/disables the custom channel background. */
    virtual void enableCustomChannelBackground(bool enable);

    /** Returns @c true if auto roaming is enabled. */
    virtual bool autoRoamingEnabled() const;
    /** Enables/disables auto roaming. */
    virtual void enableAutoRoaming(bool enable);

    /** Returns @c true if repeater check is enabled. */
    virtual bool repeaterRangeCheckEnabled() const;
    /** Enables/disables repeater check. */
    virtual void enableRepeaterRangeCheck(bool enable);
    /** Returns the number of times, the repeater out-of-range reminder is shown (1-10). */
    virtual unsigned int repeaterCheckNumNotifications() const;
    /** Sets the number of times, the repeater out-of-range reminder is shown (1-10). */
    virtual void setRepeaterCheckNumNotifications(unsigned int n);
    /** Returns the repeater check interval in seconds (5-50s). */
    virtual Interval repeaterRangeCheckInterval() const;
    /** Sets the repeater check interval in seconds (5-50s). */
    virtual void setRepeaterRangeCheckInterval(Interval intv);
    /** Returns the repeater out-of-range alert type. */
    virtual AnytoneRoamingSettingsExtension::OutOfRangeAlert repeaterOutOfRangeAlert() const;
    /** Sets the repeater out-of-range alert type. */
    virtual void setRepeaterOutOfRangeAlert(AnytoneRoamingSettingsExtension::OutOfRangeAlert alert);
    /** Returns the number of times, a repeater reconnection is tried (3-5). */
    virtual unsigned int repeaterRangeCheckCount() const;
    /** Sets the number of times, a repeater reconnection is tried (3-5). */
    virtual void setRepeaterRangeCheckCount(unsigned int n);

    /** Returns the roaming zone index. */
    virtual unsigned int defaultRoamingZoneIndex() const;
    /** Sets the roaming zone index. */
    virtual void setDefaultRoamingZoneIndex(unsigned int index);
    /** Returns the condition to start roaming. */
    virtual AnytoneRoamingSettingsExtension::RoamStart roamingStartCondition() const;
    /** Sets the condition to start roaming. */
    virtual void setRoamingStartCondition(AnytoneRoamingSettingsExtension::RoamStart cond);
    /** Returns the auto-roaming interval in minutes (1-256). */
    virtual Interval autoRoamPeriod() const;
    /** Sets the auto-roaming interval in minutes (1-256). */
    virtual void setAutoRoamPeriod(Interval minutes);
    /** Returns the effective roaming waiting time in seconds (0-30s). */
    virtual Interval autoRoamDelay() const;
    /** Sets the effective roaming waiting time in seconds (0-30s). */
    virtual void setAutoRoamDelay(Interval sec);
    /** Returns the roaming return condition. */
    virtual AnytoneRoamingSettingsExtension::RoamStart roamingReturnCondition() const;
    /** Sets the roaming return condition. */
    virtual void setRoamingReturnCondition(AnytoneRoamingSettingsExtension::RoamStart cond);

    /** Returns the mute timer in minutes. */
    virtual Interval muteTimer() const;
    /** Sets the mute timer in minutes. */
    virtual void setMuteTimer(Interval minutes);

    /** Returns the encryption type. */
    virtual AnytoneDMRSettingsExtension::EncryptionType encryptionType() const;
    /** Sets the encryption type. */
    virtual void setEncryptionType(AnytoneDMRSettingsExtension::EncryptionType type);

    AnytoneDisplaySettingsExtension::Color zoneANameColor() const;
    void setZoneANameColor(AnytoneDisplaySettingsExtension::Color color);
    AnytoneDisplaySettingsExtension::Color zoneBNameColor() const;
    void setZoneBNameColor(AnytoneDisplaySettingsExtension::Color color);
    /** Returns the name color for channel A. */
    virtual AnytoneDisplaySettingsExtension::Color channelANameColor() const;
    /** Sets the name color for channel A. */
    virtual void setChannelANameColor(AnytoneDisplaySettingsExtension::Color color);
    AnytoneDisplaySettingsExtension::Color channelBNameColor() const;
    void setChannelBNameColor(AnytoneDisplaySettingsExtension::Color color);

    bool fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
    bool updateConfig(Context &ctx, const ErrorStack &err=ErrorStack());
    bool linkConfig(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for entries. */
    struct Limit {
      /// Out of range reminder count limits.
      static constexpr IntRange repRangeReminder()    { return {1, 10}; }
      /// Repeater range check interval limits.
      static constexpr TimeRange rangeCheckInterval() {
        return TimeRange{Interval::fromSeconds(1), Interval::fromSeconds(50)};
      }
      /// Repeater reconnection count limits.
      static constexpr IntRange repeaterReconnections() { return {3,5}; }
      /// Auto-roaming interval limits.
      static constexpr TimeRange autoRoamingInterval() {
        return TimeRange{Interval::fromMinutes(1), Interval::fromMinutes(256)};
      }
      /// Auto-roaming delay limits.
      static constexpr TimeRange autoRoamDelay() {
        return TimeRange{Interval::fromSeconds(0), Interval::fromSeconds(30)};
      }
      /// Mute-timer limits.
      static constexpr TimeRange muteTimer() {
        return TimeRange{Interval::fromMinutes(1), Interval::fromMinutes(256)};
      }
    };

  protected:
    /** Some internal offset within the codeplug element. */
    struct Offset {
      /// @cond DO_NOT_DOCUEMNT
      static constexpr unsigned int sendTalkerAlias()              { return 0x0000; }
      static constexpr unsigned int talkerAliasDisplay()           { return 0x0001; }
      static constexpr unsigned int talkerAliasEncoding()          { return 0x0002; }
      static constexpr unsigned int fontColor()                    { return 0x0003; }
      static constexpr unsigned int customChannelBackground()      { return 0x0004; }
      static constexpr unsigned int defaultRoamingZone()           { return 0x0005; }
      static constexpr unsigned int roaming()                      { return 0x0006; }
      static constexpr unsigned int repRangeCheck()                { return 0x0007; }
      static constexpr unsigned int repRangeAlert()                { return 0x0008; }
      static constexpr unsigned int repRangeReminder()             { return 0x0009; }
      static constexpr unsigned int rangeCheckInterval()           { return 0x000a; }
      static constexpr unsigned int rangeCheckCount()              { return 0x000b; }
      static constexpr unsigned int roamStartCondition()           { return 0x000c; }
      static constexpr unsigned int autoRoamPeriod()               { return 0x000d; }
      static constexpr unsigned int autoRoamDelay()                { return 0x000e; }
      static constexpr unsigned int roamReturnCondition()          { return 0x000f; }
      static constexpr unsigned int muteDelay()                    { return 0x0010; }
      static constexpr unsigned int encryptionType()               { return 0x0011; }
      static constexpr unsigned int zoneANameColor()               { return 0x0012; }
      static constexpr unsigned int zoneBNameColor()               { return 0x0013; }
      static constexpr unsigned int channelANameColor()            { return 0x0014; }
      static constexpr unsigned int channelBNameColor()            { return 0x0015; }
      /// @endcond
    };
  };

  /** Implements the channel element for the BTECH DMR-6X2UV.
   *  Extends the AnytoneCodeplug::ChannelElement by the device specific features, like multiple
   *  scan lists associated with the channel.
   *
   *  Memory representation of the encoded channel element (size 0x040 bytes):
   *  @verbinclude dmr6x2uv_channel.txt */
  class ChannelElement: public AnytoneCodeplug::ChannelElement
  {
  public:
    /** Possible PTT modes for FM APRS. */
    enum class FMAPRSPTTMode {
      Off = 0, Start = 1, End = 2
    };

    /** Possible APRS report types. */
    enum class APRSType{
      Off = 0, FM = 1, DMR = 2
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns @c true, if the first scan list index is set. */
    bool hasScanListIndex() const;
    /** Returns the first scan list index (0-based). */
    unsigned scanListIndex() const;
    /** Sets the first scan list index (0-based). */
    void setScanListIndex(unsigned idx);
    /** Clears the first scan list index. */
    void clearScanListIndex();

    /** Returns @c true, if the n-th scan list index is set (n=0,...,7). */
    virtual bool hasScanListIndex(unsigned int n) const;
    /** Returns the n-th scan list index (0-based, n=0,...,7). */
    virtual unsigned scanListIndex(unsigned int n) const;
    /** Sets the n-th scan list index (0-based, n=0,...,7). */
    virtual void setScanListIndex(unsigned int n, unsigned idx);
    /** Clears the n-th scan list index (n=0,...,7). */
    virtual void clearScanListIndex(unsigned int n);

    /** Returns @c true if roaming is enabled for this channel. */
    virtual bool roamingEnabled() const;
    /** Enables/disables roaming. */
    virtual void enableRoaming(bool enable);

    /** Returns @c true, if ranging is enabled. */
    virtual bool ranging() const;
    /** Enables/disables ranging. */
    virtual void enableRanging(bool enable);

    /** Returns the DMR APRS report channel index. */
    virtual unsigned int dmrAPRSChannelIndex() const;
    /** Sets the DMR APRS report channel index. */
    virtual void setDMRAPRSChannelIndex(unsigned int idx);

    /** Returns @c true, if the reception of DMR APRS messages is enabled. */
    virtual bool dmrAPRSRXEnabled() const;
    /** Enables/disables the reception of DMR APRS messages. */
    virtual void enableDMRARPSRX(bool enable);

    /** Returns true, if the position is reported via DMR APRS on PTT. */
    virtual bool dmrAPRSPTTEnabled() const;
    /** Enables/disables reporting the position via DMR APRS on PTT. */
    virtual void enableDMRAPRSPTT(bool enable);

    /** Returns teh FM APRS PTT mode. */
    virtual FMAPRSPTTMode fmAPRSPTTMode() const;
    /** Sets the FM APRS PTT mode. */
    virtual void setFMAPRSPTTMode(FMAPRSPTTMode mode);

    /** Returns the APRS type. */
    virtual APRSType aprsType() const;
    /** Sets the APRS type. */
    virtual void setAPRSType(APRSType aprstype);

    bool linkChannelObj(Channel *c, Context &ctx) const;
    bool fromChannelObj(const Channel *c, Context &ctx);

public:
    struct Limit {
      // Maximum number of scan list indices.
      static constexpr unsigned int scanListIndices() { return 8; }
    };

protected:
    /// @cond DO_NOT_DOCUMENT
    struct Offset: public Element::Offset {
      static constexpr Bit roaming()                               { return {0x001b, 2}; }
      static constexpr Bit ranging()                               { return {0x001b, 0}; }
      static constexpr unsigned int scanListIndices()              { return 0x0036; }
      static constexpr unsigned int betweenScanListIndices()       { return 0x0001; }
      static constexpr unsigned int dmrAPRSChannelIndex()          { return 0x003e; }
      static constexpr Bit dmrAPRSRXEnable()                       { return {0x003f, 5}; }
      static constexpr Bit dmrAPRSPTTEnable()                      { return {0x003f, 4}; }
      static constexpr Bit fmAPRSPTTMode()                         { return {0x003f, 2}; }
      static constexpr Bit aprsType()                              { return {0x003f, 0}; }
    };
    /// @endcond
  };

  /** Represents the APRS settings within the binary DMR-6X2UV codeplug.
   *
   * Memory layout of APRS settings (size 0x00a0 bytes):
   * @verbinclude dmr6x2uv_aprssetting.txt
   */
  class APRSSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    APRSSettingsElement(uint8_t *ptr, unsigned size);

    /** Possible settings for the FM APRS subtone type. */
    enum class SignalingType {
      Off=0, CTCSS=1, DCS=2
    };

  public:
    /** Constructor. */
    explicit APRSSettingsElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x00a0; }

    /** Resets the settings. */
    void clear();
    bool isValid() const;

    /** Returns the FM APRS frequency. */
    virtual Frequency fmFrequency() const;
    /** Sets the FM APRS frequency. */
    virtual void setFMFrequency(Frequency f);

    /** Returns the TX delay in ms. */
    virtual Interval fmTXDelay() const;
    /** Sets the TX delay in ms. */
    virtual void setFMTXDelay(const Interval intv);

    /** Returns the sub tone settings. */
    virtual Signaling::Code txTone() const;
    /** Sets the sub tone settings. */
    virtual void setTXTone(Signaling::Code code);

    /** Returns the manual TX interval in seconds. */
    virtual Interval manualTXInterval() const;
    /** Sets the manual TX interval in seconds. */
    virtual void setManualTXInterval(Interval sec);

    /** Returns @c true if the auto transmit is enabled. */
    virtual bool autoTX() const;
    /** Returns the auto TX interval in seconds. */
    virtual Interval autoTXInterval() const;
    /** Sets the auto TX interval in seconds. */
    virtual void setAutoTXInterval(Interval sec);
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
    virtual Interval fmPreWaveDelay() const;
    /** Sets the pre-wave delay in ms. */
    virtual void setFMPreWaveDelay(Interval ms);

    /** Returns @c true if the channel points to the current/selected channel. */
    virtual bool dmrChannelIsSelected(unsigned n) const;
    /** Returns the digital channel index for the n-th system. */
    virtual unsigned dmrChannelIndex(unsigned n) const;
    /** Sets the digital channel index for the n-th system. */
    virtual void setDMRChannelIndex(unsigned n, unsigned idx);
    /** Sets the channel to the current/selected channel. */
    virtual void setDMRChannelSelected(unsigned n);

    /** Returns the destination contact for the n-th system. */
    virtual unsigned dmrDestination(unsigned n) const;
    /** Sets the destination contact for the n-th system. */
    virtual void setDMRDestination(unsigned n, unsigned idx);

    /** Returns the call type for the n-th system. */
    virtual DMRContact::Type dmrCallType(unsigned n) const;
    /** Sets the call type for the n-th system. */
    virtual void setDMRCallType(unsigned n, DMRContact::Type type);

    /** Returns @c true if the n-th system overrides the channel time-slot. */
    virtual bool dmrTimeSlotOverride(unsigned n);
    /** Returns the time slot if overridden (only valid if @c timeSlot returns true). */
    virtual DMRChannel::TimeSlot dmrTimeSlot(unsigned n) const;
    /** Overrides the time slot of the n-th selected channel. */
    virtual void setDMRTimeSlot(unsigned n, DMRChannel::TimeSlot ts);
    /** Clears the time-slot override. */
    virtual void clearDMRTimeSlotOverride(unsigned n);

    /** Returns @c true if the roaming is enabled. */
    virtual bool dmrRoaming() const;
    /** Enables/disables roaming. */
    virtual void enableDMRRoaming(bool enable);

    /** Returns the the repeater activation delay in ms. */
    virtual Interval dmrPreWaveDelay() const;
    /** Sets the repeater activation delay in ms. */
    virtual void setDMRPreWaveDelay(Interval ms);

    /** Configures this APRS system from the given generic config. */
    virtual bool fromFMAPRSSystem(const APRSSystem *sys, Context &ctx,
                                  const ErrorStack &err=ErrorStack());
    /** Constructs a generic APRS system configuration from this APRS system. */
    virtual APRSSystem *toFMAPRSSystem();
    /** Links the transmit channel within the generic APRS system based on the transmit frequency
     * defined within this APRS system. */
    virtual bool linkFMAPRSSystem(APRSSystem *sys, Context &ctx);

    /** Constructs all GPS system from the generic configuration. */
    virtual bool fromDMRAPRSSystems(Context &ctx);
    /** Encodes the given GPS system. */
    virtual bool fromDMRAPRSSystemObj(unsigned int idx, GPSSystem *sys, Context &ctx);
    /** Constructs a generic GPS system from the idx-th encoded GPS system. */
    virtual GPSSystem *toDMRAPRSSystemObj(int idx) const;
    /** Links the specified generic GPS system. */
    virtual bool linkDMRAPRSSystem(int idx, GPSSystem *sys, Context &ctx) const;

  public:
    /** Some static limits for this element. */
    struct Limit {
      /// Maximum length of call signs.
      static constexpr unsigned int callLength()                           { return 0x0006; }
      /// Maximum length of the repeater path string.
      static constexpr unsigned int pathLength()                           { return 0x0020; }
      /// Maximum number of DMR APRS systems.
      static constexpr unsigned int dmrSystems()                           { return 0x0008; }
    };

  protected:
    /** Internal used offsets within the codeplug element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int fmFrequency()                          { return 0x0001; }
      static constexpr unsigned int fmTXDelay()                            { return 0x0005; }
      static constexpr unsigned int fmSigType()                            { return 0x0006; }
      static constexpr unsigned int fmCTCSS()                              { return 0x0007; }
      static constexpr unsigned int fmDCS()                                { return 0x0008; }
      static constexpr unsigned int manualTXInterval()                     { return 0x000a; }
      static constexpr unsigned int autoTXInterval()                       { return 0x000b; }
      static constexpr unsigned int fmTXMonitor()                          { return 0x000c; }
      static constexpr unsigned int fixedLocation()                        { return 0x000d; }
      static constexpr unsigned int fixedLatDeg()                          { return 0x000e; }
      static constexpr unsigned int fixedLatMin()                          { return 0x000f; }
      static constexpr unsigned int fixedLatSec()                          { return 0x0010; }
      static constexpr unsigned int fixedLatSouth()                        { return 0x0011; }
      static constexpr unsigned int fixedLonDeg()                          { return 0x0012; }
      static constexpr unsigned int fixedLonMin()                          { return 0x0013; }
      static constexpr unsigned int fixedLonSec()                          { return 0x0014; }
      static constexpr unsigned int fixedLonWest()                         { return 0x0015; }
      static constexpr unsigned int destinationCall()                      { return 0x0016; }
      static constexpr unsigned int destinationSSID()                      { return 0x001c; }
      static constexpr unsigned int sourceCall()                           { return 0x001d; }
      static constexpr unsigned int sourceSSID()                           { return 0x0023; }
      static constexpr unsigned int path()                                 { return 0x0024; }
      static constexpr unsigned int symbolTable()                          { return 0x0039; }
      static constexpr unsigned int symbol()                               { return 0x003a; }
      static constexpr unsigned int fmPower()                              { return 0x003b; }
      static constexpr unsigned int fmPrewaveDelay()                       { return 0x003c; }
      static constexpr unsigned int dmrChannelIndices()                    { return 0x0040; }
      static constexpr unsigned int betweenDMRChannelIndices()             { return 0x0002; }
      static constexpr unsigned int dmrDestinations()                      { return 0x0050; }
      static constexpr unsigned int betweenDMRDestinations()               { return 0x0004; }
      static constexpr unsigned int dmrCallTypes()                         { return 0x0070; }
      static constexpr unsigned int betweenDMRCallTypes()                  { return 0x0001; }
      static constexpr unsigned int roamingSupport()                       { return 0x0078; }
      static constexpr unsigned int dmrTimeSlots()                         { return 0x0079; }
      static constexpr unsigned int betweenDMRTimeSlots()                  { return 0x0001; }
      static constexpr unsigned int dmrPrewaveDelay()                      { return 0x0081; }
      /// @endcond
    };
  };

  /** Reuse roaming channel bitmap from D878UV. */
  typedef D878UVCodeplug::RoamingChannelBitmapElement RoamingChannelBitmapElement ;
  /** Reuse roaming channel from D878UV. */
  typedef D878UVCodeplug::RoamingChannelElement RoamingChannelElement;
  /** Reuse roaming zone bitmap from D878UV. */
  typedef D878UVCodeplug::RoamingZoneBitmapElement RoamingZoneBitmapElement;
  /** Reuse roaming zone from D878UV. */
  typedef D878UVCodeplug::RoamingZoneElement RoamingZoneElement;

public:
  /** Hidden constructor. */
  explicit DMR6X2UVCodeplug(const QString &label, QObject *parent=nullptr);

public:
  /** Empty constructor. */
  explicit DMR6X2UVCodeplug(QObject *parent=nullptr);

protected:
  bool allocateBitmaps();
  void setBitmaps(Context &ctx);
  void allocateForDecoding();
  void allocateForEncoding();

  bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  void allocateGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  virtual bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  virtual bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

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
  /** Some limits for the codeplug. */
  struct Limit : public D868UVCodeplug::Limit {
    /// Maximum length of the FM APRS message
    static constexpr unsigned int fmAPRSMessage()                 { return 60; }
    /// Maximum number of roaming channels.
    static constexpr unsigned int roamingChannels()               { return 250; }
    /// Maximum number of roaming zones.
    static constexpr unsigned int roamingZones()                  { return 64; }
  };

protected:
  /** Some internal used offsets within the codeplug. */
  struct Offset: public D868UVCodeplug::Offset {
    ///@cond DO_NOT_DOCUMENT
    static constexpr unsigned int roamingChannelBitmap()          { return 0x01042000; }
    static constexpr unsigned int roamingChannels()               { return 0x01040000; }
    static constexpr unsigned int roamingZoneBitmap()             { return 0x01042080; }
    static constexpr unsigned int roamingZones()                  { return 0x01043000; }

    static constexpr unsigned int fmAPRSMessage()                 { return 0x02501200; }
    static constexpr unsigned int fmAPRSFrequencyNames()          { return 0x02502000; }
    static constexpr unsigned int settingsExtension()             { return 0x02501400; }
    /// @endcond
  };

  /** Some internal used sizes. */
  struct Size: public D868UVCodeplug::Size {
    ///@cond DO_NOT_DOCUMENT
    static constexpr unsigned int fmAPRSMessage()                 { return 0x00000040; }
    /// @endcond
  };
};

#endif // DMR6X2UVCODEPLUG_HH
