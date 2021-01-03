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
 * <table>
 *  <tr><th colspan="4">Channels (0080000-0x00FC2880)</th></tr>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>00800000</td> <td>00802000</td> <td>002000</td> <td>Channel bank 0, see @c channel_t</td></tr>
 *  <tr><td>00802000</td> <td>00804000</td> <td>002000</td> <td>Uknown data, related to CH bank 0?
 *    It is of exactly the same size as the channel bank 0. Mostly 0x00, a few 0xff.</td></tr>
 *  <tr><td>00840000</td> <td>00846000</td> <td>002000</td> <td>Channel bank 1</td></tr>
 *  <tr><td>00842000</td> <td>00844000</td> <td>002000</td> <td>Uknown data, related to CH bank 1?</td></tr>
 *  <tr><td>...</td> <td>...</td> <td>...</td> <td>...</td></tr>
 *  <tr><td>00FC0000</td> <td>00FC0800</td> <td>000800</td> <td>Channel bank 32, last 32 channels.</td></tr>
 *  <tr><td>00FC0800</td> <td>00FC0840</td> <td>000040</td> <td>VFO A settings, see @c channel_t.</td></tr>
 *  <tr><td>00FC0840</td> <td>00FC0880</td> <td>000040</td> <td>VFO B settings, see @c channel_t.</td></tr>
 *  <tr><td>00FC2000</td> <td>00FC2800</td> <td>000800</td> <td>Unknown data, realted to CH bank 32.</td></tr>
 *  <tr><td>00FC2800</td> <td>00FC2880</td> <td>000080</td> <td>Unknonw data, related to VFO A+B?
 *    It is of exactly the same size as the two VFO channels. Mostly 0x00, a few 0xff. Same pattern as
 *    the unknown data associated with channel banks.</td></tr>
 *
 *  <tr><th colspan="4"></th></tr>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>01000000</td> <td>0101f400</td> <td>01f400</td> <td>250 zones channel lists of 250 16bit indices each.
 *    0-based, little endian, default/padded=0xffff. Offset between channel lists 0x200, size of each list 0x1f4.</td></tr>
 *  <tr><td>01042000</td> <td>01042020</td> <td>000020</td> <td>Unknown data, default=0x00</td></tr>
 *  <tr><td>01042080</td> <td>01042090</td> <td>000010</td> <td>Unknown data, default=0x00</td></tr>
 *  <tr><td>01043000</td> <td>01043080</td> <td>000080</td> <td>Optional, roaming zone 1?</td></tr>

 *  <tr><th colspan="4">Scan lists (01080000-01441290)</th></tr>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>01080000</td> <td>01080090</td> <td>000090</td> <td>Scanlist 1</td></tr>
 *  <tr><td>01080200</td> <td>01080290</td> <td>000090</td> <td>Scanlist 2</td></tr>
 *  <tr><td>...</td> <td>...</td> <td>...</td> <td>...</td></tr>
 *  <tr><td>01441200</td> <td>01441290</td> <td>000090</td> <td>Scanlist 250</td></tr>
 *  <tr><td>01640000</td> <td>01640050</td> <td>000050</td> <td>Unknown data.</td></tr>
 *  <tr><td>01640800</td> <td>01640890</td> <td>000090</td> <td>Unknown data.</td></tr>
 *  <tr><td>02140000</td> <td>02140800</td> <td>000800</td> <td>Messages 1-8</td></tr>
 *  <tr><td>02180000</td> <td>02180400</td> <td>000800</td> <td>Messages 9-16</td></tr>
 *  <tr><td>...</td> <td>...</td> <td>...</td> <td>...</td></tr>
 *  <tr><td>02440000</td> <td>02440000</td> <td>000800</td> <td>Messages 97-100</td></tr>
 *  <tr><td>02480000</td> <td>02480010</td> <td>000010</td> <td>Unkonwn data.</td></tr>
 *  <tr><td>02480200</td> <td>02480230</td> <td>000030</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C0000</td> <td>024C0020</td> <td>000020</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C0C80</td> <td>024C0C90</td> <td>000010</td> <td>Unknown data, bitmap?, default 0x00.</td></tr>
 *  <tr><td>024C0D00</td> <td>024C1000</td> <td>000010</td> <td>Empty, set to 0x00?`</td></tr>
 *  <tr><td>024C1000</td> <td>024C10D0</td> <td>0000D0</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1100</td> <td>024C1110</td> <td>000010</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1280</td> <td>024C12A0</td> <td>000020</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1300</td> <td>024C1320</td> <td>000020</td> <td>Bitmap of 250 zones.</td></tr>
 *  <tr><td>024C1320</td> <td>024C1340</td> <td>000020</td> <td>Bitmap of 250 scan-lists.</td></tr>
 *  <tr><td>024C1340</td> <td>024C1380</td> <td>000020</td> <td>Unknown bitmap.</td></tr>
 *  <tr><td>024C1400</td> <td>024C1470</td> <td>000070</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C1500</td> <td>024C1700</td> <td>000200</td> <td>Bitmap of 4000 channels, default 0x00, 0x00 padded.</td></tr>
 *  <tr><td>024C1700</td> <td>024C1740</td> <td>000040</td> <td>Unknown, 8bit indices.</td></tr>
 *  <tr><td>024C1800</td> <td>024C1D00</td> <td>000500</td> <td>Empty, set to 0x00?</td></tr>
 *  <tr><td>024C2000</td> <td>024C23E0</td> <td>0003E0</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C2400</td> <td>024C2430</td> <td>000030</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C2600</td> <td>024C2610</td> <td>000010</td> <td>Unknown data.</td></tr>
 *  <tr><td>024C4000</td> <td>024C8000</td> <td>004000</td> <td>Unknown data.</td></tr>
 *  <tr><td>02500000</td> <td>02500630</td> <td>000630</td> <td>General settings, see @c general_settings_t.</td></tr>
 *  <tr><td>02501000</td> <td>025010A0</td> <td>0000A0</td> <td>Unknown, GPS/APRS?</td>
 *  <tr><td>02501200</td> <td>02501240</td> <td>000040</td> <td>Unknown.</td></tr>
 *  <tr><td>02501280</td> <td>025012B0</td> <td>000030</td> <td>Unknown.</td></tr>
 *  <tr><td>02501400</td> <td>02501500</td> <td>000100</td> <td>Unknown.</td></tr>
 *  <tr><td>02540000</td> <td>02541f40</td> <td>001f40</td> <td>250 Zone names.</td></tr>
 *  <tr><td>02580000</td> <td>02581f40</td> <td>001f40</td> <td>250 Radio IDs?</td></tr>
 *  <tr><td>025C0000</td> <td>025C0850</td> <td>000850</td> <td>Unknown data, status messages?</td></tr>
 *  <tr><td>025C0B00</td> <td>025C0B10</td> <td>000010</td> <td>Unknown bitmap?, default 0x00.</td>
 *  <tr><td>025C0B10</td> <td>025C0B30</td> <td>000020</td> <td>Bitmap of 250 RX group lists, default/padding 0x00.</td></tr>
 *  <tr><td>02600000</td> <td>02609C40</td> <td>009C40</td> <td>Unknown, 10000 32bit indices, little endian, default 0xffffffff</td></tr>
 *  <tr><td>02640000</td> <td>026404F0</td> <td>000500</td> <td>Contact bitmap, 10000 bit, inverted, default 0xff, 0x00 padded.</td></tr>
 *  <tr><td>02680000</td> <td>02774240</td> <td>0f4240</td> <td>10000 contacts, see @c contact_t.</td></tr>
 *  <tr><td>02900000</td> <td>02900080</td> <td>000080</td> <td>Unknown bitmap, 1024 bit, inverted, default 0xff.</td></tr>
 *  <tr><td>02900100</td> <td>02900180</td> <td>000080</td> <td>Unknown bitmap, 1024 bit, inverted, default 0xff.</td></tr>
 *  <tr><td>02940000</td> <td>02940180</td> <td>000180</td> <td>128 analog contacts.</td></tr>
 *  <tr><td>02980000</td> <td>02980120</td> <td>000120</td> <td>Grouplist 0, see @c grouplist_t.</td></tr>
 *  <tr><td>02980200</td> <td>02980320</td> <td>000120</td> <td>Grouplist 1</td></tr>
 *  <tr><td>...</td>      <td>...</td>      <td>...</td>    <td>...</td></tr>
 *  <tr><td>0299f200</td> <td>0299f320</td> <td>000120</td> <td>Grouplist 250</td></tr>
 *  <tr><td>04340000</td> <td>04340050</td> <td>000050</td> <td>Unknown data.</td></tr>
 * </table>
 * @ingroup d878uv */
class D878UVCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Represents the actual channel encoded within the binary code-plug.
   * Memeory size is 64 bytes. */
  struct __attribute__((packed)) channel_t {
    /** Defined possible channel modes, see @c channel_mode. */
    typedef enum {
      MODE_ANALOG    = 0,               ///< Analog channel.
      MODE_DIGITAL   = 1,               ///< Digital (DMR) channel.
      MODE_MIXED_A_D = 2,               ///< Mixed, transmit analog and receive digital.
      MODE_MIXED_D_A = 3                ///< Mixed, transmit digital and receive analog.
    } Mode;

    typedef enum {
      POWER_LOW = 0,                    ///< Low power, usually 1W.
      POWER_MIDDLE = 1,                 ///< Medium power, usually 2.5W.
      POWER_HIGH = 2,                   ///< High power, usually 5W.
      POWER_TURBO = 3                   ///< Higher power, usually 7W on VHF and 6W on UHF.
    } Power;

    typedef enum {
      BW_12_5_KHZ = 0,                  ///< Narrow band-width (12.5kHz).
      BW_25_KHZ = 1                     ///< High band-width (25kHz).
    } Bandwidth;

    typedef enum {
      RM_SIMPLEX = 0,                   ///< Simplex mode, that is TX frequency = RX frequency. @c tx_offset is ignored.
      RM_TXPOS = 1,                     ///< Repeater mode with positive @c tx_offset.
      RM_TXNEG = 2                      ///< Repeater mode with negative @c tx_offset.
    } RepeaterMode;

    typedef enum {
      PTTID_OFF = 0,                    ///< Never send PTT-ID.
      PTTID_START = 1,                  ///< Send PTT-ID at start.
      PTTID_END = 2,                    ///< Send PTT-ID at end.
      PTTID_START_END = 3               ///< Send PTT-ID at start and end.
    } PTTId;

    typedef enum {
      SQ_CARRIER = 0,                   ///< Open squelch on carrier.
      SQ_TONE = 1                       ///< Open squelch on matching CTCSS tone or DCS code.
    } SquelchMode;

    typedef enum {
      ADMIT_ALWAYS = 0,                 ///< Admit TX always.
      ADMIT_CH_FREE = 1,                ///< Admit TX on channel free.
      ADMIT_CC_DIFF = 2,                ///< Admit TX on mismatching color-code.
      ADMIT_CC_SAME = 3                 ///< Admit TX on matching color-code.
    } Admit;

    typedef enum {
      OPTSIG_OFF = 0,                   ///< None.
      OPTSIG_DTMF = 1,                  ///< Use DTMF.
      OPTSIG_2TONE = 2,                 ///< Use 2-tone.
      OPTSIG_5TONE = 3                  ///< Use 5-tone.
    } OptSignaling;

    typedef enum {
      APRS_REPORT_OFF = 0,
      APRS_REPORT_ANALOG = 1,
      APRS_REPORT_DIGITAL = 2
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
    uint16_t contact_index;             ///< Contact index, zero-based, little-endian
    uint16_t _unused22;                 ///< Unused, set to 0.

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
      _opt_signal           : 2,        ///< Optional signaling, see OptSignaling.
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
    uint8_t _unused54;                  ///< Unused, set to 0.
    uint8_t _unused55;                  ///< Unused, set to 0.
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

  /** Represents a scan list within the binary codeplug.
   * Memory size is 144 bytes. */
  struct __attribute__((packed)) scanlist_t {
    typedef enum {
      PRIO_CHAN_OFF = 0,                ///< Off.
      PRIO_CHAN_SEL1 = 1,               ///< Priority Channel Select 1.
      PRIO_CHAN_SEL2 = 2,               ///< Priority Channel Select 2.
      PRIO_CHAN_SEL12 = 3               ///< Priority Channel Select 1 + Priority Channel Select 2.
    } PriChannel;

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
    uint16_t    priority_ch1;           ///< Priority Channel 1: 0=Current Channel, index+1, little endian, 0xffff=Off.
    uint16_t    priority_ch2;           ///< Priority Channel 2: 0=Current Channel, index+1, little endian, 0xffff=Off.

    // Bytes 6-13
    uint16_t    look_back_a;            ///< Look Back Time A, sec*10, little endian, default 0x000f.
    uint16_t    look_back_b;            ///< Look Back Time B, sec*10, little endian, default 0x0019.
    uint16_t    dropout_delay;          ///< Dropout Delay Time, sec*10, little endian, default 0x001d.
    uint16_t    dwell;                  ///< Dwell Time, sec*10, little endian, default 0x001d.

    // Byte 14
    uint8_t     revert_channel;         ///< Revert Channel, see @c RevertChannel, default REVCH_SELECTED.

    // Bytes 15-31
    uint8_t     name[16];               ///< Scan List Name, ASCII, 0-terminated.
    uint8_t     _pad31;                 ///< Pad byte, set to 0x00.

    // Bytes 32-131
    uint16_t member[50];                ///< Channels indices, 0-based, little endian, 0xffff=empty

    // Bytes 132-143
    uint8_t _unused132[12];             ///< Unused, set to 0.
  };

  struct __attribute__((packed)) contact_t {
    typedef enum {
      CALL_PRIVATE = 0,                 ///< Private call.
      CALL_GROUP = 1,                   ///< Group call.
      CALL_ALL = 2                      ///< All call.
    } CallType;

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

    contact_t();
    void clear();

    bool isValid() const;

    DigitalContact::Type getType() const;
    void setType(DigitalContact::Type type);

    QString getName() const;
    void setName(const QString &name);

    uint32_t getId() const;
    void setId(uint32_t id);

    bool getAlert() const;
    void setAlert(bool enable);

    DigitalContact *toContactObj() const;
    void fromContactObj(const DigitalContact *contact);
  };

  /** Represents the actual RX group list encoded within the binary code-plug.
   * Memory size is 288 bytes. */
  struct __attribute__((packed)) grouplist_t {
    // Bytes 0-255
    uint32_t member[64];                ///< Contact indices, 0-based, little-endian.
    // Bytes 256-319
    uint8_t name[16];                   ///< Group-list name, up to 16 x ASCII, 0-terminated.
    uint8_t unused[16];                 ///< Unused, set to 0.

    grouplist_t();
    void clear();
    bool isValid() const;

    QString getName() const;
    void setName(const QString &name);

    RXGroupList *toGroupListObj() const;
    bool linkGroupList(RXGroupList *lst, const CodeplugContext &ctx);
    void fromGroupListObj(const RXGroupList *lst, const Config *conf);
  };

  struct __attribute__((packed)) radioid_t {
    // Bytes 0-3.
    uint32_t id;                        ///< Up to 8 BCD digits in little-endian.
    // Byte 4.
    uint8_t _unused4;                   ///< Unused, set to 0.
    // Bytes 5-20
    uint8_t name[16];                   ///< Name, up-to 16 ASCII chars, 0-terminated.
    // Bytes 21-31
    uint8_t _unused21[11];              ///< Unused, set to 0.

    radioid_t();
    void clear();
    bool isValid() const;

    QString getName() const;
    void setName(const QString name);

    uint32_t getId() const;
    void setId(uint32_t num);
  };

  struct __attribute__((packed)) general_settings_t {
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
    uint8_t _unknown16[31];
    uint8_t call_alert;            ///< Enable call tone, default=1, Off=0x00, On=0x01.

    // Byte 0x30-3f
    uint8_t _unknown48;            ///< Unknown settings block.
    uint8_t talk_permit_tone;      ///< Enable talk permit, Off=0x00, Digi=0x01, Analog=0x02, Both=0x03.
    uint8_t call_idle_tone;        ///< Enable digi-call idle tone, Off=0x00, On=0x01.
    uint16_t _unknown51;           ///< Unknown settings, default=0x01
    uint16_t _unknown52;           ///< Unknown settings, default=0x00
    uint16_t _unknown53;           ///< Unknown settings, default=0x00
    uint8_t ch_idle_tone;          ///< Enable channel idle tone, Off=0x00, On=0x01.
    uint16_t _unknown55;           ///< Unknown settings, default=0x01;
    uint8_t startup_tone;          ///< Enable Startup tone, Off=0x00, On=0x01.
    uint8_t _unknown58[6];         ///< Unknown settings block.

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

    // Bytes 0x0e0-0x600
    uint8_t  _unknown224[1312];    ///< Large unknown settings block.

    // Bytes 0x600-0x61f
    uint8_t intro_line1[16];       ///< Intro line 1, up to 14 ASCII characters, 0-terminated.
    uint8_t intro_line2[16];       ///< Intro line 2, up to 14 ASCII characters, 0-terminated.

    // Bytes 0x620-0x62f
    uint8_t password[16];          ///< Boot password, up to 8 ASCII digits, 0-terminated.

    general_settings_t();
    void clear();

    QString getIntroLine1() const;
    void setIntroLine1(const QString line);
    QString getIntroLine2() const;
    void setIntroLine2(const QString line);
  };


public:
  /** Empty constructor. */
  explicit D878UVCodeplug(QObject *parent = nullptr);

  /** Clears and resets the complete codeplug to some default values. */
  void clear();

  void setBitmaps(Config *config);
  void allocateFromBitmaps();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, bool update=true);
};

#endif // D878UVCODEPLUG_HH
