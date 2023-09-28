#ifndef GD73CODEPLUG_HH
#define GD73CODEPLUG_HH

#include "codeplug.hh"


/** Represents, encodes and decodes the device specific codeplug for a Radioddity GD-73.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>     <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First segment 0x00000-0x22014</th></tr>
 *  <tr><td>0x00000</td> <td>0x00061</td> <td>0x0061</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00061</td> <td>0x?????</td> <td>0x0084</td> <td>Radio settings, see
 *    @c GD73Codeplug::SettingsElement</td></tr>
 *  <tr><td>0x0010b</td> <td>0x?????</td> <td>0x0c41</td> <td>Zone bank, see
 *    @c GD73Codeplug::ZoneBankElement</td></tr>´
 *  <tr><td>0x00d4c</td> <td>0x?????</td> <td>0x????</td> <td>Channel bank, see
 *    @c GD73Codeplug::ChannelBankElement</td></tr>´
 * </table>
 * @ingroup gd77 */
class GD73Codeplug : public Codeplug
{
public:
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
      /// @endcond
    };
  };


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
