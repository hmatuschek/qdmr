#ifndef GD77_CODEPLUG_HH
#define GD77_CODEPLUG_HH

#include "radioddity_codeplug.hh"
#include "signaling.hh"


/** Represents, encodes and decodes the device specific codeplug for a Radioddity GD-77.
 *
 * The GD-77 & GD-77S codeplugs are almost identical to the Radioddity/Baofeng @c RD5RCodeplug, in fact
 * the memory layout (see below) and almost all of the single components of the codeplug are encoded in
 * exactly the same way. Obviously, when Baofeng and Radioddity joint to create the RD5R,
 * Radioddity provided the firmware. However, there are some small subtile differences between
 * these two codeplug formats, requiring a separate class for the GD-77. For example, the contacts
 * and scan-lists swapped the addresses and the @c channel_t encoding analog and digital channels
 * for the codeplugs are identical except for the squelch settings. Thanks for that!
 *
 * @section gd77ver Matching firmware versions
 * This class implements the codeplug for the firmware version @b 4.03.06. The codeplug format usually
 * does not change much with firmware revisions, in particular not with older radios. Unfortunately,
 * it is not possible to detect the firmware version running on the device. Consequenly, only the
 * newest firmware version is supported. However, older revisions may still work.
 *
 * @section gd77cpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is divided into two segments.
 * The first segment starts at the address 0x00080 and ends at 0x07c00 while the second section
 * starts at 0x08000 and ends at 0x1e300.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x00080-0x07c00</th></tr>
 *  <tr><td>0x00080</td> <td>0x000e0</td> <td>0x0070</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x000e0</td> <td>0x000ec</td> <td>0x000c</td> <td>General settings, see @c RadioddityCodeplug::GeneralSettingsElement.</td></tr>
 *  <tr><td>0x000ec</td> <td>0x00108</td> <td>0x0028</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00108</td> <td>0x00128</td> <td>0x0020</td> <td>Button settings, see @c RadioddityCodeplug::ButtonSettingsElement.</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 preset message texts, see @c RadioddityCodeplug::MessageBankElement.</td></tr>
 *  <tr><td>0x01370</td> <td>0x01790</td> <td>0x0420</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01790</td> <td>0x02dd0</td> <td>0x1640</td> <td>64 scan lists, see @c GD77Codeplug::ScanListBankElement and @c GD77Codeplug::ScanListElement</td></tr>
 *  <tr><td>0x02dd0</td> <td>0x02f88</td> <td>0x01b8</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x02f88</td> <td>0x03388</td> <td>0x0400</td> <td>DTMF contacts, see RadioddityCodeplug::DTMFContactElement.</td></tr>
 *  <tr><td>0x03388</td> <td>0x03780</td> <td>0x03f8</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 channels (bank 0), see @c RadioddityCodeplug::ChannelBankElement and GD77Codeplug::ChannelElement</td></tr>
 *  <tr><td>0x05390</td> <td>0x07518</td> <td>0x2188</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07518</td> <td>0x07538</td> <td>0x0020</td> <td>Boot settings, see @c RadioddityCodeplug::BootSettingsElement.</td></tr>
 *  <tr><td>0x07538</td> <td>0x07540</td> <td>0x0008</td> <td>Menu settings, see @c RadioddityCodeplug::MenuSettingsElement.</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c RadioddityCodeplug::BootTextElement.</td></tr>
 *  <tr><td>0x07560</td> <td>0x07590</td> <td>0x0030</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07590</td> <td>0x075c8</td> <td>0x0038</td> <td>VFO A settings @c RadioddityCodeplug::VFOChannelElement</td></tr>
 *  <tr><td>0x075c8</td> <td>0x07600</td> <td>0x0038</td> <td>VFO B settings @c RadioddityCodeplug::VFOChannelElement</td></tr>
 *  <tr><td>0x07600</td> <td>0x07c00</td> <td>0x0600</td> <td>??? Unknown ???</td></tr>
 *
 *  <tr><th colspan="4">Second segment 0x08000-0x1e300</th></tr>
 *  <tr><td>0x08000</td> <td>0x08010</td> <td>0x0010</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x0af10</td> <td>0x2f00</td> <td>68 zones of 80 channels each, see @c RadioddityCodeplug::ZoneBankElement @c RadioddityCodeplug::ZoneElement.</td></tr>
 *  <tr><td>0x0af10</td> <td>0x0b1b0</td> <td>0x02a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x0b1b0</td> <td>0x17620</td> <td>0xc470</td> <td>Remaining 896 channels (bank 1-7), see @c RadioddityCodeplug::ChannelBankElement, @c GD77Codeplug::ChannelElement.</td></tr>
 *  <tr><td>0x17620</td> <td>0x1d620</td> <td>0x6000</td> <td>1024 contacts, see @c GD77Codeplug::ContactElement.</td></tr>
 *  <tr><td>0x1d620</td> <td>0x1eaa0</td> <td>0x1480</td> <td>64 RX group lists, see @c GD77Codeplug::GroupListBankElement, @c GD77Codeplug::GroupListElement.</td></tr>
 *  <tr><td>0x1eaa0</td> <td>0x1eb00</td> <td>0x0060</td> <td>??? Unknown ???</td></tr>
 * </table>
 * @ingroup gd77 */
