#ifndef HD1_CODEPLUG_HH
#define HD1_CODEPLUG_HH

#include "codeplug.hh"
#include "hd1_interface.hh"
#include "channel.hh"
#include "contact.hh"

class RXGroupList;
class Zone;
class DMRRadioID;

/** Represents the binary codeplug of the Ailunce HD1.
 *
 * The radio has three address spaces (see @c HD1Interface); each is one image of this
 * codeplug:
 * - image 0 (@c HD1Interface::Small, command @c 0x0f): settings, VFOs, channel bitmaps, radio IDs;
 * - image 1 (@c HD1Interface::Bulk, command @c 0x31, byte addresses = page × 0x400):
 *   priority contacts, channels, RX group lists, zones, messages;
 * - image 2 (@c HD1Interface::Info, command @c 0x0d): the 16-byte info block.
 *
 * Only the regions the manufacturer's software reads are allocated (@c Region::elements()); a
 * write sends them back in the order and frame sizes that software uses (@c Region::writes()).
 * Everything this class does not model is written back as read.
 *
 * @section hd1cpl Memory layout
 * <table>
 *  <tr><th>Bank</th><th>Address</th><th>Size</th><th>Content</th></tr>
 *  <tr><td>0x0f</td><td>0x0000, 0x00b0</td><td>2 × 0xb0</td><td>VFO A/B (channel records, not decoded)</td></tr>
 *  <tr><td>0x0f</td><td>0x2000</td><td>375</td><td>Channel usage bitmap, see @c ChannelBitmapElement</td></tr>
 *  <tr><td>0x0f</td><td>0x2177</td><td>375</td><td>Scan bitmap, see @c ScanBitmapElement</td></tr>
 *  <tr><td>0x0f</td><td>0x2950</td><td>0x60</td><td>General settings, see @c GeneralSettingsElement</td></tr>
 *  <tr><td>0x0f</td><td>0x4000</td><td>32 × 0x14</td><td>Radio IDs, see @c RadioIdElement</td></tr>
 *  <tr><td>0x31</td><td>0x1b84 × 0x400</td><td>1000 × 0x48</td><td>Priority contacts, see @c ContactElement</td></tr>
 *  <tr><td>0x31</td><td>0x1bcc × 0x400</td><td>3000 × 0xb0</td><td>Channels, see @c ChannelElement</td></tr>
 *  <tr><td>0x31</td><td>0x21d8 × 0x400</td><td>250 × 0x8e</td><td>RX group lists, see @c GroupListElement</td></tr>
 *  <tr><td>0x31</td><td>0x2200 × 0x400</td><td>250 × 0x91</td><td>Zones, see @c ZoneElement</td></tr>
 * </table>
 * An unused record is all @c 0xff. Multi-byte values are little endian; frequencies and tones
 * are BCD.
 *
 * Records are numbered by their slot, which the radio shows as the channel (zone, contact)
 * number. qdmr has no such numbers: encoding fills the slots in list order, so gaps in a
 * codeplug written by other software close up. Bytes qdmr does not model (GPS, encryption,
 * VOX, kill/wake-up codes, …) move with their channel: a channel takes them from the record
 * read from the radio with the same name and frequencies, wherever it was; an edited channel
 * keeps the record of its own slot. A TX contact qdmr cannot represent (an address book
 * number, an empty contact slot) is kept as well.
 *
 * Contacts are referenced as LE32 <tt>100000 + index</tt> (priority contact 1 is 100000), not
 * by DMR ID. A channel does not store which RX group list it uses: the manufacturer's software
 * copies the list's members into the channel's own member list. Decoding therefore links a
 * channel to the group list with the same members, or creates one (named after the channel)
 * for a custom member set. Encoding writes every RX group list to the radio's table (the first
 * 250) and copies each channel's list members into the channel; like the manufacturer's
 * software, only group calls go into a member list.
 *
 * The channel usage bitmap must match the channel table: the manufacturer's software hides a
 * channel whose bit is set and erases it on its next write. Encoding sets it, and the scan
 * bitmap, from the channels.
 *
 * @ingroup hd1 */
