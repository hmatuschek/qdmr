#ifndef RD5R_CODEPLUG_HH
#define RD5R_CODEPLUG_HH

#include <QObject>
#include "codeplug.hh"
#include "signaling.hh"
#include "codeplugcontext.hh"

class Channel;
class Zone;
class ScanList;
class DigitalContact;
class DTMFContact;
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
 * Please note, that the codeplug is not yet fully understood and a full codeplug cannot be build
 * from scratch. That is, it is necessary to update an existing codeplug on the radio.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>     <th>Size</th>  <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x00080-0x07c00</th></tr>
 *  <tr><td>0x00080</td> <td>0x00088</td> <td>0x0008</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00088</td> <td>0x0008e</td> <td>0x0006</td> <td>Timestamp, see @c RD5RCodeplug::timestamp_t.</td></tr>
 *  <tr><td>0x0008e</td> <td>0x000e0</td> <td>0x0052</td> <td>CPS, firmware, DSP version numbers (not touched).</td></tr>
 *  <tr><td>0x000e0</td> <td>0x00108</td> <td>0x0028</td> <td>General settings, see @c RD5RCodeplug::general_settings_t.</td></tr>
 *  <tr><td>0x00108</td> <td>0x00128</td> <td>0x0020</td> <td>Button settings, see @c RD5RCodeplug::button_settings_t.</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 preset message texts, see @c RD5RCodeplug::msgtab_t.</td></tr>
 *  <tr><td>0x01370</td> <td>0x01588</td> <td>0x0218</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01588</td> <td>0x01788</td> <td>0x0200</td> <td>??? 32 Emergency systems ???</td></tr>
 *  <tr><td>0x01788</td> <td>0x02f88</td> <td>0x1800</td> <td>256 contacts, see @c RD5RCodeplug::contact_t.</td></tr>
 *  <tr><td>0x02f88</td> <td>0x03388</td> <td>0x0400</td> <td>32 DTMF contacts, see @c RD5RCodeplug::dtmf_contact_t.</td></tr>
 *  <tr><td>0x03388</td> <td>0x03780</td> <td>0x03f8</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 chanels (bank 0), see @c RD5RCodeplug::bank_t.</td></tr>
 *  <tr><td>0x05390</td> <td>0x07518</td> <td>0x2188</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07518</td> <td>0x07538</td> <td>0x0020</td> <td>Boot settings, see @c RD5RCodeplug::boot_settings_t.</td></tr>
 *  <tr><td>0x07538</td> <td>0x07540</td> <td>0x0008</td> <td>Menu settings, see @c RD5RCodeplug::menu_settings_t</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c RD5RCodeplug::intro_text_t.</td></tr>
 *  <tr><td>0x07560</td> <td>0x07590</td> <td>0x0030</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07590</td> <td>0x07600</td> <td>0x0070</td> <td>VFO settings, see @c RD5RCodeplug::vfo_settings_t.</td></tr>
 *  <tr><td>0x07600</td> <td>0x07c00</td> <td>0x0600</td> <td>??? Unknown ???</td></tr>
 *
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
 *
 * @ingroup rd5r */
class RD5RCodeplug : public CodePlug
{
  Q_OBJECT

public:
  static const int NCHAN     = 1024;   ///< Defines the number of channels.
  static const int NCONTACTS = 256;    ///< Defines the number of contacts.
  static const int NDTMF     = 32;     ///< Defines the number of DTMF contacts.
  static const int NZONES    = 250;    ///< Defines the number of zones.
  static const int NGLISTS   = 64;     ///< Defines the number of RX group lists.
  static const int NSCANL    = 250;    ///< Defines the number of scanlists.
  static const int NMESSAGES = 32;     ///< Defines the number of preset messages.

  /** Represents a configured channel within the codeplug.
   * Please note that channels are organized in banks, see @c RD5RCodeplug::bank_t for details.
   *
   * Memmory layout of encoded channel:
   * @verbinclude rd5rchannel.txt
   */
  struct __attribute__((packed)) channel_t {
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
    uint16_t _unused25;                 ///< Unknown set to 0.
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
    uint8_t _unused48_0       : 6,      ///< Unknown, set to 0.
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
      talkaround              : 1,      ///< Allow talkaround, default 0.
      _unused51_4             : 2,      ///< Unknown 0.
      vox                     : 1,      ///< VOX Enable.
      power                   : 1;      ///< Power Low or High.
    // Bytes 52-55
    uint8_t _unused52[3];               ///< Unknown set to 0.
    uint8_t squelch;                    ///< Squelch level [0,9]

