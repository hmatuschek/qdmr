#ifndef OPENGD77_CODEPLUG_HH
#define OPENGD77_CODEPLUG_HH

#include "gd77_codeplug.hh"

/** Represents, encodes and decodes the device specific codeplug for Open GD-77 firmware.
 * This codeplug is almost identical to the original GD77 codeplug.
 *
 * @section ogd77cpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is divided into two images
 * (EEPROM and Flash) and each image again into two sections.

 * The first segment of the EEPROM image starts at the address 0x000e0 and ends at 0x06000, while
 * the second EEPROM section starts at 0x07500 and ends at 0x0b000.
 *
 * The first segment of the Flash image starts at the address 0x00000 and ends at 0x011a0, while the
 * second Flash section starts at 0x7b000 and ends at 0x8ee60.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First EEPROM segment 0x000e0-0x06000</th></tr>
 *  <tr><td>0x000e0</td> <td>0x000ec</td> <td>0x000c</td> <td>General settings, see @c GD77Codeplug::general_settings_t.</td></tr>
 *  <tr><td>0x000ec</td> <td>0x00128</td> <td>0x003c</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 message texts, see @c GD77Codeplug::msgtab_t</td></tr>
 *  <tr><td>0x01370</td> <td>0x01790</td> <td>0x0420</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01790</td> <td>0x02dd0</td> <td>0x1640</td> <td>64 scan lists, see @c GD77Codeplug::scanlist_t</td></tr>
 *  <tr><td>0x02dd0</td> <td>0x03780</td> <td>0x09b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 chanels (bank 0), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><td>0x05390</td> <td>0x06000</td> <td>0x0c70</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second EEPROM segment 0x07500-0x13000</th></tr>
 *  <tr><td>0x07500</td> <td>0x07540</td> <td>0x0040</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c GD77Codeplug::intro_text_t</td></tr>
 *  <tr><td>0x07560</td> <td>0x08010</td> <td>0x0ab0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x12c10</td> <td>0xac00</td> <td>250 zones, see @c OpenGD77Codeplug::zonetab_t</td></tr>
 *  <tr><td>0x12c10</td> <td>0x13000</td> <td>0x03f0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">First Flash segment 0x00000-0x011a0</th></tr>
 *  <tr><td>0x00000</td> <td>0x011a0</td> <td>0x11a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second Flash segment 0x7b000-0x8ee60</th></tr>
 *  <tr><td>0x7b000</td> <td>0x7b1b0</td> <td>0x01b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x7b1b0</td> <td>0x87620</td> <td>0xc470</td> <td>Remaining 896 chanels (bank 1-7), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><td>0x87620</td> <td>0x8d620</td> <td>0x6000</td> <td>1024 contacts, see @c GD77Codeplug::contact_t.</td></tr>
 *  <tr><td>0x8d620</td> <td>0x8e2a0</td> <td>0x0c80</td> <td>76 RX group lists, see @c GD77Codeplug::grouptab_t</td></tr>
 *  <tr><td>0x8e2a0</td> <td>0x8ee60</td> <td>0x0bc0</td> <td>??? Unknown ???</td></tr>
 * </table>
 * @ingroup ogd77 */
class OpenGD77Codeplug: public GD77Codeplug
{
	Q_OBJECT

public:
  /** EEPROM memory bank. */
  static const uint32_t EEPROM = 0;
  /** Flash memory bank. */
  static const uint32_t FLASH  = 1;

protected:
  static const int NZONES    = 68;    ///< The number of zones.

