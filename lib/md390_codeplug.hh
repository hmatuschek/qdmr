#ifndef MD390CODEPLUG_HH
#define MD390CODEPLUG_HH

#include "tyt_codeplug.hh"

/** Device specific implementation of the codeplug for the TyT MD-390(U/V).
 *
 * The codeplug consists of a single segment. It starts at address @c 0x002000 and ends at
 * address @c 0x040000.
 *
 * @section md390cpl Codeplug structure within radio
 * The codeplug structure is reverse engineered almost completely and can be programmed from
 * scratch. That is, it is not neccessary to update an existing codeplug on the radio.
 * <table>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">Segment 0x002000-0x040000</th></tr>
 *  <tr><td>0x002000</td> <td>0x00200c</td> <td>0x0000c</td> <td>Timestamp see @c TyTCodeplug::TimestampElement.</td></tr>
 *  <tr><td>0x00200c</td> <td>0x002040</td> <td>0x00034</td> <td>Reserved, filled with 0xff. </td></tr>
 *  <tr><td>0x002100</td> <td>0x002140</td> <td>0x00040</td> <td>Button config, see @c TyTCodeplug::ButtonSettingsElement.</td></tr>
 *  <tr><td>0x002140</td> <td>0x002180</td> <td>0x00040</td> <td>Reserved, filled with 0xff.</td></tr>
 *  <tr><td>0x002040</td> <td>0x0020f0</td> <td>0x000b0</td> <td>General settings see @c TyTCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>0x002180</td> <td>0x0059c0</td> <td>0x03840</td> <td>50 Text messages @ 0x120 bytes each, see @c UV390Codeplug::message_t.</td></tr>
 *  <tr><td>0x0059c0</td> <td>0x005a70</td> <td>0x000b0</td> <td>??? Privacy keys, see @c TyTCodeplug::EncryptionElement.</td></tr>
 *  <tr><td>0x005a70</td> <td>0x005a80</td> <td>0x00010</td> <td>Unknown settings</td></tr>
 *  <tr><td>0x005a80</td> <td>0x005f90</td> <td>0x00510</td> <td>???Emergency Systems, see @c TyTCodeplug::EmergencySystemElement.</td></td>
 *  <tr><td>0x005f80</td> <td>0x00ec20</td> <td>0x008ca</td> <td>1000 contacts, see @c TyTCodeplug::ContactElement.</td></tr>
 *  <tr><td>0x00ec20</td> <td>0x0149e0</td> <td>0x05dc0</td> <td>250 RX Group lists @ 0x60 bytes each, see @c TyTCodeplug::GroupListElement.</td></tr>
 *  <tr><td>0x0149e0</td> <td>0x018860</td> <td>0x03e80</td> <td>250 Zones @ 0x40 bytes each, see @c TyTCodeplug::ZoneElement.</td></tr>
 *  <tr><td>0x018860</td> <td>0x01edf0</td> <td>0x06590</td> <td>250 Scanlists @ 0x68 bytes each, see @c TyTCodeplug::ScanListElement.</td></tr>
 *  <tr><td>0x01ee00</td> <td>0x02e800</td> <td>0x0fa00</td> <td>1000 channels, see @c TyTCodeplug::ChannelElement.</td></tr>
 *  <tr><td>0x03ec40</td> <td>0x03ed40</td> <td>0x00100</td> <td>16 GPS systems @ 0x10 bytes each, see @c TyTCodeplug::GPSSystemElement.</td></tr>
 * </table>
 *
 * @ingroup md390 */

class MD390Codeplug : public TyTCodeplug
{
  Q_OBJECT

public:
  /** Extends the common @c TyTCodeplug::ChannelElement to implement the MD-390 specific settings.
   *
   * Memory layout of the channel (size 0x0040 bytes):
   * @verbinclude md390_channel.txt */
  class ChannelElement: public TyTCodeplug::ChannelElement
  {
  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    void clear();

    /** Returns @c true if the squelch is 'tight'. */
    virtual bool tightSquelchEnabled() const;
    /** Enables/disables tight squelch. */
    virtual void enableTightSquelch(bool enable);

    /** Returns @c true if the 'compressed UDP data header' is enabled. */
    virtual bool compressedUDPHeader() const;
    /** Enables/disables 'compressed UDP data header'. */
    virtual void enableCompressedUDPHeader(bool enable);

    /** Returns @c true if the reversed burst is enabled. */
    virtual bool reverseBurst() const;
    /** Enables/disables reverse burst. */
    virtual void enableReverseBurst(bool enable);

    /** Returns the power of this channel. */
    virtual Channel::Power power() const;
    /** Sets the power of this channel. */
    virtual void setPower(Channel::Power pwr);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj() const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual void fromChannelObj(const Channel *c, Context &ctx);
  };

public:
  /** Empty constructor. */
  explicit MD390Codeplug(QObject *parent=nullptr);

  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());

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

  void clearMenuSettings();
  void clearTextMessages();
  void clearPrivacyKeys();
  void clearEmergencySystems();
};

#endif // MD390CODEPLUG_HH