    /** Constructor. */
    channel_t();

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
    void setRXTone(Signaling::Code code);
    /** Returns the CTCSS TX tone. */
    Signaling::Code getTXTone() const;
    /** Sets the CTCSS TX tone. */
    void setTXTone(Signaling::Code code);

    /** Constructs a @c Channel object from this codeplug channel. */
    Channel *toChannelObj() const;
    /** Resets this codeplug channel from the given @c Channel object. */
    void fromChannelObj(const Channel *c, const Config *conf);
    /** Links a previously constructed @c Channel object to other object within the generic
     * configuration, for example scan lists etc. */
    bool linkChannelObj(Channel *c, Config *conf, const QHash<int, int> &scan_table,
                        const QHash<int, int> &group_table, const QHash<int, int> &contact_table) const;
  };

  /** One bank of 128 channels including a bitmap for enabled channels.
   *
   * This struct represents a single bank of 128 channels, see @c RD5RCodeplug::channel_t for
   * details.
   *
   * Memmory layout of channel bank:
   * @verbinclude rd5rchannelbank.txt
   */
  struct __attribute__((packed)) bank_t {
    /** Bitmap for 128 channels, a bit is set when the coresspondig channel is valid/enabled. */
    uint8_t bitmap[16];
    /** The actual 128 channels. */
    channel_t chan[128];
  };

  /** Binary codeplug representation of VFO settings.
   *
   * This struct contains two channel settings (see @c RD5RCodeplug::channel_t). One for VFO A and
   * one for VFO B settings.
   *
   * Memmory layout of the VFO settings:
   * @verbinclude rd5rvfosettings.txt
   */
  struct __attribute__((packed)) vfo_settings_t {
    channel_t vfo_a; ///< Channel settings for VFO A.
    channel_t vfo_b; ///< Channel settings for VFO B.
  };

  /** Specific codeplug representation of a DMR contact.
   *
   * Memmory layout of the contact:
   * @verbinclude rd5rcontact.txt
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
    uint8_t _unused23;                  ///< Set to 0x00.

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

  /** Specific codeplug representation of a DTMF (analog) contact.
   *
   * Memmory layout of the DTMF contact:
   * @verbinclude rd5rdtmfcontact.txt
   */
  struct __attribute__((packed)) dtmf_contact_t {
    // Bytes 0-15
    uint8_t name[16];                   ///< Contact name in ASCII, 0xff terminated.
    // Bytes 16-24
    uint8_t number[16];                 ///< DTMF number {0-9,a-d,*,#}, 0xff terminated.

    /** Constructor. */
    dtmf_contact_t();

    /** Resets an invalidates the contact entry. */
    void clear();
    /** Returns @c true, if the contact is valid. */
    bool isValid() const;
    /** Returns the DTMF number of the contact. */
    QString getNumber() const;
    /** Sets the DTMF number of the contact. */
    bool setNumber(const QString &number);
    /** Returns the name of the contact. */
    QString getName() const;
    /** Sets the name of the contact. */
    void setName(const QString &name);

    /** Constructs a @c DTMFContact instance from this codeplug contact. */
    DTMFContact *toContactObj() const;
    /** Resets this codeplug contact from the given @c DTMFContact. */
    void fromContactObj(const DTMFContact *obj, const Config *conf);
  };

  /** Represents a single zone within the codeplug.
   *
   * Memmory layout of the zone:
   * @verbinclude rd5rzone.txt
   */
  struct __attribute__((packed)) zone_t {
    // Bytes 0-15
    uint8_t name[16];                   ///< Zone name ASCII, 0xff terminated.
    // Bytes 16-47
    uint16_t member[16];                ///< Member channel indices+1, 0=empty/not used.

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

  /** Table of zones.
   *
   * Memmory layout of the zone table/bank:
   * @verbinclude rd5rzonetab.txt
   */
  struct __attribute__((packed)) zonetab_t {
    /** A bit representing the validity of every zone. If a bit is set, the corresponding zone in
     * @c zone ist valid. */
    uint8_t bitmap[32];
    /** The list of zones. */
    zone_t  zone[NZONES];
  };

