#ifndef RT3S_GPS_CODEPLUG_HH
#define RT3S_GPS_CODEPLUG_HH

#include "codeplug.hh"
#include <QDateTime>

class Channel;
class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;

/** Represents the device specific binary codeplug for TYT UV-390 & Retevis RT3S radios.
 *
 * The codeplug consists of two segments. The first segment starts at address @c 0x002800 and ends at
 * address @c 0x040800. The second segment starts at address @c 0x110800 and ends at @c 0x1a0800. The
 * segments must align with @c 0x400 (1024 bytes).
 *
 * @section uv390cpl Codeplug structure within radio
 * <table>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x002800-0x040800</th></tr>
 *  <tr><td>0x002800</td> <td>0x00280c</td> <td>0x0000c</td> <td>Timestamp see @c UV390Codeplug::timestamp_t.</td></tr>
 *  <tr><td>0x00280c</td> <td>0x002840</td> <td>0x00034</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x002840</td> <td>0x0028e4</td> <td>0x000a4</td> <td>General settings see @c UV390Codeplug::general_settings_t.</td></tr>
 *  <tr><td>0x0028e4</td> <td>0x002980</td> <td>0x0009c</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x002980</td> <td>0x0061c0</td> <td>0x03840</td> <td>50 Text messages @ 0x120 bytes each, see @c UV390Codeplug::message_t.</td></tr>
 *  <tr><td>0x0061c0</td> <td>0x00f420</td> <td>0x09260</td> <td>??? Emergency systems ???</td></tr>
 *  <tr><td>0x00f420</td> <td>0x0151e0</td> <td>0x05dc0</td> <td>250 RX Group lists @ 0x60 bytes each, see @c UV390Codeplug::grouplist_t.</td></tr>
 *  <tr><td>0x0151e0</td> <td>0x019060</td> <td>0x03e80</td> <td>250 Zones @ 0x40 bytes each, see @c UV390Codeplug::zone_t.</td></tr>
 *  <tr><td>0x019060</td> <td>0x01f5f0</td> <td>0x06590</td> <td>250 Scanlists @ 0x68 bytes each, see @c UV390Codeplug::scanlist_t.</td></tr>
 *  <tr><td>0x01f5f0</td> <td>0x031860</td> <td>0x12270</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x031860</td> <td>0x03f320</td> <td>0x0dac0</td> <td>250 Zone-extensions @ 0xe0 bytes each, see @c UV390Codeplug::zone_ext_t.</td></tr>
 *  <tr><td>0x03f320</td> <td>0x040800</td> <td>0x014e0</td> <td>??? Unknown ???</td></tr>
 *  <tr><th colspan="4">Second segment 0x110800-0x1a0800</th></tr>
 *  <tr><td>0x110800</td> <td>0x13f600</td> <td>0x2ee00</td> <td>3000 Channels @ 0x40 bytes each, see @c UV390Codeplug::channel_t.</td></tr>
 *  <tr><td>0x13f600</td> <td>0x140800</td> <td>0x01200</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x140800</td> <td>0x198640</td> <td>0x57e40</td> <td>10000 Contacts @ 0x24 bytes each, see @c UV390Codeplug::contact_t.</td></tr>
 *  <tr><td>0x198640</td> <td>0x1a0800</td> <td>0x081c0</td> <td>??? Unknown ???</td></tr>
 * </table>
 *
 * @ingroup uv390 */
