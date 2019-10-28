#ifndef RD5R_CODEPLUG_HH
#define RD5R_CODEPLUG_HH

#include <QObject>
#include "codeplug.hh"

class Channel;
class Zone;
class ScanList;
class DigitalContact;
class RXGroupList;

/** Represents, encodes and decodes the device specific codeplug for a Baofeng/Radioddity RD-5R.
 *
 * This codeplug format is quiet funny. It reveals some history of this device. First of all, the
 * channels are organizes in two blocks. The first block contains only a single bank of 128 channels,
 * while the second block contains 7 banks with a total of 896 channels. I would guess there was a
 * previous firmware or even hardware version with only 128 channels.
 *
 * Moreover, channels, zones, rx group lists and scan lists are organized in tables or banks, with
 * some preceding bitfield indicating which channel is enabled/valid. Contacts, however, are just
 * organized in a list, where each entry has a field, indicating whether that contact is valid.
 *
 * This difference looks like, as if the firmware code for the contacts stems from a different
 * device or was developed by a different engineer. Moreover, the message list again, uses yet
 * another method. Here a simple counter preceds the messages, indicating how many valid messages
 * there are. All in all, a rather inconsistent way of representing variable length lists in the
 * codeplug. I would guess, that over time, different people/teams worked on different revisions
 * of the firmware. It must have been a real nightmare to Serge Vakulenko reverse-engineering this
 * codeplug.
 *
 * @section rd5rcpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is divided into two segments.
 * The first segment starts at the address 0x00080 and ends at 0x07c00 while the second section
 * starts at 0x08000 and ends at 0x1e300.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x00080-0x07c00</th></tr>
 *  <tr><td>0x00080</td> <td>0x00088</td> <td>0x0008</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00088</td> <td>0x0008e</td> <td>0x0006</td> <td>Timestamp, see @c RD5RCodeplug::timestamp_t.</td></tr>
 *  <tr><td>0x0008e</td> <td>0x000e0</td> <td>0x0052</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x000e0</td> <td>0x000ec</td> <td>0x000c</td> <td>General settings, see @c RD5RCodeplug::settings_t.</td></tr>
 *  <tr><td>0x000ec</td> <td>0x00128</td> <td>0x003c</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 message texts, see @c RD5RCodeplug::msgtab_t</td></tr>
 *  <tr><td>0x01370</td> <td>0x01788</td> <td>0x0418</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01788</td> <td>0x02f88</td> <td>0x1800</td> <td>256 contacts, see @c RD5RCodeplug::contact_t</td></tr>
 *  <tr><td>0x02f88</td> <td>0x03780</td> <td>0x07f8</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 chanels (bank 0), see @c RD5RCodeplug::bank_t</td></tr>
 *  <tr><td>0x05390</td> <td>0x07540</td> <td>0x21b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c RD5RCodeplug::intro_text_t</td></tr>
 *  <tr><td>0x07560</td> <td>0x07c00</td> <td>0x06a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second segment 0x08000-0x1e300</th></tr>
 *  <tr><td>0x08000</td> <td>0x08010</td> <td>0x0010</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x0af10</td> <td>0x2f00</td> <td>250 zones, see @c RD5RCodeplug::zonetab_t</td></tr>
 *  <tr><td>0x0af10</td> <td>0x0b1b0</td> <td>0x02a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x0b1b0</td> <td>0x17620</td> <td>0xc470</td> <td>Remaining 896 chanels (bank 1-7), see @c RD5RCodeplug::bank_t</td></tr>
 *  <tr><td>0x17620</td> <td>0x1cd10</td> <td>0x56f0</td> <td>250 scan lists, see @c RD5RCodeplug::scantab_t</td></tr>
 *  <tr><td>0x1cd10</td> <td>0x1d620</td> <td>0x0910</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x1d620</td> <td>0x1e2a0</td> <td>0x0c80</td> <td>64 RX group lists, see @c RD5RCodeplug::grouptab_t</td></tr>
 *  <tr><td>0x1e2a0</td> <td>0x1e300</td> <td>0x0060</td> <td>??? Unknown ???</td></tr>
 * </table>
 * @ingroup rd5r */
class RD5RCodeplug : public CodePlug
{
	Q_OBJECT

public:
  static const int NCHAN     = 1024;
  static const int NCONTACTS = 256;
  static const int NZONES    = 250;
  static const int NGLISTS   = 64;
  static const int NSCANL    = 250;
  static const int NMESSAGES = 32;

