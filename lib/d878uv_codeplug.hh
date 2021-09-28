#ifndef D878UV_CODEPLUG_HH
#define D878UV_CODEPLUG_HH

#include <QDateTime>

#include "d868uv_codeplug.hh"
#include "signaling.hh"
#include "codeplugcontext.hh"

class Channel;
class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D878UV radios.
 *
 * In contrast to many other code-plugs, the code-plug for Anytone radios are spread over a large
 * memory area. In principle, this is a good idea, as it allows to upload only the portion of the
 * codeplug that is actually configured. For example, if only a small portion of the available
 * contacts and channels are used, the amount of data that is written to the device can be
 * reduced.
 *
 * However, the implementation of this idea in this device is utter shit. The amount of
 * fragmentation of the codeplug is overwhelming. For example, while channels are organized more or
 * less nicely in continous banks, zones are distributed throughout the entire code-plug. That is,
 * the names of zones are located in a different memory section that the channel lists. Some lists
 * are defined though bit-masks others by byte-masks. All bit-masks are positive, that is 1
 * indicates an enabled item while the bit-mask for contacts is inverted.
 *
 * In general the code-plug is huge and complex. Moreover, the radio provides a huge amount of
 * options and features. To this end, reverse-engeneering this code-plug was a nightmare.
 *
 * More over, the binary code-plug file generate by the windows CPS does not directly relate to
 * the data being written to the radio. To this end the code-plug has been reverse-engineered
 * using wireshark to monitor the USB communication between the windows CPS (running in a vritual
 * box) and the device. The latter makes the reverse-engineering particularily cumbersome.
 *
 * @section d878uvcpl Codeplug structure within radio
 * <table>
 *  <tr><th colspan="3">Channels</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1500</td> <td>000200</td>      <td>Bitmap of 4000 channels, default 0x00,
 *    0x00 padded.</td></tr>
 *  <tr><td>00800000</td> <td>max. 002000</td> <td>Channel bank 0 of upto 128 channels,
 *    see @c D878UVCodeplug::ChannelElement 64 b each. </td></tr>
 *  <tr><td>00802000</td> <td>max, 002000</td> <td>Unknown data, Maybe extended channel information
 *    for channel bank 0? It is of exactly the same size as the channel bank 0. Mostly 0x00, a
 *    few 0xff.</td></tr>
 *  <tr><td>00840000</td> <td>max. 002000</td> <td>Channel bank 1 of upto 128 channels.</td></tr>
 *  <tr><td>00842000</td> <td>max. 002000</td> <td>Unknown data, related to CH bank 1?</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>00FC0000</td> <td>max. 000800</td> <td>Channel bank 32, upto 32 channels.</td></tr>
 *  <tr><td>00FC2000</td> <td>max. 000800</td> <td>Unknown data, realted to CH bank 32.</td></tr>
 *  <tr><td>00FC0800</td> <td>000040</td>      <td>VFO A settings,
 *    see @c D878UVCodeplug::ChannelElement.</td></tr>
 *  <tr><td>00FC0840</td> <td>000040</td>      <td>VFO B settings,
 *    see @c D878UVCodeplug::ChannelElement.</td></tr>
 *  <tr><td>00FC2800</td> <td>000080</td>      <td>Unknonw data, related to VFO A+B?
 *    It is of exactly the same size as the two VFO channels. Mostly 0x00, a few 0xff. Same pattern
 *    as the unknown data associated with channel banks.</td></tr>
 *
 *  <tr><th colspan="3">Zones</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1300</td> <td>000020</td>      <td>Bitmap of 250 zones.</td></tr>
 *  <tr><td>01000000</td> <td>max. 01f400</td> <td>250 zones channel lists of 250 16bit indices each.
 *    0-based, little endian, default/padded=0xffff. Offset between channel lists 0x200,
 *    size of each list 0x1f4.</td></tr>
 *  <tr><td>02540000</td> <td>max. 001f40</td> <td>250 Zone names.
 *    Each zone name is upto 16 ASCII chars long and gets 0-padded to 32b.</td></tr>
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
 *  <tr><td>02900000</td> <td>000080</td>      <td>Index list of valid ananlog contacts.</td></tr>
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
 *  <tr><td>02501000</td> <td>000040</td> <td>APRS settings,
 *    see @c D878UVCodeplug::AnalogAPRSSettingsElement.</td>
 *  <tr><td>02501040</td> <td>000060</td> <td>APRS settings,
 *    see @c D878UVCodeplug::DMRAPRSSystemsElement.</td>
 *  <tr><td>025010A0</td> <td>000060</td> <td>Extended APRS settings,
 *    see @c D878UVCodeplug::AnalogAPRSSettingsExtensionElement.</tr>
 *  <tr><td>02501200</td> <td>000040</td> <td>APRS Text, upto 60 chars ASCII, 0-padded.</td>
 *  <tr><td>02501800</td> <td>000100</td> <td>APRS-RX settings list up to 32 entries, 8b each.
 *    See @c D878UVCodeplug::AnalogAPRSRXEntryElement.</td></tr>
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>000100</td> <td>General settings,
 *    see @c D878UVCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>02500100</td> <td>000400</td> <td>Zone A & B channel list.</td></tr>
 *  <tr><td>02500500</td> <td>000100</td> <td>DTMF list</td></tr>
 *  <tr><td>02500600</td> <td>000030</td> <td>Power on settings,
 *    see @c AnytoneCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>02501280</td> <td>000030</td> <td>General settings extension 1,
 *    see @c D878UVCodeplug::AnalogAPRSRXEntryElement.</td></tr>
 *  <tr><td>02501400</td> <td>000100</td> <td>General settings extension 2,
 *    see @c D878UVCodeplug::AnalogAPRSSettingsExtensionElement.</td></tr>
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
 *  <tr><td>025C0100</td> <td>000400</td> <td>Upto 32 status messages.
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
 *  <tr><td>024C4000</td> <td>004000</td> <td>Upto 256 AES encryption keys.
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
   * Memmory layout of encoded channel (size 0x40 bytes):
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
    ChannelElement(uint8_t *ptr, uint size);

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
    /** Returns @c true if the analog APRS reporing (TX) is enabled. */
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
    virtual uint digitalAPRSSystemIndex() const;
    /** Sets the DMR APRS system index. */
    virtual void setDigitalAPRSSystemIndex(uint idx);

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
   * Binary encoding of the general settings (size 0x0100 bytes):
   * @verbinclude d878uv_generalsettings.txt */
  class GeneralSettingsElement: public AnytoneCodeplug::GeneralSettingsElement
  {
  public:
    /** Possible UI languages. */
    enum class Language {
      English = 0,                 ///< UI Language is english.
      German  = 1                  ///< UI Language is german.
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

    /** All possible STE types. */
    enum class STEType {
      Off = 0, Silent = 1, Deg120 = 2, Deg180 = 3, Deg240 = 4
    };

    /** All possible STE frequencies. */
    enum class STEFrequency {
      Off     = 0, Hz55_2  = 1, Hz259_2 = 2
    };

    /** DTMF signalling durations. */
    enum DTMFDuration {
      DTMF_DUR_50ms = 0, DTMF_DUR_100ms = 1, DTMF_DUR_200ms = 2, DTMF_DUR_300ms = 3, DTMF_DUR_500ms = 4
    };

    /** TBST (open repeater) frequencies. */
    enum class TBSTFrequency {
      Hz1000 = 0, Hz1450 = 1, Hz1750 = 2, Hz2100 = 3
    };

    /** Possible monitor slot matches. */
    enum class SlotMatch {
      Off = 0, Single = 1, Both = 2
    };

    /** Possible SMS formats. */
    enum class SMSFormat {
      M = 0, H = 1, DMR = 2,
    };

    /** Possible roaming start conditions. */
    enum class RoamStart {
      Periodic=0, OutOfRange=1
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** Resets the general settings. */
    void clear();

    /** Returns the transmit timeout in seconds. */
    virtual uint transmitTimeout() const;
    /** Sets the transmit timeout in seconds. */
    virtual void setTransmitTimeout(uint tot);

    /** Returns the UI language. */
    virtual Language language() const;
    /** Sets the UI language. */
    virtual void setLanguage(Language lang);

    /** Returns the VFO frequency step in kHz. */
    virtual double vfoFrequencyStep() const;
    /** Sets the VFO frequency step in kHz. */
    virtual void setVFOFrequencyStep(double kHz);

    /** Returns the STE type. */
    virtual STEType steType() const;
    /** Sets the STE type. */
    virtual void setSTEType(STEType type);
    /** Returns the STE frequency setting. */
    virtual STEFrequency steFrequency() const;
    /** Sets the STE frequency setting. */
    virtual void setSTEFrequency(STEFrequency freq);

    /** Returns the group call hang time in seconds. */
    virtual uint groupCallHangTime() const;
    /** Sets the group call hang time in seconds. */
    virtual void setGroupCallHangTime(uint sec);
    /** Returns the private call hang time in seconds. */
    virtual uint privateCallHangTime() const;
    /** Sets the private call hang time in seconds. */
    virtual void setPrivateCallHangTime(uint sec);

    /** Returns the pre-wave time in ms. */
    virtual uint preWaveDelay() const;
    /** Sets the pre-wave time in ms. */
    virtual void setPreWaveDelay(uint ms);
    /** Returns the wake head-period in ms. */
    virtual uint wakeHeadPeriod() const;
    /** Sets the wake head-period in ms. */
    virtual void setWakeHeadPeriod(uint ms);

    /** Retunrs the wide-FM (broadcast) channel index. */
    virtual uint wfmChannelIndex() const;
    /** Sets the wide-FM (broadcast) channel index. */
    virtual void setWFMChannelIndex(uint idx);
    /** Returns @c true if the WFM RX is in VFO mode. */
    virtual bool wfmVFOEnabled() const;
    /** Enables/disables VFO mode for WFM RX. */
    virtual void enableWFMVFO(bool enable);

    /** Returns the DTMF tone duration in ms. */
    virtual uint dtmfToneDuration() const;
    /** Sets the DTMF tone duration in ms. */
    virtual void setDTMFToneDuration(uint ms);

    /** Returns @c true if "man down" is enabled. */
    virtual bool manDown() const;
    /** Enables/disables "man down". */
    virtual void enableManDown(bool enable);

    /** Returns @c true if WFM monitor is enabled. */
    virtual bool wfmMonitor() const;
    /** Enables/disables WFM monitor. */
    virtual void enableWFMMonitor(bool enable);

    /** Returns the TBST frequency. */
    virtual TBSTFrequency tbstFrequency() const;
    /** Sets the TBST frequency. */
    virtual void setTBSTFrequency(TBSTFrequency freq);

    /** Returns @c true if the "pro mode" is enabled. */
    virtual bool proMode() const;
    /** Enables/disables the "pro mode". */
    virtual void enableProMode(bool enable);

    /** Retuns @c true if the own ID is filtered in call lists. */
    virtual bool filterOwnID() const;
    /** Enables/disables filter of own ID in call lists. */
    virtual void enableFilterOwnID(bool enable);
    /** Retuns @c true remote stun/kill is enabled. */
    virtual bool remoteStunKill() const;
    /** Enables/disables remote stun/kill. */
    virtual void enableRemoteStunKill(bool enable);
    /** Retuns @c true remote monitor is enabled. */
    virtual bool remoteMonitor() const;
    /** Enables/disables remote monitor. */
    virtual void enableRemoteMonitor(bool enable);

    /** Returns the monitor slot match. */
    virtual SlotMatch monitorSlotMatch() const;
    /** Sets the monitor slot match. */
    virtual void setMonitorSlotMatch(SlotMatch match);
    /** Retuns @c true if the monitor matches color code. */
    virtual bool monitorColorCodeMatch() const;
    /** Enables/disables monitor color code match. */
    virtual void enableMonitorColorCodeMatch(bool enable);
    /** Retuns @c true if the monitor matches ID. */
    virtual bool monitorIDMatch() const;
    /** Enables/disables monitor ID match. */
    virtual void enableMonitorIDMatch(bool enable);
    /** Retuns @c true if the monitor holds the time slot. */
    virtual bool monitorTimeSlotHold() const;
    /** Enables/disables monitor time slot hold. */
    virtual void enableMonitorTimeSlotHold(bool enable);

    /** Retunrs the "man down" delay in seconds. */
    virtual uint manDownDelay() const;
    /** Sets the "man down" delay in seconds. */
    virtual void setManDownDelay(uint sec);
    /** Returns the analog call hold in seconds. */
    virtual uint analogCallHold() const;
    /** Sets the analog call hold in seconds. */
    virtual void setAnalogCallHold(uint sec);

    /** Returns @c true if the GPS range reporting is enabled. */
    virtual bool gpsRangReporting() const;
    /** Enables/disables GPS range reporting. */
    virtual void enableGPSRangeReporting(bool enable);

    /** Returns @c true if the call channel is maintained. */
    virtual bool maintainCallChannel() const;
    /** Enables/disables maintaining the call channel. */
    virtual void enableMaintainCalLChannel(bool enable);

    /** Retruns the priority Zone A index. */
    virtual uint priorityZoneAIndex() const;
    /** Sets the priority zone A index. */
    virtual void setPriorityZoneAIndex(uint idx);
    /** Retruns the priority Zone B index. */
    virtual uint priorityZoneBIndex() const;
    /** Sets the priority zone B index. */
    virtual void setPriorityZoneBIndex(uint idx);

    /** Returns the GPS ranging interval in seconds. */
    virtual uint gpsRangingInterval() const;
    /** Sets the GPS ranging interval in seconds. */
    virtual void setGPSRangingInterval(uint sec);

    /** Returns @c true if the channel number is displayed. */
    virtual bool displayChannelNumber() const;
    /** Enables/disables display of channel number. */
    virtual void enableDisplayChannelNumber(bool enable);
    /** Returns @c true if the contact is displayed. */
    virtual bool displayContact() const;
    /** Enables/disables display of contact. */
    virtual void enableDisplayContact(bool enable);

    /** Returns the auto roaming period in minutes. */
    virtual uint autoRoamPeriod() const;
    /** Sets the auto roaming period in minutes. */
    virtual void setAutoRoamPeriod(uint min);

    /** Returns @c true if the key-tone level is adjustable. */
    virtual bool keyToneLevelAdjustable() const;
    /** Returns the key-tone level (0=adjustable). */
    virtual uint keyToneLevel() const;
    /** Sets the key-tone level. */
    virtual void setKeyToneLevel(uint level);
    /** Sets the key-tone level adjustable. */
    virtual void setKeyToneLevelAdjustable();

    /** Returns the display color for callsigns. */
    virtual Color callDisplayColor() const;
    /** Sets the display color for callsigns. */
    virtual void setCallDisplayColor(Color color);

    /** Returns @c true if the GPS units are imperical. */
    virtual bool gpsUnitsImperial() const;
    /** Enables/disables imperical GPS units. */
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

    /** Returns the auto-roam delay in seconds. */
    virtual uint autoRoamDelay() const;
    /** Sets the auto-roam delay in seconds. */
    virtual void setAutoRoamDelay(uint sec);

    /** Retruns the standby text color. */
    virtual Color standbyTextColor() const;
    /** Sets the standby text color. */
    virtual void setStandbyTextColor(Color color);
    /** Retruns the standby image color. */
    virtual Color standbyImageColor() const;
    /** Sets the standby image color. */
    virtual void setStandbyImageColor(Color color);

    /** Returns @c true if the last heard is shown. */
    virtual bool showLastHeard() const;
    /** Enables/disables the knob lock. */
    virtual void enableShowLastHeard(bool enable);

    /** Retuns the SMS format. */
    virtual SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(SMSFormat fmt);

    /** Returns the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual uint autoRepeaterMinFrequencyVHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMinFrequencyVHF(uint Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual uint autoRepeaterMaxFrequencyVHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMaxFrequencyVHF(uint Hz);

    /** Returns the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual uint autoRepeaterMinFrequencyUHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMinFrequencyUHF(uint Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual uint autoRepeaterMaxFrequencyUHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMaxFrequencyUHF(uint Hz);

    /** Returns the auto-repeater direction for VFO B. */
    virtual AutoRepDir autoRepeaterDirectionB() const;
    /** Sets the auto-repeater direction for VFO B. */
    virtual void setAutoRepeaterDirectionB(AutoRepDir dir);

    /** Returns @c true if a boot channel is set. */
    virtual bool defaultChannel() const;
    /** Enables/disables the boot channel. */
    virtual void enableDefaultChannel(bool enable);
    /** Returns the default zone index (0-based) for VFO A. */
    virtual uint defaultZoneIndexA() const;
    /** Sets the default zone (0-based) for VFO A. */
    virtual void setDefaultZoneIndexA(uint idx);
    /** Returns the default zone index (0-based) for VFO B. */
    virtual uint defaultZoneIndexB() const;
    /** Sets the default zone (0-based) for VFO B. */
    virtual void setDefaultZoneIndexB(uint idx);
    /** Returns @c true if the default channel for VFO A is VFO. */
    virtual bool defaultChannelAIsVFO() const;
    /** Returns the default channel index for VFO A.
     * Must be within default zone. If 0xff, default channel is VFO. */
    virtual uint defaultChannelAIndex() const;
    /** Sets the default channel index for VFO A. */
    virtual void setDefaultChannelAIndex(uint idx);
    /** Sets the default channel for VFO A to be VFO. */
    virtual void setDefaultChannelAToVFO();
    /** Returns @c true if the default channel for VFO B is VFO. */
    virtual bool defaultChannelBIsVFO() const;
    /** Returns the default channel index for VFO B.
     * Must be within default zone. If 0xff, default channel is VFO. */
    virtual uint defaultChannelBIndex() const;
    /** Sets the default channel index for VFO B. */
    virtual void setDefaultChannelBIndex(uint idx);
    /** Sets the default channel for VFO B to be VFO. */
    virtual void setDefaultChannelBToVFO();

    /** Returns the default roaming zone index. */
    virtual uint defaultRoamingZoneIndex() const;
    /** Sets the default roaming zone index. */
    virtual void setDefaultRoamingZoneIndex(uint idx);

    /** Returns @c true if repeater range check is enabled. */
    virtual bool repeaterRangeCheck() const;
    /** Enables/disables repeater range check. */
    virtual void enableRepeaterRangeCheck(bool enable);
    /** Returns the repeater range check period in seconds. */
    virtual uint repeaterRangeCheckInterval() const;
    /** Sets the repeater range check interval in seconds. */
    virtual void setRepeaterRangeCheckInterval(uint sec);
    /** Returns the number of repeater range checks. */
    virtual uint repeaterRangeCheckCount() const;
    /** Sets the number of repeater range checks. */
    virtual void setRepeaterRangeCheckCount(uint n);

    /** Returns the roaming start condition. */
    virtual RoamStart roamingStartCondition() const;
    /** Sets the roaming start condition. */
    virtual void setRoamingStartCondition(RoamStart cond);

    /** Returns the backlight duration during TX in seconds. */
    virtual uint backlightTXDuration() const;
    /** Sets the backlight duration during TX in seconds. */
    virtual void setBacklightTXDuration(uint sec);

    /** Retruns @c true if the "separate display" is enabled. */
    virtual bool separateDisplay() const;
    /** Enables/disables "separate display. */
    virtual void enableSeparateDisplay(bool enable);

    /** Retruns @c true if keep caller is enabled. */
    virtual bool keepCaller() const;
    /** Enables/disables keep caller. */
    virtual void enableKeepCaller(bool enable);

    /** Returns the channel name color. */
    virtual Color channelNameColor() const;
    /** Sets the channel name color. */
    virtual void setChannelNameColor(Color color);

    /** Retruns @c true if repeater check notification is enabled. */
    virtual bool repeaterCheckNotification() const;
    /** Enables/disables repeater check notification. */
    virtual void enableRepeaterCheckNotification(bool enable);

    /** Returns the backlight duration during RX in seconds. */
    virtual uint backlightRXDuration() const;
    /** Sets the backlight duration during RX in seconds. */
    virtual void setBacklightRXDuration(uint sec);

    /** Retruns @c true if roaming is enabled. */
    virtual bool roaming() const;
    /** Enables/disables repeater check notification. */
    virtual void enableRoaming(bool enable);

    /** Returns the mute delay in minutes. */
    virtual uint muteDelay() const;
    /** Sets the mute delay in minutes. */
    virtual void setMuteDelay(uint min);

    /** Returns the number of repeater check notifications. */
    virtual uint repeaterCheckNumNotifications() const;
    /** Sets the number of repeater check notifications. */
    virtual void setRepeaterCheckNumNotifications(uint num);

    /** Retruns @c true if boot GPS check is enabled. */
    virtual bool bootGPSCheck() const;
    /** Enables/disables boot GPS check. */
    virtual void enableBootGPSCheck(bool enable);
    /** Retruns @c true if boot reset is enabled. */
    virtual bool bootReset() const;
    /** Enables/disables boot reset. */
    virtual void enableBootReset(bool enable);

    bool fromConfig(const Flags &flags, Context &ctx);
    bool updateConfig(Context &ctx);
  };

  /** Implements the GPS message settings (part of general settings).
   *
   * Memory layout of the encoded GPS message (size 0x0030 bytes):
   * @verbinclude d878uv_gpsmessage.txt */
  class GPSMessageElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GPSMessageElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    GPSMessageElement(uint8_t *ptr);

    /** Resets the message. */
    void clear();

    /** Returns the GPS message. */
    virtual QString message() const;
    /** Sets the message. */
    virtual void setMessage(const QString &message);

    /** Encodes GPS message from config object. */
    virtual bool fromConfig(const Flags &flags, Context &ctx);
    /** Updates config. */
    virtual bool updateConfig(Context &ctx) const;
  };

  /** General settings extension element for the D878UV.
   *
   * Memory representation of the encoded settings element (size 0x100 bytes):
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
    GeneralSettingsExtensionElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit GeneralSettingsExtensionElement(uint8_t *ptr);

    /** Resets the settings. */
    void clear();

    /** Retruns @c true if talker alias is send. */
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
    /** Retunrs the index of the UHF 2 offset frequency. */
    virtual uint autoRepeaterUHF2OffsetIndex() const;
    /** Sets the index of the UHF 2 offset frequency. */
    virtual void setAutoRepeaterUHF2OffsetIndex(uint idx);
    /** Clears the auto repeater UHF 2 offset frequency index. */
    virtual void clearAutoRepeaterUHF2OffsetIndex();

    /** Returns @c true if the auto repeater VHF 2 offset index is set. */
    virtual bool hasAutoRepeaterVHF2OffsetIndex() const;
    /** Retunrs the index of the VHF 2 offset frequency. */
    virtual uint autoRepeaterVHF2OffsetIndex() const;
    /** Sets the index of the VHF 2 offset frequency. */
    virtual void setAutoRepeaterVHF2OffsetIndex(uint idx);
    /** Clears the auto repeater VHF 2 offset frequency index. */
    virtual void clearAutoRepeaterVHF2OffsetIndex();

    /** Returns the minimum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual uint autoRepeaterVHF2MinFrequency() const;
    /** Sets the minimum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual void setAutoRepeaterVHF2MinFrequency(uint hz);
    /** Returns the maximum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual uint autoRepeaterVHF2MaxFrequency() const;
    /** Sets the maximum frequency in Hz for the auto-repeater VHF 2 band. */
    virtual void setAutoRepeaterVHF2MaxFrequency(uint hz);
    /** Returns the minimum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual uint autoRepeaterUHF2MinFrequency() const;
    /** Sets the minimum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual void setAutoRepeaterUHF2MinFrequency(uint hz);
    /** Returns the maximum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual uint autoRepeaterUHF2MaxFrequency() const;
    /** Sets the maximum frequency in Hz for the auto-repeater UHF 2 band. */
    virtual void setAutoRepeaterUHF2MaxFrequency(uint hz);

    /** Retuns the GPS mode. */
    virtual GPSMode gpsMode() const;
    /** Sets the GPS mode. */
    virtual void setGPSMode(GPSMode mode);

    /** Encodes the settings from the config. */
    virtual bool fromConfig(const Flags &flags, Context &ctx);
    /** Update config from settings. */
    virtual bool updateConfig(Context &ctx);
  };

  /** Represents the APRS settings within the binary D878UV codeplug.
   *
   * Memmory layout of APRS settings (size 0x0040 bytes):
   * @verbinclude d878uv_aprssetting.txt
   */
  class AnalogAPRSSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSSettingsElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    explicit AnalogAPRSSettingsElement(uint8_t *ptr);

    /** Resets the settings. */
    void clear();

    /** Returns the transmit frequency in Hz. */
    virtual uint frequency() const;
    /** Sets the transmit frequency in Hz. */
    virtual void setFrequency(uint hz);

    /** Retruns the TX delay in ms. */
    virtual uint txDelay() const;
    /** Sets the TX delay in ms. */
    virtual void setTXDelay(uint ms);

    /** Retruns the sub tone settings. */
    virtual Signaling::Code txTone() const;
    /** Sets the sub tone settings. */
    virtual void setTXTone(Signaling::Code code);

    /** Retruns the manual TX interval in seconds. */
    virtual uint manualTXInterval() const;
    /** Sets the manual TX interval in seconds. */
    virtual void setManualTXInterval(uint sec);

    /** Returns @c true if the auto transmit is enabled. */
    virtual bool autoTX() const;
    /** Retruns the auto TX interval in seconds. */
    virtual uint autoTXInterval() const;
    /** Sets the auto TX interval in seconds. */
    virtual void setAutoTXInterval(uint sec);
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

    /** Retunrs the destination call. */
    virtual QString destination() const;
    /** Returns the destination SSID. */
    virtual uint destinationSSID() const;
    /** Sets the destination call & SSID. */
    virtual void setDestination(const QString &call, uint ssid);
    /** Retunrs the source call. */
    virtual QString source() const;
    /** Returns the source SSID. */
    virtual uint sourceSSID() const;
    /** Sets the source call & SSID. */
    virtual void setSource(const QString &call, uint ssid);

    /** Returns the path string. */
    virtual QString path() const;
    /** Sets the path string. */
    virtual void setPath(const QString &path);

    /** Retunrs the APRS icon. */
    virtual APRSSystem::Icon icon() const;
    /** Sets the APRS icon. */
    virtual void setIcon(APRSSystem::Icon icon);

    /** Returns the transmit power. */
    virtual Channel::Power power() const;
    /** Sets the transmit power. */
    virtual void setPower(Channel::Power power);

    /** Retunrs the pre-wave delay in ms. */
    virtual uint preWaveDelay() const;
    /** Sets the pre-wave delay in ms. */
    virtual void setPreWaveDelay(uint ms);

    /** Configures this APRS system from the given generic config. */
    virtual bool fromAPRSSystem(const APRSSystem *sys, Context &ctx);
    /** Constructs a generic APRS system configuration from this APRS system. */
    virtual APRSSystem *toAPRSSystem();
    /** Links the transmit channel within the generic APRS system based on the transmit frequency
     * defined within this APRS system. */
    virtual bool linkAPRSSystem(APRSSystem *sys, Context &ctx);

  };

  /** Represents an extension to the APRS settings.
   *
   * Memmory layout of APRS settings (0x60byte):
   * @verbinclude d878uv_aprssettingext.txt */
  class AnalogAPRSSettingsExtensionElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSSettingsExtensionElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    AnalogAPRSSettingsExtensionElement(uint8_t *ptr);

    /** Resets the settings. */
    void clear();

    /** Returns the fixed altitude in meter. */
    virtual uint fixedAltitude() const;
    /** Sets the fixed altitude in meter. */
    virtual void setFixedAltitude(uint m);

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

  /** Represents an analog APRS RX entry.
   *
   * Memmory layout of analog APRS RX entry (size 0x0008 bytes):
   * @verbinclude d878uv_aprsrxentry.txt */
  class AnalogAPRSRXEntryElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AnalogAPRSRXEntryElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    AnalogAPRSRXEntryElement(uint8_t *ptr);

    /** Resets the entry. */
    void clear();
    bool isValid() const;

    /** Returns the call sign. */
    virtual QString call() const;
    /** Returns the SSID. */
    virtual uint ssid() const;
    /** Sets the call, SSID and enables the entry. */
    virtual void setCall(const QString &call, uint ssid);
  };

  /** Represents the 8 DMR-APRS systems within the binary codeplug.
   *
   * Memmory layout of encoded DMR-APRS systems (size 0x0060 bytes):
   * @verbinclude d878uv_dmraprssystems.txt */
  class DMRAPRSSystemsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DMRAPRSSystemsElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    DMRAPRSSystemsElement(uint8_t *ptr);

    /** Resets the systems. */
    void clear();

    /** Retunrs @c true if the channel points to the current/selected channel. */
    virtual bool channelIsSelected(uint n) const;
    /** Returns the digital channel index for the n-th system. */
    virtual uint channelIndex(uint n) const;
    /** Sets the digital channel index for the n-th system. */
    virtual void setChannelIndex(uint n, uint idx);
    /** Sets the channel to the current/selected channel. */
    virtual void setChannelSelected(uint n);

    /** Returns the destination contact for the n-th system. */
    virtual uint destination(uint n) const;
    /** Sets the destination contact for the n-th system. */
    virtual void setDestination(uint n, uint idx);

    /** Returns the call type for the n-th system. */
    virtual DigitalContact::Type callType(uint n) const;
    /** Sets the call type for the n-th system. */
    virtual void setCallType(uint n, DigitalContact::Type type);

    /** Returns @c true if the n-th system overrides the channel time-slot. */
    virtual bool timeSlotOverride(uint n);
    /** Retunrs the time slot if overriden (only valid if @c timeSlot returns true). */
    virtual DigitalChannel::TimeSlot timeSlot(uint n) const;
    /** Overrides the time slot of the n-th selected channel. */
    virtual void setTimeSlot(uint n, DigitalChannel::TimeSlot ts);
    /** Clears the time-slot override. */
    virtual void clearTimeSlotOverride(uint n);

    /** Returns @c true if the roaming is enabled. */
    virtual bool roaming() const;
    /** Enables/disables roaming. */
    virtual void enableRoaming(bool enable);

    /** Returns the the repeater activation delay in ms. */
    virtual uint repeaterActivationDelay() const;
    /** Sets the repeater activation delay in ms. */
    virtual void setRepeaterActivationDelay(uint ms);

    /** Constructs all GPS system from the generic configuration. */
    virtual bool fromGPSSystems(Context &ctx);
    /** Encodes the given GPS system. */
    virtual bool fromGPSSystemObj(GPSSystem *sys, Context &ctx);
    /** Constructs a generic GPS system from the idx-th encoded GPS system. */
    virtual GPSSystem *toGPSSystemObj(int idx) const;
    /** Links the specified generic GPS system. */
    virtual bool linkGPSSystem(int idx, GPSSystem *sys, Context &ctx) const;
  };

  /** Implements the binary representation of a roaming channel within the codeplug.
   *
   * Memmory layout of roaming channel (size 0x0020 bytes):
   * @verbinclude d878uv_roamingchannel.txt */
  class RoamingChannelElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RoamingChannelElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    RoamingChannelElement(uint8_t *ptr);

    /** Resets the roaming channel. */
    void clear();

    /** Retruns the RX frequency in Hz. */
    virtual uint rxFrequency() const;
    /** Sets the RX frequeny in Hz. */
    virtual void setRXFrequency(uint hz);
    /** Retruns the TX frequency in Hz. */
    virtual uint txFrequency() const;
    /** Sets the TX frequeny in Hz. */
    virtual void setTXFrequency(uint hz);

    /** Retunrs the color code. */
    virtual uint colorCode() const;
    /** Sets the color code. */
    virtual void setColorCode(uint cc);

    /** Returns the time slot. */
    virtual DigitalChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DigitalChannel::TimeSlot ts);

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &name);

    /** Constructs a roaming channel from the given digital channel. */
    virtual bool fromChannel(const DigitalChannel *ch);
    /** Constructs/Searches a matching DigitalChannel for this roaming channel. */
    virtual DigitalChannel *toChannel(Context &ctx);
  };

  /** Represents a roaming zone within the binary codeplug.
   *
   * Memmory layout of roaming zone (0x80byte):
   * @verbinclude d878uv_roamingzone.txt */
  class RoamingZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RoamingZoneElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    RoamingZoneElement(uint8_t *ptr);

    /** Clears the roaming zone. */
    void clear();

    /** Returns @c true if the n-th member is set. */
    virtual bool hasMember(uint n) const;
    /** Returns the n-th member index. */
    virtual uint member(uint n) const;
    /** Sets the n-th member index. */
    virtual void setMember(uint n, uint idx);
    /** Clears the n-th member. */
    virtual void clearMember(uint n);

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Assembles a binary representation of the given RoamingZone instance.*/
    virtual bool fromRoamingZone(RoamingZone *zone, const QHash<DigitalChannel *, uint> &map);
    /** Constructs a @c RoamingZone instance from this configuration. */
    virtual RoamingZone *toRoamingZone() const;
    /** Links the given RoamingZone. */
    virtual bool linkRoamingZone(RoamingZone *zone, const QHash<uint, DigitalChannel *> &map);
  };

  /** Represents an AES encryption key.
   *
   * Binary representation of the key (size 0x0040 bytes):
   * @verbinclude d878uv_aeskey.txt */
  class AESEncryptionKeyElement: public Element
  {
  protected:
    /** Hidden constructor. */
    AESEncryptionKeyElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    AESEncryptionKeyElement(uint8_t *ptr);

    /** Resets the key. */
    void clear();

    bool isValid() const;

    /** Returns the key index. */
    virtual uint index() const;
    /** Sets the key index. */
    virtual void setIndex(uint idx);

    /** Returns the actual key. */
    virtual QByteArray key() const;
    /** Sets the key. */
    virtual void setKey(const QByteArray &key);
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
    RadioInfoElement(uint8_t *ptr, uint size);

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

    /** Retuns the band-select password. */
    virtual QString bandSelectPassword() const;
    /** Sets the band-select password. */
    virtual void setBandSelectPassword(const QString &passwd);

    /** Retuns the radio type. */
    virtual QString radioType() const;

    /** Retuns the program password. */
    virtual QString programPassword() const;
    /** Sets the program password. */
    virtual void setProgramPassword(const QString &passwd);

    /** Retuns the area code. */
    virtual QString areaCode() const;
    /** Retuns the serial number. */
    virtual QString serialNumber() const;
    /** Retuns the production date. */
    virtual QString productionDate() const;
    /** Retuns the manufacturer code. */
    virtual QString manufacturerCode() const;
    /** Retuns the maintained date. */
    virtual QString maintainedDate() const;
    /** Retuns the dealer code. */
    virtual QString dealerCode() const;
    /** Retuns the stock date. */
    virtual QString stockDate() const;
    /** Retuns the sell date. */
    virtual QString sellDate() const;
    /** Retuns the seller. */
    virtual QString seller() const;
    /** Retuns the maintainer note. */
    virtual QString maintainerNote() const;
  };