  /** Represents a single RX group list within the codeplug.
   *
   * Memmory layout of the RX group list:
   * @verbinclude rd5rgrouplist.txt
   */
  struct __attribute__((packed)) grouplist_t {
    // Bytes 0-15
    uint8_t name[16];                   ///< RX group list name, ASCII, 0xff terminated.
    // Bytes 16-47
    uint16_t member[16];                ///< Contact indices + 1, 0=empty.

    /** Constructor. */
    grouplist_t();
    /** Clears the group list. */
    void clear();

    /** Returns the name of the group list. */
    QString getName() const;
    /** Sets the name of the group list. */
    void setName(const QString &name);

    /** Constructs a @c RXGroupList object from the codeplug representation. */
    RXGroupList *toRXGroupListObj();
    /** Links a previously constructed @c RXGroupList to the rest of the generic configuration. */
    bool linkRXGroupListObj(int ncnt, RXGroupList *lst, const Config *conf, const QHash<int, int> &contact_table) const;
    /** Reset this codeplug representation from a @c RXGroupList object. */
    void fromRXGroupListObj(const RXGroupList *lst, const Config *conf);
  };

  /** Table of rx group lists.
   *
   * Memmory layout of the group list table:
   * @verbinclude rd5rgrouplisttab.txt
   */
  struct __attribute__((packed)) grouptab_t {
    /** Specifies the number of contacts +1 in each group list, 0=disabled. */
    uint8_t     nitems1[128];
    /** The list of RX group lists. */
    grouplist_t grouplist[NGLISTS];
  };

  /** Represents a sinle scan list within the codeplug.
   *
   * Memmory layout of the scan list.
   * @verbinclude rd5rscanlist.txt
   */
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
    bool linkScanListObj(ScanList *lst, const Config *conf, const QHash<int, int> &channel_table) const;
    /** Initializes this codeplug representation from the given @c ScanList object. */
    void fromScanListObj(const ScanList *lst, const Config *conf);
  };

  /** Table/Bank of scanlists.
   *
   * Memmory layout of the scan list table.
   * @verbinclude rd5rscanlisttab.txt
   */
  struct __attribute__((packed)) scantab_t {
    /** Byte-field to indicate which scanlist is valid. Set to 0x01 when valid, 0x00 otherwise. */
    uint8_t    valid[256];
    /** The scanlists. */
    scanlist_t scanlist[NSCANL];
  };

  /** Represents the general settings within the codeplug.
   *
   * Memmory layout of the general settings
   * @verbinclude rd5rgeneralsettings.txt
   */
  struct __attribute__((packed))  general_settings_t{
    /** Possible monitor types. */
    typedef enum {
      OPEN_SQUELCH = 0,            ///< Monitoring by opening the squelch.
      SILENT_MONITOR = 1           ///< Silent monitoring.
    } MonitorType;

    /** Possible ARTS tone settings. */
    typedef enum {
      ARTS_DISABLED = 0,           ///< ARTS tone is disabled.
      ARTS_ONCE     = 4,           ///< ARTS tone once.
      ARTS_ALWAYS   = 8            ///< ARTS tone always.
    } ARTSTone;

    /** Possible scan modes. */
    typedef enum {
      SCANMODE_TIME    = 0,
      SCANMODE_CARRIER = 1,
      SCANMODE_SEARCH  = 2
    } ScanMode;

    uint8_t radio_name[8];         ///< The radio name in ASCII, 0xff terminated.
    uint8_t radio_id[4];           ///< The radio DMR ID in BCD.

    uint32_t _reserved12;          ///< Reserved, set to 0x00000000.
    uint8_t _reserved16;           ///< Unknown, set to 0x00.

