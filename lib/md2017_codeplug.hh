#ifndef MD2017_CODEPLUG_HH
#define MD2017_CODEPLUG_HH

#include "tyt_codeplug.hh"
#include "uv390_codeplug.hh"

/** Device specific implementation of the codeplug for the TyT MD-2017.
 *
 * The codeplug consists of two segments. The first segment starts at address @c 0x002000 and ends at
 * address @c 0x040000. The second segment starts at address @c 0x110000 and ends at @c 0x1a0000. The
 * segments must align with @c 0x400 (1024 bytes).
 *
 * @section md2017cpl Codeplug structure within radio
 * The codeplug structure is reverse engineered almost completely and can be programmed from
 * scratch. That is, it is not neccessary to update an existing codeplug on the radio.
 * <table>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x002000-0x040000</th></tr>
 *  <tr><td>0x002000</td> <td>0x00200c</td> <td>0x0000c</td> <td>Timestamp see @c TyTCodeplug::TimestampElement.</td></tr>
 *  <tr><td>0x00200c</td> <td>0x002040</td> <td>0x00034</td> <td>Reserved, filled with 0xff. </td></tr>
 *  <tr><td>0x002040</td> <td>0x0020f0</td> <td>0x000b0</td> <td>General settings see @c TyTCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>0x0020f0</td> <td>0x002100</td> <td>0x00010</td> <td>Menu settings, see @c UV390Codeplug::menu_t</td></tr>
 *  <tr><td>0x002100</td> <td>0x002140</td> <td>0x00040</td> <td>Button config, see @c UV390Codeplug::buttons_t.</td></tr>
 *  <tr><td>0x002140</td> <td>0x002180</td> <td>0x00040</td> <td>Reserved, filled with 0xff.</td></tr>
 *  <tr><td>0x002180</td> <td>0x0059c0</td> <td>0x03840</td> <td>50 Text messages @ 0x120 bytes each, see @c UV390Codeplug::message_t.</td></tr>
 *  <tr><td>0x0059c0</td> <td>0x005a70</td> <td>0x000b0</td> <td>Privacy keys, see @c UV390Codeplug::privacy_t.</td></tr>
 *  <tr><td>0x005a50</td> <td>0x005f60</td> <td>0x00510</td> <td>Emergency Systems, see @c UV390Codeplug::emergency_t.</td></td>
 *  <tr><td>0x005f60</td> <td>0x00ec20</td> <td>0x08cc0</td> <td>Reserved, filled with 0xff.</td></td>
 *  <tr><td>0x00ec20</td> <td>0x0149e0</td> <td>0x05dc0</td> <td>250 RX Group lists @ 0x60 bytes each, see @c TyTCodeplug::GroupListElement.</td></tr>
 *  <tr><td>0x0149e0</td> <td>0x018860</td> <td>0x03e80</td> <td>250 Zones @ 0x40 bytes each, see @c TyTCodeplug::ZoneElement.</td></tr>
 *  <tr><td>0x018860</td> <td>0x01edf0</td> <td>0x06590</td> <td>250 Scanlists @ 0x68 bytes each, see @c TyTCodeplug::ScanListElement.</td></tr>
 *  <tr><td>0x01edf0</td> <td>0x02ef00</td> <td>0x10110</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x02ef00</td> <td>0x02ef40</td> <td>0x00040</td> <td>VFO A channel, see @c TyTCodeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef40</td> <td>0x02ef80</td> <td>0x00040</td> <td>VFO B channel, see @c TyTCodeplug::VFOChannelElement.</td></tr>
 *  <tr><td>0x02ef80</td> <td>0x02f000</td> <td>0x00080</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x02f000</td> <td>0x02f010</td> <td>0x00010</td> <td>Some unkown settings like current channel, see @c TyTCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>0x02f010</td> <td>0x031000</td> <td>0x01ff0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x031000</td> <td>0x03eac0</td> <td>0x0dac0</td> <td>250 Zone-extensions @ 0xe0 bytes each, see @c TyTCodeplug::ZoneExtElement.</td></tr>
 *  <tr><td>0x03eac0</td> <td>0x03ec40</td> <td>0x00180</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x03ec40</td> <td>0x03ed40</td> <td>0x00100</td> <td>16 GPS systems @ 0x10 bytes each, see @c TyTCodeplug::GPSSystemElement.</td></tr>
 *  <tr><td>0x03ed40</td> <td>0x040000</td> <td>0x012c0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><th colspan="4">Second segment 0x110000-0x1a0000</th></tr>
 *  <tr><td>0x110000</td> <td>0x13ee00</td> <td>0x2ee00</td> <td>3000 Channels @ 0x40 bytes each, see @c TyTCodeplug::ChannelElement.</td></tr>
 *  <tr><td>0x13ee00</td> <td>0x140000</td> <td>0x01200</td> <td>Reserved, filled with @c 0xff. </td></tr>
 *  <tr><td>0x140000</td> <td>0x197e40</td> <td>0x57e40</td> <td>10000 Contacts @ 0x24 bytes each, see @c TyTCodeplug::ContactElement.</td></tr>
 *  <tr><td>0x197e40</td> <td>0x1a0000</td> <td>0x081c0</td> <td>Reserved, filled with @c 0xff. </td></tr>
 * </table>
 *
 * @ingroup md2017 */
class MD2017Codeplug : public TyTCodeplug
{
  Q_OBJECT

public:
  /** Reuse TyT MD-UV390 channel element. */
  typedef UV390Codeplug::ChannelElement ChannelElement;

  /** Reuse TyT MD-UV390 VFO channel element. */
  typedef UV390Codeplug::VFOChannelElement VFOChannelElement;

  /** Reuse TyT MD-UV390 zone extension element. */
  typedef UV390Codeplug::ZoneExtElement ZoneExtElement;

  /** Reuse TyT MD-UV390 general settings element. */
  typedef UV390Codeplug::GeneralSettingsElement GeneralSettingsElement;

  /** Reuse TyT MD-UV390 boot settings element. */
  typedef UV390Codeplug::BootSettingsElement BootSettingsElement;

public:
  /** Constructor. */
  explicit MD2017Codeplug(QObject *parent = nullptr);
  /** Destructor. */
  virtual ~MD2017Codeplug();

public:
  void clearTimestamp();
  bool encodeTimestamp();

  void clearGeneralSettings();
  bool encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Config *config, const ErrorStack &err=ErrorStack());

  void clearChannels();
  bool encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearContacts();
  bool encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createContacts(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());

  void clearZones();
  bool encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createZones(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearGroupLists();
  bool encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGroupLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearScanLists();
  bool encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createScanLists(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearPositioningSystems();
  bool encodePositioningSystems(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createPositioningSystems(Config *config, Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkPositioningSystems(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearButtonSettings();
  bool encodeButtonSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeButtonSetttings(Config *config, const ErrorStack &err=ErrorStack());

  /** Resets/clears the boot settings. */
  virtual void clearBootSettings();
  void clearMenuSettings();
  void clearTextMessages();
  void clearPrivacyKeys();
  void clearEmergencySystems();
  /** Resets VFO settings. */
  virtual void clearVFOSettings();

};

#endif // MD2017_CODEPLUG_HH