class UV390Codeplug : public CodePlug
{
  Q_OBJECT

protected:
  /** Represents a single channel (analog or digital) within the codeplug. */
	typedef struct {
    /** Possible channel modes. */
		typedef enum {
			MODE_ANALOG  = 1,             ///< Analog channel.
			MODE_DIGITAL = 2              ///< Digital channel.
		} Mode;

    /** Bandwidth of the channel. */
		typedef enum {
			BW_12_5_KHZ = 0,              ///< 12.5 kHz narrow, (default for binary channels).
			BW_20_KHZ   = 1,              ///< 20 kHz (really?)
      BW_25_KHZ   = 2               ///< 25kHz wide.
		} Bandwidth;

		// Byte 0
    uint8_t channel_mode     : 2,   ///< Channel Mode: 1=Analog or 2=Digital, default=Analog
      bandwidth              : 2,   ///< Bandwidth: 0=12.5kHz, 1=20kHz or 2=25kHz, default = 0
      autoscan               : 1,   ///< Autoscan: 0=disable, 1=enable, default=0
      _unused0_1             : 1,   ///< Reserved = 1
      _unused0_2             : 1,   ///< Reserved = 1
      lone_worker            : 1;   ///< Lone Worker: 0=disable, 1=enable, default=0

    // Byte1
		uint8_t allow_talkaround : 1,   ///< Allow Talkaround: 0=off, 1=on, default=0
      rx_only                : 1,   ///< RX Only: 0=off, 1=on, default=0
      time_slot              : 2,   ///< Repeater slot: 2=TS2, 1=TS1, default=1
      color_code             : 4;   ///< ColorCode [0..15], default=1

    /** Possible privacy types. */
		typedef enum {
			PRIV_NONE = 0,                ///< No privacy.
			PRIV_BASIC = 1,               ///< Basic privacy.
			PRIV_ENHANCED = 2             ///< Enhenced privacy.
		} PrivacyType;

		// Byte 2
    uint8_t privacy_no       : 4,   ///< Privacy No. (+1): 1...16 ???, default=0
      privacy                : 2,   ///< Privacy: None, Basic or Enhanced ???, default=0
      private_call_conf      : 1,   ///< Private Call Confirmed ???, default=0
      data_call_conf         : 1;   ///< Data Call Confirmed ???, default=0

    /** @todo I have absolutely no idea what that means. */
		typedef enum {
			REF_LOW = 0,
			REF_MEDIUM = 1,
			REF_HIGH = 2
		} RefFrequency;

		// Byte 3
		uint8_t rx_ref_frequency : 2,   ///< RX Ref Frequency: 0=Low, 1=Medium, 2=High, default=0
      _unused3_2             : 1,   ///< Unknown = 0 ???, default = 0
      emergency_alarm_ack    : 1,   ///< Emergency Alarm Ack, default = 0
      _unused3_4             : 3,   ///< Unknown 0b110 ???
      display_pttid_dis      : 1;   ///< Display PTT ID (inverted), default = 1

    /** TX Admit criterion. */
		typedef enum {
			ADMIT_ALWAYS = 0,             ///< Always allow TX.
			ADMIT_CH_FREE = 1,            ///< Allow TX if channel is free.
			ADMIT_TONE = 2,               ///< Allow TX if CTCSS tone matches.
			ADMIT_COLOR = 3,              ///< Allow TX if color-code matches.
		} Admit;

		// Byte 4
		uint8_t tx_ref_frequency : 2,   ///< TX Ref Frequency: 0=Low, 1=Medium, 2=High, default=0
      _unused4_2             : 2,   ///< Unknow = 0b01
      vox                    : 1,   ///< VOX Enable, 0=Disable, 1=Enable, default = 0
      _unused4_5             : 1,   ///< Reserved = 1
      admit_criteria         : 2;   ///< Admit Criteria: 0=Always, 1=Channel Free or 2=Correct CTS/DCS, default=0

    /** Again, I have no idea. */
		typedef enum {
			INCALL_ALWAYS = 0,
			INCALL_ADMIT = 1,
			INCALL_TXINT = 2
		} InCall;

    /** Turn-off tone frequency.
     * This radio has a feature that allows to disable radios remote by sending a specific tone.
     * Certainly not a feature used in ham-radio. */
		typedef enum {
			TURNOFF_NONE = 3,             ///< Turn-off disabled. Default!
			TURNOFF_259_2HZ = 0,          ///< Turn-off on 259.2Hz tone.
			TURNOFF_55_2HZ = 1            ///< Turn-off on 55.2Hz tone.
		} TurnOffFreq;

		// Byte 5
    uint8_t _unused5_0    : 4,      ///< Reserved = 0 ???
      in_call_criteria    : 2,      ///< In Call Criteria: 0=Always, 1=Follow Admit Criteria, 2=TX Interrupt, default=0
      turn_off_freq       : 2;      ///< Non-QT/DQT Turn-off Freq.: 3=None, 0=259.2Hz, 1=55.2Hz, default=3

	    // Bytes 6-7
		uint16_t contact_name_index;    ///< Contact Name: Index + 1, default=0

	    // Bytes 8-9
		uint8_t tot           : 6,      ///< TOT x 15sec: 0-Infinite, 1=15s, ..., 37=555s, default=4
      _unused8_6          : 2;      ///< Unknown = 0 ???
    uint8_t tot_rekey_delay;        ///< TOT Rekey Delay: 0s...255s, default=0

	    // Bytes 10-11
		uint8_t emergency_system_index; ///< Emergency System: None, System 1...32, default=0
		uint8_t scan_list_index;        ///< Scan List: None, ScanList 1...250, default=0

		// Bytes 12
		uint8_t group_list_index;       ///< Group List: None, GroupList1...250, default=0

    // Bytes 13
		uint8_t gps_system;             ///< GPS System: Index+1 [1..16], default=0

		// Bytes 14
    uint8_t dtmf_decode1  : 1,      ///< DTMF decode 1. 0=off, 1=on
      dtmf_decode2        : 1,      ///< DTMF decode 2. 0=off, 1=on
      dtmf_decode3        : 1,      ///< DTMF decode 3. 0=off, 1=on
      dtmf_decode4        : 1,      ///< DTMF decode 4. 0=off, 1=on
      dtmf_decode5        : 1,      ///< DTMF decode 5. 0=off, 1=on
      dtmf_decode6        : 1,      ///< DTMF decode 6. 0=off, 1=on
      dtmf_decode7        : 1,      ///< DTMF decode 7. 0=off, 1=on
      dtmf_decode8        : 1;      ///< DTMF decode 8. 0=off, 1=on, default=0

    // Bytes 15
		uint8_t squelch;                ///< Squelch: 0...9, default=1

		// Bytes 16-23
		uint32_t rx_frequency;          ///< RX Frequency: 8 digits BCD, default=0x00000040
		uint32_t tx_frequency;          ///< TX Frequency: 8 digits BCD, default=0x00000040

		// Bytes 24-27
    uint16_t ctcss_dcs_receive;     ///< CTCSS/DCS Dec: 4 digits BCD, 0xffff = None, default=0xffff
		uint16_t ctcss_dcs_transmit;    ///< CTCSS/DCS Enc: 4 digits BCD, 0xffff = None, default=0xffff

	    // Bytes 28-29
		uint8_t rx_signaling_syst;      ///< Rx Signaling System: Off, DTMF-1...4, default = 0
		uint8_t tx_signaling_syst;      ///< Tx Signaling System: Off, DTMF-1...4, default = 0

    /** Possible power settings. */
		typedef enum {
			POWER_HIGH = 3,               ///< High = 5W.
			POWER_LOW = 0,                ///< Low = 1W
			POWER_MIDDLE = 2              ///< Middle = ?W.
		} Power;

		// Byte 30
    uint8_t power         : 2,      ///< Power: Low, Middle, High, default=high
      _unused30_2         : 6;      ///< 0b111111

    /** Again, I have no idea. */
		typedef enum {
			DCDM_LEADER = 0,
			DCDM_MS = 1
		} DCDM;

		// Byte 31
		uint8_t send_gps_info : 1,      ///< Send GSP Info (inv): 0=Send, 1=Disabled, default=1
      recv_gsp_info       : 1,      ///< Recv. GSP Info (inv): 0=Recv., 1=Disabled, default=1
      allow_interrupt     : 1,      ///< Allow interrupt (inv): 0=Allow, 1=Disabled, default=1
      dcdm_switch_dis     : 1,      ///< DCDM switch (inverted), default=1
      leader_ms           : 1,      ///< Leader/MS: Leader or MS, default=1
      _unused31_5         : 3;      ///< Reserved = 0b111

		// Bytes 32-63
		uint16_t name[16];              ///< Channel Name (16 x Unicode), default=0x0000

    /** Returns @c true if the channel is valid. */
    bool isValid() const;
    /** Clears and invalidates the channel. */
    void clear();

    /** Returns the RX frequency in MHz. */
    double getRXFrequency() const;
    /** Sets the RX frequency in MHz. */
    void setRXFrequency(double f);

    /** Returns the TX frequency in MHz. */
    double getTXFrequency() const;
    /** Sets the TX frequency in MHz. */
    void setTXFrequency(double f);

    /** Returns the name of the radio. */
    QString getName() const;
    /** Sets the name of the radio. */
    void setName(const QString &name);
    /** Returns the RX CTCSS tone. */
    float getRXTone() const;
    /** Sets the RX CTCSS tone. */
    void setRXTone(float tone);

    /** Returns the TX CTCSS tone. */
    float getTXTone() const;
    /** Sets the TX CTCSS tone. */
    void setTXTone(float tone);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    Channel *toChannelObj() const;
    /** Links a previously constructed channel to the rest of the configuration. */
    bool linkChannelObj(Channel *c, Config *conf) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    void fromChannelObj(const Channel *c, const Config *conf);
	} channel_t;