    uint8_t tx_preamble;           ///< TX preamble duration in 60ms steps 0=0ms, 1=60ms, default=360ms (0x06).
    uint8_t monitor_type;          ///< Monitor type, 0x00 = open squelch, 0x01 = silent.
    uint8_t vox_sensitivity;       ///< VOX sensitivity [1,10], default 0x03.
    uint8_t lowbat_intervall;      ///< Low-battery interfvall in 5sec setps, 0=0s, 1=5s, default 30s (0x06).
    uint8_t call_alert_dur;        ///< Call-alert tone duration in 5sec steps, 0=infinity, 1=5s, etc, default 0x18=120s.
    uint8_t lone_worker_response;  ///< Lone-worker response timer in minutes [1,255], default 1.
    uint8_t lone_worker_reminder;  ///< Lone-worker reminder timer in seconds [1,255], default 10.
    uint8_t grp_hang;              ///< Group-call hang-time in 500ms setps 0=0ms, 1=500ms, default=3000ms (0x06).
    uint8_t priv_hang;             ///< Private-call hang-time in 500ms setps 0=0ms, 1=500ms, default=3000ms (0x06).

    uint8_t downch_mode_vfo: 1,    ///< Down-channel mode=VFO, default 0.
      upch_mode_vfo        : 1,    ///< Up-channel mode=VFO, default 0.
      reset_tone           : 1,    ///< Reset tone, default 0.
      unknown_number_tone  : 1,    ///< Unknown number tone, default 0.
      arts_tone            : 4;    ///< ARTS tone, 0x0=disabled, 0x4=once, 0x8=always, default=once.

    uint8_t permit_digital : 1,     ///< Talk permit tone digital, default=0.
      permit_ananlog       : 1,     ///< Talk permit tone ananlog, default=0.
      selftest_tone        : 1,     ///< Self-test pass tone, default=1.
      freq_ind_tone        : 1,     ///< Channel frequency indication tone, default=0 (off).
      _reserved27_4        : 1,     ///< Unknown, set to 0.
      disable_all_tones    : 1,     ///< Disables all tones, default 0.
      savebat_receive      : 1,     ///< Save battery by disable receive, default=1.
      savebet_preamble     : 1;     ///< Save battery by disable preamble, default=1.

    uint8_t _reserved28_0  : 5,     ///< Unknown, set to 0b00000.
      disable_all_leds     : 1,     ///< Disables all LEDs, default=0.
      inh_quickkey_ovr     : 1,     ///< Inhibit quick-key override, default=0.
      _unknown38_7         : 1;     ///< Unknown, set to 1.

    uint8_t tx_exit_tone   : 4,     ///< TX exit tone, 0x0=off, 0x8=on, default off.
      dblwait_tx_active    : 1,     ///< Always TX on active channel on double-wait, not selected, default=1.
      animation            : 1,     ///< Enable animation, default 0.
      scan_mode            : 2;     ///< Scan mode 0b00=time, 0b01=carrier, 0b10=search, default time.

    uint8_t repeater_end_delay : 4, ///< Repeater-end delay [0,10], default 0.
      repeater_ste             : 4; ///< Repeater STE [0,10], default 0.

    uint8_t _reserved31;            ///< Unknown, set to 0x00.
    uint8_t prog_password[8];       ///< Programming password, 8 x ASCII 0xff terminated.

    /** Constructor. */
    general_settings_t();

    /** Resets the general settings. */
    void clear();
    /** Resets the general settings to their default values. */
    void initDefault();

    /** Returns the name of the radio. */
    QString getName() const;
    /** Sets the name of the radio. */
    void setName(const QString &n);

    /** Returns the DMR ID of the radio. */
    uint32_t getRadioId() const;
    /** Sets the DMR ID of the radio. */
    void setRadioId(uint32_t num);
  };

  /** Represents the button settings. */
  struct __attribute__((packed)) button_settings_t {
    /** Possible actions for each button on short and long press. */
    typedef enum {
      None                   = 0x00,  ///< Disables button.
      ToggleAllAlertTones    = 0x01,
      EmergencyOn            = 0x02,
      EmergencyOff           = 0x03,
      ToggleMonitor          = 0x05,  ///< Toggle monitor on channel.
      NuiaceDelete           = 0x06,
      OneTouch1              = 0x07,  ///< Performs the first of 6 user-programmable actions (call, message).
      OneTouch2              = 0x08,  ///< Performs the second of 6 user-programmable actions (call, message).
      OneTouch3              = 0x09,  ///< Performs the third of 6 user-programmable actions (call, message).
      OneTouch4              = 0x0a,  ///< Performs the fourth of 6 user-programmable actions (call, message).
      OneTouch5              = 0x0b,  ///< Performs the fifth of 6 user-programmable actions (call, message).
      OneTouch6              = 0x0c,  ///< Performs the sixt of 6 user-programmable actions (call, message).
      ToggleRepeatTalkaround = 0x0d,
      ToggleScan             = 0x0e,
      TogglePrivacy          = 0x10,
      ToggleVox              = 0x11,
      ZoneSelect             = 0x12,
      BatteryIndicator       = 0x13,
      ToggleLoneWorker       = 0x14,
      PhoneExit              = 0x16,
      ToggleFlashLight       = 0x1a,
      ToggleFMRadio          = 0x1b
    } ButtonEvent;

