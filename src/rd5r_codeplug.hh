#ifndef RD5R_CODEPLUG_HH
#define RD5R_CODEPLUG_HH

#include <QObject>
#include "codeplug.hh"

#define NCHAN           1024
#define NCONTACTS       256
#define NZONES          250
#define NGLISTS         64
#define NSCANL          250
#define NMESSAGES       32

class Channel;
class Zone;
class ScanList;
class DigitalContact;
class RXGroupList;


class RD5RCodePlug : public CodePlug
{
	Q_OBJECT

public:
	typedef struct {
		// Bytes 0-15
		uint8_t name[16];                   // Channel Name
		// Bytes 16-23
		uint32_t rx_frequency;              // RX Frequency: 8 digits BCD
		uint32_t tx_frequency;              // TX Frequency: 8 digits BCD
		// Byte 24
		uint8_t channel_mode;               // Mode: Analog or Digital
#define MODE_ANALOG     0
#define MODE_DIGITAL    1
		// Bytes 25-26
		uint8_t _unused25[2];               // 0
		// Bytes 27-28
		uint8_t tot;                        // TOT x 15sec: 0-Infinite, 1=15s... 33=495s
		uint8_t tot_rekey_delay;            // TOT Rekey Delay: 0s...255s
		// Byte 29
		uint8_t admit_criteria;             // Admit Criteria: Always, Channel Free or Color Code
#define ADMIT_ALWAYS    0
#define ADMIT_CH_FREE   1
#define ADMIT_COLOR     2
		// Bytes 30-31
		uint8_t _unused30;                  // 0x50
	    uint8_t scan_list_index;            // Scan List: None, ScanList1...250
	    // Bytes 32-35
	    uint16_t ctcss_dcs_receive;         // CTCSS/DCS Dec: 4 digits BCD or 0xffff
	    uint16_t ctcss_dcs_transmit;        // CTCSS/DCS Enc: 4 digits BCD
	    // Bytes 36-39
	    uint8_t _unused36;                  // 0
	    uint8_t tx_signaling_syst;          // Tx Signaling System: Off, DTMF
	    uint8_t _unused38;                  // 0
	    uint8_t rx_signaling_syst;          // Rx Signaling System: Off, DTMF
	    // Bytes 40-43
	    uint8_t _unused40;                  // 0x16
	    uint8_t privacy_group;              // Privacy Group: 0=None, 1=53474c39
#define PRIVGR_NONE     0
#define PRIVGR_53474C39 1
	    uint8_t colorcode_tx;               // Color Code: 0...15
	    uint8_t group_list_index;           // Group List: None, GroupList1...128
	    // Bytes 44-47
	    uint8_t colorcode_rx;               // Color Code: 0...15
	    uint8_t emergency_system_index;     // Emergency System: None, System1...32
	    uint16_t contact_name_index;        // Contact Name: Contact1...
	    // Byte 48
		uint8_t _unused48           : 6,    // 0
		    emergency_alarm_ack : 1,    // Emergency Alarm Ack
		    data_call_conf      : 1;    // Data Call Confirmed

	    // Byte 49
		uint8_t private_call_conf   : 1,    // Private Call Confirmed
		    _unused49_1         : 3,    // 0
		    privacy             : 1,    // Privacy: Off or On
		    _unused49_5         : 1,    // 0
		    repeater_slot2      : 1,    // Repeater Slot: 0=slot1 or 1=slot2
		    _unused49_7         : 1;    // 0

		// Byte 50
		uint8_t dcdm                : 1,    // Dual Capacity Direct Mode
		    _unused50_1         : 4,    // 0
		    non_ste_frequency   : 1,    // Non STE = Frequency
		    _unused50_6         : 2;    // 0

		// Byte 51
		uint8_t _unused51_0         : 1,    // 0
		    bandwidth           : 1,    // Bandwidth: 12.5 or 25 kHz
    #define BW_12_5_KHZ     0
    #define BW_25_KHZ       1
		    rx_only             : 1,    // RX Only Enable
		    talkaround          : 1,    // Allow Talkaround
		    _unused51_4         : 2,    // 0
		    vox                 : 1,    // VOX Enable
		    power               : 1;    // Power: Low, High
#define POWER_HIGH      1
#define POWER_LOW       0

		// Bytes 52-55
		uint8_t _unused52[3];               // 0
		uint8_t squelch;                    // Squelch: 0...9

	} channel_t;

	//
	// Bank of 128 channels.
	//
	typedef struct {
		uint8_t bitmap[16];                 // bit set when channel valid
		channel_t chan[128];
	} bank_t;