class GD77Codeplug: public RadioddityCodeplug
{
	Q_OBJECT

public:
  /** Channel representation within the binary codeplug.
   *
   * Each channel requires 0x38b:
   * @verbinclude gd77_channel.txt */
  class ChannelElement: public RadioddityCodeplug::ChannelElement
  {
  public:
    /** ARTS send. */
    enum ARTSMode {
      ARTS_OFF      = 0,
      ARTS_TX       = 1,
      ARTS_RX       = 2,
      ARTS_BOTH     = 3
    };

    /** STE angle. */
    enum STEAngle {
      STE_FREQUENCY = 0,              ///< STE Frequency.
      STE_120DEG    = 1,              ///< 120 degree.
      STE_180DEG    = 2,              ///< 180 degree.
      STE_240DEG    = 3               ///< 240 degree.
    };

    /** PTT ID send. */
    enum PTTId {
      PTTID_OFF     = 0,
      PTTID_START   = 1,
      PTTID_END     = 2,
      PTTID_BOTH    = 3
    };

  protected:
    /** Hidden Constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    void clear();

    /** Returns the ARTS mode. */
    virtual ARTSMode artsMode() const;
    /** Sets the ARTS mode. */
    virtual void setARTSMode(ARTSMode mode);

    /** Returns the STE angle. */
    virtual STEAngle steAngle() const;
    /** Sets the STE angle. */
    virtual void setSTEAngle(STEAngle angle);

    /** Returns the PTT ID mode. */
    virtual PTTId pttIDMode() const;
    /** Sets the PTT ID mode. */
    virtual void setPTTIDMode(PTTId mode);

    /** Returns @c true if the squech type is tight. */
    virtual bool squelchIsTight() const;
    /** Enables/disables tight squelch. */
    virtual void enableTightSquelch(bool enable);

    /** Returns @c true if lone worker is enabled. */
    virtual bool loneWorker() const;
    /** Enables/disables lone worker. */
    virtual void enableLoneWorker(bool enable);

    /** Returns @c true if auto scan is enabled. */
    virtual bool autoscan() const;
    /** Enables/disables auto scan. */
    virtual void enableAutoscan(bool enable);
  };