  /** Represents a configured channel within the codeplug. */
	typedef struct {
    /** Possible channel types. */
		typedef enum {
			MODE_ANALOG = 0,   ///< Analog channel, aka FM.
			MODE_DIGITAL = 1   ///< Digital channel, aka DMR.
		} Mode;

    /** Possible admit criteria. */
		typedef enum {
			ADMIT_ALWAYS = 0,  ///< Allow always.
			ADMIT_CH_FREE = 1, ///< Allow TX on channel free.
			ADMIT_COLOR = 2    ///< Allow TX on matching color-code.
		} Admit;

    /** Possible privacy groups, not used in ham radio. */
		typedef enum {
			PRIVGR_NONE = 0,     ///< No privacy group, default.
			PRIVGR_53474C39 = 1  ///< Privacy group 53474C39 (wtf?).
		} PrivacyGroup;

    /** Bandwidth of channel. */
		typedef enum {
			BW_12_5_KHZ = 0,     ///< 12.5kHz (default on DMR)
			BW_25_KHZ = 1        ///< 25kHz
		} Bandwidth;

    /** Transmit power for channel. */
		typedef enum {
			POWER_HIGH = 1,      ///< High power, 5W.
			POWER_LOW = 0        ///< Low power, 1W.
		} Power;

		// Bytes 0-15
		uint8_t name[16];                   ///< Channel Name 16 x ASCII 0xff terminated.
		// Bytes 16-23
		uint32_t rx_frequency;              ///< RX Frequency, 8 digits BCD.
		uint32_t tx_frequency;              ///< TX Frequency, 8 digits BCD.
		// Byte 24
		uint8_t channel_mode;               ///< Mode analog or digital.
		// Bytes 25-26
		uint16_t _unused25;               ///< Unknown set to 0.
		// Bytes 27-28
		uint8_t tot;                        ///< TOT x 15sec, [0,33], 0-Infinite.
		uint8_t tot_rekey_delay;            ///< TOT Rekey Delay in seconds [0,255].
		// Byte 29
		uint8_t admit_criteria;             ///< Admit Criteria, Always, Channel Free or Color Code.
		// Bytes 30-31
		uint8_t _unused30;                  ///< Unknown set to 0x50.
    uint8_t scan_list_index;            ///< Scan List index + 1, 0=None.
    // Bytes 32-35
    uint16_t ctcss_dcs_receive;         ///< CTCSS/DCS decode, 4 digits BCD or 0xffff=disabled.
    uint16_t ctcss_dcs_transmit;        ///< CTCSS/DCS encode, 4 digits BCD or 0xffff=disabled.
    // Bytes 36-39
    uint8_t _unused36;                  ///< Unkown set to 0.
    uint8_t tx_signaling_syst;          ///< TX signaling system index+1, 0=Off.
    uint8_t _unused38;                  ///< Unknown set to 0.
    uint8_t rx_signaling_syst;          ///< RX signaling system index+1, 0=Off.
    // Bytes 40-43
    uint8_t _unused40;                  ///< Unknown set to 0x16.
    uint8_t privacy_group;              ///< Privacy Group, 0=None, 1=53474c39.
    uint8_t colorcode_tx;               ///< Transmit color code [0,15].
    uint8_t group_list_index;           ///< RX group list index+1,  0=None.
    // Bytes 44-47
    uint8_t colorcode_rx;               ///< Receive color code [0,15]
    uint8_t emergency_system_index;     ///< Emergency system index+1, 0=None.
    uint16_t contact_name_index;        ///< Transmit contact index+1.
    // Byte 48
    uint8_t _unused48         : 6,      ///< Unknonw set to 0.
      emergency_alarm_ack     : 1,      ///< Emergency alarm ACK.
      data_call_conf          : 1;      ///< Data call confirmed.
    // Byte 49
    uint8_t private_call_conf : 1,      ///< Private call confirmed.
      _unused49_1             : 3,      ///< Unknown, set to 0.
      privacy                 : 1,      ///< Privacy: Off or On.
      _unused49_5             : 1,      ///< Unknown set to 0.
      repeater_slot2          : 1,      ///< Repeater time slot, 0=slot1 or 1=slot2.
      _unused49_7             : 1;      ///< Unknown set to 0.
		// Byte 50
		uint8_t dcdm              : 1,      ///< Dual capacity direct mode enable.
      _unused50_1             : 4,      ///< Unknown set to 0.
      non_ste_frequency       : 1,      ///< Non STE = Frequency
      _unused50_6             : 2;      ///< Unknown set to 0.
		// Byte 51
    uint8_t _unused51_0       : 1,      ///< Unknown set to 0.
      bandwidth               : 1,      ///< Bandwidth 12.5 or 25 kHz.
      rx_only                 : 1,      ///< RX only enable.
      talkaround              : 1,      ///< Allow talkaround.
      _unused51_4             : 2,      ///< Unknown 0.
      vox                     : 1,      ///< VOX Enable.
      power                   : 1;      ///< Power Low or High.
		// Bytes 52-55
    uint8_t _unused52[3];               ///< Unknown set to 0.
    uint8_t squelch;                    ///< Squelch level [0,9]

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
    float getRXTone() const;
    /** Sets the CTCSS RX tone. */
    void setRXTone(float tone);
    /** Returns the CTCSS TX tone. */
    float getTXTone() const;
    /** Sets the CTCSS TX tone. */
    void setTXTone(float tone);

    /** Constructs a @c Channel object from this codeplug channel. */
    Channel *toChannelObj() const;
    /** Resets this codeplug channel from the given @c Channel object. */
    void fromChannelObj(const Channel *c, const Config *conf);
    /** Links a previously constructed @c Channel object to other object within the generic
     * configuration, for example scan lists etc. */
    bool linkChannelObj(Channel *c, Config *conf) const;
	} channel_t;