    /** Binary representation of one-touch actions (calls & messages). */
    struct __attribute__((packed)) one_touch_t {
      uint8_t type;               ///< Specifies the type of the action, 0x00=disabled, 0x10 = digital call, 0x11 = digital message, 0x20 = analog call.
      uint16_t contact_idx;       ///< Specifies the contact index, 0x00=none or index+1.
      uint8_t message_idx;        ///< Specifies the message index, 0x00=none, index+1 or 0xff if call.
      /** Resets the one-touch settings. */
      void clear();
    };

    uint8_t  _unknown0;           ///< Unknown set to 0x01.
    uint8_t  long_press_dur;      ///< Specifies the duration for a long-press in 250ms [4,16], default=6 (1500ms).
    uint8_t  sk1_short;           ///< Event on short-press on SK1.
    uint8_t  sk1_long;            ///< Event on long-press on SK1.
    uint8_t  sk2_short;           ///< Event on short-press on SK2.
    uint8_t  sk2_long;            ///< Event on long-press on SK2.
    uint8_t  _unknown6;           ///< Unknown set to 0x13.
    uint8_t  _unknown7;           ///< Unknown set to 0x11.
    one_touch_t one_touch[6];     ///< 6 x one-touch actions.

    /** Constructor. */
    button_settings_t();

    /** Resets the button settings. */
    void clear();
    /** Resets the button settings to their default values. */
    void initDefault();
  };

  /** Represents the menu settings. */
  struct __attribute__((packed)) menu_settings_t {
    // Byte 0
    uint8_t hangtime;             ///< Menu hang-time in seconds [1,30], default 10.

    // Byte 1
    uint8_t message        : 1,   ///< Show text message menu entry, default=1.
      scan_start           : 1,   ///< Show start-scan menu entry, default=1.
      edit_scanlist        : 1,   ///< Show edit-scanlist menu entry, default=1.
      call_alert           : 1,   ///< Show call-alert menu entry, default=1.
      edit_contact         : 1,   ///< Show edit-contact menu entry, default=1.
      manual_dial          : 1,   ///< Show manual-dial menu entry, default=1.
      radio_check          : 1,   ///< Show radio-check menu entry, default=1.
      remote_monitor       : 1;   ///< Show remote-monitor menu entry, default=1.

    // Byte 2
    uint8_t radio_enable   : 1,   ///< Show radio-enable menu entry, default=1.
      radio_disable        : 1,   ///< Show radio-disable menu entry, default=1.
      prog_passwd          : 1,   ///< Show programming-password menu entry, default=1.
      talkaround           : 1,   ///< Show talkaround menu entry, default=1.
      tone                 : 1,   ///< Show tone menu entry, default=1.
      power                : 1,   ///< Show power-settings menu entry, default=1.
      backlight            : 1,   ///< Show backlight menu entry, default=1.
      introscreen          : 1;   ///< Show intro-screen menu entry, default=1.

    // Byte 3
    uint8_t keypadlock     : 1,   ///< Show key-pad lock menu entry, default=1.
      led_indicator        : 1,   ///< Show LED indicator menu entry, default=1.
      squelch              : 1,   ///< Show squelch menu entry, default=1.
      privacy              : 1,   ///< Show privacy settings menu entry, default=1.
      vox                  : 1,   ///< Show VOX menu entry, default=1.
      passwd_lock          : 1,   ///< Show password and lock menu entry, default=1.
      missed_calls         : 1,   ///< Show missed calles menu entry, default=1.
      answered_calls       : 1;   ///< Show answered calls menu entry, default=1.

