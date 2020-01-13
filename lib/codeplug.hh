#ifndef CODEPLUG_HH
#define CODEPLUG_HH

#include <QObject>
#include "dfufile.hh"
#include "userdatabase.hh"

class Config;

/** This class defines the interface all device-specific code-plugs must implement.
 * Device-specific codeplugs are derived from the common configuration and implement the
 * construction/parsing of the device specific binary configuration. */
class CodePlug: public DFUFile
{
	Q_OBJECT

protected:
  /** Hidden default constructor. */
	explicit CodePlug(QObject *parent=nullptr);

public:
  /** Destructor. */
	virtual ~CodePlug();

  /** Returns a pointer to the encoded raw data at the specified offset. */
  virtual unsigned char *data(uint32_t offset);
  /** Returns a const pointer to the encoded raw data at the specified offset. */
  virtual const unsigned char *data(uint32_t offset) const;

  /** Decodes a binary codeplug to the given abstract configuration @c config.
   * This must be implemented by the device-specific codeplug. */
	virtual bool decode(Config *config) = 0;
  /** Encodes a given abstract configuration (@c config) to the device specific binary code-plug.
   * This must be implemented by the device-specific codeplug. */
  virtual bool encode(Config *config, UserDatabase *users=nullptr) = 0;
};

#endif // CODEPLUG_HH
