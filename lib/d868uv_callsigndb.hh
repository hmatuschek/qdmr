#ifndef D868UVCALLSIGNDB_HH
#define D868UVCALLSIGNDB_HH

#include "callsigndb.hh"
#include "d868uv_codeplug.hh"

/** Represents and encodes the binary format for the call-sign database within a AnyTone AT-D868UV
 * radio.
 *
 * <table>
 *  <tr><th colspan="3">Callsign database</th></tr>
 *  <tr><th>Start</th>    <th>Size</th>        <th>Content</th></tr>
 *  <tr><td>04000000</td> <td>max. 186a00</th> <td>Index of callsign entries. Follows the same
 *   weird format as @c D868UVCodeplug::contact_map_t. Sorted by ID. Empty entries set to
 *   0xffffffffffffffff.</td></tr>
 *  <tr><td>044c0000</td> <td>unknown</td>     <td>Database limits, see @c limits_t.</td></tr>
 *  <tr><td>04500000</td> <td>unknown</td>     <td>The actual DB entries, each entry is of
 *   variable size but shares the same header, see @c entry_t. Order arbitrary.
 *   Filled with 0x00.</td></tr>
 * </table>
 *
 * @ingroup d868uv */
class D868UVCallsignDB : public CallsignDB
{
  Q_OBJECT

public:
  /** Represents the header of an entry in the callsign database.
   * Each entry is of variable size. That is, every entry has a common header containing the
   * number, call-type etc. All strings, that is contact name, city, callsign, state, country and
   * comment are 0x00 terminated strings in a lists.
   *
   * Max length for name is 16, city is 15, callsign is 8, state is 16, country is 16 and
   * comment is 16, excluding terminating 0x00.
   *
   * Memory layout of encoded Callsign/User database entry (variable size, min 0x000c-0x0064):
   * @verbinclude d868uv_callsigndbentry.txt
   */
  class EntryElement: public Codeplug::Element
  {
  public:
    /** Notification tones for callsign entry. */
    enum class RingTone {
      Off = 0,
      Tone = 1,
      Online = 2
    };

  protected:
    /** Hidden constructor. */
    EntryElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit EntryElement(uint8_t *ptr);

    void clear();

    /** Sets the call type. */
    virtual void setCallType(DMRContact::Type type);
    /** Sets the DMR ID number. */
    virtual void setNumber(unsigned num);
    /** Set/clear friend flag. */
    virtual void setFriendFlag(bool set);
    /** Sets the ring tone. */
    virtual void setRingTone(RingTone tone);
    /** Sets the entry content. */
    virtual void setContent(const QString &name, const QString &city, const QString &call,
                            const QString &state, const QString &country, const QString &comment);

    /** Constructs a database entry from the given user.
     * @returns The size of the entry. */
    virtual unsigned fromUser(const UserDatabase::User &user);

    /** Computes the size of the database entry for the given user. */
    static unsigned size(const UserDatabase::User &user);
  };

  /** Represents a bank of call-sign DB entries. */
  class EntryBankElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    EntryBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit EntryBankElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x000186a0; }

    void clear();

    /** Returns the i-th element of the bank. */
    uint8_t *entry(unsigned int i) const;
  };

  /** Same index entry used by the codeplug to map normal digital contacts to an contact index. Here
   * it maps to the byte offset within the database entries. */
  typedef D868UVCodeplug::ContactMapElement IndexEntryElement;

  /** Represents a bank of index entries. */
  class IndexBankElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    IndexBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit IndexBankElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0001f400; }

    void clear();

    /** Returns the i-th element of the bank. */
    uint8_t *entry(unsigned int i) const;
  };

  /** Stores some basic limits of the callsign db.
   *
   * Memory layout of encoded Callsign/User database entry (size 0x0010 bytes):
   * @verbinclude d868uv_callsigndblimit.txt */
  class LimitsElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    LimitsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    LimitsElement(uint8_t *ptr);

    /** Resets the limits. */
    void clear();

    /** Returns the number of entries in the DB. */
    virtual unsigned count() const;
    /** Sets the number of entries. */
    virtual void setCount(unsigned count);

    /** Returns the end-of-db address. */
    virtual unsigned endOfDB() const;
    /** Sets the end-of-db address. */
    virtual void setEndOfDB(unsigned addr);
    /** Sets the total size of the DB (updated end-of-db address). */
    virtual void setTotalSize(unsigned size);

  public:
    /** Returns the size of the encoded element. */
    static unsigned size();
  };


public:
  /** Constructor, does not allocate any memory yet. */
  explicit D868UVCallsignDB(QObject *parent=nullptr);

  /** Tries to encode as many entries of the given user-database. */
  bool encode(UserDatabase *db, const Selection &selection=Selection(),
              const ErrorStack &err=ErrorStack());

public:
  /** Some limits for the call-sign DB. */
  struct Limit {
    /// Specifies the max number of entries in the DB.
    static constexpr unsigned int entries() { return 200000; }
  };

protected:
  /** Some internal used offsets within the DB. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int index()                { return 0x04000000; }
    static constexpr unsigned int betweenIndexBanks()    { return 0x00040000; }
    static constexpr unsigned int callsigns()            { return 0x04500000; }
    static constexpr unsigned int betweenCallsignBanks() { return 0x00040000; }
    static constexpr unsigned int limits()               { return 0x044C0000; }
    /// @endcond
  };
};

#endif // D868UVCALLSIGNDB_HH