  /** Specific codeplug representation of a DMR contact for the GD77.
   *
   * Memory layout of the contact (0x18b):
   * @verbinclude gd77_contact.txt
   */
  class ContactElement: public RadioddityCodeplug::ContactElement
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);

    /** Resets the contact. */
    void clear();

    /** Returns @c true if the contact is valid. */
    bool isValid() const;
    /** Marks the entry as valid/invalid. */
    virtual void markValid(bool valid=true);

    bool fromContactObj(const DMRContact *obj, Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some internal offsets. */
    struct Offset: RadioddityCodeplug::ContactElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int validFlag() { return 0x0017; }
      /// @endcond
    };
  };

  /** Represents an RX group list within the codeplug.
   *
   * The group list is encoded as (size 0x50b):
   * @verbinclude gd77_grouplist.txt */
  class GroupListElement: public RadioddityCodeplug::GroupListElement
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0050; }

    virtual bool linkRXGroupListObj(unsigned int ncnt, RXGroupList *lst, Context &ctx, const ErrorStack &err = ErrorStack()) const;
    virtual bool fromRXGroupListObj(const RXGroupList *lst, Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits for the group list. */
    struct Limit: public RadioddityCodeplug::GroupListElement {
      static constexpr unsigned int memberCount() { return 32; }  ///< Maximum number of entries.
    };
  };

  /** Table of RX group lists.
   *
   * The RX group list table constsis of a table of number of members per group list and the actual
   * list of RX group lists. The former also acts as a byte map for valid RX group lists. If 0, the
   * group list is disabled, if 1 the group list is empty, etc. So the entry is N+1, where N is the
   * number of entries per group list.
   *
   * Encoding of group list table:
   * @verbinclude gd77_grouplistbank.txt*/
  class GroupListBankElement: public RadioddityCodeplug::GroupListBankElement
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GroupListBankElement(uint8_t *ptr);

    /** The size of the group list bank. */
    static constexpr unsigned int size() { return 0x1480; }

    uint8_t *get(unsigned n) const;

  public:
    /** Some limits for the group list bank. */
    struct Limit: public RadioddityCodeplug::GroupListBankElement::Limit {
      static constexpr unsigned int groupListCount() { return 64; }   ///< Number of group lists.
    };
  };

  /** Represents a single scan list within the GD77 codeplug.
   *
   * Encoding of scan list (size: 0x58b):
   * @verbinclude gd77_scanlist.txt */
  class ScanListElement: public RadioddityCodeplug::ScanListElement
  {
  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);

    /** Resets the scan list. */
    void clear();
  };

  /** Bank of scan lists for the GD77.
   *
   * Encoding of scan list table (size 0x1640b):
   * @verbinclude gd77_scanlistbank.txt */
  class ScanListBankElement: public RadioddityCodeplug::ScanListBankElement
  {
  protected:
    /** Hidden constructor. */
    ScanListBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ScanListBankElement(uint8_t *ptr);

    /** The size of the scan list bank. */
    static constexpr unsigned int size() { return 0x1640; }

    /** Clears the scan list bank. */
    void clear();

    uint8_t *get(unsigned n) const;

  public:
    /** Some limits for the scan list bank. */
    struct Limit: public RadioddityCodeplug::ScanListBankElement::Limit {
      static constexpr unsigned int scanListCount() { return 64; } ///< Maximum number of scan lists.
    };

  protected:
    /** Internal offsets within the element. */
    struct Offset: public RadioddityCodeplug::ScanListBankElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int bytemap()   { return 0x0000; }
      static constexpr unsigned int scanLists() { return 0x0040; }
      /// @endcond
    };
  };

public:
  /** Constructs an empty codeplug for the GD-77. */
	explicit GD77Codeplug(QObject *parent=nullptr);

public:
  void clearGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearButtonSettings();
  bool encodeButtonSettings(Context &ctx, const Flags &flags, const ErrorStack &err=ErrorStack());
  bool decodeButtonSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearMessages();
  bool encodeMessages(Context &ctx, const Flags &flags, const ErrorStack &err=ErrorStack());
  bool decodeMessages(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearScanLists();
  bool encodeScanLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createScanLists(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkScanLists(Context &ctx, const ErrorStack &err=ErrorStack());

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

  void clearGroupLists();
  bool encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearEncryption();
  bool encodeEncryption(const Flags &flags, Context &ctx, const ErrorStack &err);
  bool createEncryption(Context &ctx, const ErrorStack &err);
  bool linkEncryption(Context &ctx, const ErrorStack &err);

public:
  /** Some limits for the GD77 codeplug. */
  struct Limit {
    static constexpr unsigned int dtmfContactCount() { return 32; }   ///< Maximum number of DTMF contacts.
    static constexpr unsigned int channelBankCount() { return 8; }    ///< Number of channel banks.
    static constexpr unsigned int channelCount()     { return 1024; } ///< Maximum number of channels.
    static constexpr unsigned int contactCount()     { return 1024; } ///< Maximum number of contacts.
  };

protected:
  /** Some internal offsets within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int settings()        { return 0x0000e0; }
    static constexpr unsigned int buttons()         { return 0x000108; }
    static constexpr unsigned int messages()        { return 0x000128; }
    static constexpr unsigned int encryption()      { return 0x001370; }
    static constexpr unsigned int scanListBank()    { return 0x001790; }
    static constexpr unsigned int dtmfContacts()    { return 0x002f88; }
    static constexpr unsigned int channelBank0()    { return 0x003780; } // Channels 1-128
    static constexpr unsigned int bootSettings()    { return 0x007518; }
    static constexpr unsigned int bootText()        { return 0x007540; }
    static constexpr unsigned int menuSettings()    { return 0x007538; }
    static constexpr unsigned int vfoA()            { return 0x007590; }
    static constexpr unsigned int vfoB()            { return 0x0075c8; }
    static constexpr unsigned int zoneBank()        { return 0x008010; }
    static constexpr unsigned int channelBank1()    { return 0x00b1b0; } // Channels 129-1024
    static constexpr unsigned int contacts()        { return 0x017620; }
    static constexpr unsigned int groupListBank()   { return 0x01d620; }
    /// @endcond
  };
};

#endif // GD77_CODEPLUG_HH