	/** Represents a digital (DMR) contact within the codeplug. */
	typedef struct {
    /** Call types. */
		typedef enum {
			CALL_GROUP = 1,             ///< Group call.
			CALL_PRIVATE = 2,           ///< Private call.
			CALL_ALL = 3                ///< All call.
		} CallType;

		// Bytes 0-2
		uint8_t id[3];                ///< Call ID, [1,16777215], default = 0xffffff.

		// Byte 3
    uint8_t type          : 5,    ///< Call Type, 1=Group Call, 2=Private Call or 3 = All Call, 0=Disabled.
      receive_tone        : 1,    ///< Call Receive Tone, 0=no or 1=yes.
      _unused2            : 2;    ///< Unknown, always 0b11.

		// Bytes 4-35
		uint16_t name[16];            ///< Contact Name (16 x 16bit Unicode), default=0x00

    /** Clears and invalidates the contact. */
    void clear();
    /** Returns @c true if the contact is valid. */
    bool isValid() const;

    /** Returns the contact DMR ID. */
    uint32_t getId() const;
    /** Sets the contact DMR ID. */
    void setId(uint32_t num);
    /** Returns the name of the contact. */
    QString getName() const;
    /** Sets the name of the contact. */
    void setName(const QString &name);

    /** Constructs a generic @c DigitalContact from this codeplug representation. */
    DigitalContact *toContactObj() const;
    /** Initializes this codeplug contact from the given generic @c DigitalContact. */
    void fromContactObj(const DigitalContact *obj, const Config *conf);
	} contact_t;

