#ifndef D878UV_CODEPLUG_HH
#define D878UV_CODEPLUG_HH

#include <QDateTime>

#include "codeplug.hh"
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
class D878UVCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Represents the actual channel encoded within the binary code-plug.
   *
   * Memmory layout of encoded channel (64byte):
   * @verbinclude d878uvchannel.txt
   */
  struct __attribute__((packed)) channel_t {
    /** Defines all possible channel modes, see @c channel_mode. */
    typedef enum {
      MODE_ANALOG    = 0,               ///< Analog channel.
      MODE_DIGITAL   = 1,               ///< Digital (DMR) channel.
      MODE_MIXED_A_D = 2,               ///< Mixed, transmit analog and receive digital.
      MODE_MIXED_D_A = 3                ///< Mixed, transmit digital and receive analog.
    } Mode;

    /** Defines all possible power settings.*/
    typedef enum {
      POWER_LOW = 0,                    ///< Low power, usually 1W.
      POWER_MIDDLE = 1,                 ///< Medium power, usually 2.5W.
      POWER_HIGH = 2,                   ///< High power, usually 5W.
      POWER_TURBO = 3                   ///< Higher power, usually 7W on VHF and 6W on UHF.
    } Power;

    /** Defines all band-width settings for analog channel.*/
    typedef enum {
      BW_12_5_KHZ = 0,                  ///< Narrow band-width (12.5kHz).
      BW_25_KHZ = 1                     ///< High band-width (25kHz).
    } Bandwidth;

    /** Defines all possible repeater modes. */
    typedef enum {
      RM_SIMPLEX = 0,                   ///< Simplex mode, that is TX frequency = RX frequency. @c tx_offset is ignored.
      RM_TXPOS = 1,                     ///< Repeater mode with positive @c tx_offset.
      RM_TXNEG = 2                      ///< Repeater mode with negative @c tx_offset.
    } RepeaterMode;

    /** Defines all possible PTT-ID settings. */
    typedef enum {
      PTTID_OFF = 0,                    ///< Never send PTT-ID.
      PTTID_START = 1,                  ///< Send PTT-ID at start.
      PTTID_END = 2,                    ///< Send PTT-ID at end.
      PTTID_START_END = 3               ///< Send PTT-ID at start and end.
    } PTTId;

    /** Defines all possible squelch settings. */
    typedef enum {
      SQ_CARRIER = 0,                   ///< Open squelch on carrier.
      SQ_TONE = 1                       ///< Open squelch on matching CTCSS tone or DCS code.
    } SquelchMode;

    /** Defines all possible admit criteria. */
    typedef enum {
      ADMIT_ALWAYS = 0,                 ///< Admit TX always.
      ADMIT_CH_FREE = 1,                ///< Admit TX on channel free.
      ADMIT_CC_DIFF = 2,                ///< Admit TX on mismatching color-code.
      ADMIT_CC_SAME = 3                 ///< Admit TX on matching color-code.
    } Admit;

    /** Defines all possible optional signalling settings. */
    typedef enum {
      OPTSIG_OFF = 0,                   ///< None.
      OPTSIG_DTMF = 1,                  ///< Use DTMF.
      OPTSIG_2TONE = 2,                 ///< Use 2-tone.
      OPTSIG_5TONE = 3                  ///< Use 5-tone.
    } OptSignaling;

    /** Defines all possible APRS reporting modes. */
    typedef enum {
      APRS_REPORT_OFF = 0,              ///< No APRS (GPS) reporting at all.
      APRS_REPORT_ANALOG = 1,           ///< Use analog, actual APRS reporting.
      APRS_REPORT_DIGITAL = 2           ///< Use digital reporting.
    } APRSReport;

    /** Defines all possible APRS PTT settings. */
    typedef enum {
      APRS_PTT_OFF   = 0,               ///< Do not send APRS on PTT.
      APRS_PTT_START = 1,               ///< Send APRS at start of transmission.
      APRS_PTT_END   = 2                ///< Send APRS at end of transmission.
    } APRSPTT;


    // Bytes 0-7
    uint32_t rx_frequency;              ///< RX Frequency, 8 digits BCD, big-endian.
    uint32_t tx_offset;                 ///< TX Offset, 8 digits BCD, big-endian, sign in repeater_mode.

    // Byte 8
    uint8_t channel_mode    : 2,        ///< Mode: Analog or Digital, see @c Mode.
      power                 : 2,        ///< Power: Low, Middle, High, Turbo, see @c Power.
      bandwidth             : 1,        ///< Bandwidth: 12.5 or 25 kHz, see @c Bandwidth.
      _unused8              : 1,        ///< Unused, set to 0.
      repeater_mode         : 2;        ///< Sign of TX frequency offset, see @c RepeaterMode.

    // Byte 9
    uint8_t rx_ctcss        : 1,        ///< CTCSS decode enable.
      rx_dcs                : 1,        ///< DCS decode enable.
      tx_ctcss              : 1,        ///< CTCSS encode enable.
      tx_dcs                : 1,        ///< DCS encode enable
      reverse               : 1,        ///< CTCSS phase-reversal.
      rx_only               : 1,        ///< TX prohibit.
      call_confirm          : 1,        ///< Call confirmation enable.
      talkaround            : 1;        ///< Talk-around enable.

    // Bytes 10-15
    uint8_t ctcss_transmit;             ///< TX CTCSS tone, 0=62.5, 50=254.1, 51=custom CTCSS tone.
    uint8_t ctcss_receive;              ///< RX CTCSS tone: 0=62.5, 50=254.1, 51=custom CTCSS tone.
    uint16_t dcs_transmit;              ///< TX DCS code: 0=D000N, 511=D777N, 512=D000I, 1023=D777I, DCS code-number in octal, little-endian.
    uint16_t dcs_receive;               ///< RX DCS code: 0=D000N, 511=D777N, 512=D000I, 1023=D777I, DCS code-number in octal, little-endian.

    // Bytes 16-19
    uint16_t custom_ctcss;              ///< Custom CTCSS tone frequency: 0x09cf=251.1, 0x0a28=260, big-endian.
    uint8_t tone2_decode;               ///< 2-Tone decode: 0x00=1, 0x0f=16
    uint8_t _unused19;                  ///< Unused, set to 0.

    // Bytes 20-23
    uint32_t contact_index;             ///< Contact index, zero-based, little-endian.

    // Byte 24
    uint8_t id_index;                   ///< Index to radio ID table.

    // Byte 25
    uint8_t ptt_id          : 2,        ///< PTT ID, see PTTId.
      _unused25_1           : 2,        ///< Unused, set to 0.
      squelch_mode          : 1,        ///< Squelch mode, see @c SquelchMode.
      _unused25_2           : 3;        ///< Unused, set to 0.

    // Byte 26
    uint8_t tx_permit       : 2,        ///< TX permit, see @c Admit.
      _unused26_1           : 2,        ///< Unused, set to 0.
      opt_signal            : 2,        ///< Optional signaling, see @c OptSignaling.
      _unused26_2           : 2;        ///< Unused, set to 0.

    // Bytes 27-31
    uint8_t scan_list_index;            ///< Scan list index, 0xff=None, 0-based.
    uint8_t group_list_index;           ///< RX group-list, 0xff=None, 0-based.
    uint8_t id_2tone;                   ///< 2-Tone ID, 0=1, 0x17=24.
    uint8_t id_5tone;                   ///< 5-Tone ID, 0=1, 0x63=100.
    uint8_t id_dtmf;                    ///< DTMF ID, 0=1, 0x0f=16.

    // Byte 32
    uint8_t color_code;                 ///< Color code, 0-15

    // Byte 33
    uint8_t slot2           : 1,        ///< Timeslot, 0=TS1, 1=TS2.
      sms_confirm           : 1,        ///< Send SMS confirmation, 0=off, 1=on.
      simplex_tdma          : 1,        ///< Simplex TDMA enabled.
      _unused33_2           : 1,        ///< Unused, set to 0.
      tdma_adaptive         : 1,        ///< TDMA adaptive enable.
      rx_gps                : 1,        ///< Receive digital GPS messages.
      enh_encryption        : 1,        ///< Enable enhanced encryption.
      work_alone            : 1;        ///< Work alone, 0=off, 1=on.

    // Byte 34
    uint8_t aes_encryption;             ///< Digital AES encryption, 1-32, 0=off.

    // Bytes 35-51
    uint8_t name[16];                   ///< Channel name, ASCII, zero filled.
    uint8_t _pad51;                     ///< Pad byte, set to 0.

    // Byte 52
    uint8_t ranging         : 1,        ///< Ranging enabled.
      through_mode          : 1,        ///< Through-mode enabled.
      excl_from_roaming     : 1,        ///< Exclude channel from roaming.
      _unused52             : 5;        ///< Unused, set to 0.

    // Byte 53
    uint8_t aprs_report     : 2,        ///< Enable APRS report, see @c APRSReport.
      _unused53             : 6;        ///< Unused, set to 0.

    // Bytes 54-63
    uint8_t analog_aprs_ptt;            ///< Enable analog APRS PTT, see @c APRSPTT.
    uint8_t digi_aprs_ptt;              ///< Enable digital APRS PTT, 0=off, 1=on.
    uint8_t gps_system;                 ///< Index of DMR GPS report system, 0-7;
    int8_t  freq_correction;            ///< Signed int in 10Hz.
    uint8_t dmr_encryption;             ///< Digital encryption, 1-32, 0=off.
    uint8_t multiple_keys   : 1,        ///< Enable multiple keys.
      random_key            : 1,        ///< Enable random key.
      sms_forbid            : 1,        ///< Forbit SMS tramsission.
      _unused59_3           : 5;        ///< Unused, set to 0.
    uint32_t _unused60;                 ///< Unused, set to 0.

    /** Constructor, also clears the struct. */
    channel_t();

    /** Clears and invalidates the channel. */
    void clear();

    /** Returns @c true if the channel is valid. */
    bool isValid() const;

    /** Returns the RX frequency in MHz. */
    double getRXFrequency() const;
    /** Sets the RX frequency in MHz. */
    void setRXFrequency(double f);

    /** Returns the TX frequency in MHz. */
    double getTXFrequency() const;
    /** Sets the TX frequency in MHz.
     * @note As the TX frequency is stored as difference to the RX frequency, the RX frequency
     * should be set first. */
    void setTXFrequency(double f);

    /** Returns the name of the radio. */
    QString getName() const;
    /** Sets the name of the radio. */
    void setName(const QString &name);

    /** Returns the RX CTCSS/DCS tone. */
    Signaling::Code getRXTone() const;
    /** Sets the RX CTCSS/DCS tone. */
    void setRXTone(Signaling::Code code);
    /** Returns the TX CTCSS/DCS tone. */
    Signaling::Code getTXTone() const;
    /** Sets the TX CTCSS/DCS tone. */
    void setTXTone(Signaling::Code code);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    Channel *toChannelObj() const;
    /** Links a previously constructed channel to the rest of the configuration. */
    bool linkChannelObj(Channel *c, const CodeplugContext &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    void fromChannelObj(const Channel *c, const Config *conf);
  };

  /** Represents a digital contact within the binary codeplug.
   *
   * Memmory layout of encoded contact (100byte):
   * @verbinclude d878uvcontact.txt
   */
  struct __attribute__((packed)) contact_t {
    /** Possible call types. */
    enum CallType : uint8_t {
      CALL_PRIVATE = 0,                 ///< Private call.
      CALL_GROUP = 1,                   ///< Group call.
      CALL_ALL = 2                      ///< All call.
    };

    /** Possible ring-tone types. */
    enum AlertType : uint8_t {
      ALERT_NONE = 0,                   ///< Alert disabled.
      ALERT_RING = 1,                   ///< Ring tone.
      ALERT_ONLINE = 2                  ///< WTF?
    };

    // Byte 0
    CallType type;                      ///< Call Type: Group Call, Private Call or All Call.
    // Bytes 1-16
    uint8_t name[16];                   ///< Contact Name max 16 ASCII chars 0-terminated.
    // Bytes 17-34
    uint8_t _unused17[18];              ///< Unused, set to 0.
    // Bytes 35-38
    uint32_t id;                        ///< Call ID, BCD coded 8 digits, big-endian.
    // Byte 39
    AlertType call_alert;               ///< Call Alert. One of None, Ring, Online Alert.
    // Bytes 40-99
    uint8_t _unused40[60];              ///< Unused, set to 0.

    /** Constructs a new and empty contact. */
    contact_t();

    /** Clears the contact. */
    void clear();

    /** Returns @c true if the contact is valid. */
    bool isValid() const;

    /** Retruns the call type. */
    DigitalContact::Type getType() const;
    /** Sets the call type. */
    void setType(DigitalContact::Type type);

    /** Returns the name of the contact. */
    QString getName() const;
    /** Sets the name of the contact. */
    void setName(const QString &name);

    /** Returns the number of the contact. */
    uint32_t getId() const;
    /** Set the number of the contact. */
    void setId(uint32_t id);

    /** Retunrs @c true if a ring-tone is enabled for this contact. */
    bool getAlert() const;
    /** Enables/disables a ring-tone for this contact. */
    void setAlert(bool enable);

    /** Assembles a @c DigitalContact from this contact. */
    DigitalContact *toContactObj() const;
    /** Constructs this contact from the give @c DigitalContact. */
    void fromContactObj(const DigitalContact *contact);
  };

  /** Represents an ananlog contact within the binary codeplug.
   *
   * Memmory layout of encoded analog contact (48byte):
   * @verbinclude d878uvanalogcontact.txt
   */
  struct __attribute__((packed)) analog_contact_t {
    uint8_t number[7];                  ///< Number encoded as BCD big-endian. Although it can hold
                                        /// up to 14 digits, more than 8 will crash the manufacturer CPS.
    uint8_t digits;                     ///< Number of digits.
    uint8_t name[15];                   ///< Name, ASCII, upto 15 chars, 0-terminated & padded.
    uint8_t pad47;                      ///< Pad byte set to 0x00.
  };

  /** Represents the actual RX group list encoded within the binary code-plug.
   *
   * Memmory layout of encoded group list (288byte):
   * @verbinclude d878uvgrouplist.txt
   */
  struct __attribute__((packed)) grouplist_t {
    // Bytes 0-255
    uint32_t member[64];                ///< Contact indices, 0-based, little-endian, empty=0xffffffff.
    // Bytes 256-287
    uint8_t name[16];                   ///< Group-list name, up to 16 x ASCII, 0-terminated.
    uint8_t unused[16];                 ///< Unused, set to 0.

    /** Constructs an empty group list. */
    grouplist_t();

    /** Clears this group list. */
    void clear();
    /** Returns @c true if the group list is valid. */
    bool isValid() const;

    /** Returns the name of the group list. */
    QString getName() const;
    /** Sets the name of the group list. */
    void setName(const QString &name);

    /** Constructs a new @c RXGroupList from this group list.
     * None of the members are added yet. Call @c linkGroupList
     * to do that. */
    RXGroupList *toGroupListObj() const;
    /** Populates the @c RXGroupList from this group list. The CodeplugContext
     * is used to map the member indices. */
    bool linkGroupList(RXGroupList *lst, const CodeplugContext &ctx);
    /** Constructs this group list from the given @c RXGroupList. */
    void fromGroupListObj(const RXGroupList *lst, const Config *conf);
  };

  /** Represents a scan list within the binary codeplug.
   *
   * Memmory layout of encoded scanlist (144byte):
   * @verbinclude d878uvscanlist.txt
   */
  struct __attribute__((packed)) scanlist_t {
    /** Defines all possible priority channel selections. */
    typedef enum {
      PRIO_CHAN_OFF = 0,                ///< Off.
      PRIO_CHAN_SEL1 = 1,               ///< Priority Channel Select 1.
      PRIO_CHAN_SEL2 = 2,               ///< Priority Channel Select 2.
      PRIO_CHAN_SEL12 = 3               ///< Priority Channel Select 1 + Priority Channel Select 2.
    } PriChannel;

    /** Defines all possible reply channel selections. */
    enum RevertChannel : uint8_t {
      REVCH_SELECTED = 0,               ///< Selected.
      REVCH_SEL_TB = 1,                 ///< Selected + TalkBack.
      REVCH_PRIO_CH1 = 2,               ///< Priority Channel Select 1.
      REVCH_PRIO_CH2 = 3,               ///< Priority Channel Select 2.
      REVCH_LAST_CALLED = 4,            ///< Last Called.
      REVCH_LAST_USED = 5,              ///< Last Used.
      REVCH_PRIO_CH1_TB = 6,            ///< Priority Channel Select 1 + TalkBack.
      REVCH_PRIO_CH2_TB = 7             ///< Priority Channel Select 2 + TalkBack.
    };

    uint8_t _unused0000;                ///< Unused, set to 0.
    uint8_t prio_ch_select;             ///< Priority Channel Select, default = PRIO_CHAN_OFF.
    uint16_t priority_ch1;              ///< Priority Channel 1: 0=Current Channel, index+1, little endian, 0xffff=Off.
    uint16_t priority_ch2;              ///< Priority Channel 2: 0=Current Channel, index+1, little endian, 0xffff=Off.
    uint16_t look_back_a;               ///< Look Back Time A, sec*10, little endian, default 0x000f.
    uint16_t look_back_b;               ///< Look Back Time B, sec*10, little endian, default 0x0019.
    uint16_t dropout_delay;             ///< Dropout Delay Time, sec*10, little endian, default 0x001d.
    uint16_t dwell;                     ///< Dwell Time, sec*10, little endian, default 0x001d.
    RevertChannel revert_channel;       ///< Revert Channel, see @c RevertChannel, default REVCH_SELECTED.
    uint8_t name[16];                   ///< Scan List Name, ASCII, 0-terminated.
    uint8_t _pad001e;                   ///< Pad byte, set to 0x00.
    // Bytes 0x20
    uint16_t member[50];                ///< Channels indices, 0-based, little endian, 0xffff=empty
    uint8_t _unused0084[12];            ///< Unused, set to 0.

    /** Constructor. */
    scanlist_t();
    /** Clears the scan list. */
    void clear();
    /** Returns the name of the scan list. */
    QString getName() const;
    /** Sets the name of the scan list. */
    void setName(const QString &name);

    /** Constructs a ScanList object from this definition. This only sets the properties of
     * the scan list. To associate all members with the scan list object, call @c linkScanListObj. */
    ScanList *toScanListObj();
    /** Links all channels (members and primary channels) with the given scan-list object. */
    void linkScanListObj(ScanList *lst, CodeplugContext &ctx);
    /** Constructs the binary representation from the give config. */
    bool fromScanListObj(ScanList *lst, Config *config);
  };

  /** Represents an entry of the radio ID table within the binary codeplug.
   *
   * Memmory layout of encoded radio ID (32byte):
   * @verbinclude d878uvradioid.txt
   */
  struct __attribute__((packed)) radioid_t {
    // Bytes 0-3.
    uint32_t id;                        ///< Up to 8 BCD digits in little-endian.
    // Byte 4.
    uint8_t _unused4;                   ///< Unused, set to 0.
    // Bytes 5-20
    uint8_t name[16];                   ///< Name, up-to 16 ASCII chars, 0-terminated.
    // Bytes 21-31
    uint8_t _unused21[11];              ///< Unused, set to 0.

    /** Constructs an empty radio ID entry. */
    radioid_t();
    /** Clears the radio ID enty. */
    void clear();
    /** Returns @c true if the radio ID entry is valid. */
    bool isValid() const;

    /** Returns the name of the radio ID entry. */
    QString getName() const;
    /** Sets the name of the radio ID entry. */
    void setName(const QString name);

    /** Returns the radio ID of the entry. */
    uint32_t getId() const;
    /** Sets the radio ID of the entry. */
    void setId(uint32_t num);
  };

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

  /** At 0x02500100, size 0x400. */
  struct __attribute__((packed)) zone_channels_t {
    // Bytes 0x000-0x1ff
    uint16_t zone_a_channel[250];  ///< Channel index whithin each zone for channel A. Index 0-based litte-endian.
    uint8_t  _pad01f4[12];         ///< pad bytes.
    // Bytes 0x200-0x3ff
    uint16_t zone_b_channel[250];  ///< Channel index whithin each zone for channel B. Index 0-based litte-endian.
    uint8_t  _pad03f4[12];         ///< pad bytes.
  };

  /** At 0x02500500, size 0x100. */
  struct __attribute__((packed)) dtmf_numbers_t {
    // Bytes 0x000-0x0ff
    uint8_t  numbers[16][16];      ///< DTMF numbers, unused set to 0xff
  };

  /** At 0x02500600, size 0x030. */
  struct __attribute__((packed)) boot_settings_t {
    // Bytes 0x600-0x61f
    uint8_t intro_line1[16];       ///< Intro line 1, up to 14 ASCII characters, 0-terminated.
    uint8_t intro_line2[16];       ///< Intro line 2, up to 14 ASCII characters, 0-terminated.

    // Bytes 0x620-0x62f
    uint8_t password[16];          ///< Boot password, up to 8 ASCII digits, 0-terminated.

    /** Clears the general setting. */
    void clear();

    /** Returns the first intro-line. */
    QString getIntroLine1() const;
    /** Sets the first intro-line. */
    void setIntroLine1(const QString line);

    /** Returns the second intro-line. */
    QString getIntroLine2() const;
    /** Sets the second intro-line. */
    void setIntroLine2(const QString line);

    /** Updates the general settings from the given abstract configuration. */
    void fromConfig(const Config *config, const Flags &flags);
    /** Updates the abstract configuration from this general settings. */
    void updateConfig(Config *config);
  };

  /** General settings extension 1.
   *
   * At 0x02501280, size 0x30 bytes. */
  struct __attribute__((packed)) general_settings_ext1_t {
    uint8_t gps_message[32];       ///< GPS message text, upto 32b ASCII text, 0x00 padded.
    uint8_t _unkown0020[16];       ///< Unknown settings block.

    /** Derives the general settings from the given abstact configuration. */
    void fromConfig(const Config *conf, const Flags &flags);
  };

  /** General settings extension 2.
   *
   * At 0x02501400, size 0x100 bytes. */
  struct __attribute__((packed)) general_settings_ext2_t {
    enum TalkerAliasDisplay : uint8_t {
      TA_DISPLAY_OFF = 0,
      TA_DISPLAY_CONTACTS = 1,
      TA_DISPLAY_AIR = 2
    };

    enum TalkerAliasEncoding : uint8_t {
      TA_ENCODING_ISO8 = 0,
      TA_ENCODING_ISO7 = 1,
      TA_ENCODING_UNICODE = 2,
    };

    // Byte 0x00
    uint8_t send_alias;            ///< Send talker alias, 0=off, 1=on.
    uint8_t _unknown0001[15];      ///< Unused, filled with 0x00.
    // Byte 0x10
    uint8_t _unknown0010[14];      ///< Unknown.
    TalkerAliasDisplay ta_display; ///< Talker alias display priority, see @c TalkerAliasDisplay.
    TalkerAliasEncoding ta_enc;    ///< Talker alias encoding, see @c TalkerAliasEncoding.
    // Byte 0x20
    uint8_t _unknown0020[2];       ///< Unused, set to 0x00.
    uint8_t autorep_uhf2_off;      ///< Auto repeater offset UHF2 index, 0-based, disabled=0xff.
    uint8_t autorep_vhf2_off;      ///< Auto repeater offset VHF2 index, 0-based, disabled=0xff.
    uint32_t autorep_vhf2_min;     ///< Auto repeater VHF2 minimum frequency, in 10Hz, little endian.
    uint32_t autorep_vfh2_max;     ///< Auto repeater VHF2 maximum frequency, in 10Hz, little endian.
    uint32_t autorep_uhf2_min;     ///< Auto repeater UHF2 minimum frequency, in 10Hz, little endian.
    // Byte 0x30
    uint32_t autorep_uhf2_max;     ///< Auto repeater UHF2 maximum frequency, in 10Hz, little endian.
    uint8_t _unknown0034;          ///< Unknown.
    uint8_t gps_mode;              ///< GPS mode, GPS=0x00, BDS=0x01, GPS+BDS=0x02.
    uint8_t _unknown0036[10];      ///< Unknown.
    // Byte 0x40
    uint8_t _unknown0040[16];      ///< Unknown.
    // Byte 0x50
    uint8_t _unknown0050[16];      ///< Unknown.
    // Byte 0x60
    uint8_t _unknown0060[16];      ///< Unknown.
    // Byte 0x70
    uint8_t _unknown0070[16];      ///< Unknown.
    // Byte 0x80
    uint8_t _unknown0080[16];      ///< Unknown.
    // Byte 0x90
    uint8_t _unknown0090[16];      ///< Unknown.
    // Byte 0xa0
    uint8_t _unknown00a0[16];      ///< Unknown.
    // Byte 0xb0
    uint8_t _unknown00b0[16];      ///< Unknown.
    // Byte 0xc0
    uint8_t _unknown00c0[16];      ///< Unknown.
    // Byte 0xd0
    uint8_t _unknown00d0[16];      ///< Unknown.
    // Byte 0xe0
    uint8_t _unknown00e0[16];      ///< Unknown.
    // Byte 0xf0
    uint8_t _unknown00f0[16];      ///< Unknown.

    /** Derives the general settings from the given abstact configuration. */
    void fromConfig(const Config *conf, const Flags &flags);
  };


  /** Some weird linked list of valid message indices.
   *
   * Memmory layout of encoded radio ID (16byte):
   * @verbinclude d878uvmessagelist.txt
   */
  struct __attribute__((packed)) message_list_t {
    uint8_t _unknown0[2];          ///< Unused, set to 0x00.
    uint8_t _next_index;           ///< Index of next message, 0xff=EOL.
    uint8_t _current_index;        ///< Index of this message.
    uint8_t _unknown4[12];         ///< Unused, set to 0x00.
  };

  /** Represents a prefabricated SMS message within the binary codeplug.
   *
   * Memmory layout of encoded radio ID (256byte):
   * @verbinclude d878uvmessage.txt
   */
  struct __attribute__((packed)) message_t {
    char text[99];                 ///< Up to 99 ASCII chars, 0-padded.
    uint8_t _unused100[157];       ///< Unused, set to 0.
  };

  /** Represents an encryption key.
   * Size is 64b. */
  struct __attribute__((packed)) encryption_key_t {
    uint8_t index;                 ///< Index/number of excryption key, off=0x00.
    uint8_t key[32];               ///< Binary encryption key.
    uint8_t _unused34;             ///< Unused, set to 0x00;
    uint8_t _unknown35;            ///< Fixed to 0x40.
    uint8_t _unused36[28];         ///< Unused, set to 0x00;
  };

  /** Represents analog quick-call settings within the binary code-plug.
   * Size is 2 bytes. */
  struct __attribute__((packed)) analog_quick_call_t {
    /** Analog quick-call types. */
    typedef enum {
      AQC_None = 0,                ///< None, quick-call disabled.
      AQC_DTMF = 1,                ///< DTMF call.
      AQC_2TONE = 2,               ///< 2-tone call.
      AQC_5TONE = 3                ///< 5-tone call
    } Type;

    uint8_t call_type;             ///< Type of quick call, see @c Type.
    uint8_t call_id_idx;           ///< Index of whom to call. 0xff=none.
  };

  /** Represents hot-key settings. */
  struct __attribute__((packed)) hotkey_t {
    /** Hot-key types. */
    typedef enum {
      HOTKEY_CALL = 0,             ///< Perform a call.
      HOTKEY_MENU = 1              ///< Show a menu item.
    } Type;

    /** Possible menu hot-key settings. */
    typedef enum {
      HOTKEY_MENU_SMS = 1,         ///< Show SMS menu.
      HOTKEY_MENU_NEW_SMS = 2,     ///< Create new SMS.
      HOTKEY_MENU_HOT_TEXT = 3,    ///< Send a hot-text.
      HOTKEY_MENU_RX_SMS = 4,      ///< Show SMS inbox.
      HOTKEY_MENU_TX_SMS = 5,      ///< Show SMS outbox.
      HOTKEY_MENU_CONTACT = 6,     ///< Show contact list.
      HOTKEY_MENU_MANUAL_DIAL = 7, ///< Show manual dial.
      HOTKEY_MENU_CALL_LOG = 8     ///< Show call log.
    } MenuItem;

    /** Possible hot-key calls. */
    typedef enum {
      HOTKEY_CALL_ANALOG = 0,      ///< Perform an analog call.
      HOTKEY_CALL_DIGITAL = 1      ///< Perform a digital call.
    } CallType;

    /** Possible digital call types. */
    typedef enum {
      HOTKEY_DIGI_CALL_OFF = 0xff, ///< Call disabled.
      HOTKEY_GROUP_CALL = 0,       ///< Perform a group call.
      HOTKEY_PRIVATE_CALL = 1,     ///< Perform private call.
      HOTKEY_ALLCALL = 2,          ///< Perform all call.
      HOTKEY_HOT_TEXT = 3,         ///< Send a text message.
      HOTKEY_CALL_TIP = 4,         ///< Send a call tip (?).
      HOTKEY_STATE = 5             ///< Send a state message.
    } DigiCallType;

    uint8_t type;                  ///< Type of the hot-key, see @c Type.
    uint8_t meun_item;             ///< The menu item if type=HOTKEY_MENU, see @c MenuItem.
    uint8_t call_type;             ///< The call type if type=HOTKEY_CALL, see @c CallType.
    uint8_t digi_call_type;        ///< The digital call variant if call_type=HOTKEY_CALL_ANALOG, see @c DigiCallType.
    uint32_t call_obj;             ///< 32bit index of contact to call, little-endian. 0xffffffff=off.
                                   /// If call_type=HOTKEY_CALL_ANALOG, index of analog quick call.
                                   /// If call_type=HOTKEY_CALL_DIGIAL, index of any contact (see @c contact_t).
    uint8_t content;               ///< Content index, 0xff=none.
                                   /// If digi_call_type=HOTKEY_HOT_TEXT, index of message.
                                   /// If digi_call_type=HOTKEY_STATE, index of state message.
    uint8_t _unused9[39];          ///< Unused, set to 0x00.
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

  /** Represents the 8 GPS systems within the binary codeplug.
   *
   * Memmory layout of GPS systems (0x60byte):
   * @verbinclude d878uvgpssetting.txt
   */
  struct __attribute__((packed)) gps_systems_t {
    // byte 0x00
    uint16_t digi_channels[8];     ///< 8 16bit channel indices in little-endian. VFO A=4000,
                                   ///< VFO B=4001, Current=4002.
    // bytes 0x10-0x2f
    uint32_t talkgroups[8];        ///< Talkgroup IDs for all digi APRS channels, BCD encoded, big-endian.
    // bytes 0x30-0x4f
    uint8_t calltypes[8];          ///< Calltype for all digi APRS chanels, 0=private, 1=group, 3=all call.
    uint8_t roaming_support;       ///< Roaming support. 0=off, 1=on.
    uint8_t timeslots[8];          ///< Timeslots for all digi APRS channels. 0=Ch sel, 1=TS1, 2=TS2.
    uint8_t rep_act_delay;         ///< Repeater activation delay in multiples of 100ms.
                                   /// Default 0, range 0-1000ms.
    uint8_t _unknown66[30];        ///< Unknown, set to 0.

    /** Constructor, resets the GPS systems. */
    gps_systems_t();
    /** Reset the GPS systems. */
    void clear();
    /** Returns @c true if the specified GPS system is valid. */
    bool isValid(int idx) const;

    /** Returns the contact ID to send GPS information to for the idx-th system. */
    uint32_t getContactId(int idx) const;
    /** Sets the contact ID for the idx-th GPS system. */
    void setContactId(int idx, uint32_t number);
    /** Returns the call type for the idx-th GPS system. */
    DigitalContact::Type getContactType(int idx) const;
    /** Set the call type for the idx-th GPS system. */
    void setContactType(int idx, DigitalContact::Type type);

    /** Retruns the channel index for the idx-th GPS system. */
    uint16_t getChannelIndex(int idx) const;
    /** Sets the channel idx for th idx-th GPS system. */
    void setChannelIndex(int idx, uint16_t ch_index);

    /** Constructs all GPS system from the generic configuration. */
    void fromGPSSystems(const Config *conf);
    /** Encodes the given GPS system. */
    void fromGPSSystemObj(GPSSystem *sys, const Config *conf);
    /** Constructs a generic GPS system from the idx-th encoded GPS system. */
    GPSSystem *toGPSSystemObj(int idx) const;
    /** Links the specified generic GPS system. */
    bool linkGPSSystem(int idx, GPSSystem *sys, const CodeplugContext &ctx) const;
  };

  /** Binary representation of the analog alarm settings.
   * Size 0x6 bytes. */
  struct __attribute__((packed)) analog_alarm_setting_t {
    /** Possible alarm types. */
    typedef enum {
      ALARM_AA_NONE = 0,           ///< No alarm at all.
      ALARM_AA_TX_AND_BG = 1,      ///< Transmit and background.
      ALARM_AA_TX_AND_ALARM = 2,   ///< Transmit and alarm
      ALARM_AA_BOTH = 3,           ///< Both?
    } Action;

    /** Possible alarm signalling types. */
    typedef enum {
      ALARM_ENI_NONE = 0,          ///< No alarm code signalling.
      ALARM_ENI_DTMF = 1,          ///< Send alarm code as DTMF.
      ALARM_ENI_5TONE = 2          ///< Send alarm code as 5-tone.
    } ENIType;

    uint8_t action;                ///< Action to take, see @c Action.
    uint8_t eni_type;              ///< ENI type, see @c ENIType.
    uint8_t emergency_id_idx;      ///< Emergency ID index, 0-based.
    uint8_t time;                  ///< Alarm time in seconds, default 10.
    uint8_t tx_dur;                ///< TX duration in seconds, default 10.
    uint8_t rx_dur;                ///< RX duration in seconds, default 60.
  };

  /** Represents an entry in the DMR ID -> contact map within the binary code-plug. */
  struct __attribute__((packed)) contact_map_t {
    uint32_t id_group;             ///< Combined ID and group-call flag. The ID is encoded in
                                   /// BCD in big-endian, shifted to the left by one bit. Bit 0 is
                                   /// then the group-call flag. The cobined id and flag is then
                                   /// stored in little-endian.
    uint32_t contact_index;        ///< Index to contact, 32bit little endian.

    /** Constructor, clears the map entry. */
    contact_map_t();
    /** Clears the map entry. */
    void clear();
    /** Returns @c true if this entry is valid. */
    bool isValid() const;
    /** Returns @c true if the entry is a group-call contact. */
    bool isGroup() const;
    /** Returns the DMR ID of the entry. */
    uint32_t ID() const;
    /** Sets the ID and group-call flag of the entry. */
    void setID(uint32_t id, bool group);
    /** Returns the contact index of the entry. */
    uint32_t index() const;
    /** Sets the contact index of the entry. */
    void setIndex(uint32_t index);
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
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());
};

#endif // D878UVCODEPLUG_HH
