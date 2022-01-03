#ifndef CDR300UVCODEPLUG_HH
#define CDR300UVCODEPLUG_HH

#include "kydera_codeplug.hh"

/** Device specific implementation of the binary codeplug for Kydera CDR-300UV and Retevis RT73
 * devices.
 *
 * @ingroup cdr300uv */
class CDR300UVCodeplug : public KyderaCodeplug
{
  Q_OBJECT

public:
  explicit CDR300UVCodeplug(QObject *parent=nullptr);
};

#endif // CDR300UVCODEPLUG_HH
