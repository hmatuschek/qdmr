#ifndef D890UVCODEPLUG_HH
#define D890UVCODEPLUG_HH

#include "anytone_codeplug_gen2.hh"
#include "channel.hh"
#include "contact.hh"

class RXGroupList;
class ScanList;
class DMRRadioID;


/** Represents the binary codeplug of the AnyTone AT-D890UV.
 *
 * Models channels, zones, scan lists, DMR contacts, RX group lists and radio IDs.
 *
 * Writing to the flash of the radio erases the complete erase unit (0x40000 bytes) the data is
 * written to. Hence, every other region within an erase unit that gets written must be read first
 * and written back unchanged, see @c allocatePreserved(). Erase units holding no modelled region
 * are neither read nor written.
 *
 * Unless stated otherwise, layouts and template values are taken from a codeplug read from a
 * radio (FW V100) holding the factory configuration, i.e., written by the vendor. The addresses
 * of the regions are confirmed by USB captures of the vendor CPS reading and writing the radio.
 *
 * @ingroup d890uv */
class D890UVCodeplug : public AnytoneCodeplugGen2
{
  Q_OBJECT

public:
  /** Represents a channel record of 0x80 bytes.
   *
   * The factory channels differ only in frequency, mode and name. Their remaining bytes form the
   * template, see @c clear(). */
  class ChannelElement: public Element
  {
  public:
    /** Channel modes. */
    enum class Mode {
      Analog = 0, Digital = 1, MixedAnalog = 2, MixedDigital = 3
    };
    /** Repeater modes, i.e., sign of the TX offset. */
    enum class RepeaterMode {
      Simplex = 0, Positive = 1, Negative = 2
    };
    /** Sub-tone signaling modes. */
    enum class SignalingMode {
      None = 0, CTCSS = 1, DCS = 2
    };

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    /** Size of the record. */
    static constexpr unsigned int size() { return 0x0080; }

    /** Resets the record to the template. */
    void clear() override;

    /** Returns the RX frequency. */
    Frequency rxFrequency() const;
    /** Returns the TX frequency. */
    Frequency txFrequency() const;
    /** Sets RX and TX frequency. */
    void setFrequencies(const Frequency &rx, const Frequency &tx);
    /** Returns the channel mode. */
    Mode mode() const;
    /** Sets the channel mode. */
    void setMode(Mode mode);
    /** Returns the power. */
    Channel::Power power() const;
    /** Sets the power. */
    void setPower(Channel::Power power);
    /** Returns @c true for wide band. */
    bool wide() const;
    /** Sets the bandwidth. */
    void setWide(bool wide);
    /** Returns @c true if TX is disabled. */
    bool rxOnly() const;
    /** Disables TX. */
    void setRXOnly(bool enable);
    /** Returns the RX sub-tone. */
    SelectiveCall rxTone() const;
    /** Sets the RX sub-tone and the matching squelch mode. */
    void setRXTone(const SelectiveCall &tone);
    /** Returns the TX sub-tone. */
    SelectiveCall txTone() const;
    /** Sets the TX sub-tone. */
    void setTXTone(const SelectiveCall &tone);
    /** Returns the admit criterion as stored. */
    unsigned admit() const;
    /** Sets the admit criterion. */
    void setAdmit(unsigned admit);
    /** Returns the contact index. */
    unsigned contactIndex() const;
    /** Sets the contact index. */
    void setContactIndex(unsigned idx);
    /** Returns the radio ID index. */
    unsigned radioIDIndex() const;
    /** Sets the radio ID index. */
    void setRadioIDIndex(unsigned idx);
    /** Returns @c true if a scan list is set. */
    bool hasScanListIndex() const;
    /** Returns the scan list index. */
    unsigned scanListIndex() const;
    /** Sets the scan list index, @c 0xff clears it. */
    void setScanListIndex(unsigned idx);
    /** Returns @c true if a group list is set. */
    bool hasGroupListIndex() const;
    /** Returns the group list index. */
    unsigned groupListIndex() const;
    /** Sets the group list index, @c 0xff clears it. */
    void setGroupListIndex(unsigned idx);
    /** Returns the color code. */
    unsigned colorCode() const;
    /** Sets RX and TX color code. */
    void setColorCode(unsigned cc);
    /** Returns the time slot. */
    DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    void setTimeSlot(DMRChannel::TimeSlot ts);
    /** Returns the name. */
    QString name() const;
    /** Sets the name. */
    void setName(const QString &name);