	/** Represents a zone within the codeplug.
	 * Please note that a zone consists of two structs the @c zone_t and the @c zone_ext_t.
	 * The latter adds additional channels for VFO A and the channels for VFO B. */
	typedef struct {
    // Bytes 0-31
    uint16_t name[16];              ///< Zone Name (Unicode).

    // Bytes 32-63
    uint16_t member_a[16];          ///< Member A, channel indices+1 , 0=empty/EOL

    /** Returns @c true if the zone is valid. */
    bool isValid() const;
    /** Clears and invalidates the zone. */
    void clear();

    /** Returns the name of the zone. */
    QString getName() const;
    /** Sets the name of the zone. */
    void setName(const QString &name);

    /** Contructs a generic Zone object from this codeplug zone. */
    Zone *toZoneObj() const;
    /** Links a previously created generic Zone to the rest of the generic configuration. */
    bool linkZone(Zone *zone, Config *conf) const;
    /** Initializes this codeplug zone from the given generic zone. */
    void fromZoneObj(const Zone *zone, const Config *conf);
	} zone_t;

  /** Extended zone data.
   * The zone definition @c zone_t contains only a single set of 16 channels. For each zone
   * definition, there is a zone extension which extends a zone to zwo sets of 64 channels each. */
	typedef struct {
    // Bytes 0-95
    uint16_t ext_a[48];             ///< Member A: Channels 17...64, 0=empty/EOL

    // Bytes 96-223
    uint16_t member_b[64];          ///< Member B: Channels 1...64, 0=empty/EOL

    /** Returns @c true if the zone extension is valid. */
    bool isValid() const;
    /** Clears and invalidates this zone extension. */
    void clear();

    /** Links a previously constructed generic @c Zone to the rest of the generic configuration. */
    bool linkZone(Zone *zone, Config *conf) const;
    /** Initializes this zone extension from the given generic zone. */
    void fromZoneObj(const Zone *zone, const Config *conf);
	} zone_ext_t;

