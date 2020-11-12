#ifndef D878UV_CODEPLUG_HH
#define D878UV_CODEPLUG_HH

#include <QDateTime>

#include "codeplug.hh"
#include "signaling.hh"

class Channel;
class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D878UV radios.
 *
 * @ingroup d878uv */
class D878UVCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit D878UVCodeplug(QObject *parent = nullptr);

  /** Clears and resets the complete codeplug to some default values. */
  void clear();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config);
};

#endif // D878UVCODEPLUG_HH