class HD1Codeplug : public Codeplug
{
  Q_OBJECT

public:
  /** One memory region: bank, start address (bytes) and size (bytes). */
  struct Region {
    HD1Interface::Bank bank;   ///< Address space.
    uint32_t address;          ///< Start address in bytes.
    uint32_t size;             ///< Size in bytes.
    /** The allocated, non-overlapping elements, sorted by bank and address. */
    static QList<Region> elements();
    /** The regions a codeplug read covers, in the order the manufacturer's software reads them. */
    static QList<Region> reads();
    /** The regions a codeplug write covers, in order (frame size from @c writeFrameSize()). */
    static QList<Region> writes();
    /** Frame size of the writes in @c bank. */
    static uint32_t writeFrameSize(HD1Interface::Bank bank);
  };

  /** Encodes a contact reference. */
  struct ContactRef {
    static constexpr uint32_t base() { return 100000; }      ///< Reference of priority contact 1.
    static constexpr uint32_t none() { return 0xffffffff; }  ///< No contact (member lists).
    /** No TX contact: the manufacturer's software writes 0 (and turns @c none() into 0). */
    static constexpr uint32_t noTXContact() { return 0; }
  };

  /** A channel (also the VFO records), 0xb0 bytes.
   *
   * <table>
   *  <tr><th>Offset</th><th>Size</th><th>Content</th></tr>
   *  <tr><td>0x00</td><td>128</td><td>Member list: 32 × LE32 contact reference, 0xffffffff = empty</td></tr>
   *  <tr><td>0x80</td><td>4</td><td>Unknown, always 0xffffffff</td></tr>
   *  <tr><td>0x84</td><td>14</td><td>Name, ASCII, zero padded</td></tr>
   *  <tr><td>0x92</td><td>1</td><td>GPS report contact index, 0 = none</td></tr>
   *  <tr><td>0x94</td><td>4</td><td>RX frequency, BCD LE32 in 10 Hz</td></tr>
   *  <tr><td>0x98</td><td>4</td><td>TX frequency, as RX</td></tr>
   *  <tr><td>0x9c</td><td>4</td><td>TX contact reference, 0 = none</td></tr>
   *  <tr><td>0xa0</td><td>1</td><td>Bit 7 GPS, 6 work alone, 5 VOX, 4 send GPS, 0–3 VOX level − 1</td></tr>
   *  <tr><td>0xa1</td><td>1</td><td>Bit 7 relay, 6 digital, 5 TX below RX, 4 TX above RX, 2–3 power (0 low, 1 mid,
   *    2 high), 1 talkaround, 0 scan add</td></tr>
   *  <tr><td>0xa2</td><td>1</td><td>GPS report interval index, 0 = off</td></tr>
   *  <tr><td>0xa3</td><td>1</td><td>TOT in 15 s steps, 0 = endless</td></tr>
   *  <tr><td>0xa4</td><td>2</td><td>RX tone, LE16: CTCSS as BCD in 0.1 Hz; DCS as 0x8000 | 0x4000
   *    (inverted) | three octal digits as nibbles; 0xffff = none</td></tr>
   *  <tr><td>0xa6</td><td>2</td><td>TX tone, as RX</td></tr>
   *  <tr><td>0xa8</td><td>1</td><td>Bit 7 promiscuous, 5–6 encryption, 0–3 key − 1</td></tr>
   *  <tr><td>0xa9</td><td>1</td><td>Bit 7 usually set on digital channels, 6 wide (25 kHz), 4–5
   *    analog TX authority, 2 fixed key decryption</td></tr>
   *  <tr><td>0xaa</td><td>1</td><td>Bits 4–7 colour code, 1–3 work mode (0 simplex, 1 repeater,
   *    4 double slot), 0 time slot 2</td></tr>
   *  <tr><td>0xab</td><td>1</td><td>Bits 6–7 TX authority (0 prohibit, 1 always, 2 colour code,
   *    3 channel free), 5 receive GPS, 0–4 radio ID index</td></tr>
   *  <tr><td>0xac</td><td>1</td><td>Kill code index, 0 = none</td></tr>
   *  <tr><td>0xae</td><td>1</td><td>Wake-up code index, 0 = none</td></tr>
   * </table> */
  class ChannelElement: public Element
  {
  public:
    /** TX permission of digital channels. */
    enum class TXAuthority { Prohibit = 0, Always = 1, ColorCode = 2, ChannelFree = 3 };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ChannelElement(uint8_t *ptr);

