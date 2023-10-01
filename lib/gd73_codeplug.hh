#ifndef GD73CODEPLUG_HH
#define GD73CODEPLUG_HH

#include "codeplug.hh"
#include "interval.hh"
#include "ranges.hh"


/** Represents, encodes and decodes the device specific codeplug for a Radioddity GD-73.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>     <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x00000-0x22014</th></tr>
 *  <tr><td>0x00000</td> <td>0x00061</td> <td>0x0061</td>  <td>Basic info, see
 *    @c GD73Codeplug::InformationElement</td></tr>
 *  <tr><td>0x00061</td> <td>0x0010b</td> <td>0x00aa</td>  <td>Radio settings, see
 *    @c GD73Codeplug::SettingsElement</td></tr>
 *  <tr><td>0x0010b</td> <td>0x00d4c</td> <td>0x0c41</td>  <td>Zone bank, see
 *    @c GD73Codeplug::ZoneBankElement</td></tr>
 *  <tr><td>0x00d4c</td> <td>0x1254e</td> <td>0x11802</td> <td>Channel bank, see
 *    @c GD73Codeplug::ChannelBankElement</td></tr>
 *  <tr><td>0x125ff</td> <td>0x1c201</td> <td>0x9c02</td>  <td>Contact bank, see
 *    @c GD73Codeplug::ContactBankElement</td></tr>
 *  <tr><td>0x1c201</td> <td>0x21310</td> <td>0x510f</td>  <td>Group list bank, see
 *    @c GD73Codeplug::GroupListBankElement</td></tr>
 *  <tr><td>0x21310</td> <td>0x21911</td> <td>0x0601</td>  <td>Scan list bank, see
 *    @c GD73Codeplug::ScanListBankElement</td></tr>
 *  <tr><td>0x21911</td> <td>0x2191f</td> <td>0x000e</td>  <td>DMR settings, see
 *    @c GD73Codeplug::DMRSettingsElement</td></tr>
 *  <tr><td>0x2191f</td> <td>0x2196f</td> <td>0x0050</td>  <td>16 encryption keys, see
 *    @c GD73Codeplug::EncryptionKeyBankElement</td></tr>
 *  <tr><td>0x2196f</td> <td>0x21e80</td> <td>0x0511</td>  <td>Message bank, see
 *    @c GD73Codeplug::MessageBankElement</td></tr>
 *  <tr><td>0x21e80</td> <td>0x21e94</td> <td>0x0014</td>  <td>4 DTMF systems, see
 *    @c GD73Codeplug::DTMFSystemBankElement</td></tr>
 *  <tr><td>0x21e94</td> <td>0x21f24</td> <td>0x0090</td>  <td>16 DTMF numbers, see
 *    @c GD73Codeplug::DTMFNumberBankElement</td></tr>
 *  <tr><td>0x21f24</td>  <td>0x21fc4</td> <td>0x00a0</td>  <td>32 DTMF PTT settings, see
 *    @c GD73Codeplug::DTMFPTTSettingBankElement</td></tr>
 *  <tr><td>0x21fc4</td>  <td>0x22014</td> <td>0x0050</td>  <td>Unused, filled with 0x00</td></tr>
 * </table>
 *
 * @ingroup gd73 */
