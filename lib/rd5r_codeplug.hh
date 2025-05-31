#ifndef RD5R_CODEPLUG_HH
#define RD5R_CODEPLUG_HH

#include <QObject>
#include "radioddity_codeplug.hh"
#include "signaling.hh"

class Channel;

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
 * another method. Here a simple counter precedes the messages, indicating how many valid messages
 * there are. All in all, a rather inconsistent way of representing variable length lists in the
 * codeplug. I would guess, that over time, different people/teams worked on different revisions
 * of the firmware. It must have been a real nightmare to Serge Vakulenko reverse-engineering this
 * codeplug.
 *
 * @section rd5rcpl Codeplug structure within radio
 * This implementation targets firmware version 2.1.6.
 *
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
 *  <tr><td>0x00088</td> <td>0x0008e</td> <td>0x0006</td> <td>Timestamp, see @c RadioddityCodeplug::TimestampElement.</td></tr>
 *  <tr><td>0x0008e</td> <td>0x000e0</td> <td>0x0052</td> <td>CPS, firmware, DSP version numbers (not touched).</td></tr>
 *  <tr><td>0x000e0</td> <td>0x00108</td> <td>0x0028</td> <td>General settings, see @c RadioddityCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>0x00108</td> <td>0x00128</td> <td>0x0020</td> <td>Button settings, see @c RadioddityCodeplug::ButtonSettingsElement.</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 preset message texts, see @c RadioddityCodeplug::MessageBankElement.</td></tr>
 *  <tr><td>0x01370</td> <td>0x01588</td> <td>0x0218</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01588</td> <td>0x01788</td> <td>0x0200</td> <td>??? 32 Emergency systems ???</td></tr>
 *  <tr><td>0x01788</td> <td>0x02f88</td> <td>0x1800</td> <td>256 contacts, see @c RadioddityCodeplug::ContactElement.</td></tr>
 *  <tr><td>0x02f88</td> <td>0x03388</td> <td>0x0400</td> <td>32 DTMF contacts, see @c RadioddityCodeplug::DTMFContactElement.</td></tr>
 *  <tr><td>0x03388</td> <td>0x03780</td> <td>0x03f8</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 channels (bank 0), see @c RadioddityCodeplug::ChannelBankElement
 *                                                            and @c RD5RCodeplug::ChannelElement.</td></tr>
 *  <tr><td>0x05390</td> <td>0x07518</td> <td>0x2188</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07518</td> <td>0x07538</td> <td>0x0020</td> <td>Boot settings, see @c RadioddityCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>0x07538</td> <td>0x07540</td> <td>0x0008</td> <td>Menu settings, see @c RadioddityCodeplug::MenuSettingsElement.</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c RadioddityCodeplug::BootTextElement.</td></tr>
 *  <tr><td>0x07560</td> <td>0x07590</td> <td>0x0030</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07590</td> <td>0x075c8</td> <td>0x0038</td> <td>VFO A settings @c RadioddityCodeplug::ChannelElement</td></tr>
 *  <tr><td>0x075c8</td> <td>0x07600</td> <td>0x0038</td> <td>VFO B settings @c RadioddityCodeplug::ChannelElement</td></tr>
 *  <tr><td>0x07600</td> <td>0x07c00</td> <td>0x0600</td> <td>??? Unknown ???</td></tr>
 *
 *  <tr><th colspan="4">Second segment 0x08000-0x1e300</th></tr>
 *  <tr><td>0x08000</td> <td>0x08010</td> <td>0x0010</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x0af10</td> <td>0x2f00</td> <td>250 zones, see @c RadioddityCodeplug::ZoneBankElement.</td></tr>
 *  <tr><td>0x0af10</td> <td>0x0b1b0</td> <td>0x02a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x0b1b0</td> <td>0x17620</td> <td>0xc470</td> <td>Remaining 896 channels (bank 1-7), see @c RadioddityCodeplug::ChannelBankElement
 *                                                            and @c RD5RCodeplug::ChannelElement.</td></tr>
 *  <tr><td>0x17620</td> <td>0x1cd10</td> <td>0x56f0</td> <td>250 scan lists, see @c RadioddityCodeplug::ScanListBankElement</td></tr>
 *  <tr><td>0x1cd10</td> <td>0x1d620</td> <td>0x0910</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x1d620</td> <td>0x1e2a0</td> <td>0x0c80</td> <td>64 RX group lists, see @c RadioddityCodeplug::GroupListBankElement</td></tr>
 *  <tr><td>0x1e2a0</td> <td>0x1e300</td> <td>0x0060</td> <td>??? Unknown ???</td></tr>
 * </table>
 *
 * @ingroup rd5r */
class RD5RCodeplug : public RadioddityCodeplug
{
  Q_OBJECT

public:
  /** Implements the specialization of the Radioddity channel for the RD5R radio.
   *
   * Memory layout of encoded channel:
   * @verbinclude rd5r_channel.txt */
  class ChannelElement: public RadioddityCodeplug::ChannelElement
  {
  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    void clear();