	//
	// Contact data.
	//
	typedef struct {
	    // Bytes 0-15
	    uint8_t name[16];                   // Contact Name, ff terminated

	    // Bytes 16-19
	    uint8_t id[4];                      // BCD coded 8 digits
#define GET_ID(x) (((x)[0] >> 4) * 10000000 +\
	((x)[0] & 15) * 1000000 +\
	((x)[1] >> 4) * 100000 +\
	((x)[1] & 15) * 10000 +\
	((x)[2] >> 4) * 1000 +\
	((x)[2] & 15) * 100 +\
	((x)[3] >> 4) * 10 +\
	((x)[3] & 15))
#define CONTACT_ID(ct) GET_ID((ct)->id)

		// Byte 20
		uint8_t type;                       // Call Type: Group Call, Private Call or All Call
#define CALL_GROUP      0
#define CALL_PRIVATE    1
#define CALL_ALL        2

		// Bytes 21-23
		uint8_t receive_tone;               // Call Receive Tone: 0=Off, 1=On
		uint8_t ring_style;                 // Ring style: 0-10
		uint8_t _unused23;                  // 0xff for used contact, 0 for blank entry

	} contact_t;

	//
	// Zone data.
	//
	typedef struct {
		// Bytes 0-15
		uint8_t name[16];                   // Zone Name

		// Bytes 16-47
		uint16_t member[16];                // Member: channels 1...16
	} zone_t;

	//
	// Table of zones.
	//
	typedef struct {
	    uint8_t bitmap[32];                 // bit set when zone valid
	    zone_t  zone[NZONES];
	} zonetab_t;

	//
	// Group list data.
	//
	typedef struct {
	    // Bytes 0-15
	    uint8_t name[16];                   // Group List Name

	    // Bytes 16-47
	    uint16_t member[16];                // Contacts
	} grouplist_t;

	//
	// Table of group lists.
	//
	typedef struct {
	    uint8_t     nitems1[128];           // N+1, zero when disabled
	    grouplist_t grouplist[NGLISTS];
	} grouptab_t;

	//
	// Scan list data.
	//
	typedef struct {
	    // Bytes 0-14
	    uint8_t name[15];                   // Scan List Name
	    // Byte 15
	    uint8_t _unused             : 4,    // 0
		    channel_mark        : 1,    // Channel Mark, default 1
		    pl_type             : 2,    // PL Type, default 3
    #define PL_NONPRI       0               // Non-Priority Channel
	#define PL_DISABLE      1               // Disable
    #define PL_PRI          2               // Priority Channel
    #define PL_PRI_NONPRI   3               // Priority and Non-Priority Channels
		    talkback            : 1;    // Talkback, default 1

		// Bytes 16-79
		uint16_t member[32];                // Channels (+1)
#define CHAN_SELECTED   1               // Selected

	    // Bytes 80-85
	    uint16_t priority_ch1;              // Priority Channel 1, chan+1 or 0=None, 1=Selected
	    uint16_t priority_ch2;              // Priority Channel 2
	    uint16_t tx_designated_ch;          // Tx Designated Channel, chan+1 or 0=Last Active Channel

	    // Bytes 86-87
	    uint8_t sign_hold_time;             // Signaling Hold Time (x25 = msec) default 40=1000ms
	    uint8_t prio_sample_time;           // Priority Sample Time (x250 = msec) default 8=2000ms

	} scanlist_t;

	//
	// Table of scanlists.
	//
	typedef struct {
	    uint8_t    valid[256];              // byte=1 when scanlist valid
	    scanlist_t scanlist[NSCANL];
	} scantab_t;

	//
	// General settings.
	//
	typedef struct {
	    // Bytes e0-e7
	    uint8_t radio_name[8];
	    // Bytes e8-eb
	    uint8_t radio_id[4];
	} general_settings_t;

	//
	// Intro messages.
	//
	typedef struct {
	    // Bytes 7540-754f
	    uint8_t intro_line1[16];
	    // Bytes 7550-755f
	    uint8_t intro_line2[16];
	} intro_text_t;

	//
	// Table of text messages.
	//
	typedef struct {
	    uint8_t count;                      // number of messages
	    uint8_t _unused1[7];                // 0
	    uint8_t len[NMESSAGES];             // message length
	    uint8_t _unused2[NMESSAGES];        // 0
	    uint8_t message[NMESSAGES*144];     // messages
	} msgtab_t;

public:
	RD5RCodePlug(QObject *parent=0);

	bool decode(Config *config);
	bool encode(Config *config);

protected:
	bank_t *_unpack_get_bank(int i);
	channel_t *_unpack_get_channel(int i);
	grouplist_t *_unpack_get_grouplist(int index);
	QString _unpack_ascii(const unsigned char *text, unsigned nchars);
	double _unpack_frequency(unsigned data);
	double _unpack_tone(unsigned data);
	bool _pack_channel(Config *config, int i, Channel *channel);
	bool _pack_zone(Config *config, int i, Zone *zone);
	bool _pack_scanlist(Config *config, int i, ScanList *list);
	bool _pack_contact(Config *config, int i, DigitalContact *contact);
	bool _pack_grouplist(Config *config, int i, RXGroupList *list);
};

#endif // RD5R_CODEPLUG_HH
