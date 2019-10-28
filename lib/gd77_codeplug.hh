#ifndef GD77_CODEPLUG_HH
#define GD77_CODEPLUG_HH

#include "codeplug.hh"


/** Represents, encodes and decodes the device specific codeplug for a Radioddity GD-77.
 *
 * The GD-77 & GD-77S codeplugs are almost identical to the Radioddity @c RD5RCodeplug, in fact
 * the memory layout (see below) and almost all of the single parts of the codeplug are encoded in
 * exactly the same way. Obviously, when Baofeng and Radioddity joint to create the RD5R,
 * Radioddity provided the firmware. However, there are some small subtile differences between
 * these two codeplug formats, requireing a separate class for the GD-77. For example the
 * @c channel_t encoding analog and digital channels for the codeplug are identical except for the
 * squelch settings. Thanks for that!
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
 * @ingroup gd77 */
class GD77Codeplug: public CodePlug
{
	Q_OBJECT

protected:
	static const int NCHAN     = 1024;
  static const int NCONTACTS = 1024;
	static const int NZONES    = 250;
	static const int NGLISTS   = 76;
	static const int NSCANL    = 64;
	static const int NMESSAGES = 32;

	/** Channel representation within the binary codeplug. */
	typedef struct {
		typedef enum {
			MODE_ANALOG  = 0,
			MODE_DIGITAL = 1
		} Mode;

		typedef enum {
			ADMIT_ALWAYS  = 0,
			ADMIT_CH_FREE = 1,
			ADMIT_COLOR   = 2
		} Admit;

		typedef enum {
			PRIVGR_NONE     = 0,
			PRIVGR_53474C39 = 1
		} PrivGroup;

		typedef enum {
			SQ_TIGHT  = 0,
			SQ_NORMAL = 1
		} SquelchType;

		typedef enum {
			BW_12_5_KHZ = 0,
			BW_25_KHZ   = 1
		} Bandwidth;

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
		uint8_t tot;                      ///< TOT x 15sec: 0-Infinite, 1=15s... 33=495s, default=0
		uint8_t tot_rekey_delay;          ///< TOT Rekey Delay: 0s...255s
		// Byte 29
		uint8_t admit_criteria;           ///< Admit Criteria: Always, Channel Free or Color Code
		// Bytes 30-31
		uint8_t _unused30;                ///< Unused, set to @c 0x50.
		uint8_t scan_list_index;          ///< Scan List index + 1: 0=None, [1,250].
		// Bytes 32-35
		uint16_t ctcss_dcs_receive;       // CTCSS/DCS Dec: 4 digits BCD or 0xffff
		uint16_t ctcss_dcs_transmit;      // CTCSS/DCS Enc: 4 digits BCD
		// Bytes 36-39
		uint8_t _unused36;                // 0
		uint8_t tx_signaling_syst;        // Tx Signaling System: Off, DTMF
		uint8_t _unused38;                // 0
		uint8_t rx_signaling_syst;        // Rx Signaling System: Off, DTMF
		// Bytes 40-43
		uint8_t _unused40;                // 0x16
		uint8_t privacy_group;            // Privacy Group: 0=None, 1=53474c39
		uint8_t colorcode_tx;             // Color Code: 0...15
		uint8_t group_list_index;         // Group List: None, GroupList1...128
		// Bytes 44-47
		uint8_t colorcode_rx;             // Color Code: 0...15
		uint8_t emergency_system_index;   // Emergency System: None, System1...32
		uint16_t contact_name_index;      // Contact Name: Contact1...
		// Byte 48
		uint8_t _unused48          : 6,   // 0
		  emergency_alarm_ack      : 1,   // Emergency Alarm Ack
		  data_call_conf           : 1;   // Data Call Confirmed
		// Byte 49
		uint8_t private_call_conf  : 1,   // Private Call Confirmed
		  _unused49_1              : 3,   // 0
		  privacy                  : 1,   // Privacy: Off or On
		  _unused49_5              : 1,   // 0
		  repeater_slot2           : 1,   // Repeater Slot: 0=slot1 or 1=slot2
		  _unused49_7              : 1;   // 0
		// Byte 50
		uint8_t dcdm               : 1,   // Dual Capacity Direct Mode
		  _unused50_1              : 4,   // 0
		  non_ste_frequency        : 1,   // Non STE = Frequency
		  _unused50_6              : 2;   // 0
		// Byte 51
		uint8_t squelch            : 1,   // Squelch
		  bandwidth                : 1,   // Bandwidth: 12.5 or 25 kHz
		  rx_only                  : 1,   // RX Only Enable
		  talkaround               : 1,   // Allow Talkaround
		  _unused51_4              : 2,   // 0
		  vox                      : 1,   // VOX Enable
		  power                    : 1;   // Power: Low, High
    // Bytes 52-55
		uint8_t _unused52[4];             // 0
	} channel_t;

