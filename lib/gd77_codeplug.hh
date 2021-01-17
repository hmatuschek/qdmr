#ifndef GD77_CODEPLUG_HH
#define GD77_CODEPLUG_HH

#include "codeplug.hh"
#include "rd5r_codeplug.hh"
#include "signaling.hh"
#include "codeplugcontext.hh"


/** Represents, encodes and decodes the device specific codeplug for a Radioddity GD-77.
 *
 * The GD-77 & GD-77S codeplugs are almost identical to the Radioddity/Baofeng @c RD5RCodeplug, in fact
 * the memory layout (see below) and almost all of the single components of the codeplug are encoded in
 * exactly the same way. Obviously, when Baofeng and Radioddity joint to create the RD5R,
 * Radioddity provided the firmware. However, there are some small subtile differences between
 * these two codeplug formats, requireing a separate class for the GD-77. For example, the contacts
 * and scan-lists swapped the addresses and the @c channel_t encoding analog and digital channels
 * for the codeplugs are identical except for the squelch settings. Thanks for that!
 *
 * @section gd77cpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is divided into two segments.
 * The first segment starts at the address 0x00080 and ends at 0x07c00 while the second section
 * starts at 0x08000 and ends at 0x1e300.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x00080-0x07c00</th></tr>
 *  <tr><td>0x00080</td> <td>0x00088</td> <td>0x0008</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00088</td> <td>0x0008e</td> <td>0x0006</td> <td>Timestamp, see @c GD77Codeplug::timestamp_t.</td></tr>
 *  <tr><td>0x0008e</td> <td>0x000e0</td> <td>0x0052</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x000e0</td> <td>0x000ec</td> <td>0x000c</td> <td>General settings, see @c GD77Codeplug::general_settings_t.</td></tr>
 *  <tr><td>0x000ec</td> <td>0x00128</td> <td>0x003c</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 message texts, see @c GD77Codeplug::msgtab_t</td></tr>
 *  <tr><td>0x01370</td> <td>0x01790</td> <td>0x0420</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01790</td> <td>0x02dd0</td> <td>0x1640</td> <td>64 scan lists, see @c GD77Codeplug::scanlist_t</td></tr>
 *  <tr><td>0x02dd0</td> <td>0x03780</td> <td>0x09b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 chanels (bank 0), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><td>0x05390</td> <td>0x07540</td> <td>0x21b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c GD77Codeplug::intro_text_t</td></tr>
 *  <tr><td>0x07560</td> <td>0x07c00</td> <td>0x06a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second segment 0x08000-0x1e300</th></tr>
 *  <tr><td>0x08000</td> <td>0x08010</td> <td>0x0010</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x0af10</td> <td>0x2f00</td> <td>68 zones of 80 channels each, see @c GD77Codeplug::zonetab_t</td></tr>
 *  <tr><td>0x0af10</td> <td>0x0b1b0</td> <td>0x02a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x0b1b0</td> <td>0x17620</td> <td>0xc470</td> <td>Remaining 896 chanels (bank 1-7), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><td>0x17620</td> <td>0x1d620</td> <td>0x6000</td> <td>1024 contacts, see @c GD77Codeplug::contact_t.</td></tr>
 *  <tr><td>0x1d620</td> <td>0x1e2a0</td> <td>0x0c80</td> <td>64 RX group lists, see @c GD77Codeplug::grouptab_t</td></tr>
 *  <tr><td>0x1e2a0</td> <td>0x1e300</td> <td>0x0060</td> <td>??? Unknown ???</td></tr>
 * </table>
 * @ingroup gd77 */
class GD77Codeplug: public CodePlug
{
	Q_OBJECT

protected:
	static const int NCHAN     = 1024;  ///< The number of channels.
  static const int NCONTACTS = 1024;  ///< The number of contacts.
  static const int NZONES    = 250;   ///< The number of zones.
	static const int NGLISTS   = 76;    ///< The number of RX group-lists.
	static const int NSCANL    = 64;    ///< The number of scan-lists.
	static const int NMESSAGES = 32;    ///< The number of predefined text messages.

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

    /** Possible power settings. */
		typedef enum {
			POWER_HIGH = 1,                 ///< High power = 5W.
			POWER_LOW  = 0                  ///< Low power = 1W.
		} Power;

		// Bytes 0-15
		uint8_t name[16];                 ///< Channel Name
		// Bytes 16-23
		uint32_t rx_frequency;            ///< RX Frequency: 8 digits BCD
		uint32_t tx_frequency;            ///< TX Frequency: 8 digits BCD
		// Byte 24
		uint8_t channel_mode;             ///< Mode: Analog or Digital
		// Bytes 25-26
		uint16_t _unused25;               ///< Unused, set to 0.
		// Bytes 27-28
		uint8_t tot;                      ///< TOT in 15sec steps: 0=Infinite.
		uint8_t tot_rekey_delay;          ///< TOT Rekey Delay in seconds [0, 255]s
		// Byte 29
		uint8_t admit_criteria;           ///< Admit Criteria: Always, Channel Free or Color Code
		// Bytes 30-31
		uint8_t _unused30;                ///< Unused, set to @c 0x50.
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
		  power                    : 1;   ///< Power either Low or High.
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

