#include "userdatabase.hh"
#include "d878uv.hh"
#include "config.hh"
#include "logger.hh"

#include "d878uv_codeplug.hh"
#include "d878uv_limits.hh"
// uses same callsign db as 878
#include "d868uv_callsigndb.hh"

#define RBSIZE 16
#define WBSIZE 16


D878UV::D878UV(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("Anytone AT-D878UV", device, parent), _limits(nullptr)
{
  _codeplug = new D878UVCodeplug(this);
  _codeplug->clear();
  _callsigns = new D868UVCallsignDB(this);
  _satellites = new AnytoneSatelliteConfig(this);

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info;
  if (_dev)
    _dev->getInfo(info);

  switch (info.bands) {
  case 0x00:
  case 0x01:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} }, info.version, this);
    break;
  case 0x02:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x03:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x04:
    _limits = new D878UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(434.), Frequency::fromMHz(438.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(434.), Frequency::fromMHz(438.)} }, info.version, this);
    break;
  case 0x05:
    _limits = new D878UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(434.), Frequency::fromMHz(437.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(434.), Frequency::fromMHz(437.)} }, info.version, this);
    break;
  case 0x06:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} }, info.version, this);
    break;
  case 0x07:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                 {Frequency::fromMHz(420.), Frequency::fromMHz(450.)} }, info.version, this);
    break;
  case 0x08:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(470.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(470.)} }, info.version, this);
    break;
  case 0x09:
    _limits = new D878UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(432.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(432.)} }, info.version, this);
    break;
  case 0x0a:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(450.)} }, info.version, this);
    break;
  case 0x0b:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(520.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(520.)} }, info.version, this);
    break;
  case 0x0c:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(490.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(490.)} }, info.version, this);
    break;
  case 0x0d:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(403.), Frequency::fromMHz(470.)} }, info.version, this);
    break;
  case 0x0e:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(220.),Frequency::fromMHz(225.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(520.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(220.),Frequency::fromMHz(225.)},
                                 {Frequency::fromMHz(400.), Frequency::fromMHz(520.)} }, info.version, this);
    break;
  case 0x0f:
    _limits = new D878UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                 {Frequency::fromMHz(420.), Frequency::fromMHz(520.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                 {Frequency::fromMHz(420.), Frequency::fromMHz(520.)} }, info.version, this);
    break;
  case 0x10:
    _limits = new D878UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(147.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(147.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x11:
    _limits = new D878UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                               { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)} }, info.version, this);
    break;
  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Do not check frequency range.";
    _limits = new D878UVLimits({}, {}, info.version, this);
    break;
  }
}

RadioInfo
D878UV::info() const {
  return RadioInfo::byID(RadioInfo::D878UV);
}

const RadioLimits &
D878UV::limits() const {
  return *_limits;
}

RadioInfo
D878UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::D878UV, "d878uv", "AT-D878UV", "AnyTone", {AnytoneGD32Interface::interfaceInfo()});
}