class GD73Codeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Implements the information element.
   *
   * Memory representation of the element (size 000h bytes):
   * @verbinclude gd73_timestamp.txt */
  class InformationElement: public Element
  {
  protected:
    /** Hidden constructor. */
    InformationElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    InformationElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0061; }

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of serial number. */
      static constexpr unsigned int serial()                                { return 16; }
      /** Maximum length of model name. */
      static constexpr unsigned int modelName()                             { return 16; }
      /** Maximum length of device id. */
      static constexpr unsigned int deviceID()                              { return 16; }
      /** Maximum length of model number. */
      static constexpr unsigned int modelNumber()                           { return 16; }
      /** Maximum length of software version. */
      static constexpr unsigned int softwareVersion()                       { return 16; }
    };

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int frequencyRange()                        { return 0x0000; }
      static constexpr unsigned int dateCentury()                           { return 0x0001; }
      static constexpr unsigned int dateYear()                              { return 0x0002; }
      static constexpr unsigned int dateMonth()                             { return 0x0003; }
      static constexpr unsigned int dateDay()                               { return 0x0004; }
      static constexpr unsigned int dateHour()                              { return 0x0005; }
      static constexpr unsigned int dateMinute()                            { return 0x0006; }
      static constexpr unsigned int dateSecond()                            { return 0x0007; }
      static constexpr unsigned int serial()                                { return 0x0011; }
      static constexpr unsigned int modelName()                             { return 0x0021; }
      static constexpr unsigned int deviceID()                              { return 0x0031; }
      static constexpr unsigned int modelNumber()                           { return 0x0041; }
      static constexpr unsigned int softwareVersion()                       { return 0x0051; }
      /// @endcond
    };
  };


  /** Implements one of the 5 one-touch settings elements.
   *
   * Memory representation of the element (size 0005h bytes):
   * @verbinclude gd73_one_touch_element.txt */
  class OneTouchSettingElement: public Element
  {
  protected:
    /** Hidden constructor. */
    OneTouchSettingElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    OneTouchSettingElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0005; }
  };


  /** Implements the radio settings.
   *
   * Memory representation within the binary codeplug (size: ????h):
   * @verbinclude gd73_settings_element.txt.
   */
  class SettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    SettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    SettingsElement(uint8_t *ptr);

    /** Returns the size of the settings element. */
    static constexpr unsigned int size() { return 0x0084; }

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int dmrId()                                 { return 0x0020; }
      static constexpr unsigned int voxLevel()                              { return 0x0026; }
      static constexpr unsigned int squelchLevel()                          { return 0x0027; }
      static constexpr unsigned int tot()                                   { return 0x0028; }
      static constexpr unsigned int txInterrupt()                           { return 0x0029; }
      static constexpr unsigned int powerSave()                             { return 0x002a; }
      static constexpr unsigned int powerSaveTimeout()                      { return 0x002b; }
      static constexpr unsigned int readLockEnable()                        { return 0x002c; }
      static constexpr unsigned int writeLockEnable()                       { return 0x002d; }
      static constexpr unsigned int channelDisplayMode()                    { return 0x002f; }
      static constexpr unsigned int readLockPin()                           { return 0x0030; }
      static constexpr unsigned int writeLockPin()                          { return 0x0036; }
      static constexpr unsigned int dmrMicGain()                            { return 0x003d; }
      static constexpr unsigned int fmMicGain()                             { return 0x003f; }
      static constexpr unsigned int loneWorkerResponseTimeout()             { return 0x0040; }
      static constexpr unsigned int loneWorkerReminderPeriod()              { return 0x0042; }
      static constexpr unsigned int bootDisplayMode()                       { return 0x0043; }
      static constexpr unsigned int bootTextLine1()                         { return 0x0044; }
      static constexpr unsigned int bootTextLine2()                         { return 0x0064; }
      static constexpr unsigned int keyToneEnable()                         { return 0x0084; }
      static constexpr unsigned int keyToneVolume()                         { return 0x0085; }
      static constexpr unsigned int lowBatToneEnable()                      { return 0x0086; }
      static constexpr unsigned int lowBatToneVolume()                      { return 0x0087; }
      static constexpr unsigned int longPressDuration()                     { return 0x0088; }
      static constexpr unsigned int progFuncKey1ShortPress()                { return 0x008b; }
      static constexpr unsigned int progFuncKey1LongPress()                 { return 0x008c; }
      static constexpr unsigned int progFuncKey2ShortPress()                { return 0x008d; }
      static constexpr unsigned int progFuncKey2LongPress()                 { return 0x008e; }
      static constexpr unsigned int oneTouchSettings()                      { return 0x0090; }
      static constexpr unsigned int betweenOneTouchSettings()               { return OneTouchSettingElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single zone within the binary codeplug.
   *
   * Memory representation of the zone (size 0031h):
   * @verbinclude gd73_zone_element.txt */
  class ZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ZoneElement(uint8_t *ptr);

    /** Returns the size of the zone element. */
    static constexpr unsigned int size() { return 0x0031; }

  public:
    /** Some limits for the element. */
    struct Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength()                            { return 8; }
      /** Maximum number of channels per zone. */
      static constexpr unsigned int channelCount()                          { return 16; }
    };

  protected:
    /** Internal offsets within the zone element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int channeCount()                           { return 0x0020; }
      static constexpr unsigned int channelIndices()                        { return 0x0021; }
      static constexpr unsigned int betweenChannelIndices()                 { return 0x0002; }
      /// @endcond
    };
  };


  /** Implements the bank of zones.
   *
   * See also @c GD73Codeplug::ZoneElement.
   *
   * Memory representation of the zone bank (size ????h bytes):
   * @verbinclude gd73_zone_bank.txt */
  class ZoneBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ZoneBankElement(uint8_t *ptr);

    /** Returns the size of the zone bank element. */
    static constexpr unsigned int size() { return 0x0c41; }

  public:
    /** Some limits for the zone bank. */
    struct Limit {
      /** Maximum number of zones. */
      static constexpr unsigned int zoneCount()                             { return 64; }
    };

  protected:
    /** Internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int zoneCount()                             { return 0x0000; }
      static constexpr unsigned int zones()                                 { return 0x0001; }
      static constexpr unsigned int betweenZones()                          { return ZoneElement::size(); }
      /// @endcond
    };
  };


  /** Implements an FM/DMR channel.
   *
   * Memory representaion of the channel (size ):
   * @verbinclude gd73_channel_element.txt */
  class ChannelElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns the size of the channel element. */
    static constexpr unsigned int size() { return 0x0046; }

  public:
    /** Some limits for the channel. */
    struct Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength()                            { return 16; }
    };

  protected:
    /** Internal used offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int bandwidth()                             { return 0x0020; }
      static constexpr unsigned int scanList()                              { return 0x0021; }
      static constexpr unsigned int channelType()                           { return 0x0022; }
      static constexpr unsigned int talkaround()                            { return 0x0023; }
      static constexpr unsigned int rxOnly()                                { return 0x0024; }
      static constexpr unsigned int scanAutoStart()                         { return 0x0026; }
      static constexpr unsigned int rxFrequency()                           { return 0x0027; }
      static constexpr unsigned int txFrequency()                           { return 0x002b; }
      static constexpr unsigned int dtmfPTTSettingsIndex()                  { return 0x002f; }
      static constexpr unsigned int power()                                 { return 0x0030; }
      static constexpr unsigned int admid()                                 { return 0x0031; }
      static constexpr unsigned int rxToneMode()                            { return 0x0034; }
      static constexpr unsigned int rxCTCSS()                               { return 0x0035; }
      static constexpr unsigned int rxDCS()                                 { return 0x0036; }
      static constexpr unsigned int txToneMode()                            { return 0x0037; }
      static constexpr unsigned int txCTCSS()                               { return 0x0038; }
      static constexpr unsigned int txDCS()                                 { return 0x0039; }
      static constexpr unsigned int timeslot()                              { return 0x003c; }
      static constexpr unsigned int colorcode()                             { return 0x003d; }
      static constexpr unsigned int groupListIndex()                        { return 0x003e; }
      static constexpr unsigned int contactIndex()                          { return 0x0040; }
      static constexpr unsigned int emergencySystemIndex()                  { return 0x0042; }
      static constexpr unsigned int encryptionKeyIndex()                    { return 0x0044; }
      /// @endcond
    };
  };


  /** Implements the bank of channels within the binary codeplug.
   *
   * See @c GD73Codeplug::ChannelElement for details on how the channels are encoded.
   *
   * Memory representation of the channel bank (size 11802h bytes):
   * @verbinclude gd73_channel_bank.txt */
  class ChannelBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ChannelBankElement(uint8_t *ptr);

    /** Returns the size of the channel bank. */
    static constexpr unsigned int size() { return 0x11802; }

  public:
    /** Some limits for the channel bank. */
    struct Limit {
      /** Maximum number of channels. */
      static constexpr unsigned int channelCount()                          { return 1024; }
    };
  protected:
    /** Internal offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int channelCount()                          { return 0x0000; }
      static constexpr unsigned int channels()                              { return 0x0001; }
      static constexpr unsigned int betweenChannels()                       { return ChannelElement::size(); }
      /// @endcond
    };
  };


  /** Implements the contact element.
   *
   * Memory representation of the contact (size 9c02h bytes):
   * @verbinclude gd73_contact_bank.txt */
  class ContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x025; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength()                            { return 16; }
    };

  protected:
    /** Some internal offsets within the contact. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int type()                                  { return 0x0020; }
      static constexpr unsigned int id()                                    { return 0x0021; }
      /// @endcond
    };
  };


  /** Implements the contact bank within the codeplug.
   *
   * See @c GD73Codeplug::ContactElement for contact encoding.
   *
   * Memory representation of the contact bank (size 9c02h bytes):
   * @verbinclude gd73_contact_bank.txt */
  class ContactBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x9c02; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of contacts. */
      static constexpr unsigned int contactCount()                          { return 1024; }
    };

  protected:
    /** Some internal offsets within the contact bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int contactCount()                          { return 0x0000; }
      static constexpr unsigned int contacts()                              { return 0x0804; }
      static constexpr unsigned int betweenContacts()                       { return ContactElement::size(); }
      /// @endcond
    };
  };


  /** Encodes a group list.
   *
   * Memory representation of the group list (size 0053h bytes):
   * @verbinclude gd73_group_list_element.txt */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0053; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength()                          { return 8; }
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                         { return 33; }
    };

  protected:
    /** Some internal offsets within the group list. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int memberCount()                           { return 0x0010; }
      static constexpr unsigned int members()                               { return 0x0011; }
      static constexpr unsigned int betweenMembers()                        { return 0x0002; }
      /// @endcond
    };
  };


  /** Encodes the bank of group lists.
   *
   * See @c GD73Codeplug::GroupListElement for group list encoding.
   *
   * Memory representation of group list bank (size 510fh bytes):
   * @verbinclude gd73_group_list_bank.txt */
  class GroupListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x510f; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                         { return 250; }
    };

  protected:
    /** Some internal offsets within the group list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int memberCount()                           { return 0x0000; }
      static constexpr unsigned int members()                               { return 0x0001; }
      static constexpr unsigned int betweenMembers()                        { return GroupListElement::size(); }
      /// @endcond
    };
  };


  /** Implements a scan list.
   *
   * Memory representation of the scan list (size 005fh bytes):
   * @verbinclude gd73_scan_list_element.txt */
  class ScanListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x005f; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength()                          { return 8; }
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                         { return 32; }
    };

  protected:
    /** Some internal offsets within the scan list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()                                  { return 0x0000; }
      static constexpr unsigned int memberCount()                           { return 0x0010; }
      static constexpr unsigned int members()                               { return 0x0011; }
      static constexpr unsigned int betweenMembers()                        { return 0x0002; }
      static constexpr unsigned int priChannel1Mode()                       { return 0x0051; }
      static constexpr unsigned int priChannel2Mode()                       { return 0x0052; }
      static constexpr unsigned int priChannel1Zone()                       { return 0x0053; }
      static constexpr unsigned int priChannel2Zone()                       { return 0x0054; }
      static constexpr unsigned int priChannel1Channel()                    { return 0x0055; }
      static constexpr unsigned int priChannel2Channel()                    { return 0x0057; }
      static constexpr unsigned int txChannelMode()                         { return 0x0059; }
      static constexpr unsigned int txChannelZone()                         { return 0x005a; }
      static constexpr unsigned int txChannelChannel()                      { return 0x005b; }
      static constexpr unsigned int holdTime()                              { return 0x005d; }
      static constexpr unsigned int txHoldTime()                            { return 0x005e; }
      /// @endcond
    };
  };


  /** Implements the bank of scan lists.
   *
   * See @c GD73Codeplug::ScanListElement for the encoding of the single scan lists.
   *
   * Memory representation of the scan list bank (size 0601h bytes):
   * @verbinclude gd73_scan_list_bank.txt */
  class ScanListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ScanListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ScanListBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x601; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                           { return 16; }
    };

  protected:
    /** Some internal offsets within the scan list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int memberCount()                           { return 0x0000; }
      static constexpr unsigned int members()                               { return 0x0011; }
      static constexpr unsigned int betweenMembers()                        { return ScanListElement::size(); }
      /// @endcond
    };
  };


  /** Implements the DMR settings element.
   *
   * Memory representation of the settings (size 000eh bytes):
   * @verbinclude gd73_dmr_settings_element.txt */
  class DMRSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DMRSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DMRSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x000e; }

  public:
    /** Some limits. */
    struct Limit {
      /** The range of call hang times. */
      static constexpr TimeRange callHangTime() { return TimeRange{
          Interval::fromSeconds(1), Interval::fromSeconds(90)
        }; }
      /** The range of active wait times. */
      static constexpr TimeRange activeWaitTime() { return TimeRange{
          Interval::fromMilliseconds(120), Interval::fromMilliseconds(600)
        }; }
      /** The range of active retries. */
      static constexpr IntRange activeRetires() { return IntRange{ 1, 10}; }
      /** The maximum number of TX preambles. */
      static constexpr unsigned int txPreambles() { return 63; }
    };

  protected:
    /** Some internal offsets within the scan list bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int callHangTime()                          { return 0x0000; }
      static constexpr unsigned int activeWaitTime()                        { return 0x0001; }
      static constexpr unsigned int activeRetries()                         { return 0x0002; }
      static constexpr unsigned int txPreambles()                           { return 0x0003; }
      static constexpr unsigned int decodeDisableRadio()                    { return 0x0004; }
      static constexpr unsigned int decodeCheckRadio()                      { return 0x0005; }
      static constexpr unsigned int decodeEnableRadio()                     { return 0x0006; }
      /// @endcond
    };
  };


  /** Implements the encryption key element.
   *
   * Memory representation (size 0005h bytes):
   * @verbinclude gd73_encryption_key_element.txt */
  class EncryptionKeyElement: public Element
  {
  protected:
    /** Hidden constructor. */
    EncryptionKeyElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    EncryptionKeyElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x005; }


  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int size()        { return 0x0000; }
      static constexpr unsigned int key()         { return 0x0001; }
      /// @endcond
    };
  };


  /** Implements the encryption key-bank.
   *
   * Memory representation (size 0050h bytes):
   * @verbinclude gd73_encryption_key_bank.txt */
  class EncryptionKeyBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    EncryptionKeyBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    EncryptionKeyBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0050; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of keys. */
      static constexpr unsigned int keys()        { return 16; }
    };

  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int keys()        { return 0x0000; }
      static constexpr unsigned int betweenKeys() { return EncryptionKeyElement::size(); }
      /// @endcond
    };
  };


  /** Implements a message.
   * Memory representation (size 0051h bytes):
   * @verbinclude gd73_message_element.txt */
  class MessageElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    MessageElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x051; }


  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int size()        { return 0x0000; }
      static constexpr unsigned int text()        { return 0x0001; }
      /// @endcond
    };
  };


  /** Implements the message bank element.
   *
   * See @c GD73Codeplug::MessageElement for encoding of the single messages.
   *
   * Memory representation of the bank (size 0511h bytes):
   * @verbinclude gd73_message_bank.txt */
  class MessageBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    MessageBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x511; }

  public:
    /** Some limits. */
    struct Limit {
      /** The maximum number of members. */
      static constexpr unsigned int memberCount()                           { return 16; }
    };

  protected:
    /** Some internal offsets within the message bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int memberCount()                           { return 0x0000; }
      static constexpr unsigned int members()                               { return 0x0001; }
      static constexpr unsigned int betweenMembers()                        { return MessageElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single DTMF system.
   * Memory representation of the system (size 0005h bytes):
   * @verbinclude gd73_dtmf_system_element.txt */
  class DTMFSystemElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFSystemElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFSystemElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x005; }

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum preamble duration in ms. */
      static constexpr Interval preambleDuration() { return Interval::fromMilliseconds(1000); }
      /** Range for tone duration. */
      static constexpr TimeRange toneDuration() { return TimeRange{
          Interval::fromMilliseconds(30), Interval::fromMilliseconds(1900)
        }; }
      /** Range for pause duration. */
      static constexpr TimeRange pauseDuration() { return TimeRange{
          Interval::fromMilliseconds(30), Interval::fromMilliseconds(1900)
        }; }
      /** Rang of dead time. */
      static constexpr TimeRange deadTime() { return TimeRange{
          Interval::fromMilliseconds(200), Interval::fromSeconds(33)
        }; }
    };

  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int sidetone()                              { return 0x0000; }
      static constexpr unsigned int preambleDuration()                      { return 0x0001; }
      static constexpr unsigned int toneDuration()                          { return 0x0002; }
      static constexpr unsigned int pauseDuration()                         { return 0x0003; }
      static constexpr unsigned int deadTime()                              { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the bank of 4 DTMF systems.
   * @c GD73Codeplug::DTMFSystemElement for encoding of each system. */
  class DTMFSystemBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFSystemBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFSystemBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0014; }

  public:
    /** Some limits. */
    struct Limit {
      /** The number of members. */
      static constexpr unsigned int memberCount()                           { return 4; }
    };

  protected:
    /** Some internal offsets within the message bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()                               { return 0x0000; }
      static constexpr unsigned int betweenMembers()                        { return DTMFSystemElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single DTMF number.
   * Memory representation of the DTMF number (size 000ah bytes):
   * @verbinclude gd73_dtmf_code_element.txt */
  class DTMFNumberElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFNumberElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFNumberElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x000a; }

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum number of digita. */
      static constexpr unsigned int digits() { return 16; }
    };

  protected:
    /** Internal used offsets within the bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count()                                 { return 0x0000; }
      static constexpr unsigned int digits()                                { return 0x0001; }
      /// @endcond
    };
  };


  /** Implements the bank of 16 DTMF numbers.
   * @c GD73Codeplug::DTMFNumberElement for encoding of each system. */
  class DTMFNumberBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFNumberBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFNumberBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0090; }

  public:
    /** Some limits. */
    struct Limit {
      /** The number of members. */
      static constexpr unsigned int memberCount()                           { return 16; }
    };

  protected:
    /** Some internal offsets within the number bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()                               { return 0x0000; }
      static constexpr unsigned int betweenMembers()                        { return DTMFNumberElement::size(); }
      /// @endcond
    };
  };


  /** Implements a single DTMF PTT setting.
   * Memory representation of the DTMF PTT setting (size 0005h bytes):
   * @verbinclude gd73_dtmf_ptt_settings.txt */
  class DTMFPTTSettingElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFPTTSettingElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFPTTSettingElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0005; }

  protected:
    /** Internal used offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int systemIndex()                           { return 0x0000; }
      static constexpr unsigned int pttIDType()                             { return 0x0001; }
      static constexpr unsigned int pttIDMode()                             { return 0x0002; }
      static constexpr unsigned int connectIDIndex()                        { return 0x0003; }
      static constexpr unsigned int disconnectIDIndex()                     { return 0x0004; }
      /// @endcond
    };
  };


  /** Implements the bank of 32 DTMF PTT settings.
   * @c GD73Codeplug::DTMFPTTSettingElement for encoding of each element. */
  class DTMFPTTSettingBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFPTTSettingBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFPTTSettingBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00a0; }

  public:
    /** Some limits. */
    struct Limit {
      /** The number of members. */
      static constexpr unsigned int memberCount()                           { return 32; }
    };

  protected:
    /** Some internal offsets within the number bank. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()                               { return 0x0000; }
      static constexpr unsigned int betweenMembers()                        { return DTMFPTTSettingElement::size(); }
      /// @endcond
    };
  };

public:
  /** Default constructor. */
  explicit GD73Codeplug(QObject *parent = nullptr);

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;
  bool decode(Config *config, const ErrorStack &err=ErrorStack());
  bool encode(Config *config, const Flags &flags=Flags(), const ErrorStack &err=ErrorStack());

protected:
  /** Internal used offsets within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int settings()                                { return 0x00061; }
    static constexpr unsigned int zoneBank()                                { return 0x0010b; }
    /// @endcond
  };
};


#endif // GD73CODEPLUG_HH
