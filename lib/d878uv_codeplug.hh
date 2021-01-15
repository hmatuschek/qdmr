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
 *  <tr><td>01043000</td> <td>000080</td>      <td>Optional, roaming zone 1?</td></tr>
 *
 *  <tr><th colspan="3">Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02600000</td> <td>max. 009C40</td> <td>Index list of valid contacts.
 *    10000 32bit indices, little endian, default 0xffffffff</td></tr>
 *  <tr><td>02640000</td> <td>000500</td>      <td>Contact bitmap, 10000 bit, inverted, default 0xff, 0x00 padded.</td></tr>
 *  <tr><td>02680000</td> <td>max. 0f4240</td> <td>10000 contacts, see @c contact_t.
 *    As each contact is 100b, they do not align with the 16b blocks being transferred to the device.
 *    Hence contacts are organized internally in groups of 4 contacts forming a "bank". </td></tr>
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
 *  <tr><td>02501280</td> <td>000030</td> <td>Unknown, set to 0x00. </td></tr>
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>000630</td> <td>General settings, see @c general_settings_t.</td></tr>
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
 *  <tr><td>024C2000</td> <td>0003F0</td> <td>List of 250 offset frequencies.
 *    32bit little endian frequency in 10Hz. I.e., 600kHz = 60000. Default 0x00000000, 0x00 padded.</td></tr>
 *  <tr><td>02501400</td> <td>000100</td> <td>Talkeralias settings, see @c talkeralias_setting_t.</td></tr>
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
 *  <tr><th colspan="3">ID -> Contact map</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>04340000</td> <td>max. 013880</td> <td>DMR ID to contact index map, see @c contact_map_t.
 *    Sorted by ID, empty entries set to 0xffffffffffffffff.</td>
 *
 *  <tr><th colspan="3">Still unknown</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>01042000</td> <td>000020</td> <td>Roaming channel bitmask.</td></tr>
 *  <tr><td>01042080</td> <td>000010</td> <td>Unknown data, default=0x00</td></tr>
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
 *  <tr><td>04340000</td> <td>000050</td> <td>Unknown data, some how related to contacts.</td></tr>
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

    // Bytes 0-7
    uint32_t rx_frequency;              ///< RX Frequency, 8 digits BCD, big-endian.
    uint32_t tx_offset;                 ///< TX Offset, 8 digits BCD, big-endian, sign in repeater_mode.

    // Byte 8
    uint8_t channel_mode    : 2,        ///< Mode: Analog or Digital
      power                 : 2,        ///< Power: Low, Middle, High, Turbo
      bandwidth             : 1,        ///< Bandwidth: 12.5 or 25 kHz
      _unused8              : 1,        ///< Unused, set to 0.
      repeater_mode         : 2;        ///< Sign of TX frequency offset.

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
    uint16_t custom_ctcss;              ///< Custom CTCSS tone frequency: 0x09cf=251.1, 0x0a28=260, big-endian?.
    uint8_t tone2_decode;               ///< 2-Tone decode: 0x00=1, 0x0f=16
    uint8_t _unused19;                  ///< Unused, set to 0.

    // Bytes 20-23
    uint32_t contact_index;             ///< Contact index, zero-based, little-endian, none=0xffffffff.

    // Byte 24
    uint8_t id_index;                   ///< Index to radio-ID table.

    // Byte 25
    uint8_t ptt_id          : 2,        ///< PTT ID, see PTTId.
      _unused25_1           : 2,        ///< Unused, set to 0.
      squelch_mode          : 1,        ///< Squelch mode, see SquelchMode.
      _unused25_2           : 3;        ///< Unused, set to 0.

    // Byte 26
    uint8_t tx_permit       : 2,        ///< TX permit, see Admin.
      _unused26_1           : 2,        ///< Unused, set to 0.
      opt_signal            : 2,        ///< Optional signaling, see OptSignaling.
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
      _unused33_1           : 1,        ///< Unused, set to 0.
      simplex_tdma          : 1,        ///< Simplex TDMA enabled.
      _unused33_2           : 1,        ///< Unused, set to 0.
      tdma_adaptive         : 1,        ///< TDMA adaptive enable.
      rx_gps                : 1,        ///< Receive digital GPS messages.
      enh_encryption        : 1,        ///< Enable enhanced encryption.
      work_alone            : 1;        ///< Work alone, 0=off, 1=on.

    // Byte 34
    uint8_t encryption;                 ///< Digital encryption, 1-32, 0=off.

    // Bytes 35-51
    uint8_t name[16];                   ///< Channel name, ASCII, zero filled.
    uint8_t _unused51;                  ///< Unused, set to 0.

    // Byte 52
    uint8_t ranging         : 1,        ///< Ranging enabled.
      through_mode          : 1,        ///< Through-mode enabled.
      excl_from_roaming     : 1,        ///< Exclude channel from roaming.
      _unused52             : 5;        ///< Unused, set to 0.

    // Byte 53
    uint8_t aprs_report     : 2,        ///< Enable APRS report.
      _unused53             : 6;        ///< Unused, set to 0.

    // Bytes 54-63
    uint8_t analog_aprs_ptt;            ///< Enable analog APRS PTT, 0=off, 1=start of transmission, 2=end of transmission.
    uint8_t digi_aprs_ptt;              ///< Enable digital APRS PTT, 0=off, 1=on.
    uint8_t gps_system;                 ///< Index of DMR GPS report system, 0-7;
    uint8_t _unused57[7];               ///< Unused, set to 0.´

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
    typedef enum {
      CALL_PRIVATE = 0,                 ///< Private call.
      CALL_GROUP = 1,                   ///< Group call.
      CALL_ALL = 2                      ///< All call.
    } CallType;

    /** Possible ring-tone types. */
    typedef enum {
      ALERT_NONE = 0,                   ///< Alert disabled.
      ALERT_RING = 1,                   ///< Ring tone.
      ALERT_ONLINE = 2                  ///< WTF?
    } AlertType;

    // Byte 0
    uint8_t type;                       ///< Call Type: Group Call, Private Call or All Call.
    // Bytes 1-16
    uint8_t name[16];                   ///< Contact Name max 16 ASCII chars 0-terminated.
    // Bytes 17-34
    uint8_t _unused17[18];              ///< Unused, set to 0.
    // Bytes 35-38
    uint32_t id;                        ///< Call ID, BCD coded 8 digits, little-endian.
    // Byte 39
    uint8_t call_alert;                 ///< Call Alert. One of None, Ring, Online Alert.
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
    uint8_t number[7];                  ///< Number encoded as BCD little-endian.
    uint8_t digits;                     ///< Number of digits.
    uint8_t name[16];                   ///< Name, ASCII, upto 16 chars, 0-terminated & padded.
  };

  /** Represents the actual RX group list encoded within the binary code-plug.
   *
   * Memmory layout of encoded group list (288byte):
   * @verbinclude d878uvgrouplist.txt
   */
  struct __attribute__((packed)) grouplist_t {
    // Bytes 0-255
    uint32_t member[64];                ///< Contact indices, 0-based, little-endian.
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
    typedef enum {
      REVCH_SELECTED = 0,               ///< Selected.
      REVCH_SEL_TB = 1,                 ///< Selected + TalkBack.
      REVCH_PRIO_CH1 = 2,               ///< Priority Channel Select 1.
      REVCH_PRIO_CH2 = 3,               ///< Priority Channel Select 2.
      REVCH_LAST_CALLED = 4,            ///< Last Called.
      REVCH_LAST_USED = 5,              ///< Last Used.
      REVCH_PRIO_CH1_TB = 6,            ///< Priority Channel Select 1 + TalkBack.
      REVCH_PRIO_CH2_TB = 7             ///< Priority Channel Select 2 + TalkBack.
    } RevertChannel;

    // Bytes 0-1
    uint8_t _unused0;                   ///< Unused, set to 0.
    uint8_t prio_ch_select;             ///< Priority Channel Select, default = PRIO_CHAN_OFF.

    // Bytes 2-5
    uint16_t priority_ch1;              ///< Priority Channel 1: 0=Current Channel, index+1, little endian, 0xffff=Off.
    uint16_t priority_ch2;              ///< Priority Channel 2: 0=Current Channel, index+1, little endian, 0xffff=Off.

    // Bytes 6-13
    uint16_t look_back_a;               ///< Look Back Time A, sec*10, little endian, default 0x000f.
    uint16_t look_back_b;               ///< Look Back Time B, sec*10, little endian, default 0x0019.
    uint16_t dropout_delay;             ///< Dropout Delay Time, sec*10, little endian, default 0x001d.
    uint16_t dwell;                     ///< Dwell Time, sec*10, little endian, default 0x001d.

    // Byte 14
    uint8_t revert_channel;             ///< Revert Channel, see @c RevertChannel, default REVCH_SELECTED.

    // Bytes 15-31
    uint8_t name[16];                   ///< Scan List Name, ASCII, 0-terminated.
    uint8_t _pad31;                     ///< Pad byte, set to 0x00.

    // Bytes 32-131
    uint16_t member[50];                ///< Channels indices, 0-based, little endian, 0xffff=empty

    // Bytes 132-143
    uint8_t _unused132[12];             ///< Unused, set to 0.

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

  /** Represents the general config of the radio within the binary codeplug. */
  struct __attribute__((packed)) general_settings_t {
    /** Possible power-on display settings. */
    typedef enum {
      PWRON_DEFAULT = 0,           ///< Default.
      PWRON_CUSTOM_TEXT = 1,       ///< Custom text.
      PWRON_CUSTOM_IMG = 2         ///< Custom image.
    } PowerOnDisplay;

    // Bytes 0x00-0x0f.
    uint8_t keytone;               ///< Enable key tones, Off=0x00, On=0x01.
    uint8_t _unknown1[5];          ///< Unknown settings, default= 00 00 00 00 04 00.
    uint8_t power_on;              ///< Power-on display, see @c PowerOnDisplay, default=PWRON_DEFAULT.
    uint8_t pwron_passwd;          ///< Boot password enabled, Off=0x00, On=0x01.
    uint8_t _unknown8[8];          ///< Unknown settings block.

    // Bytes 0x10-0x2f.
    uint8_t _unknown16[31];        ///< Unknown settings block.
    uint8_t call_alert;            ///< Enable call tone, default=1, Off=0x00, On=0x01.

    // Byte 0x30-3f
    uint8_t _unknown48;            ///< Unknown settings block.
    uint8_t talk_permit_tone;      ///< Enable talk permit, Off=0x00, Digi=0x01, Analog=0x02, Both=0x03.
    uint8_t call_idle_tone;        ///< Enable digi-call idle tone, Off=0x00, On=0x01.
    uint8_t _unknown51;            ///< Unknown settings, default=0x01
    uint8_t _unknown52;            ///< Unknown settings, default=0x00
    uint8_t _unknown53;            ///< Unknown settings, default=0x00
    uint8_t ch_idle_tone;          ///< Enable channel idle tone, Off=0x00, On=0x01.
    uint8_t _unknown55;            ///< Unknown settings, default=0x01;
    uint8_t startup_tone;          ///< Enable Startup tone, Off=0x00, On=0x01.
    uint8_t _unknown57[7];         ///< Unknown settings block.

    // Bytes 0x40-0xaf
    uint8_t _unknown64[112];       ///< Unknown settings block;

    // Bytes 0xb0-0xbf
    uint8_t _unknown176[11];       ///< Unknown settings block;
    uint8_t key_tone_adj;          ///< Key tone adjust, fixed 0x01-0x0f, or 0x00=variable.
    uint8_t _unknown188[4];        ///< Unknown settings block;

    // Bytes 0xc0-0xcf
    uint8_t _unknown192[16];       ///< Unknown settings block;

    // Bytes 0xd0-0xdf
    uint8_t _unknown208[6];        ///< Unknown settings block;
    uint8_t boot_ch;               ///< Enable default channel on boot, Off=0x00, On=0x01.
    uint8_t vfo_a_zone_index;      ///< Index of default zone for VFO A, 0-based, default=0.
    uint8_t vfo_b_zone_index;      ///< Index of default zone for VFO B, 0-based, default=0.
    uint8_t vfo_a_ch_index;        ///< Default channel index (within selected zone) for VFO A, 0-based, default=0, 0xff=VFO.
    uint8_t vfo_b_ch_index;        ///< Default channel index (within selected zone) for VFO B, 0-based, default=0, 0xff=VFO.
    uint8_t _unknown219[5];        ///< Unknown settings block.

    // Bytes 0x0e0-0x5ff
    uint8_t  _unknown224[1312];    ///< Large unknown settings block.

    // Bytes 0x600-0x61f
    uint8_t intro_line1[16];       ///< Intro line 1, up to 14 ASCII characters, 0-terminated.
    uint8_t intro_line2[16];       ///< Intro line 2, up to 14 ASCII characters, 0-terminated.

    // Bytes 0x620-0x62f
    uint8_t password[16];          ///< Boot password, up to 8 ASCII digits, 0-terminated.

    /** Constructs an empty general settings. */
    general_settings_t();
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

    // byte 0x00
    uint8_t _unknown0;             ///< Unknown, set to 0xff.
    uint32_t frequency;            ///< TX frequency, BCD encoded, little endian in 10Hz.
    uint8_t tx_delay;              ///< TX delay, multiples of 20ms, default=1200ms.
    uint8_t sig_type;              ///< Signalling type, default=0.
    uint8_t ctcss;                 ///< CTCSS tone-code, default=0.
    uint16_t dcs;                  ///< DCS code, little endian, default=0x0013.
    uint8_t manual_tx_interval;    ///< Manual TX intervals in seconds.
    uint8_t auto_tx_interval;      ///< Auto TX interval in multiples of 30s.
    uint8_t tx_tone_enable;        ///< TX enable, 0=off, 1=on.
    uint8_t fixed_location;        ///< Fixed location data, 0=off, 1=on.

    uint8_t lat_deg;               ///< Latitude in degree.
    uint8_t lat_min;               ///< Latitude minutes.
    uint8_t lat_sec;               ///< Latitude seconds (1/100th of a minute).
    uint8_t north_south;           ///< North or south flag, north=0, south=1.
    uint8_t lon_deg;               ///< Longitude in degree.
    uint8_t lon_min;               ///< Longitude in minutes.
    uint8_t lon_sec;               ///< Longitude in seconds (1/100th of a minute).
    uint8_t east_west;             ///< East or west flag, east=0, west=1.

    uint8_t to_call[6];            ///< Destination call, 6 x ASCII, 0-padded.
    uint8_t to_ssid;               ///< Destination SSID.

    uint8_t from_call[6];          ///< Source call, 6 x ASCII, 0-padded.
    uint8_t from_ssid;             ///< Source SSID.

    // byte 0x24
    uint8_t path[20];              ///< Path string, upto 20 ASCII chars, 0-padded.

    // byte 0x38
    uint8_t _unknown56;            ///< Unknown set to 00.
    char symbol;                   ///< ASCII-char for APRS icon table, ie. '/' or '\' for primary
                                   ///  and alternate icon table respectively.
    char map_icon;                 ///< ASCII-char of APRS map icon.
    uint8_t power;                 ///< Transmit power.
    uint8_t prewave_delay;         ///< Prewave delay in 10ms steps.

    // bytes 0x3d
    uint8_t _unknown61;            ///< Unknown, set to 01.
    uint8_t _unknown62;            ///< Unknown, set to 03.
    uint8_t _unknown63;            ///< Unknown, set to ff.

    void setAutoTxInterval(int sec);
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

    gps_systems_t();
    void clear();
    bool isValid(int idx) const;

    uint32_t getContactId(int idx) const;
    void setContactId(int idx, uint32_t number);
    DigitalContact::Type getContactType(int idx) const;
    void setContactType(int idx, DigitalContact::Type type);

    uint16_t getChannelIndex(int idx) const;
    void setChannelIndex(int idx, uint16_t ch_index);

    void fromGPSSystems(const Config *conf);
    void fromGPSSystemObj(GPSSystem *sys, const Config *conf);
    GPSSystem *toGPSSystemObj(int idx) const;
    bool linkGPSSystem(int idx, GPSSystem *sys, const CodeplugContext &ctx) const;
  };

  /** Binary representation of the talker alias setting.
   * This code-plug section is yet to be reverse-engineered.
   * Size 0x100 bytes. */
  struct __attribute__((packed)) talkeralias_setting_t {
    // Byte 0x00
    uint8_t send_alias;            ///< Send talker alias, 0=off, 1=on.
    uint8_t _unknown1[15];         ///< Unknown.
    // Byte 0x10
    uint8_t _unknown16[16];        ///< Unknown.
    // Byte 0x20
    uint8_t _unknown32[16];        ///< Unknown.
    // Byte 0x30
    uint8_t _unknown48[16];        ///< Unknown.
    // Byte 0x40
    uint8_t _unknown64[16];        ///< Unknown.
    // Byte 0x50
    uint8_t _unknown80[16];        ///< Unknown.
    // Byte 0x60
    uint8_t _unknown90[16];        ///< Unknown.
    // Byte 0x70
    uint8_t _unknown112[16];       ///< Unknown.
    // Byte 0x80
    uint8_t _unknown128[16];       ///< Unknown.
    // Byte 0x90
    uint8_t _unknown144[16];       ///< Unknown.
    // Byte 0xa0
    uint8_t _unknown160[16];       ///< Unknown.
    // Byte 0xb0
    uint8_t _unknown176[16];       ///< Unknown.
    // Byte 0xc0
    uint8_t _unknown192[16];       ///< Unknown.
    // Byte 0xd0
    uint8_t _unknown208[16];       ///< Unknown.
    // Byte 0xe0
    uint8_t _unknown224[16];       ///< Unknown.
    // Byte 0xf0
    uint8_t _unknown240[16];       ///< Unknown.
  };

  /** Binary representation of the analog alarm settings.
   * This code-plug secion is yet to be reverse-engineered.
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
  bool encode(Config *config, bool update=true);
};

#endif // D878UVCODEPLUG_HH
