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
 * @ingroup d878uv */
class D878UVCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Represents the actual channel encoded within the binary code-plug. */
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

    // Bytes 0-7
    uint32_t    rx_frequency;           ///< RX Frequency, 8 digits BCD, big-endian.
    uint32_t    tx_offset;              ///< TX Offset, 8 digits BCD, big-endian, sign in repeater_mode.

    // Byte 8
    uint8_t     channel_mode    : 2,    ///< Mode: Analog or Digital
                power           : 2,    ///< Power: Low, Middle, High, Turbo
                bandwidth       : 1,    ///< Bandwidth: 12.5 or 25 kHz
                _unused8        : 1,    ///< Unused, set to 0.
                repeater_mode   : 2;    ///< Sign of TX frequency offset.

    // Byte 9
    uint8_t     rx_ctcss        : 1,    ///< CTCSS decode enable.
                rx_dcs          : 1,    ///< DCS decode enable.
                tx_ctcss        : 1,    ///< CTCSS encode enable.
                tx_dcs          : 1,    ///< DCS encode enable
                reverse         : 1,    ///< CTCSS phase-reversal.
                rx_only         : 1,    ///< TX prohibit.
                call_confirm    : 1,    ///< Call confirmation enable.
                talkaround      : 1;    ///< Talk-around enable.

    // Bytes 10-15
    uint8_t     ctcss_transmit;         ///< TX CTCSS tone, 0=62.5, 50=254.1, 51=custom CTCSS tone.
    uint8_t     ctcss_receive;          ///< RX CTCSS tone: 0=62.5, 50=254.1, 51=custom CTCSS tone.
    uint16_t    dcs_transmit;           ///< TX DCS code: 0=D000N, 511=D777N, 512=D000I, 1023=D777I, DCS code-number in octal, little-endian.
    uint16_t    dcs_receive;            ///< RX DCS code: 0=D000N, 511=D777N, 512=D000I, 1023=D777I, DCS code-number in octal, little-endian.

    // Bytes 16-19
    uint16_t    custom_ctcss;           ///< Custom CTCSS tone frequency: 0x09cf=251.1, 0x0a28=260, big-endian?.
    uint8_t     tone2_decode;           ///< 2-Tone decode: 0x00=1, 0x0f=16
    uint8_t     _unused19;              ///< Unused, set to 0.

    // Bytes 20-23
    uint16_t    contact_index;          ///< Contact index, zero-based, little-endian
    uint16_t    _unused22;              ///< Unused, set to 0.

    // Byte 24
    uint8_t     id_index;               ///< Index to radio-ID table.

    // Byte 25
    uint8_t     ptt_id          : 2,    ///< PTT ID, see PTTId.
                _unused25_1     : 2,    ///< Unused, set to 0.
                squelch_mode    : 1,    ///< Squelch mode, see SquelchMode.
                _unused25_2     : 3;    ///< Unused, set to 0.

    // Byte 26
    uint8_t     tx_permit       : 2,    ///< TX permit, see Admin.
                _unused26_1     : 2,    ///< Unused, set to 0.
                _opt_signal     : 2,    ///< Optional signaling, see OptSignaling.
                _unused26_2     : 2;    ///< Unused, set to 0.

    // Bytes 27-31
    uint8_t     scan_list_index;        ///< Scan list index, 0xff=None, 0-based.
    uint8_t     group_list_index;       ///< RX group-list, 0xff=None, 0-based.
    uint8_t     id_2tone;               ///< 2-Tone ID, 0=1, 0x17=24.
    uint8_t     id_5tone;               ///< 5-Tone ID, 0=1, 0x63=100.
    uint8_t     id_dtmf;                ///< DTMF ID, 0=1, 0x0f=16.

    // Byte 32
    uint8_t     color_code;             ///< Color code, 0-15

    // Byte 33
    uint8_t     slot2           : 1,    ///< Timeslot, 0=TS1, 1=TS2.
                _unused33_1     : 1,    ///< Unused, set to 0.
                simplex_tdma    : 1,    ///< Simplex TDMA enabled.
                _unused33_2     : 1,    ///< Unused, set to 0.
                tdma_adaptive   : 1,    ///< TDMA adaptive enable.
                _unused33_3     : 1,    ///< Unused, set to 0.
                enh_encryption  : 1,    ///< Enable enhanced encryption.
                work_alone      : 1;    ///< Work alone, 0=off, 1=on.

    // Byte 34
    uint8_t     encryption;             ///< Digital encryption, 1-32, 0=off.

    // Bytes 35-51
    uint8_t     name[16];               ///< Channel name, ASCII, zero filled.
    uint8_t     _unused51;              ///< Unused, set to 0.

    // Byte 52
    uint8_t     ranging         : 1,    ///< Ranging enabled.
                through_mode    : 1,    ///< Through-mode enabled.
                _unused52       : 6;    ///< Unused, set to 0.

    // Byte 53
    uint8_t     aprs_report     : 1,    ///< Enable APRS report.
                _unused53       : 7;    ///< Unused, set to 0.

    // Bytes 54-63
    uint8_t     aprs_channel;           ///< APRS report channel index, 0-7.
    uint8_t     _unused55[9];           ///< Unused, set to 0.´

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
    uint8_t call_alert;                 ///< Call Alert: None, Ring, Online Alert
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

  struct __attribute__((packed)) grouplist_t {
    // Bytes 0-255
    uint32_t member[64];                ///< Contact indices, 0-based, little-endian.
    // Bytes 256-319
    uint8_t name[35];                   ///< Group-list name, up to 16 x ASCII, 0-terminated.
    uint8_t unused[29];                 ///< Unused, set to 0.

    grouplist_t();
    void clear();
    bool isValid() const;

    QString getName() const;
    void setName(const QString &name);

    RXGroupList *toGroupListObj() const;
    bool linkGroupList(RXGroupList *lst, const CodeplugContext &ctx);
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


public:
  /** Empty constructor. */
  explicit D878UVCodeplug(QObject *parent = nullptr);

  /** Clears and resets the complete codeplug to some default values. */
  void clear();

  void allocateFromBitmaps();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config);
};

#endif // D878UVCODEPLUG_HH
