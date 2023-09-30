#ifndef GD73CODEPLUG_HH
#define GD73CODEPLUG_HH

#include "codeplug.hh"


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
 *    @c GD73Codeplig::EncryptionKeyBank</td></tr>
 *  <tr><td>0x2196f</td> <td>0x21e80</td> <td>0x0511</td>  <td>Message bank, see
 *    @c GD73Codeplug::MessageBankElement</td></tr>
 *  <tr><td>0x21e80</td> <td>0x21e94</td> <td>0x0014</td>  <td>4 DTMF systems, see
 *    @c GD73Codeplug::DTMFSystemBankElement</td></tr>
 *  <tr><td>0x21e94</td> <td>0x21f24</td> <td>0x0090</td>  <td>16 DTMF codes, see
 *    @c GD73Codeplug::DTMFCodeBankElement</td></tr>
 *  <tr><td>0x21f24</td>  <td>0x21fc4</td> <td>0x00a0</td>  <td>32 DTMF PTT settings, see
 *    @c GD73Codeplug::DTMFPTTSettingsBank</td></tr>
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