	/** Bank of 128 channels including a bitmap for enabled channels. */
	typedef struct {
    /** Bitmap for 128 channels, a bit is set when the coresspondig channel is valid/enabled. */
		uint8_t bitmap[16];
    /** The actual 128 channels. */
		channel_t chan[128];
	} bank_t;

	/** Specific codeplug representation of a DMR contact. */
	typedef struct {
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
		uint8_t is_valid;                   ///< 0xff for used contact, 0x00 for blank entry.

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
	} contact_t;

	/** Represents a single zone within the codeplug. */
	typedef struct {
		// Bytes 0-15
		uint8_t name[16];                   ///< Zone name ASCII, 0xff terminated.
		// Bytes 16-47
		uint16_t member[16];                ///< Member channel indices+1, 0=empty/not used.

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
    bool linkZoneObj(Zone *zone, const Config *conf) const;
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    void fromZoneObjA(const Zone *zone, const Config *conf);
    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    void fromZoneObjB(const Zone *zone, const Config *conf);
	} zone_t;

	/** Table of zones. */
	typedef struct {
    /** A bit representing the validity of every zone. If a bit is set, the corresponding zone in
     * @c zone ist valid. */
    uint8_t bitmap[32];
    /** The list of zones. */
    zone_t  zone[NZONES];
	} zonetab_t;

	/** Represents a single RX group list within the codeplug. */
	typedef struct {
    // Bytes 0-15
    uint8_t name[16];                   ///< RX group list name, ASCII, 0xff terminated.
    // Bytes 16-47
    uint16_t member[16];                ///< Contact indices + 1, 0=empty.

    /** Returns the name of the group list. */
    QString getName() const;
    /** Sets the name of the group list. */
    void setName(const QString &name);

    /** Constructs a @c RXGroupList object from the codeplug representation. */
    RXGroupList *toRXGroupListObj();
    /** Links a previously constructed @c RXGroupList to the rest of the generic configuration. */
    bool linkRXGroupListObj(RXGroupList *lst, const Config *conf) const;
    /** Reset this codeplug representation from a @c RXGroupList object. */
    void fromRXGroupListObj(const RXGroupList *lst, const Config *conf);
	} grouplist_t;

	/** Table of rx group lists. */
	typedef struct {
    /** Specifies the number of contacts +1 in each group list, 0=disabled. */
    uint8_t     nitems1[128];
    /** The list of RX group lists. */
    grouplist_t grouplist[NGLISTS];
	} grouptab_t;

