#include "d868uv.hh"

#include "d868uv_codeplug.hh"
#include "d868uv_callsigndb.hh"
#include "d868uv_limits.hh"

#include "config.hh"
#include "logger.hh"

#define RBSIZE 16
#define WBSIZE 16


D868UV::D868UV(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("Anytone AT-D868UV", device, parent), _limits(nullptr)
{
  _codeplug = new D868UVCodeplug(this);
  _codeplug->clear();
  _callsigns = new D868UVCallsignDB(this);

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info;
  if (_dev)
    _dev->getInfo(info);

  switch (info.bands) {
  case 0x00:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} }, info.version, this);
    break;
  case 0x01:
    _limits = new D868UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                 {Frequency::fromMHz(420.), Frequency::fromMHz(450.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                 {Frequency::fromMHz(420.), Frequency::fromMHz(450.)} }, info.version, this);
    break;
  case 0x02:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x03:
    _limits = new D868UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x04:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} }, info.version, this);
    break;
  case 0x05:
    _limits = new D868UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} }, info.version, this);
    break;
  case 0x06:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} }, info.version, this);
    break;
  case 0x07:
    _limits = new D868UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} }, info.version, this);
    break;
  case 0x08:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(440.), Frequency::fromMHz(470.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(440.), Frequency::fromMHz(470.)} }, info.version, this);
    break;
  case 0x09:
    _limits = new D868UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(432.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(432.)} }, info.version, this);
    break;
  case 0x0a:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(450.)} }, info.version, this);
    break;
  case 0x0b:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x0c:
    _limits = new D868UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(403.), Frequency::fromMHz(470.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(403.), Frequency::fromMHz(470.)} }, info.version, this);
    break;

  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Ignore frequency limits.";
    _limits = new D868UVLimits({}, {}, info.version, this);
    break;
  }
}

const RadioLimits &
D868UV::limits() const {
  return *_limits;
}

RadioInfo
D868UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::D868UVE, "d868uve", "AT-D868UVE", "AnyTone", AnytoneInterface::interfaceInfo(),
        QList<RadioInfo>{
          RadioInfo(RadioInfo::D868UV, "d868uv", "AT-D868UV", "AnyTone", AnytoneInterface::interfaceInfo())
        });
}