	/** Representation of an RX group list within the codeplug. */
	typedef struct {
    // Bytes 0-31
    uint16_t name[16];              ///< Group List Name (16 x 16bit Unicode)
    // Bytes 32-95
    uint16_t member[32];            ///< Contacts index list 0-terminated.

    /** Returns @c true if the RX group list is valid. */
    bool isValid() const;
    /** Clears and invalidates the group list. */
    void clear();

    /** Retruns the name of the group list. */
    QString getName() const;
    /** Sets the name of the group list. */
    void setName(const QString &name);

    /** Constructs a generic @c RXGroupList from this codeplug group list. */
    RXGroupList *toRXGroupListObj() const;
    /** Links a previously constructed RX group list to the rest of the generic configuration. */
    bool linkRXGroupList(RXGroupList *obj, Config *conf) const;
    /** Initializes this codeplug group list from the given generic RX group list. */
    void fromRXGroupListObj(const RXGroupList *obj, const Config *conf);
	} grouplist_t;

	/** Represents a scan list within the codeplug. */
	typedef struct {
    // Bytes 0-31
    uint16_t name[16];              ///< Scan List Name (16 x unicode), default=0

    // Bytes 32-37
    uint16_t priority_ch1;          ///< Priority Channel 1 index or 0x0000, default=0xffff
    uint16_t priority_ch2;          ///< Priority Channel 2 index or 0x0000, default=0xffff
    uint16_t tx_designated_ch;      ///< Tx Designated Channel or 0xffff, default=0xffff

    // Bytes 38-41
    uint8_t _unused38;              ///< Unknown = 0xf1
    uint8_t sign_hold_time;         ///< Signaling Hold Time (x25 = msec), default=0x14
    uint8_t prio_sample_time;       ///< Priority Sample Time (x250 = msec), default=0x08
    uint8_t _unused41;              ///< Unused = 0xff

    // Bytes 42-103
    uint16_t member[31];            ///< Channels, zero-terminated list, default=0

    /** Returns @c true if the scanlist is valid. */
    bool isValid() const;
    /** Clears and invalidates the scanlist. */
    void clear();

    /** Returns the name of the scanlist. */
    QString getName() const;
    /** Sets the name of the scanlist. */
    void setName(const QString &n);

    /** Constructs a generic @c ScanList from this codeplug representation. */
    ScanList *toScanListObj() const;
    /** Links the previously constructed generic scanlist to the rest of the configuration. */
    bool linkScanListObj(ScanList *l, Config *conf) const;
    /** Initializes this codeplug scanlist from the given generic scanlist. */
    void fromScanListObj(const ScanList *l, const Config *conf);
	} scanlist_t;

