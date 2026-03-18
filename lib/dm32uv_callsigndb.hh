#ifndef DM32UV_CALLSIGNDB_HH
#define DM32UV_CALLSIGNDB_HH

#include "callsigndb.hh"
#include "codeplug.hh"
#include "userdatabase.hh"


/** Implements the call-sign DB for the DM32UV. *
 * @ingroup dm32uv */
class DM32UVCallsignDB : public CallsignDB
{
  Q_OBJECT

public:
  class HeaderElement: public Codeplug::Element
  {
  public:
    /** Constructor. */
    explicit HeaderElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0010; }

    void clear() override;

    /** Returns the number of entries. */
    virtual unsigned int count() const;
    /** Sets the number of entries. */
    virtual void setCount(unsigned int count);

  protected:
    /** Some internal offsets. */
    struct Offset: Codeplug::Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int count() { return 0x0000; }
      /// @endcond
    };
  };


  class EntryElement: public Codeplug::Element
  {
  public:
    /** Constructor. */
    explicit EntryElement(uint8_t *ptr);

    /** Size of the element. */
    static unsigned int size() { return 0x005c; }

    void clear() override;
    bool isValid() const override;

    /** Returns the name of the entry. */
    virtual QString name() const;
    /** Sets the name of the entry. */
    virtual void setName(const QString &name);

    /** Returns the DMR id of the entry. */
    virtual unsigned int id() const;
    /** Sets the DMR id of the entry. */
    virtual void setId(unsigned int id);

    /** Retruns the callsign field. */
    virtual QString callsign() const;
    /** Sets the callsign field. */
    virtual void setCallsign(const QString &cal);

    /** Retruns the city field. */
    virtual QString city() const;
    /** Sets the city field. */
    virtual void setCity(const QString &city);

    /** Retruns the province field. */
    virtual QString province() const;
    /** Sets the province field. */
    virtual void setProvince(const QString &provice);

    /** Retruns the country field. */
    virtual QString country() const;
    /** Sets the country field. */
    virtual void setCountry(const QString &country);

    /** Retruns the remark field. */
    virtual QString remark() const;
    /** Sets the remark field. */
    virtual void setRemark(const QString &remark);

    /** Encode entry. */
    virtual bool encode(const UserDatabase::User &user, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the entry. */
    struct Limit: Codeplug::Element::Limit {
      /// Maximum name length.
      static constexpr unsigned int name()     { return 16; }
      static constexpr unsigned int callsign() { return 8; }
      static constexpr unsigned int city()     { return 16; }
      static constexpr unsigned int province() { return 16; }
      static constexpr unsigned int country()  { return 16; }
      static constexpr unsigned int remark()   { return 16; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: Codeplug::Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()     { return 0x0000; }
      static constexpr unsigned int id()       { return 0x0010; }
      static constexpr unsigned int callsign() { return 0x0014; }
      static constexpr unsigned int city()     { return 0x001c; }
      static constexpr unsigned int province() { return 0x002c; }
      static constexpr unsigned int country()  { return 0x003c; }
      static constexpr unsigned int remark()   { return 0x004c; }
      /// @endcond
    };
  };


public:
  explicit DM32UVCallsignDB(QObject *parent = nullptr);

  bool encode(UserDatabase *db, const Flags &selection,const ErrorStack &err=ErrorStack());

public:
  /** Some limits for the DB. */
  struct Limit: Codeplug::Element::Limit {
    /// Maximum number of db entries.
    static constexpr unsigned int entries()         { return 50000; }
    // Number of entires per block.
    static constexpr unsigned int entriesPerBlock() { return 44; }
  };

protected:
  /** Some internal offsets. */
  struct Offset: Codeplug::Element::Limit {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int count()           { return 0x278000; }
    static constexpr unsigned int block0()          { return 0x278010; }
    static constexpr unsigned int block1()          { return 0x279000; }
    static constexpr unsigned int betweenBlocks()   { return 0x1000; }
    /// @endcond
  };
};

#endif // DM32UV_CALLSIGNDB_HH
