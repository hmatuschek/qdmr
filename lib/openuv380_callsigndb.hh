#ifndef OPENUV380CALLSIGNDB_HH
#define OPENUV380CALLSIGNDB_HH

#include "opengd77base_callsigndb.hh"
#include "userdatabase.hh"

/** Represents and encodes the binary format for the call-sign database within the radio.
 *
 * The memory layout of the call-sign DB is relatively simple. The DB starts at address
 * @c 0x00030000 with a maximum size of @c 0x00040000. The first 12bytes form the DB header
 * (see @c OpenGD77CallsignDB::userdb_t) followed by the DB entries (see
 * @c OpenGD77CallsignDB::userdb_entry_t).
 *
 * The entries can be of variable size. The size of each entry is encoded in the header. QDMR uses
 * a fixed size of 19bytes per entry. The entries must be sorted in ascending order to allow for an
 * efficient binary search. No index table is used here.
 *
 * @ingroup ogd77 */
class OpenUV380CallsignDB : public OpenGD77BaseCallsignDB
{
  Q_OBJECT

public:
  class DatabaseEntryElement: public OpenGD77BaseCallsignDB::DatabaseEntryElement
  {
  public:
    /** Constructor. */
    DatabaseEntryElement(uint8_t *ptr);

    /** The size of the entry. */
    static constexpr unsigned int size() { return 0x001b; }

    void clear() override;

    /** Encodes the text. */
    void setText(const QString &text) override;

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      // The length of the text.
      static constexpr unsigned int textLength() { return 32; }
    };
  };

public:
  /** Constructor.
   * @param extended If @c true, some extended callsign db memory is used. */
  explicit OpenUV380CallsignDB(bool extended, QObject *parent=nullptr);

  static constexpr unsigned int size0() { return 0x040000; }
  static constexpr unsigned int size1() { return 0xd28000; }

  /** Encodes as many entries as possible of the given user-database. */
  bool encode(UserDatabase *calldb, const Flags &selection=Flags(),
              const ErrorStack &err=ErrorStack());

public:
  /** Some limits of the callsign DB. */
  struct Limit: public OpenGD77BaseCallsignDB::Limit {
    /// Number of entries, segment 0.
    static constexpr unsigned int entries0() {
      return (size0()-DatabaseHeaderElement::size())/DatabaseEntryElement::size();
    }
    /// Number of entries, segment 1.
    static constexpr unsigned int entries1() {
      return size1()/DatabaseEntryElement::size();
    }
    static constexpr unsigned int entries() {
      return entries0() + entries1();
    }
  };

protected:
  /** Some internal offsets within the callsign db. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int header()   { return 0x050000; }
    static constexpr unsigned int entries0() { return header() + DatabaseHeaderElement::size(); }
    static constexpr unsigned int entries1() { return 0x0d8000; }
    /// @endcond
  };
};

#endif // OPENGD77CALLSIGNDB_HH