  /** Specific codeplug representation of a DMR contact.
   *
   * Memmory layout of the contact:
   * @verbinclude gd77contact.txt
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
    uint8_t _unknown23;                 ///< Usually 0xff if valid, 0x00 otherwise. For OpenGD77
                                        ///< unknown flags.

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

  /** Represents a single zone within the codeplug. This representation is identical to the
   * zone representation within the RD-5R codeplug. Hence, it gets reused. */
  typedef  RD5RCodeplug::zone_t zone_t;
  /** Table of zones. This representation is identical to the zone table within the RD-5R codeplug.
   * Hence, it gets reused. */
  typedef RD5RCodeplug::zonetab_t zonetab_t;

	/** Represents an RX group list within the codeplug. */
	typedef struct __attribute__((packed)) {
    // Bytes 0-15
    uint8_t name[16];                 ///< RX group list name, 16x ASCII, 0xff terminated.
    // Bytes 16-79
    uint16_t member[32];              ///< Contact indices, 0=not used or index + 1.

    /** Returns the name of the group list. */
    QString getName() const;
    /** Sets the name of the group list. */
    void setName(const QString &name);

    /** Constructs a @c RXGroupList object from the codeplug representation. */
    RXGroupList *toRXGroupListObj();
    /** Links a previously constructed @c RXGroupList to the rest of the generic configuration. */
    bool linkRXGroupListObj(RXGroupList *lst, const CodeplugContext &ctx) const;
    /** Reset this codeplug representation from a @c RXGroupList object. */
    void fromRXGroupListObj(const RXGroupList *lst, const Config *conf);
	} grouplist_t;

	/** Table of RX group lists. */
	typedef struct __attribute__((packed)) {
    uint8_t     nitems1[128];         ///< Number of members (N+1) for every group list, zero when disabled.
    grouplist_t grouplist[NGLISTS];   ///< The actual grouplists.
	} grouptab_t;

  /** Represents a sinle scan list within the codeplug. */
  struct __attribute__((packed)) scanlist_t {
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
    uint8_t _unused       : 4,    ///< Unknown set to 1.
      channel_mark        : 1,    ///< Channel mark, default 1.
      pl_type             : 2,    ///< PL type, default 3.
      talkback            : 1;    ///< Talkback, default 1.
    // Bytes 16-79
    uint16_t member[32];          ///< Channel indices, 0=not used/EOL or channel index+2.

    // Bytes 80-85
    uint16_t priority_ch1;        ///< Priority channel 1 index, index+2 or 0=None, 1=selected.
    uint16_t priority_ch2;        ///< Priority channel 2 index, index+2 or 0=None, 1=selected.
    uint16_t tx_designated_ch;    ///< Designated TX channel, channel index+1 or 0=last active channel.

    // Bytes 86-87
    uint8_t sign_hold_time;       ///< Signaling Hold Time (x25 = msec) default 40=1000ms.
    uint8_t prio_sample_time;     ///< Priority Sample Time (x250 = msec) default 8=2000ms.

    /** Constructor. */
    scanlist_t();

    /** Resets the scan list. */
    void clear();
    /** Returns the name of the scan list. */
    QString getName() const;
    /** Sets the name of the scan list. */
    void setName(const QString &name);

    /** Constrcuts a @c ScanList object from this codeplug representation. */
    ScanList *toScanListObj() const;
    /** Links a previously constructed @c ScanList object to the rest of the generic configuration. */
    bool linkScanListObj(ScanList *lst, const CodeplugContext &ctx) const;
    /** Initializes this codeplug representation from the given @c ScanList object. */
    void fromScanListObj(const ScanList *lst, const Config *conf);
  };

  /** Table/Bank of scanlists. */
  struct __attribute__((packed)) scantab_t {
    /** Byte-field to indicate which scanlist is valid. Set to 0x01 when valid, 0x00 otherwise. */
    uint8_t    valid[NSCANL];
    /** The scanlists. */
    scanlist_t scanlist[NSCANL];
  };

	/** Represents the general settings within the codeplug. This representation is identical to
	 * the general settings of the RD-5R codeplug. Hence, it gets reused here. */
	typedef RD5RCodeplug::general_settings_t general_settings_t;

  /** Represents the intro lines within the codeplug. This representation is identical to
	 * the intro lines of the RD-5R codeplug. Hence, it gets reused here. */
	typedef RD5RCodeplug::intro_text_t intro_text_t;

  /** Represents the table of preset messages within the codeplug. This representation is identical to
	 * the table of messages of the RD-5R codeplug. Hence, it gets reused here. */
	typedef RD5RCodeplug::msgtab_t msgtab_t;

  /** Represents the timestamp within the codeplug. This representation is identical to
	 * the timestamp of the RD-5R codeplug. Hence, it gets reused here. */
	typedef RD5RCodeplug::timestamp_t timestamp_t;

public:
  /** Constructs an empty codeplug for the GD-77. */
	explicit GD77Codeplug(QObject *parent=nullptr);

	/** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, bool update=true);
};

#endif // GD77_CODEPLUG_HH
