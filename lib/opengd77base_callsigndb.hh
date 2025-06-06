#ifndef OPENGD77BASECALLSIGNDB_HH
#define OPENGD77BASECALLSIGNDB_HH

#include "codeplug.hh"
#include "callsigndb.hh"
#include "userdatabase.hh"

/** Represents and encodes the binary format for all call-sign databases within OpenGD77 radios.
 * @ingroup ogd77 */
class OpenGD77BaseCallsignDB : public CallsignDB
{
  Q_OBJECT

public:
  /** Represents a user-db entry within the binary codeplug.
   *
   * Memory representation of the call-sign DB entry (size 0x13 bytes):
   * @verbinclude opengd77_callsign_db_entry.txt */
  struct __attribute__((packed)) userdb_entry_t {
    uint32_t number;                    ///< DMR ID stored in BCD little-endian.
    char name[15];                      ///< Call or name, up to 15 ASCII chars, 0x00 padded.

    /** Constructor. */
    userdb_entry_t();
    /** Resets the entry. */
    void clear();

    /** Returns the DMR ID number. */
    uint32_t getNumber() const;
    /** Sets the DMR ID number. */
    void setNumber(uint32_t number);

    /** Returns the name of the entry. */
    QString getName() const;
    /** Sets the name of the entry, 15b max.
     * The name gets truncated if longer than 15b. */
    void setName(const QString &name);

    /** Encodes the given user. */
    void fromEntry(const UserDatabase::User &user);
  };


  /** Represents a single encoded database entry.
   * Consists of a DMR ID and compressed text. The text size is fixed to 16 chars. Each
   * char is encoded using a 6bit table. Thus 16 chars are stored in 12 bytes. */
  class DatabaseEntryElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    DatabaseEntryElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DatabaseEntryElement(uint8_t *ptr);

    /** The size of the entry. */
    static constexpr unsigned int size() { return 0x000f; }

    void clear();

    /** Encodes the DMR ID. */
    void setId(unsigned int id);
    /** Encodes the text. */
    void setText(const QString &text);
    /** Encodes the given user. */
    void fromEntry(const UserDatabase::User &user);

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      // The length of the text.
      static constexpr unsigned int textLength() { return 16; }
    };

  protected:
    /** Internal offsets within entry. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int dmrID() { return 0x0000; }
      static constexpr unsigned int text()  { return 0x0003; }
      /// @endcond
    };

    static QVector<QChar> _lut;
  };


  /** Represents the header of the callsign database. */
  class DatabaseHeaderElement: public Codeplug::Element
  {
  public:
    /// Possible formats.
    enum class Format {
      Uncompressed = 45,
      Compressed   = 78
    };

  protected:
    /** Hidden constructor. */
    DatabaseHeaderElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DatabaseHeaderElement(uint8_t *ptr);

    /** The size of the header. */
    static constexpr unsigned int size() { return 0x000c; }

    void clear();

    void setEntryCount(unsigned int count);

  public:
    /** Some limits for the header. */
    struct Limit: public Element::Limit {
      /// None..
    };

  protected:
    /** Internal offsets within the header. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int magic()      { return 0x0000; }
      static constexpr unsigned int format()     { return 0x0002; }
      static constexpr unsigned int entrySize()  { return 0x0003; }
      static constexpr unsigned int version()    { return 0x0004; }
      static constexpr unsigned int entryCount() { return 0x0008; }
      /// @endcond
    };
  };


public:
  /** Constructor. */
  explicit OpenGD77BaseCallsignDB(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~OpenGD77BaseCallsignDB();

  /** Encodes as many entries as possible of the given user-database. */
  virtual bool encode(UserDatabase *calldb, const Selection &selection=Selection(),
                      const ErrorStack &err=ErrorStack()) = 0;

public:
  /** Some limits for this callsign DB. */
  struct Limit {
    /** Maximum block size. */
    static constexpr unsigned int blockSize() { return 32; }
  };
};

#endif // OPENGD77BASECALLSIGNDB_HH