	/** Codeplug representation of the general settings. */
	typedef struct {
    uint16_t intro_line1[10];             ///< Intro line 1: 10 x 16bit unicode, default=0
    uint16_t intro_line2[10];             ///< Intro line 2: 10 x 16bit unicode, default=0
    uint8_t  _unused40[24];               ///< Reserved: 24 bytes 0xff.

    uint8_t  _unused64_0     : 1,         ///< Reserved = 0
      _unused64_1            : 1,         ///< Reserved = 1
      disable_all_leds       : 1,         ///< Disable all LEDs: 1 = Off, 0 = On, default=1
      _unused64_3            : 1,         ///< Reserved = 1.
      monitor_type           : 1,         ///< Monitor type: 1 = Open Squelch, 0 = Silent, default=1
      _unused64_6            : 1,         ///< Reserved = 1.
      tx_mode                : 2;         ///< TX Mode: 0=LastCallCH, 1=LastCallCH+HandCH, 2 = DesignedCH, 3 = DesignedCH+HandCH, default=3

    uint8_t  save_preamble   : 1,         ///< Save Preamble: 0=Disabled, 1=Enabled, default=1
      save_mode_receive      : 1,         ///< Save Mode Receive: 0=Disabled, 1=Enabled, default=1
      disable_all_tones      : 1,         ///< Disable all tones: 0=True, 1=False, default=0
      _unused65_3            : 1,         ///< Reserved = 1
      ch_free_indication_tone: 1,         ///< Ch. free indication tone: 0 = Enabled, 1 = Disabled, default=1
      pw_and_lock_enable     : 1,         ///< Password and lock enable: 0 = Enabled, 1 = Disabled, default=1
      talk_permit_tone       : 2;         ///< Talk permit tone: 0=None, 1=Digital, 2=Analog, 3=Analog+Digital, default=0


    uint8_t  _unused66_0     : 1,         ///< Reserved = 0
      channel_voice_announce : 1,         ///< Channel voice announce, 0=Off, 1=On, default=0
      _unused66_2            : 1,         ///< Reserved = 0
      _unused66_3            : 1,         ///< Reserved = 1
      intro_picture          : 1,         ///< Intro Picture: 0=Off, 1=On, default=1
      keypad_tones           : 1,         ///< Keypad tones: 0=Off, 1=On, default=0
      _unused66_6            : 2;         ///< Reserved = 0b11

    uint8_t  _unused67_0     : 2,         ///< Reserved = 0b11
      mode_select_a          : 1,         ///< Mode select A: 1 = MR, 0 = VFO, default=1
      _unused67_1            : 4,         ///< Reserved = 0b1111
      mode_select_b          : 1;         ///< Mode select B: 1 = MR, 0 = VFO, default=1

    uint8_t  radio_id[3];                 ///< Radio DMR ID, default=0
    uint8_t  _unused71;                   ///< Pad byte = 0x00

    uint8_t  tx_preamble_duration;        ///< Tx preamble duration: x*60ms [0..144], default=0x0a
    uint8_t  group_call_hang_time;        ///< Group call hang time: x*100ms [0..70], default=0x1e
    uint8_t  private_call_hang_time;      ///< Private call hang time: x*100ms [0..70], default=0x28
    uint8_t  vox_sensitivity;             ///< VOX sensitivity: [1..10], default=0x03
    uint8_t  _unused76;                   ///< Reserved = 0x00
    uint8_t  _unused77;                   ///< Reserved = 0x00
    uint8_t  rx_low_battery_interval;     ///< RX low battery interval: x*5s [0..127], default=0x18
    uint8_t  call_alert_tone_duration;    ///< Call alert tone duration: x*5s [1..240], 0=Continous, default=0x00
    uint8_t  lone_worker_response_time;   ///< Lone Worker response time: x in minutes [1..255], default=0x01,
    uint8_t  lone_worker_reminder_time;   ///< Lone Worker reminder time: x in minutes [1..255], default=0x0a,
    uint8_t  _unused82;                   ///< Reserved = 0x00
    uint8_t  scan_digital_hang_time;      ///< Scan digital hang time @ 0x22b8: x*100ms [5..100], default=0x0a,
    uint8_t  scan_analog_hang_time;       ///< Scan analog hang time @ 0x22b9: x*100ms [0..100], default=0x0a

    uint8_t backlight_time       : 2,     ///< Backlight time: 0=Always, 1=5s, 2=10s, 3=15s., default=0x02
      _unused85_2                : 6;     ///< Reserved = 0

    uint8_t  set_keypad_lock_time;        ///< Set keypad lock time: 0xff = manual, 0x01=5s, 0x02=10s, 0x03=15s, default=0xff
    uint8_t  mode;                        ///< Mode: 0xff = Channel, 0x00 = Frequency, @see mode_select_a, mode_select_b, default=0xff

    uint32_t power_on_password;           ///< Power on password: 8 x BCD, 0x00000000
    uint32_t radio_prog_password;         ///< Radio prog password: 8 x BCD 0xffffffff = Disabled, default=0xffffffff

    uint8_t  pc_prog_password[8];         ///< PC prog password: ASCII, 0x00 terminated, all 0xff = disabled, default=0xff

    uint8_t  _unused104[3];               ///< Reserved = 0xffffff

    uint8_t  group_call_match    : 1,     ///< Group call match: 0=Off, 1=On, default=1
      private_call_match         : 1,     ///< Private call match: 0=Off, 1=On, default=1
      _unused107_4               : 1,     ///< Reserved = 1
      time_zone                  : 5;     ///< Timezone 0=UTC-12, 1=UTC-11, ..., 12=UTC, 13=UTC+1, ..., 24=UTC+12, default=0x0c

    uint32_t  _unused108;                 ///< Reserved = 0xffffffff

    uint16_t radio_name[16];              ///< Radio name 16-bit unicode., default=0x00

    uint8_t  channel_hang_time;           ///< Channel hang time @ 0x22f5: x*100ms [0..70], default=0x1e

    uint8_t  _unused145;                  ///< Unused @ 0x22f6 = 0xff.

    uint8_t  _unused146_0        : 2,     ///< Unused = 0b11.
      public_zone                : 1,     ///< Public zone: 1 = enabled, 0 = disabled, default=1
      _unused146_4               : 5;     ///< Unused = 0b11111.

    uint8_t  _unused147;                  ///< Unused @ 0x22f8 = 0xff

    uint8_t radio_id1[3];                 ///< Radio ID 1 @ 0x22f9, default=0x000001
    uint8_t _pad151;                      ///< Padding byte @ 0x22fc = 0x00

    uint8_t radio_id2[3];                 ///< Radio ID 1 @ 0x22fd, default=0x000002
    uint8_t _pad155;                      ///< Padding byte @ 0x2300 = 0x00

    uint8_t radio_id3[3];                 ///< Radio ID 1 @ 0x2301, default=0x000003
    uint8_t _pad159;                      ///< Padding byte @ 0x2304 = 0x00

    uint8_t _unused_160_0       : 3,      ///< Reserved = 0b111
      mic_level                 : 3,      ///< Mic Level 0=1, 1=2, ..., 5=6 [0..5], default=0
      edit_radio_id             : 1,      ///< Edit Radio ID 0=Enable, 1=Disable, default=0
      _unused_160_7             : 1;      ///< Reserved @ 0x2305 = 1

    /** Resets this general settings. */
    void clear();
    /** Returns the radio DMR ID. */
    uint32_t getRadioId() const;
    /** Sets the radio DMR ID. */
    void setRadioId(uint32_t num);
    /** Returns the first additional radio DMR ID. */
    uint32_t getRadioId1() const;
    /** Sets the first additional radio DMR ID. */
    void setRadioId1(uint32_t num);
    /** Returns the second additional radio DMR ID. */
    uint32_t getRadioId2() const;
    /** Sets the second additional radio DMR ID. */
    void setRadioId2(uint32_t num);
    /** Returns the third additional radio DMR ID. */
    uint32_t getRadioId3() const;
    /** Set the third additional radio DMR ID. */
    void setRadioId3(uint32_t num);
    /** Returns the name of the radio. */
    QString getName() const;
    /** Sets the name of the radio. */
    void setName(const QString &n);
    /** Returns the first intro line. */
    QString getIntroLine1() const;
    /** Sets the first intro line. */
    void setIntroLine1(const QString &text);
    /** Returns the second intro line. */
    QString getIntroLine2() const;
    /** Sets the second intro line. */
    void setIntroLine2(const QString &text);
    /** Updates the generic configuration from this general settings. */
    bool updateConfigObj(Config *conf) const;
    /** Updates this codeplug general settings from the generic configuration. */
    void fromConfigObj(const Config *conf);
  } general_settings_t;

