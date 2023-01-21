#ifndef DMR6X2UVCODEPLUG_HH
#define DMR6X2UVCODEPLUG_HH

#include "d878uv_codeplug.hh"

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
 *  <tr><td>02501000</td> <td>000040</td> <td>APRS settings,
 *    see @c D878UVCodeplug::AnalogAPRSSettingsElement.</td>
 *  <tr><td>02501040</td> <td>000060</td> <td>DMR-APRS systems,
 *    see @c D878UVCodeplug::DMRAPRSSystemsElement.</td>
 *  <tr><td>02501200</td> <td>000040</td> <td>APRS Text, up to 60 chars ASCII, 0-padded.</td>
 *  <tr><td>02501280</td> <td>000030</td> <td>GPS template message, ASCII, 0-padded.</td> </tr>
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

public:
  /** General settings element for the DMR-6X2UV.
   *
   * Extends the @c AnytoneCodeplug::GeneralSettingsElement by the device specific settings for
   * the BTECH DMR-6X2UV.
   *
   * Memory representation of the encoded settings element (size 0x0e0 bytes):
   * @verbinclude dmr6x2uv_generalsettings.txt */
  class GeneralSettingsElement: public AnytoneCodeplug::GeneralSettingsElement
  {
  public:
    /** Possible colors for the callsign etc. */
    enum class DisplayColor {
      Orange=0x00, Red=0x01, Yellow=0x02, Green=0x03, Turquoise=0x04, Blue=0x05, White=0x06, Black=0x07
    };

    /** Possible GPS modes. */
    enum class SimplexRepeaterSlot {
      TS1 = 0, TS2 = 1, Channel = 2
    };

    /** Possible SMS formats. */
    enum class SMSFormat {
      MFormat = 0x00,
      HFormat = 0x01,
      DMRStandard = 0x02
    };

    /** Possible GPS units. */
    enum class GPSUnits {
      Metric = 0x00,
      Imperial = 0x01
    };

    /** Possible background colors. */
    enum class BackgroundColor {
      Black = 0x00,
      Blue  = 0x01
    };

  protected:
    /** Hidden Constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);

    /** Retunrs the color for call-signs. */
    virtual DisplayColor callsignDisplayColor() const;
    /** Sets the color for call-signs. */
    void setCallsignDisplayColor(DisplayColor color);

    /** Returns @c true if the simplex repeater feature is enabled. */
    virtual bool simplexRepeaterEnabled() const;
    /** Enables disables the simplex repeater feature. */
    virtual void enableSimplexRepeater(bool enable);
    /** Returns @c true if the speaker is switched on during RX in simplex repeater mode,
     * see @c simplexRepeaterEnabled. */
    virtual bool monitorSimplexRepeaterEnabled() const;
    /** Enables/disables the speaker during RX in simplex repeater mode. */
    virtual void enableMonitorSimplexRepeater(bool enable);
    /** Returns the time-slot in simplex repeater mode. */
    virtual SimplexRepeaterSlot simplexRepeaterTimeslot() const;
    /** Sets the time-slot in simplex repeater mode. */
    virtual void setSimplexRepeaterTimeslot(SimplexRepeaterSlot slot);

    /** Returns the GPS ranging intervall in seconds. */
    virtual unsigned int gpsRangingIntervall() const;
    /** Sets the GPS ranging intervall in seconds. */
    virtual void setGPSRangingIntervall(unsigned int sec);
    /** Returns @c true if the current contact is shown. */
    virtual bool currentContactShown() const;
    /** Shows/hides the current contact. */
    virtual void showCurrentContact(bool show);
    /** Key sound volume. Any value from 1-15, 0 means adjustable. */
    virtual unsigned int keySoundVolume() const;
    /** Sets the key sound volume. Any value from 1-15, 0 means adjustable. */
    virtual void setKeySoundVolume(unsigned int vol);

    /** Returns @c true if the "professional key" gets locked too. */
    virtual bool professionalKeyLock() const;
    /** Enables/disables professional key lock. */
    virtual void enableProfessionalKeyLock(bool enable);
    /** Returns @c true if the side keys gets locked too. */
    virtual bool sideKeyLock() const;
    /** Enables/disables side key lock. */
    virtual void enableSideKeyLock(bool enable);
    /** Returns @c true if the keyboard gets locked too. */
    virtual bool keyboadLock() const;
    /** Enables/disables keyboard lock. */
    virtual void enableKeyboradLock(bool enable);
    /** Returns @c true if the knob gets locked too. */
    virtual bool knobLock() const;
    /** Enables/disables knob lock. */
    virtual void enableKnobLock(bool enable);

    /** Returns @c true if the last call is shown on startup. */
    virtual bool lastCallShownOnStartup() const;
    /** Enables/disables showing last call on startup. */
    virtual void enableShowLastCallOnStartup(bool enable);

    /** Returns the SMS format. */
    virtual SMSFormat smsFormat() const;
    /** Sets the SMS format. */
    virtual void setSMSFormat(SMSFormat format);

    /** Retuns the units used to display distances. */
    virtual GPSUnits gpsUnits() const;
    /** Sets the GPS units. */
    virtual void setGPSUnits(GPSUnits units);

    /** Returns the VHF frequency range in MHz for the auto-repeater feature.
     * If a frequency within this range is chosen, the radio will transmit at the specified
     * offset. */
    virtual QPair<double,double> vhfAutoRepeaterFrequencyRange() const;
    /** Sets the VHF auto-repeater frequency range in MHz. */
    virtual void setVHFAutoRepeaterFrequencyRange(double lower, double upper);
    /** Returns the UHF frequency range in MHz for the auto-repeater feature.
     * If a frequency within this range is chosen, the radio will transmit at the specified
     * offset. */
    virtual QPair<double,double> uhfAutoRepeaterFrequencyRange() const;
    /** Sets the UHF auto-repeater frequency range in MHz. */
    virtual void setUHFAutoRepeaterFrequencyRange(double lower, double upper);
    /** Returns auto-repeater direction for VFO B. */
    virtual AutoRepDir autoRepeaterDirectionB() const;
    /** Sets the auto-repeater direction for VFO B. */
    virtual void setAutoRepeaterDirectionB(AutoRepDir dir);

    /** Returns @c true if the address book is send with its own code (WTF?). */
    virtual bool addressBookIsSendWithOwnCode() const;
    /** Enables/disables sending address book with own code. */
    virtual void enableSendAddressbookWithOwnCode(bool enable);

    /** Returns @c true if a default boot channel is enabled. */
    virtual bool defaultBootChannelEnabled() const;
    /** Enables/disables default boot channel. */
    virtual void enableDefaultBootChannel(bool enable);
    /** Returns the zone of the boot channel for VFO A. */
    virtual unsigned int bootZoneAIndex() const;
    /** Sets the index of the zone of the boot channel for VFO A. */
    virtual void setBootZoneAIndex(unsigned int index);
    /** Returns @c true if the boot channel for VFO A is the VFO. */
    virtual bool bootChannelAIsVFO() const;
    /** Returns the index of the boot channel for VFO A. */
    virtual unsigned int bootChannelAIndex() const;
    /** Sets the boot channel index for VFO A. */
    virtual void setBootChannelAIndex(unsigned int index);
    /** Sets the boot channel for VFO A to VFO. */
    virtual void setBootChannelAIsVFO();
    /** Returns the zone of the boot channel for VFO B. */
    virtual unsigned int bootZoneBIndex() const;
    /** Sets the index of the zone of the boot channel for VFO B. */
    virtual void setBootZoneBIndex(unsigned int index);
    /** Returns @c true if the boot channel for VFO B is the VFO. */
    virtual bool bootChannelBIsVFO() const;
    /** Returns the index of the boot channel for VFO B. */
    virtual unsigned int bootChannelBIndex() const;
    /** Sets the boot channel index for VFO B. */
    virtual void setBootChannelBIndex(unsigned int index);
    /** Sets the boot channel for VFO B to VFO. */
    virtual void setBootChannelBIsVFO();

    /** Returns @c true if the last caller is kept. */
    virtual bool lastCallerIsKept() const;
    /** Enables/disables if last caller is kept. */
    virtual void enableKeepLastCaller(bool enable);

    /** Returns true if the RX backlight stays on infinitely. */
    virtual bool rxBacklightIsAlwaysOn() const;
    /** Returns the duration of the RX backlight in seconds (max 30s). */
    virtual unsigned int rxBacklightDuration() const;
    /** Sets the RX backlight duration in seconds (max 30s). */
    virtual void setRXBacklightDuration(unsigned int dur);
    /** Enables the RX backlight to stay on infinitely. */
    virtual void setRXBacklightAlwaysOn();

    /** Returns the background color. */
    virtual BackgroundColor backgroundColor() const;
    /** Sets the background color. */
    virtual void setBackgroundColor(BackgroundColor color);

    /** Returns the group-call hang time, if group call was dialed manually. */
    virtual unsigned int manualDialedGroupCallHangTime() const;
    /** Sets the group-call hang time, if the group call was dialed maually. */
    virtual void setManualDialedGroupCallHangTime(unsigned int dur);
    /** Returns the private-call hang time, if private call was dialed manually. */
    virtual unsigned int manualDialedPrivateCallHangTime() const;
    /** Sets the private-call hang time, if the private call was dialed maually. */
    virtual void setManualDialedPrivateCallHangTime(unsigned int dur);

    /** Encodes the settings from the config. */
    virtual bool fromConfig(const Flags &flags, Context &ctx);
    /** Update config from settings. */
    virtual bool updateConfig(Context &ctx);
  };

  /** Implements some settings extension for the BTECH DMR-6X2UV.
   *
   * Memory representation of the encoded settings element (size 0x0e0 bytes):
   * @verbinclude dmr6x2uv_settingsextension.txt */
  class ExtendedSettingsElement: public Codeplug::Element
  {
  public:
    /** Possible talker alias encodings. */
    enum class TalkerAliasEncoding {
      ISO8=0x00, ISO7=0x01, Unicode=0x02
    };

    /** Possible display priorities for the talker alias. */
    enum class TalkerAliasSource {
      None=0x00, Database=0x01, OverTheAir=0x02
    };

    /** Possible font colors. */
    enum class FontColor {
      White=0x00, Black=0x01, Orange=0x02, Red=0x03, Yellow=0x04, Green=0x05, Turquoise=0x06, Blue=0x07
    };

    /** Possible name colors. */
    enum class NameColor {
      Orange=0x00, Red=0x01, Yellow=0x02, Green=0x03, Turquoise=0x04, Blue=0x05, White=0x06, Black=0x07
    };

    /** Possible repeater out-of-range alerts. */
    enum class OutOfRangeAlert {
      None = 0x00, Bell = 0x01, Voice = 0x02
    };

    /** Possible roaming conditions. */
    enum class RoamingCondition {
      FixedTime=0x00, OutOfRange=0x01
    };

    /** Possible encryption types. */
    enum class EncryptionType {
      Basic = 0x00, AES = 0x01
    };

  protected:
    /** Hidden Constructor. */
    ExtendedSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ExtendedSettingsElement(uint8_t *ptr);

    /** Resets the general settings. */
    void clear();

    /** Returns @c true, if a talker alias is send. */
    virtual bool talkerAliasIsSend() const;
    /** Enables/disables sending the talker alias. */
    virtual void enableSendTalkerAlias(bool enable);
    /** Retunrs the talker alias source. */
    virtual TalkerAliasSource talkerAliasSource() const;
    /** Sets the talker alias source. */
    virtual void setTalkerAliasSource(TalkerAliasSource source);
    /** Retunrs the talker alias encoding. */
    virtual TalkerAliasEncoding talkerAliasEncoding() const;
    /** Sets the talker alias encoding. */
    virtual void setTalkerAliasEncoding(TalkerAliasEncoding encoding);

    /** Returns the font color. */
    virtual FontColor fontColor() const;
    /** Sets the font color. */
    virtual void setFontColor(FontColor color);

    /** Returns @c true if the custom channel background is enabled. */
    virtual bool customChannelBackgroundEnabled() const;
    /** Enables/disables the custom channel background. */
    virtual void enableCustomChannelBackground(bool enable);

    /** Returns @c true if auto roaming is enabled. */
    virtual bool autoRoamingEnabled() const;
    /** Enables/disables auto roaming. */
    virtual void enableAutoRoaming(bool enable);

    /** Returns @c true if repeater check is enabled. */
    virtual bool repeaterCheckEnabled() const;
    /** Enables/disables repeater check. */
    virtual void enableRepeaterCheck(bool enable);
    /** Returns the number of times, the repeater out-of-range reminder is shown (1-10). */
    virtual unsigned int repeaterOutOfRangeReminder() const;
    /** Sets the number of times, the repeater out-of-range reminder is shown (1-10). */
    virtual void setRepeaterOutOfRangeReminder(unsigned int n);
    /** Returns the repeater check intervall in seconds (5-50s). */
    virtual unsigned int repeaterCheckIntervall() const;
    /** Sets the repeater check intervall in seconds (5-50s). */
    virtual void setRepeaterCheckIntervall(unsigned int intervall);
    /** Returns the repeater out-of-range alert type. */
    virtual OutOfRangeAlert repeaterOutOfRangeAlert() const;
    /** Sets the repeater out-of-range alert type. */
    virtual void setRepeaterOutOfRangeAlert(OutOfRangeAlert alert);
    /** Returns the number of times, a repeater reconnection is tried (3-5). */
    virtual unsigned int repeaterReconnections() const;
    /** Sets the number of times, a repeater reconnection is tried (3-5). */
    virtual void setRepeaterReconnections(unsigned int n);

    /** Retunrs the roaming zone index. */
    virtual unsigned int roamingZoneIndex() const;
    /** Sets the roaming zone index. */
    virtual void setRoamingZoneIndex(unsigned int index);
    /** Returns the condition to start roaming. */
    virtual RoamingCondition startRoamingCondition() const;
    /** Sets the condition to start roaming. */
    virtual void setStartRoamingCondition(RoamingCondition cond);
    /** Returns the auto-roaming intervall in minutes (1-256). */
    virtual unsigned int autoRoamingIntervall() const;
    /** Sets the auto-roaming intervall in minutes (1-256). */
    virtual void setAutoRoamingIntervall(unsigned int minutes);
    /** Returns the effective roaming waiting time in seconds (0-30s). */
    virtual unsigned int effectiveRoamingWaitingTime() const;
    /** Sets the effective roaming waiting time in seconds (0-30s). */
    virtual void setEffectiveRoamingWaitingTime(unsigned int sec);
    /** Returns the roaming return condition. */
    virtual RoamingCondition roamingReturnCondition() const;
    /** Sets the roaming return condition. */
    virtual void setRoamingReturnCondition(RoamingCondition cond);

    /** Returns the mute timer in minutes. */
    virtual unsigned int muteTimer() const;
    /** Sets the mute timer in minutes. */
    virtual void setMuteTimer(unsigned int minutes);

    /** Returns the encryption type. */
    virtual EncryptionType encryptionType() const;
    /** Sets the encryption type. */
    virtual void setEncryptionType(EncryptionType type);

    /** Returns the name color for zone A. */
    virtual NameColor zoneANameColor() const;
    /** Sets the name color for zone A. */
    virtual void setZoneANameColor(NameColor color);
    /** Returns the name color for zone B. */
    virtual NameColor zoneBNameColor() const;
    /** Sets the name color for zone B. */
    virtual void setZoneBNameColor(NameColor color);
    /** Returns the name color for channel A. */
    virtual NameColor channelANameColor() const;
    /** Sets the name color for channel A. */
    virtual void setChannelANameColor(NameColor color);
    /** Returns the name color for channel B. */
    virtual NameColor channelBNameColor() const;
    /** Sets the name color for channel B. */
    virtual void setChannelBNameColor(NameColor color);

    /** Encodes the settings from the config. */
    virtual bool fromConfig(const Flags &flags, Context &ctx);
    /** Update config from settings. */
    virtual bool updateConfig(Context &ctx);
  };

  /** Implements the channel element for the BTECH DMR-6X2UV.
   *  Extends the AnytoneCodeplug::ChannelElement by the device specific features, like multiple
   *  scan lists associated with the channel.
   *
   *  Memory representation of the encoded channel element (size 0x040 bytes):
   *  @verbinclude dmr6x2uv_channel.txt */
  class ChannelElement: public AnytoneCodeplug::ChannelElement
  {
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
    bool ranging() const;
    /** Enables/disables ranging. */
    void enableRanging(bool enable);

  };

public:
  /** Hidden constructor. */
  explicit DMR6X2UVCodeplug(const QString &label, QObject *parent=nullptr);

public:
  /** Empty constructor. */
  explicit DMR6X2UVCodeplug(QObject *parent=nullptr);

protected:
  bool allocateBitmaps();
  void setBitmaps(Config *config);
  void allocateForDecoding();
  void allocateUpdated();
  void allocateForEncoding();

  bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  virtual void allocateGeneralSettings();
  virtual bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  virtual bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

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
};

#endif // DMR6X2UVCODEPLUG_HH