    /** Size of a record. */
    static constexpr unsigned int size() { return 0x00b0; }

    /** An empty record is all 0xff. */
    bool isValid() const override;
    /** Resets the record to the defaults of a new channel in the manufacturer's software. */
    void clear() override;

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);
    /** Returns the RX frequency. */
    virtual Frequency rxFrequency() const;
    /** Sets the RX frequency. */
    virtual void setRXFrequency(const Frequency &f);
    /** Returns the TX frequency. */
    virtual Frequency txFrequency() const;
    /** Sets the TX frequency and the offset direction bits; set the RX frequency first. */
    virtual void setTXFrequency(const Frequency &f);
    /** Returns @c true for a digital (DMR) channel. */
    virtual bool isDigital() const;
    /** Sets the channel type. */
    virtual void setDigital(bool digital);
    /** Returns the transmit power. */
    virtual Channel::Power power() const;
    /** Sets the transmit power. */
    virtual void setPower(Channel::Power power);
    /** Returns the scan-add flag. */
    virtual bool scanAdd() const;
    /** Sets the scan-add flag. */
    virtual void setScanAdd(bool add);
    /** Returns the talkaround flag. */
    virtual bool talkaround() const;
    /** Sets the talkaround flag. */
    virtual void enableTalkaround(bool enable);
    /** Returns the transmit timeout; infinite for "endless". */
    virtual Interval timeout() const;
    /** Sets the transmit timeout (15 s steps, at most 600 s; infinite = endless). */
    virtual void setTimeout(const Interval &tot);
    /** Returns the bandwidth. */
    virtual FMChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth. */
    virtual void setBandwidth(FMChannel::Bandwidth bw);
    /** Returns the RX sub tone. */
    virtual SelectiveCall rxTone() const;
    /** Sets the RX sub tone. */
    virtual void setRXTone(const SelectiveCall &tone);
    /** Returns the TX sub tone. */
    virtual SelectiveCall txTone() const;
    /** Sets the TX sub tone. */
    virtual void setTXTone(const SelectiveCall &tone);
    /** Returns the colour code. */
    virtual unsigned int colorCode() const;
    /** Sets the colour code. */
    virtual void setColorCode(unsigned int cc);
    /** Returns the time slot. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);
    /** Returns the TX authority (admit criterion of digital channels). */
    virtual TXAuthority txAuthority() const;
    /** Sets the TX authority. */
    virtual void setTXAuthority(TXAuthority auth);
    /** Returns the radio ID index (0-based). */
    virtual unsigned int radioIdIndex() const;
    /** Sets the radio ID index. */
    virtual void setRadioIdIndex(unsigned int idx);
    /** Returns the TX contact reference (see @c ContactRef). */
    virtual uint32_t txContact() const;
    /** Sets the TX contact reference. */
    virtual void setTXContact(uint32_t ref);
    /** Returns the member list (contact references, empty entries dropped). */
    virtual QList<uint32_t> members() const;
    /** Sets the member list (at most 32). */
    virtual void setMembers(const QList<uint32_t> &refs);

