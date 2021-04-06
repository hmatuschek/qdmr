#ifndef D878UV_CODEPLUG_HH
#define D878UV_CODEPLUG_HH

#include <QDateTime>

#include "d868uv_codeplug.hh"
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
 *  <tr><td>01042000</td> <td>000020</td>      <td>Roaming channel bitmask, up to 250 bits, 0-padded, default 0.</td></tr>
 *  <tr><td>01040000</td> <td>max. 0x1f40</td> <td>Optional up to 250 roaming channels, of 32b each.
 *    See @c roaming_channel_t for details.</td></tr>
 *  <tr><td>01042080</td> <td>000010</td>      <td>Roaming zone bitmask, up to 64 bits, 0-padded, default 0.</td></tr>
 *  <tr><td>01043000</td> <td>max. 0x2000</td> <td>Optional up to 64 roaming zones, of 128b each.
 *    See @c roaming_zone_t for details.</td></tr>
 *
 *  <tr><th colspan="3">Contacts</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>02600000</td> <td>max. 009C40</td> <td>Index list of valid contacts.
 *    10000 32bit indices, little endian, default 0xffffffff</td></tr>
 *  <tr><td>02640000</td> <td>000500</td>      <td>Contact bitmap, 10000 bit, inverted, default 0xff, 0x00 padded.</td></tr>
 *  <tr><td>02680000</td> <td>max. 0f4240</td> <td>10000 contacts, see @c contact_t.
 *    As each contact is 100b, they do not align with the 16b blocks being transferred to the device.
 *    Hence contacts are organized internally in groups of 4 contacts forming a "bank". </td></tr>
 *  <tr><td>04340000</td> <td>max. 013880</td> <td>DMR ID to contact index map, see @c contact_map_t.
 *    Sorted by ID, empty entries set to 0xffffffffffffffff.</td>
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
 *
 *  <tr><th colspan="3">General Settings</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
 *  <tr><td>02500000</td> <td>000630</td> <td>General settings, see @c general_settings_base_t.</td></tr>
 *  <tr><td>02501280</td> <td>000030</td> <td>General settings extension 1, see @c general_settings_ext1_t.</td></tr>
 *  <tr><td>02501400</td> <td>000100</td> <td>General settings extension 2, see @c general_settings_ext2_t.</td></tr>
 *  <tr><td>024C2000</td> <td>0003F0</td> <td>List of 250 auto-repeater offset frequencies.
 *    32bit little endian frequency in 10Hz. I.e., 600kHz = 60000. Default 0x00000000, 0x00 padded.</td></tr>
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
 *  <tr><th colspan="3">Still unknown</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>   <th>Content</th></tr>
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
 * </table>
 *
 * @ingroup d878uv */
class D878UVCodeplug : public D868UVCodeplug
{
  Q_OBJECT

public:
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

    /** Hemisphere settings for the fixed location beacon. */
    typedef enum {
      NORTH = 0,
      SOUTH = 1,
      EAST  = 0,
      WEST  = 1
    } Hemisphere;

    // byte 0x00
    uint8_t _unknown0;             ///< Unknown, set to 0xff.
    uint32_t frequency;            ///< TX frequency, BCD encoded, little endian in 10Hz.
    uint8_t tx_delay;              ///< TX delay, multiples of 20ms, default=1200ms.
    uint8_t sig_type;              ///< Signalling type, 0=off, 1=ctcss, 2=dcs, default=off.
    uint8_t ctcss;                 ///< CTCSS tone-code, default=0.
    uint16_t dcs;                  ///< DCS code, little endian, default=0x0013.
    uint8_t manual_tx_interval;    ///< Global manual TX intervals in seconds.
    uint8_t auto_tx_interval;      ///< Global auto TX interval in multiples of 30s.
    uint8_t tx_tone_enable;        ///< TX tone enable, 0=off, 1=on.

    uint8_t fixed_location;        ///< Fixed location data, 0=off, 1=on.
    uint8_t lat_deg;               ///< Latitude in degree.
    uint8_t lat_min;               ///< Latitude minutes.
    uint8_t lat_sec;               ///< Latitude seconds (1/100th of a minute).
    uint8_t north_south;           ///< North or south flag, north=0, south=1.
    uint8_t lon_deg;               ///< Longitude in degree.
    uint8_t lon_min;               ///< Longitude in minutes.
    uint8_t lon_sec;               ///< Longitude in seconds (1/100th of a minute).
    uint8_t east_west;             ///< East or west flag, east=0, west=1.

    uint8_t to_call[6];            ///< Destination call, 6 x ASCII, 0x20-padded.
    uint8_t to_ssid;               ///< Destination SSID, 0xff=None.

    uint8_t from_call[6];          ///< Source call, 6 x ASCII, 0x20-padded.
    uint8_t from_ssid;             ///< Source SSID, 0xff=None.

    // byte 0x24
    uint8_t path[20];              ///< Path string, upto 20 ASCII chars, 0-padded.
    uint8_t _pad56;                ///< Pad-byte 0x00.

    // byte 0x39
    char table;                    ///< ASCII-char for APRS icon table, ie. '/' or '\' for primary
                                   ///  and alternate icon table respectively.
    char icon;                     ///< ASCII-char of APRS map icon.
    uint8_t power;                 ///< Transmit power.
    uint8_t prewave_delay;         ///< Prewave delay in 10ms steps.