	/** Represents a sinle scan list within the codeplug. */
	typedef struct {
    /** Possible priority channel types. */
		typedef enum {
			PL_NONPRI = 0,              ///< Only non-priority channels.
			PL_DISABLE = 1,             ///< Disable priority channels.
			PL_PRI = 2,                 ///< Only priority channels.
			PL_PRI_NONPRI = 3           ///< Priority and non-priority channels.
		} PriorityType;

    // Bytes 0-14
    uint8_t name[15];             ///< Scan list name, ASCII, 0xff terminated.
    // Byte 15
    uint8_t _unused       : 4,    ///< Unknown set to 0.
      channel_mark        : 1,    ///< Channel mark, default 1.
      pl_type             : 2,    ///< PL type, default 3.
      talkback            : 1;    ///< Talkback, default 1.

    // Bytes 16-79
    uint16_t member[32];          ///< Channel indices +1.

    // Bytes 80-85
    uint16_t priority_ch1;        ///< Priority channel 1 index, index+2 or 0=None, 1=selected.
    uint16_t priority_ch2;        ///< Priority channel 2 index, index+2 or 0=None, 1=selected.
    uint16_t tx_designated_ch;    ///< Designated TX channel, channel index +1 or 0=last active channel.

    // Bytes 86-87
    uint8_t sign_hold_time;       ///< Signaling Hold Time (x25 = msec) default 40=1000ms.
    uint8_t prio_sample_time;     ///< Priority Sample Time (x250 = msec) default 8=2000ms.

    /** Resets the scan list. */
    void clear();
    /** Returns the name of the scan list. */
    QString getName() const;
    /** Sets the name of the scan list. */
    void setName(const QString &name);

    /** Constrcuts a @c ScanList object from this codeplug representation. */
    ScanList *toScanListObj() const;
    /** Links a previously constructed @c ScanList object to the rest of the generic configuration. */
    bool linkScanListObj(ScanList *lst, const Config *conf) const;
    /** Initializes this codeplug representation from the given @c ScanList object. */
    void fromScanListObj(const ScanList *lst, const Config *conf);
  } scanlist_t;

	/** Table/Bank of scanlists. */
  typedef struct {
    /** Byte-field to indicate which scanlist is valid. Set to 0x01 when valid, 0x00 otherwise. */
    uint8_t    valid[256];
    /** The scanlists. */
    scanlist_t scanlist[NSCANL];
	} scantab_t;

	/** Represents the general settings within the codeplug. */
	typedef struct {
    // Bytes e0-e7
    uint8_t radio_name[8];        ///< The radio name in ASCII, 0xff terminated.
    // Bytes e8-eb
    uint8_t radio_id[4];          ///< The radio DMR ID in BCD.

    /** Returns the name of the radio. */
    QString getName() const;
    /** Sets the name of the radio. */
    void setName(const QString &n);

    /** Returns the DMR ID of the radio. */
    uint32_t getRadioId() const;
    /** Sets the DMR ID of the radio. */
    void setRadioId(uint32_t num);
	} general_settings_t;

	/** Represents the intro messages within the codeplug. */
	typedef struct {
    // Bytes 7540-754f
    uint8_t intro_line1[16];       ///< The first intro line, ASCII, 0xff terminated.
    // Bytes 7550-755f
    uint8_t intro_line2[16];       ///< The second intro line, ASCII, 0xff terminated.

    /** Returns the first intro line. */
    QString getIntroLine1() const;
    /** Set the first intro line. */
    void setIntroLine1(const QString &text);
    /** Returns the second intro line. */
    QString getIntroLine2() const;
    /** Sets the second intro line. */
    void setIntroLine2(const QString &text);
	} intro_text_t;

	/** Represents the table of text messages within the codeplug. */
	typedef struct {
    uint8_t count;                      ///< Number of messages.
    uint8_t _unused1[7];                ///< Unknown, set to 0.
    uint8_t len[NMESSAGES];             ///< Message lengths.
    uint8_t _unused2[NMESSAGES];        ///< Unknown, set to 0.
    uint8_t message[NMESSAGES][144];    ///< The actual messages, ASCII

    /** Clears all messages. */
    void clear();
    /** Gets a particular message. */
    QString getMessage(int i) const;
    /** Adds particular message to the list. */
    bool addMessage(const QString &text);
  } msgtab_t;

  /** Represents the codeplug time-stamp within the codeplug. */
  typedef struct {
    uint16_t year_bcd;                  ///< The year in 4 x BCD
    uint8_t month_bcd;                  ///< The month in 2 x BCD
    uint8_t day_bcd;                    ///< The day in 2 x BCD.
    uint8_t hour_bcd;                   ///< The hour in 2 x BCD.
    uint8_t minute_bcd;                 ///< The minute in 2 x BCD.

    /** Returns the timestamp. */
    QDateTime get() const;
    /** Set the time-stamp to now. */
    void setNow();
    /** Set the time-stamp to the given date and time. */
    void set(const QDateTime &dt);
  } timestamp_t;

public:
  /** Empty constructor. */
	RD5RCodeplug(QObject *parent=0);

  /** Decodes the read codeplug and stores the result into the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration into this codeplug. */
	bool encode(Config *config);
};

#endif // RD5R_CODEPLUG_HH