    /** Creates the channel object (without references). */
    virtual Channel *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links contact and radio ID; group lists and the scan list are linked by the bank. */
    virtual bool link(Channel *channel, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes the channel (references resolved through @c ctx). */
    virtual bool encode(const Channel *channel, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      static constexpr unsigned int nameLength() { return 14; }      ///< Name length.
      static constexpr unsigned int members() { return 32; }         ///< Member list length.
      static constexpr unsigned int maxTimeout() { return 600; }     ///< TOT in seconds.
    };

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int members() { return 0x00; }
      static constexpr unsigned int unknown80() { return 0x80; }
      static constexpr unsigned int name() { return 0x84; }
      static constexpr unsigned int rxFrequency() { return 0x94; }
      static constexpr unsigned int txFrequency() { return 0x98; }
      static constexpr unsigned int txContact() { return 0x9c; }
      static constexpr Bit digital() { return {0xa1, 6}; }
      static constexpr Bit offsetDown() { return {0xa1, 5}; }
      static constexpr Bit offsetUp() { return {0xa1, 4}; }
      static constexpr Bit power() { return {0xa1, 2}; }
      static constexpr Bit talkaround() { return {0xa1, 1}; }
      static constexpr Bit scanAdd() { return {0xa1, 0}; }
      static constexpr unsigned int timeout() { return 0xa3; }
      static constexpr unsigned int rxTone() { return 0xa4; }
      static constexpr unsigned int txTone() { return 0xa6; }
      static constexpr Bit digitalA9() { return {0xa9, 7}; }
      static constexpr Bit bandwidth() { return {0xa9, 6}; }
      static constexpr Bit colorCode() { return {0xaa, 4}; }
      static constexpr Bit timeSlot() { return {0xaa, 0}; }
      static constexpr Bit txAuthority() { return {0xab, 6}; }
      static constexpr Bit receiveGPS() { return {0xab, 5}; }
      static constexpr Bit radioId() { return {0xab, 0}; }
      /// @endcond
    };
  };

  /** The channel table: 3000 records from @c 0x31 @c 0x1bcc×0x400.
   * Also holds the matching of unmodelled bytes when channels move (see @c HD1Codeplug). */
  class ChannelBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ChannelBankElement(uint8_t *ptr);
    /** Size of the table. */
    static constexpr unsigned int size() { return Limit::channels()*ChannelElement::size(); }

    /** Returns the n-th channel record. */
    virtual ChannelElement channel(unsigned int n) const;

    /** Decodes all channels (and the synthetic scan list of the scan-add flags). */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links all channels, including their RX group lists. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes all channels. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      static constexpr unsigned int channels() { return 3000; }  ///< Channels.
    };
  };

  /** A priority contact, 0x48 bytes: DMR ID (LE32) at 0, call type at 4 (4 group, 5 private,
   * 6 all call), name at 5, city, province and country at 0x15, 0x25, 0x35 (16 characters each,
   * zero padded), 0xffffff at 0x45. */
  class ContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);
    /** Size of a record. */
    static constexpr unsigned int size() { return 0x0048; }
    bool isValid() const override;
    void clear() override;

    /** Returns the DMR ID. */
    virtual unsigned int number() const;
    /** Sets the DMR ID. */
    virtual void setNumber(unsigned int number);
    /** Returns the call type. */
    virtual DMRContact::Type type() const;
    /** Sets the call type. */
    virtual void setType(DMRContact::Type type);
    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);

    /** Creates the contact object. */
    virtual DMRContact *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes the contact; city, province and country are kept. */
    virtual bool encode(const DMRContact *contact, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      static constexpr unsigned int nameLength() { return 16; }  ///< Name length.
    };

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int number() { return 0x00; }
      static constexpr unsigned int type() { return 0x04; }
      static constexpr unsigned int name() { return 0x05; }
      static constexpr unsigned int unknown45() { return 0x45; }
      /// @endcond
    };
  };

  /** The priority contact table: 1000 records from @c 0x31 @c 0x1b84×0x400. */
  class ContactBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ContactBankElement(uint8_t *ptr);
    /** Size of the table. */
    static constexpr unsigned int size() { return Limit::contacts()*ContactElement::size(); }
    /** Returns the n-th contact record. */
    virtual ContactElement contact(unsigned int n) const;
    /** Decodes all contacts. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes all DMR contacts. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      static constexpr unsigned int contacts() { return 1000; }  ///< Contacts.
    };
  };

  /** An RX group list, 0x8e bytes: name (10 characters, padded with 0xff) at 0, 32 contact
   * references at 0x0a, 0xffffffff at 0x8a. */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit GroupListElement(uint8_t *ptr);
    /** Size of a record. */
    static constexpr unsigned int size() { return 0x008e; }
    bool isValid() const override;
    void clear() override;

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);
    /** Returns the members (contact references, empty entries dropped). */
    virtual QList<uint32_t> members() const;
    /** Sets the members (at most 32). */
    virtual void setMembers(const QList<uint32_t> &refs);

    /** Creates the group list object (without members). */
    virtual RXGroupList *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Adds the members. */
    virtual bool link(RXGroupList *list, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes name and members (group calls only). */
    virtual bool encode(const RXGroupList *list, Context &ctx, const ErrorStack &err=ErrorStack());

    /** The member references of @c list as the manufacturer's software stores them: group
     * calls only, at most 32. */
    static QList<uint32_t> memberRefs(const RXGroupList *list, Context &ctx);

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      static constexpr unsigned int nameLength() { return 10; }  ///< Name length.
      static constexpr unsigned int members() { return 32; }     ///< Members.
    };

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name() { return 0x00; }
      static constexpr unsigned int members() { return 0x0a; }
      static constexpr unsigned int unknown8a() { return 0x8a; }
      /// @endcond
    };
  };

  /** The RX group list table: 250 records from @c 0x31 @c 0x21d8×0x400. */
  class GroupListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit GroupListBankElement(uint8_t *ptr);
    /** Size of the table. */
    static constexpr unsigned int size() { return Limit::groupLists()*GroupListElement::size(); }
    /** Returns the n-th group list record. */
    virtual GroupListElement groupList(unsigned int n) const;
    /** Decodes all group lists. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links all group lists. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes the first 250 group lists. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      static constexpr unsigned int groupLists() { return 250; }  ///< Group lists.
    };
  };

  /** A zone, 0x91 bytes: channel count at 0, 64 × LE16 0-based channel indices (0xffff = none)
   * at 1, name (16 characters, zero padded) at 0x81. */
  class ZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ZoneElement(uint8_t *ptr);
    /** Size of a record. */
    static constexpr unsigned int size() { return 0x0091; }
    bool isValid() const override;
    void clear() override;

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);
    /** Returns the n-th channel index (0-based); 0xffff = empty. */
    virtual unsigned int channelIndex(unsigned int n) const;
    /** Sets the n-th channel index. */
    virtual void setChannelIndex(unsigned int n, unsigned int idx);
    /** Clears the n-th channel entry. */
    virtual void clearChannelIndex(unsigned int n);

    /** Creates the zone object (without channels). */
    virtual Zone *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Adds the channels. */
    virtual bool link(Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes name and channels (list A; zones are split before encoding). */
    virtual bool encode(const Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      static constexpr unsigned int nameLength() { return 16; }  ///< Name length.
      static constexpr unsigned int channels() { return 64; }    ///< Channels per zone.
    };

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count() { return 0x00; }
      static constexpr unsigned int channels() { return 0x01; }
      static constexpr unsigned int name() { return 0x81; }
      /// @endcond
    };
  };

  /** The zone table: 250 records from @c 0x31 @c 0x2200×0x400. */
  class ZoneBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit ZoneBankElement(uint8_t *ptr);
    /** Size of the table. */
    static constexpr unsigned int size() { return Limit::zones()*ZoneElement::size(); }
    /** Returns the n-th zone record. */
    virtual ZoneElement zone(unsigned int n) const;
    /** Decodes all zones. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links all zones. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes all zones. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      static constexpr unsigned int zones() { return 250; }  ///< Zones.
    };
  };

  /** A radio ID, 0x14 bytes: DMR ID (LE32) at 0, name (16 characters, zero padded) at 4. */
  class RadioIdElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RadioIdElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit RadioIdElement(uint8_t *ptr);
    /** Size of a record. */
    static constexpr unsigned int size() { return 0x0014; }
    bool isValid() const override;
    void clear() override;

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);
    /** Returns the DMR ID. */
    virtual unsigned int number() const;
    /** Sets the DMR ID. */
    virtual void setNumber(unsigned int number);

    /** Creates the radio ID object. */
    virtual DMRRadioID *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes the radio ID. */
    virtual bool encode(const DMRRadioID *id, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      static constexpr unsigned int nameLength() { return 16; }  ///< Name length.
    };

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int number() { return 0x00; }
      static constexpr unsigned int name() { return 0x04; }
      /// @endcond
    };
  };

  /** The radio ID table: 32 records from @c 0x0f @c 0x4000. */
  class RadioIdBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RadioIdBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit RadioIdBankElement(uint8_t *ptr);
    /** Size of the table. */
    static constexpr unsigned int size() { return Limit::ids()*RadioIdElement::size(); }
    /** Returns the n-th radio ID record. */
    virtual RadioIdElement id(unsigned int n) const;
    /** Decodes all radio IDs; the first one becomes the default ID. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Encodes all radio IDs. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: Element::Limit {
      static constexpr unsigned int ids() { return 32; }  ///< Radio IDs.
    };
  };

  /** The channel usage bitmap at @c 0x0f @c 0x2000: bit n (LSB first) is cleared when channel
   * n+1 exists. */
  class ChannelBitmapElement: public InvertedBitmapElement
  {
  public:
    /** Constructor. */
    explicit ChannelBitmapElement(uint8_t *ptr);
    /** Size of the bitmap. */
    static constexpr unsigned int size() { return (ChannelBankElement::Limit::channels()+7)/8; }
  };

  /** The scan bitmap at @c 0x0f @c 0x2177: bit n (LSB first) is cleared when channel n+1 exists
   * and is in the scan list (empty slots have the bit set). */
  class ScanBitmapElement: public InvertedBitmapElement
  {
  public:
    /** Constructor. */
    explicit ScanBitmapElement(uint8_t *ptr);
    /** Size of the bitmap. */
    static constexpr unsigned int size() { return (ChannelBankElement::Limit::channels()+7)/8; }
  };

  /** The general settings at @c 0x0f @c 0x2950, 0x60 bytes. Only squelch (0x20, 0–9) and voice
   * prompts (0x21 bit 5) are mapped to qdmr settings; the passwords at 0x00/0x06 and everything
   * else are kept. */
  class GeneralSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);
    /** Size of the element. */
    static constexpr unsigned int size() { return 0x0060; }

    /** Returns the squelch level (0–9). */
    virtual unsigned int squelch() const;
    /** Sets the squelch level. */
    virtual void setSquelch(unsigned int level);
    /** Returns @c true if voice prompts are on. */
    virtual bool voicePrompts() const;
    /** Enables voice prompts. */
    virtual void enableVoicePrompts(bool enable);

    /** Decodes the settings. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Encodes the settings. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some internal offsets. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int squelch() { return 0x20; }
      static constexpr Bit voicePrompts() { return {0x21, 5}; }
      /// @endcond
    };
  };

  /** Addresses of the tables. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr uint32_t channelBitmap() { return 0x2000; }            // bank 0x0f
    static constexpr uint32_t scanBitmap() { return 0x2177; }               // bank 0x0f
    static constexpr uint32_t generalSettings() { return 0x2950; }          // bank 0x0f
    static constexpr uint32_t radioIds() { return 0x4000; }                 // bank 0x0f
    static constexpr uint32_t contacts() { return 0x1b84*0x400; }           // bank 0x31
    static constexpr uint32_t channels() { return 0x1bcc*0x400; }           // bank 0x31
    static constexpr uint32_t groupLists() { return 0x21d8*0x400; }         // bank 0x31
    static constexpr uint32_t zones() { return 0x2200*0x400; }              // bank 0x31
    /// @endcond
  };

public:
  /** Empty codeplug with all regions allocated. */
  explicit HD1Codeplug(QObject *parent=nullptr);

  Config *preprocess(Config *config, const ErrorStack &err=ErrorStack()) const override;
  bool postprocess(Config *config, const ErrorStack &err=ErrorStack()) const override;

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const override;
  bool decode(Config *config, const ErrorStack &err=ErrorStack()) override;
  bool encode(Config *config, const Flags &flags=Flags(), const ErrorStack &err=ErrorStack()) override;

  /** Pointer to @c address of @c bank, or @c nullptr if not allocated. */
  uint8_t *bankData(HD1Interface::Bank bank, uint32_t address);

protected:
  /** Decodes all elements. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Links all elements. */
  virtual bool linkElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encodes all elements. */
  virtual bool encodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // HD1_CODEPLUG_HH