    /** Returns the squelch level. */
    virtual unsigned squelch() const;
    /** Sets the squelch level. */
    virtual void setSquelch(unsigned level);

    bool fromChannelObj(const Channel *c, Context &ctx, const ErrorStack &err=ErrorStack());
    Channel *toChannelObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    bool linkChannelObj(Channel *c, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  protected:
    /** Internal offsets within the channel element. */
    struct Offset: RadioddityCodeplug::ChannelElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int squelch() { return 0x0037; }
      /// @endcond
    };
  };


  /** Implements the timestamp for RD-5R codeplugs.
   *
   * Encoding of messages (size: 0x0006b):
   * @verbinclude rd5r_timestamp.txt */
  class TimestampElement: Element
  {
  protected:
    /** Hidden constructor. */
    TimestampElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit TimestampElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~TimestampElement();

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0006; }

    /** Resets the timestamp. */
    void clear();

    /** Returns the time stamp. */
    virtual QDateTime get() const;
    /** Sets the time stamp. */
    virtual void set(const QDateTime &ts=QDateTime::currentDateTime());

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int year() { return 0x0000; }
      static constexpr unsigned int month() { return 0x0002; }
      static constexpr unsigned int day() { return 0x0003; }
      static constexpr unsigned int hour() { return 0x0004; }
      static constexpr unsigned int minute() { return 0x0005; }
      /// @endcond
    };
  };


  /** Implements the encoding/decoding of encryption keys for the RD-5R radio.
   * @note The RD5R only supports a single basic DMR encryption key with a fixed value!
   *
   * Encoding of encryption keys (size: 0x00088):
   * @verbinclude radioddity_privacy.txt */
  class EncryptionElement: public RadioddityCodeplug::EncryptionElement
  {
  public:
    /** Constructor. */
    EncryptionElement(uint8_t *ptr);

    bool isBasicKeySet(unsigned n) const;
    QByteArray basicKey(unsigned n) const;
    void setBasicKey(unsigned n, const QByteArray &key);
  };

public:
  /** Empty constructor. */
  RD5RCodeplug(QObject *parent=0);

  void clear();

public:
  bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears the time-stamp in the codeplug. */
  virtual void clearTimestamp();
  /** Sets the time-stamp. */
  virtual bool encodeTimestamp(const ErrorStack &err=ErrorStack());

  void clearGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearButtonSettings();
  bool encodeButtonSettings(Context &ctx, const Flags &flags, const ErrorStack &err=ErrorStack());
  bool decodeButtonSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearMessages();
  bool encodeMessages(Context &ctx, const Flags &flags, const ErrorStack &err=ErrorStack());
  bool decodeMessages(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearContacts();
  bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearDTMFContacts();
  bool encodeDTMFContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createDTMFContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearChannels();
  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearBootSettings();
  void clearMenuSettings();

  void clearBootText();
  bool encodeBootText(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeBootText(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearVFOSettings();

  void clearZones();
  bool encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createZones(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearScanLists();
  bool encodeScanLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createScanLists(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearGroupLists();
  bool encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearEncryption();
  bool encodeEncryption(const Flags &flags, Context &ctx, const ErrorStack &err);
  bool createEncryption(Context &ctx, const ErrorStack &err);
  bool linkEncryption(Context &ctx, const ErrorStack &err);

public:
  /** Some limits for the codeplug. */
  struct Limit {
    static constexpr unsigned int channelBankCount() { return 8; }   ///< The number of channel banks.
    static constexpr unsigned int channelCount() { return 1024; }    ///< Maximum number of channels in the codeplug.
    static constexpr unsigned int contactCount() { return 256; }     ///< Maximum number of DMR contacts.
    static constexpr unsigned int dtmfContactCount() { return 32; }  ///< Maximum number of DTMF contacts.
    static constexpr unsigned int zoneCount() { return 250; }        ///< Maximum number of zones.
  };

protected:
  /** Some internal offsets within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int timestamp()     { return 0x000088; }
    static constexpr unsigned int settings()      { return 0x0000e0; }
    static constexpr unsigned int buttons()       { return 0x000108; }
    static constexpr unsigned int messages()      { return 0x000128; }
    static constexpr unsigned int encryption()    { return 0x001370; }
    static constexpr unsigned int contacts()      { return 0x001788; }
    static constexpr unsigned int dtmfContacts()  { return 0x002f88; }
    static constexpr unsigned int channelBank0()  { return 0x003780; }
    static constexpr unsigned int bootSettings()  { return 0x007518; }
    static constexpr unsigned int menuSettings()  { return 0x007538; }
    static constexpr unsigned int bootText()      { return 0x007540; }
    static constexpr unsigned int vfoA()          { return 0x007590; }
    static constexpr unsigned int vfoB()          { return 0x0075c8; }
    static constexpr unsigned int zoneBank()      { return 0x008010; }
    static constexpr unsigned int channelBank1()  { return 0x00b1b0; }
    static constexpr unsigned int scanListBank()  { return 0x017620; }
    static constexpr unsigned int groupListBank() { return 0x01d620; }
    /// @endcond
  };
};

#endif // RD5R_CODEPLUG_HH