public:
  /** Empty constructor. */
  explicit D878UVCodeplug(QObject *parent = nullptr);

  /** Clears and resets the complete codeplug to some default values. */
  void clear();

  /** Sets all bitmaps for the given config. */
  void setBitmaps(Config *config);

  /** Allocate all code-plug elements that must be downloaded for decoding. All code-plug elements
   * with the radio that are not represented within the common Config are omitted. */
  void allocateForDecoding();
  /** Allocate all code-plug elements that must be written back to the device to maintain a working
   * codeplug. These elements might be updated during encoding. */
  void allocateUpdated();
  /** Allocate all code-plug elements that are defined through the common Config. */
  void allocateForEncoding();

protected:
  bool decodeElements(Context &ctx);
  bool encodeElements(const Flags &flags, Context &ctx);

  void allocateChannels();
  bool encodeChannels(const Flags &flags, Context &ctx);
  bool createChannels(Context &ctx);
  bool linkChannels(Context &ctx);

  void allocateGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx);
  bool decodeGeneralSettings(Context &ctx);

  void allocateGPSSystems();
  bool encodeGPSSystems(const Flags &flags, Context &ctx);
  bool createGPSSystems(Context &ctx);
  bool linkGPSSystems(Context &ctx);

  /** Allocates memory to store all roaming channels and zones. */
  virtual void allocateRoaming();
  /** Encodes the roaming channels and zones. */
  virtual bool encodeRoaming(const Flags &flags, Context &ctx);
  /** Creates roaming channels and zones from codeplug. */
  virtual bool createRoaming(Context &ctx);
  /** Links roaming channels and zones. */
  virtual bool linkRoaming(Context &ctx);
};

#endif // D878UVCODEPLUG_HH
