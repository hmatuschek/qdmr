#include "cdr300uv.hh"
#include "cdr300uv_codeplug.hh"

CDR300UV::CDR300UV(const QString &name, KyderaInterface *device, QObject *parent)
  : KyderaRadio(name, device, parent)//, _codeplug(new CDR300UVCodeplug(this))
{
  // pass...
}


