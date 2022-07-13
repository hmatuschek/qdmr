#ifndef ANYTONEFILEREADER_HH
#define ANYTONEFILEREADER_HH

#include <QFile>
#include "config.hh"
#include "codeplugcontext.hh"


/** This class implements a reader of AnyTone codeplug files.
 * @warning This is mostly incomplete.
 * @ingroup anytone */
class AnytoneFileReader
{
public:
  /** Base class for all elements in the codeplug file. */
  class Element
  {
  protected:
    /** Hidden constructor. */
    Element(const uint8_t *ptr);

  public:
    /** Destructor. */
    virtual ~Element();
    /** Returns the storage size of the element. */
    virtual size_t size() const = 0;

  protected:
    /** Pointer to the actual element. */
    const uint8_t *_data;
  };

protected:
  /** Constructs a configuration from the given codeplug file in memory (@c data, @c size). */
  AnytoneFileReader(Config *config, const uint8_t *data, size_t size, QString &message);

public:
  /** Destructor. */
  virtual ~AnytoneFileReader();

protected:
  /** Read all elements. */
  virtual bool read();

  /** Read header of file. */
  virtual bool readHeader() = 0;
  /** Link elements. */
  virtual bool linkHeader() = 0;
  /** Read all channels. */
  virtual bool readChannels() = 0;
  /** Link elements. */
  virtual bool linkChannels() = 0;
  /** Read all radio IDs. */
  virtual bool readRadioIDs() = 0;
  /** Link elements. */
  virtual bool linkRadioIDs() = 0;
  /** Read all zones. */
  virtual bool readZones() = 0;
  /** Link elements. */
  virtual bool linkZones() = 0;
  /** Read all scal lists. */
  virtual bool readScanLists() = 0;
  /** Link elements. */
  virtual bool linkScanLists() = 0;
  /** Read all DTMF contacts. */
  virtual bool readAnalogContacts() = 0;
  /** Link elements. */
  virtual bool linkAnalogContacts() = 0;

public:
  /** Use this static function to read a codeplug from the manufacturer CPS file. */
  static bool read(const QString &filename, Config *config, QString &message);

protected:
  /** Offset-element map. */
  CodeplugContext _context;
  const uint8_t * const _start;
  const uint8_t *_data;
  size_t _size;
  QString &_message;
};

#endif // ANYTONEFILEREADER_HH