	//
	// Bank of 128 channels.
	//
	typedef struct {
		uint8_t bitmap[16];               // bit set when channel valid
		channel_t chan[128];
	} bank_t;

	//
	// Contact data.
	//
	typedef struct {
    typedef enum {
      CALL_GROUP   = 0,
      CALL_PRIVATE  = 1,
      CALL_ALL = 2
    } Type;

    // Bytes 0-15
    uint8_t name[16];                 // Contact Name, ff terminated
		// Bytes 16-19
		uint8_t id[4];                    // BCD coded 8 digits
    // Byte 20
    uint8_t type;                     // Call Type: Group Call, Private Call or All Call
    // Bytes 21-23
    uint8_t receive_tone;             // Call Receive Tone: 0=Off, 1=On
    uint8_t ring_style;               // Ring style: 0-10
    uint8_t _unused23;                // 0xff for used contact, 0 for blank entry

    uint32_t getId() const;
	} contact_t;

	//
	// Zone data.
	//
	typedef struct {
    // Bytes 0-15
    uint8_t name[16];                 // Zone Name

    // Bytes 16-47
    uint16_t member[16];              // Member: channels 1...16
	} zone_t;

	//
	// Table of zones.
	//
	typedef struct {
    uint8_t bitmap[32];               // bit set when zone valid
    zone_t  zone[NZONES];
	} zonetab_t;

	//
	// Group list data.
	//
	typedef struct {
    // Bytes 0-15
    uint8_t name[16];                 // Group List Name
    // Bytes 16-79
    uint16_t member[32];              // Contacts
	} grouplist_t;

	//
	// Table of group lists.
	//
	typedef struct {
    uint8_t     nitems1[128];         // N+1, zero when disabled
    grouplist_t grouplist[NGLISTS];   //
	} grouptab_t;

	//
	// Scan list data.
	//
	typedef struct {
    typedef enum {
      PL_NONPRI     = 0,              // Non-Priority Channel
      PL_DISABLE    = 1,              // Disable
      PL_PRI        = 2,              // Priority Channel
      PL_PRI_NONPRI = 3               // Priority and Non-Priority Channels
    } Type;

    // Bytes 0-14
    uint8_t name[15];                 // Scan List Name

    // Byte 15
    uint8_t _unused       : 4,        // 0
      channel_mark        : 1,        // Channel Mark, default 1
      pl_type             : 2,        // PL Type, default 3
      talkback            : 1;        // Talkback, default 1

    // Bytes 16-79
    uint16_t member[32];              // Channel indices +2, 0=not used, 1=selected

    // Bytes 80-85
    uint16_t priority_ch1;            // Priority Channel 1, 0=none, 1=selected or index+2.
    uint16_t priority_ch2;            // Priority Channel 2, 0=none, 1=selected or index+2.
    uint16_t tx_designated_ch;        // Tx Designated Channel, chan+2 or 0=Last Active Channel

    // Bytes 86-87
    uint8_t sign_hold_time;           // Signaling Hold Time (x25 = msec) default 40=1000ms
    uint8_t prio_sample_time;         // Priority Sample Time (x250 = msec) default 8=2000ms
	} scanlist_t;

	//
	// Table of scanlists.
	//
	typedef struct {
    uint8_t    valid[NSCANL];         // byte=1 when scanlist valid
    scanlist_t scanlist[NSCANL];      //
	} scantab_t;

	//
	// General settings.
	//
	typedef struct {
    // Bytes e0-e7
    uint8_t radio_name[8];            // Radio name 8 x ASCII 0xff terminated.

    // Bytes e8-eb
    uint8_t radio_id[4];              // RadioID
	} general_settings_t;

	//
	// Intro messages.
	//
	typedef struct {
    // Bytes 7540-754f
    uint8_t intro_line1[16];          // First intro line.
    // Bytes 7550-755f
    uint8_t intro_line2[16];          // Second intro line
	} intro_text_t;

	//
	// Table of text messages.
	//
	typedef struct {
    uint8_t count;                    // number of messages
    uint8_t _unused1[7];              // 0
    uint8_t len[NMESSAGES];           // message length
    uint8_t _unused2[NMESSAGES];      // 0
    uint8_t message[NMESSAGES*144];   // messages
  } msgtab_t;

public:
	explicit GD77Codeplug(QObject *parent=nullptr);

	/** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
	bool encode(Config *config);
};

#endif // GD77_CODEPLUG_HH