  /** Represents a single message within the codeplug. */
  typedef struct {
    uint16_t text[144];                   ///< Message text (144 x 16bit Unicode), 0-terminated

    /** Returns @c true if the message is valid. */
    inline bool isValid() const {
      return 0 != text[0];
    }
    /** Returns the message text. */
    inline QString getText() const {
      QString txt; txt.reserve(144);
      for (int i=0; (i<144) && (0!=text[i]); i++)
        txt.append(QChar(text[i]));
      return txt;
    }
    /** Sets the message text. */
    inline void setText(const QString text) {
      memset(this->text, 0, 288);
      for (int i=0; (i<128) && (i<text.size()); i++)
        this->text[i] = text.at(i).unicode();
    }
  } message_t;

  /** Codeplug representation of programming time-stamp and CPS version. */
  typedef struct {
    uint8_t _pad0;                       ///< Fixed 0xff
    uint8_t date[7];                     ///< YYYY-MM-DD hh:mm:ss as 14 BCD numbers.
    uint8_t cps_version[4];              ///< CPS version vv.vv, encoded using map "0123456789:;<=>?".

    /** Returns @c true if the timestamp is valid. */
    bool isValid() const;
    /** Clears the timestamp. */
    void clear();

    /** Returns the timestamp. */
    QDateTime getTimestamp() const;
    /** Sets the timestamp. */
    void setTimestamp(const QDateTime &dt=QDateTime::currentDateTimeUtc());
    /** Returns the CSP version string. */
    QString cpsVersion() const;
  } timestamp_t;

  /** Represents a single GPS system within the codeplug.
   * @todo Verify layout and offset! */
  typedef struct {
    uint16_t revert_channel;              ///< Revert channel index, 0=current.
    uint8_t  repeat_interval;             ///< Repeat interval x*30s, 0=off.
    uint8_t  _unused_3;                   ///< Reserved =0xff.
    uint16_t destination;                 ///< Destination contact index, 0=none.
    uint8_t  _unused_6[10];               ///< Padding all = 0xff
  } gpssystem_t;

	/** Represents an entry within the callsign database.
   * @todo Implement generic config representation for callsign database. */
	typedef struct {
    unsigned dmrid   : 24;      ///< DMR id in BCD
    unsigned _unused : 8;       ///< Unknown set to 0xff.
    char     callsign[16];      ///< ASCII zero-terminated
    char     name[100];         ///< Descriptive name, nickname, city, state, country.
  } callsign_t;

public:
  /** Default constructor. */
	explicit UV390Codeplug(QObject *parent = nullptr);

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
	bool encode(Config *config);

  /** Returns the size of the binary codeplug. */
  size_t size() const;
};

#endif // RT3S_GPS_CODEPLUG_HH
