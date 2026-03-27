#ifndef D168UV_CODEPLUG_HH
#define D168UV_CODEPLUG_HH

#include <QDateTime>

#include "d878uv_codeplug.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class DMRAPRSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D878UVII radios.
 *
 * This class only implements the difference to the AT-D878UV codeplug. In fact there is only a
 * difference in the address of the contact ID<->Index map.
 *
 * @ingroup d878uv2 */
class D168UVCodeplug : public D878UVCodeplug
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit D168UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D168UVCodeplug(QObject *parent = nullptr);

protected:
  /** Internal used offsets within the codeplug. */
  struct Offset: public D878UVCodeplug::Offset {
    /// @cond DO_NOT_DOCUMENT

    /// @endcond
  };
};

#endif // D168UV_CODEPLUG_HH
