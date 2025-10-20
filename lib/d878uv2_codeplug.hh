#ifndef D878UV2_CODEPLUG_HH
#define D878UV2_CODEPLUG_HH

#include <QDateTime>

#include "d878uv_codeplug.hh"
#include "signaling.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D878UVII radios.
 *
 * This class only implements the difference to the AT-D878UV codeplug. In fact there is only a
 * difference in the address of the contact ID<->Index map.
 *
 * @ingroup d878uv2 */
class D878UV2Codeplug : public D878UVCodeplug
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit D878UV2Codeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D878UV2Codeplug(QObject *parent = nullptr);

protected:
  void allocateContacts();
  bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  /** Internal used offsets within the codeplug. */
  struct Offset: public D878UVCodeplug::Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int contactIdTable()       { return 0x04800000; }
    /// @endcond
  };
};

#endif // D878UVCODEPLUG_HH
