#ifndef ANYTONEFILEREADER_HH
#define ANYTONEFILEREADER_HH

#include <QFile>
#include "config.hh"
#include "codeplugcontext.hh"


/** This class implements a reader of AnyTone codeplug files.
 * @ingroup anytone */
class AnytoneFileReader
{
public:
  class Element
  {
  protected:
    Element(const uint8_t *ptr);

  public:
    virtual ~Element();

    virtual size_t size() const = 0;

  protected:
    const uint8_t *_data;
  };

protected:
  AnytoneFileReader(Config *config, const uint8_t *data, size_t size, QString &message);

public:
  virtual ~AnytoneFileReader();

protected:
  virtual bool read();

  virtual bool readHeader() = 0;
  virtual bool linkHeader() = 0;
  virtual bool readChannels() = 0;
  virtual bool linkChannels() = 0;
  virtual bool readRadioIDs() = 0;
  virtual bool linkRadioIDs() = 0;
  virtual bool readZones() = 0;
  virtual bool linkZones() = 0;
  virtual bool readScanLists() = 0;
  virtual bool linkScanLists() = 0;
  virtual bool readAnalogContacts() = 0;
  virtual bool linkAnalogContacts() = 0;

public:
  static bool read(const QString &filename, Config *config, QString &message);

protected:
  CodeplugContext _context;
  const uint8_t * const _start;
  const uint8_t *_data;
  size_t _size;
  QString &_message;
};

#endif // ANYTONEFILEREADER_HH
