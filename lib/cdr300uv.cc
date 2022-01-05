#include "cdr300uv.hh"
#include "cdr300uv_codeplug.hh"
#include "kydera_interface.hh"

CDR300UV::CDR300UV(const QString &name, KyderaInterface *device, QObject *parent)
  : KyderaRadio(name, device, parent)//, _codeplug(new CDR300UVCodeplug(this))
{
  // pass...
}

RadioInfo
CDR300UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::CDR300UV, "cdr300uv", "CDR-300UV", "Kydera");
}