    /** Creates a channel object, references are resolved by @c linkChannelObj. */
    Channel *toChannelObj() const;
    /** Resolves the references of the channel. */
    bool linkChannelObj(Channel *ch, Context &ctx) const;
    /** Encodes the given channel. */
    bool fromChannelObj(const Channel *ch, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of the name in UTF-16 characters. */
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Internal offsets. 0x00-0x22 match the D878UV channel record bit for bit. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int rxFrequency()       { return 0x0000; }
      static constexpr unsigned int txOffset()          { return 0x0004; }
      static constexpr Bit mode()                       { return {0x0008, 0}; }
      static constexpr Bit power()                      { return {0x0008, 2}; }
      static constexpr Bit bandwidth()                  { return {0x0008, 4}; }
      static constexpr Bit repeaterMode()               { return {0x0008, 6}; }
      static constexpr Bit rxSignaling()                { return {0x0009, 0}; }
      static constexpr Bit txSignaling()                { return {0x0009, 2}; }
      static constexpr Bit rxOnly()                     { return {0x0009, 5}; }
      static constexpr unsigned int txCTCSS()           { return 0x000a; }
      static constexpr unsigned int rxCTCSS()           { return 0x000b; }
      static constexpr unsigned int txDCS()             { return 0x000c; }
      static constexpr unsigned int rxDCS()             { return 0x000e; }
      static constexpr unsigned int contactIndex()      { return 0x0014; }
      static constexpr unsigned int radioIDIndex()      { return 0x0018; }
      static constexpr Bit squelchMode()                { return {0x0019, 4}; }
      static constexpr Bit admit()                      { return {0x001a, 0}; }
      static constexpr unsigned int scanListIndex()     { return 0x001b; }
      static constexpr unsigned int groupListIndex()    { return 0x001c; }
      static constexpr unsigned int colorCode()         { return 0x0020; }
      static constexpr Bit timeSlot()                   { return {0x0021, 0}; }
      static constexpr unsigned int txColorCode()       { return 0x0043; }
      static constexpr unsigned int name()              { return 0x0044; }
      /// @endcond
    };
  };

  /** Represents a scan list record of 0x200 bytes.
   *
   * The member list at 0x30 holds 50 channels: In the factory record (8 members), every unused
   * member reads 0xffff, except for a single 0x00 at 0x94, i.e., where member 50 would be. That
   * byte and the rest of the record are kept as in the template. */
  class ScanListElement: public Element
  {
  public:
    /** Constructor. */
    explicit ScanListElement(uint8_t *ptr);

    /** Size of the record. */
    static constexpr unsigned int size() { return 0x0200; }

    /** Resets the record to the template. */
    void clear() override;

    /** Returns the name. */
    QString name() const;
    /** Sets the name. */
    void setName(const QString &name);
    /** Returns the number of members. */
    unsigned int count() const;
    /** Returns the n-th member channel index. */
    unsigned int member(unsigned int n) const;
    /** Sets the n-th member channel index. */
    void setMember(unsigned int n, unsigned int idx);
    /** Returns the primary (@c n=0) or secondary (@c n=1) priority channel, if enabled.
     * Returns @c 0xffff if not set, @c 0 for the selected channel, otherwise the index plus one. */
    unsigned int priorityChannel(unsigned int n) const;
    /** Sets and enables the primary (@c n=0) or secondary (@c n=1) priority channel, encoded as
     * returned by @c priorityChannel. */
    void setPriorityChannel(unsigned int n, unsigned int code);

    /** Creates a scan list object, members are resolved by @c linkScanListObj. */
    ScanList *toScanListObj() const;
    /** Resolves the members of the scan list. */
    bool linkScanListObj(ScanList *lst, Context &ctx) const;
    /** Encodes the given scan list. */
    bool fromScanListObj(const ScanList *lst, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of the name in UTF-16 characters. */
      static constexpr unsigned int nameLength() { return 16; }
      /** Maximum number of members. */
      static constexpr unsigned int members()    { return 50; }
    };