    // Byte 4
    uint8_t outgoing_calls : 1,   ///< Show outgoing calls menu entry, default=1.
      ch_display_mode      : 1,   ///< Show channel display mode menu entry, default=1.
      dbl_standby          : 1,   ///< Show double-standby (dual-watch) menu entry, default=1.
      _unknown4_3          : 1,   ///< Unknown set to 0.
      _unknown4_4          : 1,   ///< Unknown set to 0.
      _unknown4_5          : 1,   ///< Unknown set to 1.
      _unknown4_6          : 1,   ///< Unknown set to 1.
      _unknown4_7          : 1;   ///< Unknown set to 1.

    // Byte 5
    uint8_t  _unknown5;           ///< Unknown set to 0xff.

    // Byte 6
    uint8_t key_lock_time  : 2,   ///< Keypad auto-lock time, 0=manual, 1=5s, 2=10s, 3=15s, default=manual.
      backlight_time       : 2,   ///< Backlight on-time, 0=always, 1=5s, 2=10s, 3=15s, default=15s.
      _unknown6_4          : 2,   ///< Unknown, default=0.
      channel_display      : 2;   ///< Channel display type, 0=number, 1=name, 2=frequency, default=name.

    // Byte 7
    uint8_t  _unknown7_0   : 4,   ///< Unknown set to 0.
      _unknown7_4          : 1,   ///< Unknown set to 1.
      keytone              : 1,   ///< Enable key-tone, default=0.
      double_wait          : 2;   ///< Double-wait (dual-watch) mode. 1=Double/Double, 2=Double/Single.
  };

  /** Represents the boot settings within the binary codeplug. */
  struct __attribute__((packed)) boot_settings_t {
    uint8_t boot_text;                  ///< Shows intro lines at boot (see @c RD5RCodeplug::intro_text_t), 0=image, 1=text, default=1.
    uint8_t boot_password_enabled;      ///< Enables to boot-password, 0=disabled, 1=enabled, default=0.
    uint8_t _unknown2;                  ///< Unkown, set to 0.
    uint8_t _unknown3;                  ///< Unkown, set to 0.
    uint8_t password[3];                ///< Boot password 6 x BCD number.
    uint8_t _unknown7;                  ///< Unkown, set to 0.
    uint8_t empty[24];                  ///< Unkown (boot image?), set to 0.
  };

  /** Represents the intro messages within the codeplug. */
  struct __attribute__((packed)) intro_text_t {
    // Bytes 7540-754f
    uint8_t intro_line1[16];       ///< The first intro line, ASCII, 0xff terminated.
    // Bytes 7550-755f
    uint8_t intro_line2[16];       ///< The second intro line, ASCII, 0xff terminated.

    /** Constructor. */
    intro_text_t();
    /** Clear intro text. */
    void clear();

    /** Returns the first intro line. */
    QString getIntroLine1() const;
    /** Set the first intro line. */
    void setIntroLine1(const QString &text);
    /** Returns the second intro line. */
    QString getIntroLine2() const;
    /** Sets the second intro line. */
    void setIntroLine2(const QString &text);
  };

  /** Represents the table of text messages within the codeplug. */
  struct __attribute__((packed)) msgtab_t {
    uint8_t count;                      ///< Number of messages.
    uint8_t _unused1[7];                ///< Unknown, set to 0.
    uint8_t len[NMESSAGES];             ///< Message lengths.
    uint8_t _unused2[NMESSAGES];        ///< Unknown, set to 0.
    uint8_t message[NMESSAGES][144];    ///< The actual messages, ASCII

    /** Constructor. */
    msgtab_t();
    /** Clears all messages. */
    void clear();
    /** Gets a particular message. */
    QString getMessage(int i) const;
    /** Adds particular message to the list. */
    bool addMessage(const QString &text);
  };

  /** Represents the codeplug time-stamp within the codeplug. */
  struct __attribute__((packed)) timestamp_t {
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
  };

public:
  /** Empty constructor. */
  RD5RCodeplug(QObject *parent=0);

  /** Resets the codeplug to some default values. */
  void clear();

  /** Decodes the read codeplug and stores the result into the given generic configuration. */
  bool decode(Config *config);
  /** Encodes the given generic configuration into this codeplug. */
  bool encode(Config *config, bool update=true);
};

#endif // RD5R_CODEPLUG_HH
