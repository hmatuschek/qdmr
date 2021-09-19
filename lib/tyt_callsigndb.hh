#ifndef TYTCALLSIGNDB_HH
#define TYTCALLSIGNDB_HH

#include "codeplug.hh"
#include "callsigndb.hh"
#include "userdatabase.hh"

/** Base class for all call-sign DBs of TyT/Retevis devices.
 *
 * @section tytcdb Callsign database structure
 * <table>
 *  <tr><th>Start</th>    <th>End</th>      <th>Size</th>     <th>Content</th></tr>
 *  <tr><th colspan="4">Callsign database 0x0200000-0x1000000</th></tr>
 *  <tr><td>0x200000</td> <td>0x204004</td> <td>0x04004</td>  <td>Callsign database index table, see @c TyTCallsignDB::IndexEntryElement</td></tr>
 *  <tr><td>0x204004</td> <td>0xffffdc</td> <td>0xdfbfd8</td> <td>max 122197 callsign database entries, see @c TyTCallsignDB::EntryElement. </td></tr>
 *  <tr><td>0xffffdc</td> <td>0x1000000</td> <td>0x00025</td> <td>Padding, filled with @c 0xff.</td></tr>
 * </table>

 * @ingroup tyt */
class TyTCallsignDB : public CallsignDB
{
  Q_OBJECT

public:
  /** Represents a search index over the complete call-sign database.
   *
   * Memmory layout of encoded call-sign/user database:
   * @verbinclude tytcallsigndbindex.txt
   */
  class IndexElement: public Codeplug::Element
  {
  public:
    /** Represents an index entry, a pair of DMR ID and call-sign DB index.
     *
     * Memmory layout of encoded call-sign/user database index entry:
     * @verbinclude tytcallsigndbindexentry.txt
     */
    class Entry: public Codeplug::Element
    {
    protected:
      /** Hidden constructor. */
      Entry(uint8_t *ptr, size_t size);

    public:
      /** Constructor. */
      explicit Entry(uint8_t *ptr);
      /** Destructor. */
      virtual ~Entry();

      void clear();

      /** Sets the index entry. */
      virtual void set(uint id, uint index);
    };

  protected:
    /** Hidden constructor. */
    IndexElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit IndexElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~IndexElement();

    void clear();

    /** Sets the number of entries in the DB. */
    virtual void setNumEntries(uint n);
    /** Sets the given index entry. */
    virtual void setIndexEntry(uint n, uint id, uint index);
  };

  /** Represents an entry within the call-sign database.
   * The call-sign DB entries must be ordered by their DMR IDs.
   *
   * Memmory layout of encoded call-sign/user database index entry:
   * @verbinclude tytcallsigndbentry.txt
   */
  class EntryElement: public Codeplug::Element
  {
  protected:
    /** Hidden constructor. */
    EntryElement(uint8_t *ptr, size_t size);
  public:
    /** Constructor. */
    explicit EntryElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~EntryElement();

    void clear();

    /** Encodes the given user. */
    virtual void set(const UserDatabase::User &user);
  };

protected:
  /** Hidden constructor. Use one of the device specific call-sign DB classes. */
  explicit TyTCallsignDB(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~TyTCallsignDB();

  bool encode(UserDatabase *db, const Selection &selection);

protected:
  /** Allocates required space for index and @c n call-signs. */
  virtual void alloate(uint n);

  /** Clears the call-sign DB index. */
  virtual void clearIndex();
  /** Sets the number of entries in the DB. */
  virtual void setNumEntries(uint n);
  /** Sets the given index entry. */
  virtual void setIndexEntry(uint n, uint id, uint index);
  /** Sets a given call-sign entry. */
  virtual void setEntry(uint n, const UserDatabase::User &user);
};

#endif // TYTCALLSIGNDB_HH