  protected:
    /** Internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      // 0x01-0x0d match the D878UV scan list record.
      static constexpr unsigned int priorityChannels()  { return 0x0001; }
      static constexpr unsigned int primary()           { return 0x0002; }
      static constexpr unsigned int secondary()         { return 0x0004; }
      static constexpr unsigned int name()              { return 0x000e; }
      static constexpr unsigned int members()           { return 0x0030; }
      /// @endcond
    };
  };

  /** Represents a DMR contact record of 0xc8 bytes, zeros unless set. */
  class ContactElement: public Element
  {
  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);

    /** Size of the record. */
    static constexpr unsigned int size() { return 0x00c8; }

    /** Resets the record to the template. */
    void clear() override;

    /** Creates a contact object. */
    DMRContact *toContactObj() const;
    /** Encodes the given contact. */
    bool fromContactObj(const DMRContact *contact);

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of the name in UTF-16 characters. */
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int type()              { return 0x0000; }
      static constexpr unsigned int number()            { return 0x0002; }
      static constexpr unsigned int name()              { return 0x0006; }
      /// @endcond
    };
  };

  /** Represents an RX group list record of 0x120 bytes: 64 contact indices (u32, little endian,
   * 0xffffffff if unused) followed by the name. */
  class GroupListElement: public Element
  {
  public:
    /** Constructor. */
    explicit GroupListElement(uint8_t *ptr);

    /** Size of the record. */
    static constexpr unsigned int size() { return 0x0120; }

    /** Resets the record to the template. */
    void clear() override;

    /** Creates a group list object, members are resolved by @c linkGroupListObj. */
    RXGroupList *toGroupListObj() const;
    /** Resolves the members of the group list. */
    bool linkGroupListObj(RXGroupList *lst, Context &ctx) const;
    /** Encodes the given group list. */
    bool fromGroupListObj(const RXGroupList *lst, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of the name in UTF-16 characters. */
      static constexpr unsigned int nameLength() { return 16; }
      /** Maximum number of members. */
      static constexpr unsigned int members()    { return 64; }
    };

  protected:
    /** Internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members()           { return 0x0000; }
      static constexpr unsigned int name()              { return 0x0100; }
      /// @endcond
    };
  };

  /** Represents a radio ID record of 0x40 bytes. */
  class RadioIDElement: public Element
  {
  public:
    /** Constructor. */
    explicit RadioIDElement(uint8_t *ptr);

    /** Size of the record. */
    static constexpr unsigned int size() { return 0x0040; }

    /** Resets the record to the template. */
    void clear() override;

    /** Creates a radio ID object. */
    DMRRadioID *toRadioIDObj() const;
    /** Encodes the given radio ID. */
    bool fromRadioIDObj(const DMRRadioID *id);

  public:
    /** Some limits. */
    struct Limit {
      /** Maximum length of the name in UTF-16 characters. */
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Internal offsets. */
    struct Offset: Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int number()            { return 0x0000; }
      static constexpr unsigned int name()              { return 0x0004; }
      /// @endcond
    };
  };

  /** An untyped view onto a simple record, i.e., zone member lists, zone names and the contact ID
   * map, which are handled inline. */
  class RecordElement: public Element
  {
  public:
    /** Constructor. */
    RecordElement(uint8_t *ptr, size_t size);
  };

  /** Some limits of the codeplug. */
  struct Limit {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int channels()            { return 4000; }
    static constexpr unsigned int channelsPerBank()     { return 128; }
    static constexpr unsigned int zones()               { return 250; }
    static constexpr unsigned int zoneMembers()         { return 160; }
    static constexpr unsigned int zoneNameLength()      { return 16; }
    // The scan list bitmap has room for 250, but only the first 100 records are valid.
    static constexpr unsigned int scanLists()           { return 100; }
    static constexpr unsigned int contacts()            { return 10000; }
    static constexpr unsigned int contactsPerBank()     { return 1000; }
    static constexpr unsigned int contactsPerBlock()    { return 4; }
    static constexpr unsigned int groupLists()          { return 250; }
    static constexpr unsigned int radioIDs()            { return 250; }
    /// @endcond
  };

protected:
  /** Addresses of the regions within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int channelBitmap()       { return 0x03482a00; }
    static constexpr unsigned int channelBanks()        { return 0x01000000; }
    static constexpr unsigned int betweenChannelBanks() { return 0x00080000; }
    static constexpr unsigned int zoneBitmap()          { return 0x03482c00; }
    static constexpr unsigned int zoneChannels()        { return 0x02000000; }
    static constexpr unsigned int betweenZoneChannels() { return 0x00000200; }
    static constexpr unsigned int zoneNames()           { return 0x03600000; }
    static constexpr unsigned int betweenZoneNames()    { return 0x00000040; }
    static constexpr unsigned int radioIDBitmap()       { return 0x03482c40; }
    static constexpr unsigned int radioIDs()            { return 0x03680000; }
    static constexpr unsigned int scanListBitmap()      { return 0x03482c60; }
    static constexpr unsigned int scanLists()           { return 0x02100000; }
    static constexpr unsigned int groupListBitmap()     { return 0x03701510; }
    static constexpr unsigned int groupLists()          { return 0x03780000; }
    static constexpr unsigned int betweenGroupLists()   { return 0x00000200; }
    static constexpr unsigned int contactBitmap()       { return 0x03980000; }
    static constexpr unsigned int contactBanks()        { return 0x03a00000; }
    static constexpr unsigned int betweenContactBanks() { return 0x00080000; }
    // Sorted map from (BCD ID << 1 | group call) to the contact index, 8 bytes per entry,
    // terminated by 0xff. The radio resolves received talk groups through it.
    static constexpr unsigned int contactIDMap()        { return 0x03f00000; }
    /// @endcond
  };

  /** Sizes of the bitmaps. */
  struct Size {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int channelBitmap()       { return 0x0200; }
    static constexpr unsigned int zoneBitmap()          { return 0x0020; }
    static constexpr unsigned int radioIDBitmap()       { return 0x0020; }
    static constexpr unsigned int scanListBitmap()      { return 0x0020; }
    static constexpr unsigned int groupListBitmap()     { return 0x0020; }
    static constexpr unsigned int contactBitmap()       { return 0x0500; }
    static constexpr unsigned int zoneChannels()        { return 0x0200; }
    static constexpr unsigned int zoneName()            { return 0x0020; }
    static constexpr unsigned int contactIDMapEntry()   { return 0x0008; }
    /// @endcond
  };

public:
  /** Empty constructor. */
  explicit D890UVCodeplug(QObject *parent=nullptr);

  /** Clears the codeplug, leaving only the bitmaps. */
  void clear() override;

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const override;
  bool decode(Config *config, const ErrorStack &err=ErrorStack()) override;
  bool encode(Config *config, const Flags &flags=Flags(), const ErrorStack &err=ErrorStack()) override;

protected:
  /** Allocates the regions to preserve, see @c allocatePreserved(). */
  void allocateUpdated() override;
  /** Allocates the records marked in the bitmaps. */
  void allocateForDecoding() override;

  /** Allocates the bitmaps. */
  void allocateBitmaps();
  /** Allocates the regions, that are not modelled, but share an erase unit with modelled ones.
   * These get read from the radio before, and written back unchanged with the encoded codeplug.
   * Regions not allocated yet are filled with 0xff, i.e., erased flash. */
  void allocatePreserved();
  /** Returns @c true if all elements are located in erase units whose content is known, and all
   * known regions of these units are allocated, i.e., will be written back. */
  bool verifyEraseUnits(const ErrorStack &err=ErrorStack()) const;
  /** Sets the bitmaps for the indexed config. */
  void setBitmaps(Context &ctx);
  /** Allocates and resets the records for the indexed config. */
  void allocateForEncoding(Context &ctx);
  /** Returns @c true if no element overlaps the sector management blocks of the flash. */
  bool verifySectorSafety(const ErrorStack &err=ErrorStack()) const;

  /** Returns the address of the n-th channel. */
  static uint32_t channelAddress(unsigned int n);
  /** Returns the address of the n-th contact. */
  static uint32_t contactAddress(unsigned int n);
  /** Returns the address of the block of contacts holding the n-th contact. */
  static uint32_t contactBlockAddress(unsigned int n);
};

#endif // D890UVCODEPLUG_HH
