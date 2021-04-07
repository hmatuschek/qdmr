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
 *  <tr><td>024C1500</td> <td>000200</td>      <td>Bitmap of 4000 channels, default 0x00, 0x00 padded.</td></tr>
 *  <tr><td>00800000</td> <td>max. 002000</td> <td>Channel bank 0 of upto 128 channels, see @c channel_t 64 b each. </td></tr>
 *  <tr><td>00802000</td> <td>max, 002000</td> <td>Unknown data, Maybe extended channel information for channel bank 0?
 *    It is of exactly the same size as the channel bank 0. Mostly 0x00, a few 0xff.</td></tr>
 *  <tr><td>00840000</td> <td>max. 002000</td> <td>Channel bank 1 of upto 128 channels.</td></tr>
 *  <tr><td>00842000</td> <td>max. 002000</td> <td>Unknown data, related to CH bank 1?</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>00FC0000</td> <td>max. 000800</td> <td>Channel bank 32, upto 32 channels.</td></tr>
 *  <tr><td>00FC2000</td> <td>max. 000800</td> <td>Unknown data, realted to CH bank 32.</td></tr>
 *  <tr><td>00FC0800</td> <td>000040</td>      <td>VFO A settings, see @c channel_t.</td></tr>
 *  <tr><td>00FC0840</td> <td>000040</td>      <td>VFO B settings, see @c channel_t.</td></tr>
 *  <tr><td>00FC2800</td> <td>000080</td>      <td>Unknonw data, related to VFO A+B?
 *    It is of exactly the same size as the two VFO channels. Mostly 0x00, a few 0xff. Same pattern as
 *    the unknown data associated with channel banks.</td></tr>
 *
 *  <tr><th colspan="3">Zones</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>024C1300</td> <td>000020</td>      <td>Bitmap of 250 zones.</td></tr>
 *  <tr><td>01000000</td> <td>max. 01f400</td> <td>250 zones channel lists of 250 16bit indices each.
 *    0-based, little endian, default/padded=0xffff. Offset between channel lists 0x200, size of each list 0x1f4.</td></tr>
 *  <tr><td>02540000</td> <td>max. 001f40</td> <td>250 Zone names.
 *    Each zone name is upto 16 ASCII chars long and gets 0-padded to 32b.</td></tr>
 *
 *  <tr><th colspan="3">Roaming</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>01042000</td> <td>000020</td>      <td>Roaming channel bitmask, up to 250 bits, 0-padded, default 0.</td></tr>
 *  <tr><td>01040000</td> <td>max. 0x1f40</td> <td>Optional up to 250 roaming channels, of 32b each.
 *    See @c roaming_channel_t for details.</td></tr>
 *  <tr><td>01042080</td> <td>000010</td>      <td>Roaming zone bitmask, up to 64 bits, 0-padded, default 0.</td></tr>
 *  <tr><td>01043000</td> <td>max. 0x2000</td> <td>Optional up to 64 roaming zones, of 128b each.
 *    See @c roaming_zone_t for details.</td></tr>
 *
 *  <tr><th colspan="3">Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02600000</td> <td>max. 009C40</td> <td>Index list of valid contacts.
 *    10000 32bit indices, little endian, default 0xffffffff</td></tr>
 *  <tr><td>02640000</td> <td>000500</td>      <td>Contact bitmap, 10000 bit, inverted, default 0xff, 0x00 padded.</td></tr>
 *  <tr><td>02680000</td> <td>max. 0f4240</td> <td>10000 contacts, see @c contact_t.
 *    As each contact is 100b, they do not align with the 16b blocks being transferred to the device.
 *    Hence contacts are organized internally in groups of 4 contacts forming a "bank". </td></tr>
 *  <tr><td>04340000</td> <td>max. 013880</td> <td>DMR ID to contact index map, see @c contact_map_t.
 *    Sorted by ID, empty entries set to 0xffffffffffffffff.</td>
 *
 *  <tr><th colspan="3">Analog Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02900000</td> <td>000080</td>      <td>Index list of valid ananlog contacts.</td></tr>
 *  <tr><td>02900100</td> <td>000080</td>      <td>Bytemap for 128 analog contacts.</td></tr>
 *  <tr><td>02940000</td> <td>max. 000180</td> <td>128 analog contacts. See @c analog_contact_t.
 *    As each analog contact is 24b, they do not align with the 16b transfer block-size. Hence
 *    analog contacts are internally organized in groups of 2. </td></tr>
 *
 *  <tr><th colspan="3">RX Group Lists</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>025C0B10</td> <td>000020</td>      <td>Bitmap of 250 RX group lists, default/padding 0x00.</td></tr>
 *  <tr><td>02980000</td> <td>max. 000120</td> <td>Grouplist 0, see @c grouplist_t.</td></tr>
 *  <tr><td>02980200</td> <td>max. 000120</td> <td>Grouplist 1</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>0299f200</td> <td>max. 000120</td> <td>Grouplist 250</td></tr>
 *
 *  <tr><th colspan="3">Scan lists</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1340</td> <td>000020</td> <td>Bitmap of 250 scan lists.</td></tr>
 *  <tr><td>01080000</td> <td>000090</td> <td>Bank 0, Scanlist 1, see @c scanlist_t. </td></tr>
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
 *  <tr><td>02580000</td> <td>max. 001f40</td> <td>250 Radio IDs. See @c radioid_t.</td></tr>
 *
 *  <tr><th colspan="3">GPS/APRS</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02501000</td> <td>000040</td> <td>APRS settings, see @c aprs_setting_t.</td>
 *  <tr><td>02501040</td> <td>000060</td> <td>APRS settings, see @c gps_systems_t.</td>
 *  <tr><td>02501200</td> <td>000040</td> <td>APRS Text, upto 60 chars ASCII, 0-padded.</td>
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>000630</td> <td>General settings, see @c general_settings_base_t.</td></tr>
 *  <tr><td>02501280</td> <td>000030</td> <td>General settings extension 1, see @c general_settings_ext1_t.</td></tr>
 *  <tr><td>02501400</td> <td>000100</td> <td>General settings extension 2, see @c general_settings_ext2_t.</td></tr>
 *  <tr><td>024C2000</td> <td>0003F0</td> <td>List of 250 auto-repeater offset frequencies.
 *    32bit little endian frequency in 10Hz. I.e., 600kHz = 60000. Default 0x00000000, 0x00 padded.</td></tr>
 *
 *  <tr><th colspan="3">Messages</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>01640000</td> <td>max. 000100</td> <td>Some kind of linked list of messages.
 *    See @c message_list_t. Each entry has a size of 0x10.</td></tr>
 *  <tr><td>01640800</td> <td>000090</td>      <td>Bytemap of up to 100 valid messages.
 *    0x00=valid, 0xff=invalid, remaining 46b set to 0x00.</td></tr>
 *  <tr><td>02140000</td> <td>max. 000800</td> <td>Bank 0, Messages 1-8.
 *    Each message consumes 0x100b. See @c message_t. </td></tr>
 *  <tr><td>02180000</td> <td>max. 000800</td> <td>Bank 1, Messages 9-16</td></tr>
 *  <tr><td>...</td>      <td>...</td>         <td>...</td></tr>
 *  <tr><td>02440000</td> <td>max. 000800</td> <td>Bank 12, Messages 97-100</td></tr>
 *
 *  <tr><th colspan="3">Hot Keys</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>025C0000</td> <td>000100</td> <td>4 analog quick-call settings. See @c analog_quick_call_t.</td>
 *  <tr><td>025C0B00</td> <td>000010</td> <td>Status message bitmap.</td>
 *  <tr><td>025C0100</td> <td>000400</td> <td>Upto 32 status messages.
 *    Length unknown, offset 0x20. ASCII 0x00 terminated and padded.</td>
 *  <tr><td>025C0500</td> <td>000360</td> <td>18 hot-key settings, see @c hotkey_t</td></tr>
 *
 *  <tr><th colspan="3">Encryption keys</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C4000</td> <td>004000</td> <td>Upto 256 AES encryption keys.
 *    See @c encryption_key_t.</td></tr>
 *
 *  <tr><th colspan="3">Misc</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C1400</td> <td>000020</td> <td>Alarm setting, see @c analog_alarm_setting_t.</td></tr>
 *
 *  <tr><th colspan="3">FM Broadcast</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02480210</td> <td>000020</td>      <td>Bitmap of 100 FM broadcast channels.</td></tr>
 *  <tr><td>02480000</td> <td>max. 000200</td> <td>100 FM broadcast channels. Encoded
 *    as 8-digit BCD little-endian in 100Hz. Filled with 0x00.</td></tr>
 *  <tr><td>02480200</td> <td>000010</td>      <td>FM broadcast VFO frequency. Encoded
 *    as 8-digit BCD little-endian in 100Hz. Filled with 0x00.</td></tr>
 *
 *  <tr><th colspan="3">Still unknown</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>024C0000</td> <td>000020</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C0C80</td> <td>000010</td> <td>Unknown data, bitmap?, default 0x00.</td></tr>
 *  <tr><td>024C0D00</td> <td>000200</td> <td>Empty, set to 0x00?`</td></tr>
 *  <tr><td>024C1000</td> <td>0000D0</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1100</td> <td>000010</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1280</td> <td>000020</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1440</td> <td>000030</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1700</td> <td>000040</td> <td>Unknown, 8bit indices.</td></tr>
 *  <tr><td>024C1800</td> <td>000500</td> <td>Empty, set to 0x00?</td></tr>
 *  <tr><td>024C2400</td> <td>000030</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C2600</td> <td>000010</td> <td>Unknown data.</td></tr>
 * </table>
 *
 * @ingroup d878uv */
class D878UVCodeplug : public D868UVCodeplug
{
  Q_OBJECT

public:
  /** Represents the general config of the radio within the binary codeplug.
   *
   * At 0x02500000, size 0x100. */
  struct __attribute__((packed)) general_settings_base_t {
    /** Possible power-on display settings. */
    enum PowerOnDisplay : uint8_t {
      PWR_ON_DEFAULT = 0,           ///< Default.
      PWR_ON_CUSTOM_TEXT = 1,       ///< Custom text.
      PWR_ON_CUSTOM_IMG = 2         ///< Custom image.
    };

    /** Possible display modes. */
    enum DisplayMode : uint8_t {
      DISPLAY_CHANNEL = 0,         ///< Display channel name.
      DISPLAY_FREQUENCY = 1        ///< Display channel frequency.
    };

    /** Controlls the automatic shut down. */
    enum AutoShutdown : uint8_t {
      AUTO_SHUTDOWN_OFF = 0,       ///< Automatic shut down disabled.
      AUTO_SHUTDOWN_10min = 1,     ///< Automatic shut down after 10min.
      AUTO_SHUTDOWN_30min = 2,     ///< Automatic shut down after 30min.
      AUTO_SHUTDOWN_60min = 3,     ///< Automatic shut down after 60min.
      AUTO_SHUTDOWN_120min = 4     ///< Automatic shut down after 120min.
    };

    /** Possible UI languages. */
    enum Language : uint8_t {
      LANG_ENGLISH = 0,                 ///< UI Language is english.
      LANG_GERMAN  = 1                  ///< UI Language is german.
    };

    /** Possible VFO frequency steps. */
    enum FreqStep : uint8_t {
      FREQ_STEP_2_5kHz = 0,             ///< 2.5kHz
      FREQ_STEP_5kHz = 1,               ///< 5kHz
      FREQ_STEP_6_25kHz = 2,            ///< 6.25kHz
      FREQ_STEP_10kHz = 3,              ///< 10kHz
      FREQ_STEP_12_5kHz = 4,            ///< 12.5kHz
      FREQ_STEP_20kHz = 5,              ///< 20kHz
      FREQ_STEP_25kHz = 6,              ///< 25kHz
      FREQ_STEP_50kHz = 7               ///< 50kHz
    };

    /** Possible power-save modes. */
    enum PowerSave : uint8_t {
      PWR_SAVE_OFF = 0,                ///< Power save disabled.
      PWR_SAVE_1_TO_1 = 1,             ///< Power save 1:1.
      PWR_SAVE_2_TO_1 = 2              ///< Power save 2:1.
    };

    enum VFOScanType : uint8_t {
      SCAN_TYPE_TO = 0,
      SCAN_TYPE_CO = 1,
      SCAN_TYPE_SE = 2
    };

    enum KeyFunction : uint8_t {
      KF_OFF = 0x00, KF_VOLTAGE = 0x01, KF_POWER = 0x02, KF_REPEATER = 0x03, KF_REVERSE = 0x04,
      KF_DIGITAL_ENCRYPTION = 0x05, KF_CALL = 0x06, KF_VOX = 0x07, KF_VFO_CHANNEL = 0x08,
      KF_SUB_PTT = 0x09, KF_SCAN = 0x0a, KF_FM = 0x0b, KF_ALARM = 0x0c, KF_RECORD_SWITCH = 0x0d,
      KF_RECORD = 0x0e, KF_SMS = 0x0f, KF_DIAL = 0x10, KF_GPS_INFORMATION = 0x11, KF_MONITOR = 0x12,
      KF_MAIN_CHANNEL_SWITCH = 0x13, KF_HOT_KEY_1 = 0x14, KF_HOT_KEY_2 = 0x15, KF_HOT_KEY_3 = 0x16,
      KF_HOT_KEY_4 = 0x17, KF_HOT_KEY_5 = 0x18, KF_HOT_KEY_6 = 0x19, KF_WORK_ALONE = 0x1a,
      KF_NUISANCE_DELETE = 0x1b, KF_DIGITAL_MONITOR = 0x1c, KF_SUB_CH_SWITCH = 0x1d,
      KF_PRIORITY_ZONE = 0x1e, KF_VFO_SCAN = 0x1f, KF_MIC_SOUND_QUALITY = 0x20,
      KF_LAST_CALL_REPLY = 0x21, KF_CHANNEL_TYPE_SWITCH = 0x22, KF_RANGING = 0x23,
      KF_ROAMING = 0x24, KF_CHANEL_RANGING = 0x25, KF_MAX_VOLUME = 0x26, KF_SLOT_SWITCH = 0x27,
      KF_APRS_TYPE_SWITCH = 0x28, KF_ZONE_SELECT = 0x29, KF_TIMED_ROAMING_SET = 0x2a,
      KF_APRS_SET = 0x2b, KF_MUTE_TIMEING = 0x2c, KF_CTCSS_DCS_SET = 0x2d, KF_TBST_SEND = 0x2e,
      KF_BLUETOOTH = 0x2f, KF_GPS = 0x30, KF_CHANNEL_NAME = 0x31, KF_CDT_SCAN = 0x32
    };

    enum STEType : uint8_t {
      STE_TYPE_OFF     = 0,
      STE_TYPE_SILENT  = 1,
      STE_TYPE_DEG_120 = 2,
      STE_TYPE_DEG_180 = 3,
      STE_TYPE_DEG_240 = 4
    };

    enum STEFrequency : uint8_t {
      STE_FREQ_OFF = 0,
      STE_FREQ_STE_55_2Hz = 1,
      STE_FREQ_STE_259_2Hz = 2
    };

    enum DTMFDuration : uint8_t {
      DTMF_DUR_50ms = 0,
      DTMF_DUR_100ms = 1,
      DTMF_DUR_200ms = 2,
      DTMF_DUR_300ms = 3,
      DTMF_DUR_500ms = 4
    };

    enum BackLightDur : uint8_t {
      BACKLIGHT_ALWAYS = 0,
      BACKLIGHT_5s = 1,
      BACKLIGHT_10s = 2,
      BACKLIGHT_15s = 3,
      BACKLIGHT_20s = 4,
      BACKLIGHT_25s = 5,
      BACKLIGHT_30s = 6,
      BACKLIGHT_1min = 7,
      BACKLIGHT_2min = 8,
      BACKLIGHT_3min = 9,
      BACKLIGHT_4min = 10,
      BACKLIGHT_5min = 11,
      BACKLIGHT_15min = 12,
      BACKLIGHT_30min = 13,
      BACKLIGHT_45min = 14,
      BACKLIGHT_60min = 15,
    };

    enum TBSTFrequency : uint8_t {
      TBST_1000Hz = 0,
      TBST_1450Hz = 1,
      TBST_1750Hz = 2,
      TBST_2100Hz = 3
    };

    enum PermitTone : uint8_t {
      PERMIT_OFF = 0,
      PERMIT_DIGITAL = 1,
      PERMIT_ANALOG = 2,
      PERMIT_BOTH = 3
    };

    enum VOXSource : uint8_t {
      VOXSRC_BUILT_IN = 0,
      VOXSRC_EXTERNAL = 1,
      VOXSRC_BOTH     = 2
    };

    enum Color : uint8_t {
      COL_ORANGE    = 0,
      COL_RED       = 1,
      COL_YELLOW    = 2,
      COL_GREEN     = 3,
      COL_TURQUOISE = 4,
      COL_BLUE      = 5,
      COL_WHITE     = 6
    };

    enum SMSFormat : uint8_t {
      SMS_FMT_M = 0,
      SMS_FMT_H = 1,
      SMS_FMT_DMR = 2,
    };

    // Bytes 0x00-0x0f.
    uint8_t keytone;               ///< Enable key tones, Off=0x00, On=0x01.
    DisplayMode display_mode;      ///< Display channel or frequency, see @c DisplayMode, default channel=0x00.
    uint8_t auto_keylock;          ///< Enables auto key-lock.
    AutoShutdown auto_shutdown;    ///< Automatic shutdown timer, see @c AutoShutdown, default off=0x00.
    uint8_t transmit_timeout;      ///< Transmit timeout (TOT) in multiples of 30s, default 0=off.
    Language language;             ///< UI Language, see @c Language, default english.
    PowerOnDisplay power_on;       ///< Power-on display, see @c PowerOnDisplay, default=PWRON_DEFAULT.
    uint8_t pwron_passwd;          ///< Boot password enabled, Off=0x00, On=0x01.
    FreqStep freq_step;            ///< VFO Frequency step, see FreqStep.
    uint8_t sql_level_a;           ///< Squelch level VFO A [0,5], 0=off.
    uint8_t sql_level_b;           ///< Squelch level VFO B [0,5], 0=off.
    PowerSave power_save;          ///< Power-save settings, @c see PowerSave.
    uint8_t vox_level;             ///< Vox/BT level [1,3], 0=off.
    uint8_t vox_delay;             ///< Delay in 0.1s steps + 0.5s offset, value [0, 25].
    VFOScanType vfo_scan_type;     ///< Sets the VFO scan type.
    uint8_t mic_gain;              ///< Microphone gain value in [0,4], default 2.

    // Bytes 0x10-0x1f.
    KeyFunction pf1_short;
    KeyFunction pf2_short;
    KeyFunction pf3_short;
    KeyFunction p1_short;
    KeyFunction p2_short;
    uint8_t vfo_mode_a;            ///< Enables VFO mode for VFO A, default memory mode = 0x00.
    uint8_t vfo_mode_b;            ///< Enables VFO mode for VFO B, default memory mode = 0x00.
    STEType ste_type;              ///< STE type of CTCSS.
    STEFrequency ste_freq_no_sig;  ///< STE when no signal.
    uint8_t grpcall_hang_time;     ///< Group call hang time [1,30] = 1..30s, 0x1f=30min, 0x20=infinite.
    uint8_t privcall_hang_time;    ///< Private call hang time [1,30] = 1..30s, 0x1f=30min, 0x20=infinite.
    uint8_t prewave_time;          ///< Prewave delay in multiples of 20ms, values [0,50].
    uint8_t wake_head_period;      ///< Wake head period in multiples of 20ms, values [0,50].
    uint8_t fm_channel_index;      ///< Index of current FM channel, 0-based.
    uint8_t fm_vfo_mode;           ///< Enabled FM (broadcast) VFO mode, default channel mode=0x00.
    uint8_t current_zone_a;        ///< Zone index 0-based.

    // Bytes 0x20-0x2f.
    uint8_t current_zone_b;        ///< Zone index 0-based.
    uint8_t _unused0021;           ///< Unused, set to 0x00.
    uint8_t record_enable;         ///< Enable recording function, default off=0x00.
    DTMFDuration dtmf_duration;    ///< DTMF transmit duration.
    uint8_t enable_man_down;       ///< Enable man down alarm, default off=0x00.
    uint8_t _unused0025;           ///< Unused, set to 0x00.
    uint8_t display_brightness;    ///< Display brightness value [0,4].
    BackLightDur backlight_dur;    ///< Specifies backlight duration, see @c BackLightDur.
    uint8_t gps_enable;            ///< Enable GPS, Off=0x00, On=0x01.
    uint8_t enable_sms_alert;      ///< Enables SMS alert, default Off=0x00.
    uint8_t _unknown002a;          ///< Unknown, set to 0x01.
    uint8_t enable_fm_monitor;     ///< Enables FM (broadcast) monitor, default off=0x00.
    uint8_t main_ch_set_b;         ///< Set main channel is B, A if 0x00.
    uint8_t enable_sub_ch_mode;    ///< Enable sub-channel mode, default off=0x00.
    TBSTFrequency tbst_frequency;  ///< Sets the TBST frquency.
    uint8_t call_alert;            ///< Enable call tone, default=1, Off=0x00, On=0x01.

    // Byte 0x30-3f
    uint8_t timezone;              ///< Time zone, GMT-12=0x00, GMT=0x0c, GMT+1=0x0d, GMT+13=0x19.
    PermitTone talk_permit_tone;   ///< Enable talk permit, Off=0x00, Digi=0x01, Analog=0x02, Both=0x03.
    uint8_t enable_idle_tone;      ///< Enable digi-call idle tone, Off=0x00, On=0x01.
    VOXSource vox_source;          ///< Specifies the VOX source, default external = 0x01.
    uint8_t enable_pro_mode;       ///< Select amateur mode (0x00) or professional mode (0x01).
    uint8_t _unused0035;           ///< Unused set to 0x00
    uint8_t ch_idle_tone;          ///< Enable channel idle tone, Off=0x00, On=0x01.
    uint8_t menu_exit_time;        ///< Menu exit time in multiples of 5s + 5s offset.
    uint8_t filter_own_missed;     ///< Enable filter own ID in missed calls, default off=0x00.
    uint8_t startup_tone;          ///< Enable Startup tone, Off=0x00, On=0x01.
    uint8_t call_end_box;          ///< Show call end prompt box, default off=0x00.
    uint8_t max_sp_volume;         ///< Maximum speaker volume, value in [1,8], default 5.
    uint8_t remote_stun;           ///< Enable remote stun and kill, default off=0x00.
    uint8_t _unused003d;           ///< Unused, set to 0x00.
    uint8_t remote_monitor;        ///< Enable remote monitor, default off = 0x00.
    uint8_t gps_sms_enable;        ///< Enable GPS SMS reporting, Off=0x00, On=0x01.

    // Bytes 0x40-0x4f
    uint8_t _unknown0040;          ///< Unknown settings, default set to 0x01;
    KeyFunction pf1_long;
    KeyFunction pf2_long;
    KeyFunction pf3_long;
    KeyFunction p1_long;
    KeyFunction p2_long;
    uint8_t long_key_time;         ///< Specifies the long-press time in seconds + 1s offset, value in [0,4].
    uint8_t show_volume_change;    ///< Display volume change promt.
    uint8_t autorep_vfo_a;         ///< Auto repeater offset direction VFO A, Off=0x00, positive=0x01, negative=0x02, default=off.
    uint8_t digi_mon_slot;         ///< Digital monitor slot, default off=0x00, single slot=0x01, both slots=0x02.
    uint8_t digi_mon_cc;           ///< Digital monitor match color code, default any=0x00, match=0x01.
    uint8_t digi_mon_id;           ///< Digital monitor match ID, default any=0x00, match=0x01.
    uint8_t mon_slot_hold;         ///< Digital monitor slot hold, default off=0x00.
    uint8_t show_last_caller;      ///< Show last caller, off=0x00, show ID=0x01, show call sign=0x02, show both=0x03.
    uint8_t _unused003e;           ///< Unused, set to 0x00.
    uint8_t man_down_delay;        ///< Man down alarm delay in seconds [0,255].

    // Bytes 0x50-0x5f
    uint8_t analog_call_hold;      ///< Analog call hold time in seconds [0,30].
    uint8_t enable_time_display;   ///< Show time on display, default on=0x01.
    uint8_t max_hp_volume;         ///< Maximum headphone volume, value in [0,8], default 0.
    uint8_t gps_message_enable;    ///< Enable GPS text message, Off=0x00, On=0x01
    uint8_t _unknown0054[3];       ///< Unknown settings block;
    uint8_t enh_mic_sound;         ///< Enhanced mic sound quality, Off=0x00, On=0x01.
    uint32_t vfo_scan_uhf_start;   ///< Start frequency of UHF VFO scan, in multiples of 10Hz, little-endian.
    uint32_t vfo_scan_uhf_stop;    ///< End frequency of UHF VFO scan, in multiples of 10Hz, little-endian.

    // Bytes 0x60-0x6f
    uint32_t vfo_scan_vhf_start;   ///< Start frequency of VHF VFO scan, in multiples of 10Hz, little-endian.
    uint32_t vfo_scan_vhf_stop;    ///< End frequency of VHF VFO scan, in multiples of 10Hz, little-endian.
    uint8_t autorep_uhf1_off;      ///< Auto repeater offset UHF1 index, 0-based, disabled=0xff.
    uint8_t autorep_vhf1_off;      ///< Auto repeater offset VHF1 index, 0-based, disabled=0xff.
    uint8_t _unknown0069[4];       ///< Unknown settings block;
    uint8_t maintain_call_ch;      ///< Call channel is maintained, default on=0x01.
    uint8_t pri_zone_a;            ///< Priority zone index VFO A, index of zone 0-based.

    // Bytes 0x70-0xaf
    uint8_t pri_zone_b;            ///< Priority zone index VFO B, index of zone 0-based.
    uint8_t _unused0071;           ///< Unused, set to 0x00.
    uint16_t call_tone_freq[5];    ///< Sequence of frequencies for call tone in Hz, little-endian.
    uint16_t call_tone_dur[5];     ///< Sequence of druations for call tone in Hz, little-endian.
    uint16_t idle_tone_freq[5];    ///< Sequence of frequencies for channel idle tone in Hz, little-endian.
    uint16_t idle_tone_dur[5];     ///< Sequence of druations for channel idle tone in Hz, little-endian.
    uint16_t callend_tone_freq[5]; ///< Sequence of frequencies for call end tone in Hz, little-endian.
    uint16_t callend_tone_dur[5];  ///< Sequence of druations for call end tone in Hz, little-endian.
    uint8_t  record_delay;         ///< Record delay in multiple of 0.2 seconds.
    uint8_t  call_disp_mode;       ///< Call display mode, off=0x00, 0x01=call sign, 0x02=name.

    // Bytes 0xb0-0xbf
    uint8_t _unknown00b0[5];       ///< Unknown settings block.
    uint8_t gps_sms_interval;      ///< GPS ranging SMS interval in seconds [5,255], default=5.
    uint8_t _unknown00b6[2];       ///< Unknown settings block.
    uint8_t disp_channel_number;   ///< Display channel number, default on=0x01.
    uint8_t disp_contact;          ///< Display current contact, default on=0x01.
    uint8_t roam_period;           ///< Auto roaming period in minutes -1, 1min=0x00, 2m=0x01, 256min=0xff, default=1min.
    uint8_t key_tone_adj;          ///< Key tone adjust, fixed 0x01-0x0f, or 0x00=variable.
    Color callsign_color;          ///< Call sign display color.
    uint8_t gps_unit;              ///< GPS units Metric=0x00, Imperial=0x01.
    uint8_t key_lock_knob : 1,     ///< Lock knowb.
      key_lock_keyboard   : 1,     ///< Lock keyboard.
      _unused00be_2       : 1,     ///< Unused, set to 0.
      key_lock_sidekey    : 1,     ///< Lock side keys.
      key_lock_forced     : 1,     ///< Forced lock.
      _unknown00be_5      : 3;     ///< Unused, set to 0.
    uint8_t roam_wait;             ///< Auto roaming wait time in seconds off=0x00, 1s=0x01, 30s=0x1e, default=off.

    // Bytes 0xc0-0xcf
    Color standby_text_color;      ///< Standby text color.
    Color standby_img_color;       ///< Standby background image color.
    uint8_t show_last_call_launch; ///< Show last call on launch.
    SMSFormat sms_format;          ///< SMS format.
    uint32_t autorep_vhf1_min;     ///< Auto repeater VHF1 minimum frequency, in 10Hz, little endian.
    uint32_t autorep_vhf1_max;     ///< Auto repeater VHF1 maximum frequency, in 10Hz, little endian.
    uint32_t autorep_uhf1_min;     ///< Auto repeater UHF1 minimum frequency, in 10Hz, little endian.

    // Bytes 0xd0-0xdf
    uint32_t autorep_uhf1_max;     ///< Auto repeater UHF1 maximum frequency, in 10Hz, little endian.
    uint8_t autorep_vfo_b;         ///< Auto repeater offset direction VFO B, Off=0x00, positive=0x01, negative=0x02, default=off.
    uint8_t _unknown00d5;          ///< Unknown setting.
    uint8_t _unknown00d6;          ///< Unused set to 0x00.
    uint8_t boot_ch;               ///< Enable default channel on boot, Off=0x00, On=0x01.
    uint8_t vfo_a_zone_index;      ///< Index of default zone for VFO A, 0-based, default=0.
    uint8_t vfo_b_zone_index;      ///< Index of default zone for VFO B, 0-based, default=0.
    uint8_t vfo_a_ch_index;        ///< Default channel index (within selected zone) for VFO A, 0-based, default=0, 0xff=VFO.
    uint8_t vfo_b_ch_index;        ///< Default channel index (within selected zone) for VFO B, 0-based, default=0, 0xff=VFO.
    uint8_t roam_default_zone;     ///< Roaming default zone index, 0-based.
    uint8_t repchk_enable;         ///< Repeater range check enable, Off=0x00, On=0x01.
    uint8_t repchk_interval;       ///< Repeater range check interval in multiple of 5 seconds, 30s=0x05, 35s=0x06, default=30s.
    uint8_t repchk_recon;          ///< Number of reconnections 3=0x00, 4=0x01, 5=0x02, default=5.

    // Bytes 0x0e0-0x0ef
    uint8_t roam_start_cond;       ///< Auto roaming start condition, Periodic=0x00, Out-of-range=0x01.
    uint8_t backlight_delay_tx;    ///< Backlight delay during TX in seconds [0,30].
    uint8_t separate_display;      ///< Separate display default off=0x00.
    uint8_t keep_last_caller;      ///< Keep last caller on channel switch, default off=0x00.
    Color channel_name_color;      ///< Color of channel name.
    uint8_t repchk_notify;         ///< Repeater range check notification Off=0x00, Beep=0x01, Voice=0x02, default=Voice.
    uint8_t backlight_delay_rx;    ///< Backlight delay during RX in seconds always=0x00.
    uint8_t roam_enable;           ///< Enable roaming, Off=0x00, On=0x01.
    uint8_t _unused00e8;           ///< Unused, set to 0x00.
    uint8_t mute_delay;            ///< Mute delay in minutes -1 [0,255].
    uint8_t repchk_num_notify;     ///< Number of repeater out-of-range notifications, 1=0x00, 2=0x01, 3=0x02, 10=0x0b, default=3.
    uint8_t startup_gps_test;      ///< Test GPS on boot, default off=0x00.
    uint8_t startup_reset;         ///< Startup reset.
    uint8_t _unknown00ed[3];       ///< Unused set to 0x00.

    // Bytes 0x0f0-0x0ff
    uint8_t  _unknown00f0[16];     ///< Unknown settings block.

    /** Constructs an empty general settings. */
    general_settings_base_t();
    /** Clears the general setting. */
    void clear();

    /** Decodes the microphone gain. */
    uint getMicGain() const;
    /** Encodes the microphone gain. */
    void setMicGain(uint gain);

    /** Updates the general settings from the given abstract configuration. */
    void fromConfig(const Config *config, const Flags &flags);
    /** Updates the abstract configuration from this general settings. */
    void updateConfig(Config *config);
  };

  /** Represents the APRS settings within the binary codeplug.
   *
   * Memmory layout of APRS settings (0x40byte):
   * @verbinclude d878uvaprssetting.txt
   */
  struct __attribute__((packed)) aprs_setting_t {
    /** Possible signalling for APRS repeater.*/
    typedef enum {
      SIG_OFF = 0,                 ///< No signalling.
      SIG_CTCSS = 1,               ///< CTCSS signalling.
      SIG_DCS = 2                  ///< DCS signalling.
    } SignalingType;

    /** Power setting for the APRS/GPS channel. */
    typedef enum {
      POWER_LOW = 0,               ///< Low power (usually about 1W).
      POWER_MID = 1,               ///< Medium power (usually about 2W).
      POWER_HIGH = 2,              ///< High power (usually about 5W).
      POWER_TURBO = 3              ///< Highest power (upto 7W).
    } Power;

    /** Hemisphere settings for the fixed location beacon. */
    typedef enum {
      NORTH = 0,
      SOUTH = 1,
      EAST  = 0,
      WEST  = 1
    } Hemisphere;

    // byte 0x00
    uint8_t _unknown0;             ///< Unknown, set to 0xff.
    uint32_t frequency;            ///< TX frequency, BCD encoded, little endian in 10Hz.
    uint8_t tx_delay;              ///< TX delay, multiples of 20ms, default=1200ms.
    uint8_t sig_type;              ///< Signalling type, 0=off, 1=ctcss, 2=dcs, default=off.
    uint8_t ctcss;                 ///< CTCSS tone-code, default=0.
    uint16_t dcs;                  ///< DCS code, little endian, default=0x0013.
    uint8_t manual_tx_interval;    ///< Global manual TX intervals in seconds.
    uint8_t auto_tx_interval;      ///< Global auto TX interval in multiples of 30s.
    uint8_t tx_tone_enable;        ///< TX tone enable, 0=off, 1=on.

    uint8_t fixed_location;        ///< Fixed location data, 0=off, 1=on.
    uint8_t lat_deg;               ///< Latitude in degree.
    uint8_t lat_min;               ///< Latitude minutes.
    uint8_t lat_sec;               ///< Latitude seconds (1/100th of a minute).
    uint8_t north_south;           ///< North or south flag, north=0, south=1.
    uint8_t lon_deg;               ///< Longitude in degree.
    uint8_t lon_min;               ///< Longitude in minutes.
    uint8_t lon_sec;               ///< Longitude in seconds (1/100th of a minute).
    uint8_t east_west;             ///< East or west flag, east=0, west=1.

    uint8_t to_call[6];            ///< Destination call, 6 x ASCII, 0x20-padded.
    uint8_t to_ssid;               ///< Destination SSID, 0xff=None.

    uint8_t from_call[6];          ///< Source call, 6 x ASCII, 0x20-padded.
    uint8_t from_ssid;             ///< Source SSID, 0xff=None.

    // byte 0x24
    uint8_t path[20];              ///< Path string, upto 20 ASCII chars, 0-padded.
    uint8_t _pad56;                ///< Pad-byte 0x00.

    // byte 0x39
    char table;                    ///< ASCII-char for APRS icon table, ie. '/' or '\' for primary
                                   ///  and alternate icon table respectively.
    char icon;                     ///< ASCII-char of APRS map icon.
    uint8_t power;                 ///< Transmit power.
    uint8_t prewave_delay;         ///< Prewave delay in 10ms steps.

    // bytes 0x3d
    uint8_t _unknown61;            ///< Unknown, set to 01.
    uint8_t _unknown62;            ///< Unknown, set to 03.
    uint8_t _unknown63;            ///< Unknown, set to ff.

    /** Returns @c true, if the APRS setting is vaild. That is, it has a valid frequency,
     * destination and source calls. */
    bool isValid() const;

    /** Decodes the transmit frequency. */
    double getFrequency() const;
    /** Encodes the given frequency. */
    void setFrequency(double freq);

    /** Decodes the auto TX period. */
    int getAutoTXInterval() const;
    /** Encodes the auto TX period. */
    void setAutoTxInterval(int sec);

    /** Decodes the manual TX interval in seconds. */
    int getManualTXInterval() const;
    /** Encodes the manual TX interval in seconds. */
    void setManualTxInterval(int sec);

    /** Decodes the destination call. */
    QString getDestination() const;
    /** Encodes the given destination call. */
    void setDestination(const QString &call, uint8_t ssid);

    /** Decodes the destination SSID. */
    QString getSource() const;
    /** Encodes the destination SSID. */
    void setSource(const QString &call, uint8_t ssid);

    /** Decodes the APRS path. */
    QString getPath() const;
    /** Encodes the given APRS path. */
    void setPath(const QString &path);

    /** Decodes the TX signaling. */
    Signaling::Code getSignaling() const;
    /** Encodes the TX signaling. */
    void setSignaling(Signaling::Code signaling);

    /** Decodes the transmit power. */
    Channel::Power getPower() const;
    /** Encodes the given transmit power. */
    void setPower(Channel::Power pwr);

    /** Decodes the APRS map icon. */
    APRSSystem::Icon getIcon() const;
    /** Encodes the specified map icon. */
    void setIcon(APRSSystem::Icon icon);

    /** Configures this APRS system from the given generic config. */
    void fromAPRSSystem(APRSSystem *sys);
    /** Constructs a generic APRS system configuration from this APRS system. */
    APRSSystem *toAPRSSystem();
    /** Links the transmit channel within the generic APRS system based on the transmit frequency
     * defined within this APRS system. */
    void linkAPRSSystem(APRSSystem *sys, CodeplugContext &ctx);
  };

  /** Implements the binary representation of a roaming channel within the codeplug.
   *
   * Memmory layout of roaming channel (0x20byte):
   * @verbinclude d878uvroamingchannel.txt */
  struct __attribute__((packed)) roaming_channel_t {
    uint32_t rx_frequency;         ///< RX frequency 8-digit BCD big-endian as MMMkkkHH.
    uint32_t tx_frequency;         ///< TX frequency 8-digit BCD big-endian as MMMkkkHH.
    uint8_t colorcode;             ///< Colorcode 1-16.
    uint8_t timeslot;              ///< Timeslot, 0=TS1, 1=TS2.
    uint8_t name[16];              ///< Channel name, 16byte ASCII 0-terminated.
    uint8_t _unused26[6];          ///< Unused, set to 0x00

    /** Decodes the RX frequency. */
    double getRXFrequency() const;
    /** Encodes the given RX frequency. */
    void setRXFrequency(double f);
    /** Decodes the TX frequency. */
    double getTXFrequency() const;
    /** Encodes the given TX frequency. */
    void setTXFrequency(double f);
    /** Returns the time-slot of the roaming channel. */
    DigitalChannel::TimeSlot getTimeslot() const;
    /** Sets the time-slot of the roaming channel. */
    void setTimeslot(DigitalChannel::TimeSlot ts);
    /** Returns the color-code [0-15] of the roaming channel. */
    uint getColorCode() const;
    /** Sets the color-code [0-15] of the roaming channel. */
    void setColorCode(uint8_t cc);
    /** Decodes the name of the roaming channel. */
    QString getName() const;
    /** Encodes the name of the roaming channel. */
    void setName(const QString &name);

    /** Constructs a roaming channel from the given digital channel. */
    void fromChannel(DigitalChannel *ch);
    /** Constructs/Searches a matching DigitalChannel for this roaming channel. */
    DigitalChannel *toChannel(CodeplugContext &ctx);
  };

  /** Represents a roaming zone within the binary codeplug.
   *
   * Memmory layout of roaming zone (0x80byte):
   * @verbinclude d878uvroamingzone.txt */
  struct __attribute__((packed)) roaming_zone_t {
    uint8_t channels[64];          ///< List of roaming channel indices 0-based, 0xff=unused/end-of-list.
    uint8_t name[16];              ///< Roaming zone name, 16b ASCII 0x00 padded.
    uint8_t _unused80[48];         ///< Unused, set to 0x00.

    /** Returns the name of the roaming zone. */
    QString getName() const;
    /** Sets the name of the roaming zone. */
    void setName(const QString &name);

    /** Assembles a binary representation of the given RoamingZone instance.*/
    void fromRoamingZone(RoamingZone *zone, const QHash<DigitalChannel *, int> &map);
    /** Constructs a @c RoamingZone instance from this configuration. */
    RoamingZone *toRoamingZone();
    /** Links the given RoamingZone. */
    bool linkRoamingZone(RoamingZone *zone, CodeplugContext &ctx);
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
  void allocateUntouched();
  /** Allocate all code-plug elements that are defined through the common Config. */
  void allocateForEncoding();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config, CodeplugContext &ctx);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

protected:
  void allocateGeneralSettings();
  bool encodeGeneralSettings(Config *config, const Flags &flags);
  bool decodeGeneralSettings(Config *config);
};

#endif // D878UVCODEPLUG_HH