  /** Channel representation within the binary codeplug. */
  typedef struct __attribute__((packed)) {
    /** Possible channel types analog (FM) or digital (DMR). */
    typedef enum {
      MODE_ANALOG  = 0,               ///< An analog (FM) channel.
      MODE_DIGITAL = 1                ///< A digital (DMR) channel.
    } Mode;

    /** Possible admit criteria. */
    typedef enum {
      ADMIT_ALWAYS  = 0,              ///< Always allow transmit.
      ADMIT_CH_FREE = 1,              ///< Allow transmit if channel is free.
      ADMIT_COLOR   = 2               ///< Allow transmit if channel is free and colorcode matches.
    } Admit;

    /** Possible privacy settings (unused/forbidden in hamradio). */
    typedef enum {
      PRIVGR_NONE     = 0,            ///< No privacy.
      PRIVGR_53474C39 = 1             ///< Some privacy.
    } PrivGroup;

    /** Possible squelch settings. */
    typedef enum {
      SQ_TIGHT  = 0,                  ///< Tight squelch (whatever that means).
      SQ_NORMAL = 1                   ///< Norma squelch settings.
    } SquelchType;

    /** Possible bandwidths for ananlog channels. Digital channels are set to 12.5kHz by default. */
    typedef enum {
      BW_12_5_KHZ = 0,                ///< 12.5kHz channel width (default for digital channels).
      BW_25_KHZ   = 1                 ///< 25kHz channel, wastes some energy.
    } Bandwidth;

    /** All possible power settings. */
    typedef enum {
      POWER_GLOBAL =  0,              ///< Use global power setting.
      POWER_50mW   =  1,              ///< About 50mW.
      POWER_250mW  =  2,              ///< About 250mW.
      POWER_500mW  =  3,              ///< About 500mW.
      POWER_750mW  =  4,              ///< About 750mW.
      POWER_1W     =  5,              ///< About 1W.
      POWER_2W     =  6,              ///< About 2W.
      POWER_3W     =  7,              ///< About 3W.
      POWER_4W     =  8,              ///< About 4W.
      POWER_5W     =  9,              ///< About 5W.
      POWER_MAX    = 10,              ///< Maximum power (5.5W on UHF, 7W on VHF).
    } Power;


    // Bytes 0-15
    uint8_t name[16];                 ///< Channel Name
    // Bytes 16-23
    uint32_t rx_frequency;            ///< RX Frequency: 8 digits BCD
    uint32_t tx_frequency;            ///< TX Frequency: 8 digits BCD
    // Byte 24
    uint8_t channel_mode;             ///< Mode: Analog or Digital
    // Byte 25
    uint8_t power;                    ///< Power setting (default=0, i.e., global).
    // Byte 26
    uint8_t _unused26;                ///< Unused, set to 0.
    // Bytes 27-28
    uint8_t tot;                      ///< TOT in 15sec steps: 0=Infinite.
    uint8_t tot_rekey_delay;          ///< TOT Rekey Delay in seconds [0, 255]s
    // Byte 29
    uint8_t admit_criteria;           ///< Admit Criteria: Always, Channel Free or Color Code
    // Bytes 30-31
    uint8_t _unused30;                ///< Unknown set to @c 0x50.
    uint8_t scan_list_index;          ///< Scan List index: 0=None or index + 1.
    // Bytes 32-35
    uint16_t ctcss_dcs_receive;       ///< RX CTCSS/DCS setting, 4 digits BCD or 0xffff if disabled.
    uint16_t ctcss_dcs_transmit;      ///< TX CTCSS/DCS setting, 4 digits BCD or 0xffff if disabled.
    // Bytes 36-39
    uint8_t _unused36;                ///< Unused set to @c 0x00.
    uint8_t tx_signaling_syst;        ///< Tx Signaling System: Off, DTMF
    uint8_t _unused38;                ///< Unused set to @c 0x00.
    uint8_t rx_signaling_syst;        ///< Rx Signaling System: Off, DTMF
    // Bytes 40-43
    uint8_t _unused40;                ///< Unknown set to @c 0x16.
    uint8_t privacy_group;            ///< Privacy Group 0=None, 1=53474c39
    uint8_t colorcode_tx;             ///< TX Color Code [0,15].
    uint8_t group_list_index;         ///< Group List index 0=None or index+1.
    // Bytes 44-47
    uint8_t colorcode_rx;             ///< RX Color Code: [0,15] (usually identical to TX colorcode).
    uint8_t emergency_system_index;   ///< Emergency system index, 0=None or index + 1.
    uint16_t contact_name_index;      ///< Contact index, 0=None or index+1.
    // Byte 48
    uint8_t _unused48          : 6,   ///< Unused set to @c 0x00.
      emergency_alarm_ack      : 1,   ///< Emergency alarm ack flag.
      data_call_conf           : 1;   ///< Data-call confirmed flag.
    // Byte 49
    uint8_t private_call_conf  : 1,   ///< Private call confirmed flag.
      _unused49_1              : 3,   ///< Unused set to @c 0b000.
      privacy                  : 1,   ///< Privacy enabled flag.
      _unused49_5              : 1,   ///< Unused set to @c 0b0.
      repeater_slot2           : 1,   ///< Repeater Slot 0=slot1 or 1=slot2.
      _unused49_7              : 1;   ///< Unused set to @c 0b0.
    // Byte 50
    uint8_t dcdm               : 1,   ///< Dual capacity direct mode flag (do not use it).
      _unused50_1              : 4,   ///< Unused set to 0b0000.
      non_ste_frequency        : 1,   ///< Non STE = Frequency?
      _unused50_6              : 2;   ///< Unused set to 0b00
    // Byte 51
    uint8_t squelch            : 1,   ///< Squelch settings (tight or normal).
      bandwidth                : 1,   ///< Bandwidth 12.5 or 25 kHz.
      rx_only                  : 1,   ///< RX only flag.
      talkaround               : 1,   ///< Allow talkaround flag.
      _unused51_4              : 2,   ///< Unused set to 0b00.
      vox                      : 1,   ///< VOX enable flag.
      unused51_7               : 1;   ///< Is power flag in original GD77, unused here.
    // Bytes 52-55
    uint32_t _unused52;               ///< Unused set to 0.

    /** Returns @c true if the channel is valid. */
    bool isValid() const;
    /** Clears the channel settings. */
    void clear();
    /** Returns the RX frequency in MHz. */
    double getRXFrequency() const;
    /** Sets the RX frequency in MHz. */
    void setRXFrequency(double f);
    /** Returns the TX frequency in MHz. */
    double getTXFrequency() const;
    /** Sets the TX frequency in MHz. */
    void setTXFrequency(double f);
    /** Returns the channel name. */
    QString getName() const;
    /** Sets the channel name. */
    void setName(const QString &name);
    /** Returns the CTCSS RX tone. */
    Signaling::Code getRXTone() const;
    /** Sets the CTCSS RX tone. */
    void setRXTone(Signaling::Code tone);
    /** Returns the CTCSS TX tone. */
    Signaling::Code getTXTone() const;
    /** Sets the CTCSS TX tone. */
    void setTXTone(Signaling::Code tone);

    /** Constructs a @c Channel object from this codeplug channel. */
    Channel *toChannelObj() const;
    /** Resets this codeplug channel from the given @c Channel object. */
    void fromChannelObj(const Channel *c, const Config *conf);
    /** Links a previously constructed @c Channel object to other object within the generic
     * configuration, for example scan lists etc. */
    bool linkChannelObj(Channel *c, const CodeplugContext &ctx) const;
  } channel_t;