    // bytes 0x3d
    uint8_t _unknown61;            ///< Unknown, set to 01.
    uint8_t _unknown62;            ///< Unknown, set to 03.
    uint8_t _unknown63;            ///< Unknown, set to ff.

    /** Returns @c true, if the APRS setting is vaild. That is, it has a valid frequency,
     * destination and source calls. */
    bool isValid() const;

    /** Decodes the transmit frequency. */
    double getFrequency() const;
    /** Encodes the given frequency. */
    void setFrequency(double freq);

    /** Decodes the auto TX period. */
    int getAutoTXInterval() const;
    /** Encodes the auto TX period. */
    void setAutoTxInterval(int sec);

    /** Decodes the manual TX interval in seconds. */
    int getManualTXInterval() const;
    /** Encodes the manual TX interval in seconds. */
    void setManualTxInterval(int sec);

    /** Decodes the destination call. */
    QString getDestination() const;
    /** Encodes the given destination call. */
    void setDestination(const QString &call, uint8_t ssid);

    /** Decodes the destination SSID. */
    QString getSource() const;
    /** Encodes the destination SSID. */
    void setSource(const QString &call, uint8_t ssid);

    /** Decodes the APRS path. */
    QString getPath() const;
    /** Encodes the given APRS path. */
    void setPath(const QString &path);

    /** Decodes the TX signaling. */
    Signaling::Code getSignaling() const;
    /** Encodes the TX signaling. */
    void setSignaling(Signaling::Code signaling);

    /** Decodes the transmit power. */
    Channel::Power getPower() const;
    /** Encodes the given transmit power. */
    void setPower(Channel::Power pwr);

    /** Decodes the APRS map icon. */
    APRSSystem::Icon getIcon() const;
    /** Encodes the specified map icon. */
    void setIcon(APRSSystem::Icon icon);

    /** Configures this APRS system from the given generic config. */
    void fromAPRSSystem(APRSSystem *sys);
    /** Constructs a generic APRS system configuration from this APRS system. */
    APRSSystem *toAPRSSystem();
    /** Links the transmit channel within the generic APRS system based on the transmit frequency
     * defined within this APRS system. */
    void linkAPRSSystem(APRSSystem *sys, CodeplugContext &ctx);
  };

  /** Implements the binary representation of a roaming channel within the codeplug.
   *
   * Memmory layout of roaming channel (0x20byte):
   * @verbinclude d878uvroamingchannel.txt */
  struct __attribute__((packed)) roaming_channel_t {
    uint32_t rx_frequency;         ///< RX frequency 8-digit BCD big-endian as MMMkkkHH.
    uint32_t tx_frequency;         ///< TX frequency 8-digit BCD big-endian as MMMkkkHH.
    uint8_t colorcode;             ///< Colorcode 1-16.
    uint8_t timeslot;              ///< Timeslot, 0=TS1, 1=TS2.
    uint8_t name[16];              ///< Channel name, 16byte ASCII 0-terminated.
    uint8_t _unused26[6];          ///< Unused, set to 0x00

    /** Decodes the RX frequency. */
    double getRXFrequency() const;
    /** Encodes the given RX frequency. */
    void setRXFrequency(double f);
    /** Decodes the TX frequency. */
    double getTXFrequency() const;
    /** Encodes the given TX frequency. */
    void setTXFrequency(double f);
    /** Returns the time-slot of the roaming channel. */
    DigitalChannel::TimeSlot getTimeslot() const;
    /** Sets the time-slot of the roaming channel. */
    void setTimeslot(DigitalChannel::TimeSlot ts);
    /** Returns the color-code [0-15] of the roaming channel. */
    uint getColorCode() const;
    /** Sets the color-code [0-15] of the roaming channel. */
    void setColorCode(uint8_t cc);
    /** Decodes the name of the roaming channel. */
    QString getName() const;
    /** Encodes the name of the roaming channel. */
    void setName(const QString &name);

    /** Constructs a roaming channel from the given digital channel. */
    void fromChannel(DigitalChannel *ch);
    /** Constructs/Searches a matching DigitalChannel for this roaming channel. */
    DigitalChannel *toChannel(CodeplugContext &ctx);
  };

  /** Represents a roaming zone within the binary codeplug.
   *
   * Memmory layout of roaming zone (0x80byte):
   * @verbinclude d878uvroamingzone.txt */
  struct __attribute__((packed)) roaming_zone_t {
    uint8_t channels[64];          ///< List of roaming channel indices 0-based, 0xff=unused/end-of-list.
    uint8_t name[16];              ///< Roaming zone name, 16b ASCII 0x00 padded.
    uint8_t _unused80[48];         ///< Unused, set to 0x00.

    /** Returns the name of the roaming zone. */
    QString getName() const;
    /** Sets the name of the roaming zone. */
    void setName(const QString &name);

    /** Assembles a binary representation of the given RoamingZone instance.*/
    void fromRoamingZone(RoamingZone *zone, const QHash<DigitalChannel *, int> &map);
    /** Constructs a @c RoamingZone instance from this configuration. */
    RoamingZone *toRoamingZone();
    /** Links the given RoamingZone. */
    bool linkRoamingZone(RoamingZone *zone, CodeplugContext &ctx);
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
  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config, CodeplugContext &ctx);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());
};

#endif // D878UVCODEPLUG_HH
