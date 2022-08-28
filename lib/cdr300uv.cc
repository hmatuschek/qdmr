#include "cdr300uv.hh"
#include "cdr300uv_codeplug.hh"
#include "cdr300uv_limits.hh"
#include "kydera_interface.hh"

RadioLimits *CDR300UV::_limits = nullptr;


CDR300UV::CDR300UV(KyderaInterface *device, QObject *parent)
  : KyderaRadio("CDR-300UV", device, parent), _codeplug(new CDR300UVCodeplug(this))
{
  // pass...
}


RadioInfo
CDR300UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::CDR300UV, "cdr300uv", "CDR-300UV", "Kydera",
        USBDeviceInfo(USBDeviceInfo::Class::Serial, 0x067B, 0x2303, false, false),
        QList<RadioInfo>{
          RadioInfo(RadioInfo::RT73, "rt73", "RT73", "Retevis",
            USBDeviceInfo(USBDeviceInfo::Class::Serial, 0x067B, 0x2303, false, false)),
          RadioInfo(RadioInfo::DB25D, "db25d", "DB25-D", "Radioddity",
            USBDeviceInfo(USBDeviceInfo::Class::Serial, 0x067B, 0x2303, false, false))
        });
}

const RadioLimits&
CDR300UV::limits() const {
  if (nullptr == _limits)
    _limits = new CDR300UVLimits();
  return *_limits;
}