  /** A Bank of 128 channels. */
  typedef struct __attribute__((packed)) {
    uint8_t bitmap[16];               ///< Corresponding bit is set when channel is valid.
    channel_t chan[128];              ///< The list of channels.
  } bank_t;

  /** Represents a single zone within the codeplug. */
  struct __attribute__((packed)) zone_t {
    // Bytes 0-15
    uint8_t name[16];                   ///< Zone name ASCII, 0xff terminated.
    // Bytes 16-47
    uint16_t member[80];                ///< Member channel indices+1, 0=empty/not used.

    /** Constructor. */
    zone_t();

    /** Returns @c true if the zone entry is valid. */
    bool isValid() const;
    /** Resets and invalidates the zone entry. */
    void clear();
    /** Retruns the zone name. */
    QString getName() const;
    /** Sets the zone name. */
    void setName(const QString &name);

    /** Constructs a generic @c Zone object from this codeplug zone. */
    Zone *toZoneObj() const;
    /** Links a previously constructed @c Zone object to the rest of the configuration. That is
     * linking to the referred channels. */
    bool linkZoneObj(Zone *zone, const CodeplugContext &ctx) const;
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    void fromZoneObjA(const Zone *zone, const Config *conf);
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    void fromZoneObjB(const Zone *zone, const Config *conf);
  };

  /** Table of zones. */
  struct __attribute__((packed)) zonetab_t {
    /** A bit representing the validity of every zone. If a bit is set, the corresponding zone in
     * @c zone ist valid. */
    uint8_t bitmap[32];
    /** The list of zones. */
    zone_t  zone[NZONES];
  };

  /** Specific codeplug representation of a DMR contact.
   *
   * Memmory layout of the contact:
   * @verbinclude opengd77contact.txt
   */
  struct __attribute__((packed)) contact_t {
    /** Possible call types. */
    typedef enum {
      CALL_GROUP   = 0,                 ///< A group call.
      CALL_PRIVATE = 1,                 ///< A private call.
      CALL_ALL     = 2                  ///< An all-call.
    } CallType;

    // Bytes 0-15
    uint8_t name[16];                   ///< Contact name in ASCII, 0xff terminated.
    // Bytes 16-19
    uint8_t id[4];                      ///< BCD coded 8 digits DMR ID.
    // Byte 20
    uint8_t type;                       ///< Call Type, one of Group Call, Private Call or All Call.
    // Bytes 21-23
    uint8_t receive_tone;               ///< Call Receive Tone, 0=Off, 1=On.
    uint8_t ring_style;                 ///< Ring style: [0,10]
    uint8_t timeslot_override;          ///< Time-slot override, 0x00=Off, 0x01=TS1, 0x02=TS2.

    /** Constructor. */
    contact_t();

    /** Resets an invalidates the contact entry. */
    void clear();
    /** Returns @c true, if the contact is valid. */
    bool isValid() const;
    /** Returns the DMR ID of the contact. */
    uint32_t getId() const;
    /** Sets the DMR ID of the contact. */
    void setId(uint32_t num);
    /** Returns the name of the contact. */
    QString getName() const;
    /** Sets the name of the contact. */
    void setName(const QString &name);

    /** Constructs a @c DigitalContact instance from this codeplug contact. */
    DigitalContact *toContactObj() const;
    /** Resets this codeplug contact from the given @c DigitalContact. */
    void fromContactObj(const DigitalContact *obj, const Config *conf);
  };

public:
  /** Constructs an empty codeplug for the GD-77. */
  explicit OpenGD77Codeplug(QObject *parent=nullptr);

	/** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, bool update=true);
};

#endif // OPENGD77_CODEPLUG_HH
