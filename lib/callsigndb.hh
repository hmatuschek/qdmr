#ifndef CALLSIGNDB_HH
#define CALLSIGNDB_HH

#include "dfufile.hh"

// Forward decl.
class UserDatabase;

/** Abstract base class of all callsign database implementations.
 * This class defines the interface for all device-specific binary encodings of call sign
 * databases. The interface is particularly simple: reimplement the @c encode method.
 * @ingroup conf */
class CallsignDB : public DFUFile
{
  Q_OBJECT

public:
  /** Controls the selection of callsigns from the @c UserDatabase to be encoded into the
   * callsign db. */
  class Selection {
  public:
    /** Constructor. */
    Selection(int64_t count=-1);
    /** Copy constructor. */
    Selection(const Selection &other);

    /** Returns @c true if the selection has a limit on the number of callsigns to encode. */
    bool hasCountLimit() const;
    /** Returns the limit of callsigns to encode. */
    size_t countLimit() const;
    /** Sets the count limit. */
    void setCountLimit(size_t n);
    /** Clears the count limit. */
    void clearCountLimit();

  protected:
    /** Specifies the maximum amount of callsigns to add. If negative, the device limit should be
     * used. */
    int64_t _count;
  };

protected:
  /** Hidden constructor. */
  explicit CallsignDB(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~CallsignDB();

  /** Encodes the given user db into the device specific callsign db. */
  virtual bool encode(UserDatabase *db, const Selection &selection=Selection(),
                      const ErrorStack &err=ErrorStack()) = 0;
};

#endif // CALLSIGNDB_HH
