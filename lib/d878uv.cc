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
  _callsigns = new D868UVCallsignDB(this);

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info; _dev->getInfo(info);
  switch (info.bands) {
  case 0x00:
  case 0x01:
    _limits = new D878UVLimits({ {136., 174.}, {400., 480.} },
                               { {136., 174.}, {400., 480.} }, info.version, this);
    break;
  case 0x02:
    _limits = new D878UVLimits({ {136., 174.}, {430., 440.} },
                               { {136., 174.}, {430., 440.} }, info.version, this);
    break;
  case 0x03:
    _limits = new D878UVLimits({ {136., 174.}, {400., 480.} },
                               { {144., 146.}, {430., 440.} }, info.version, this);
    break;
  case 0x04:
    _limits = new D878UVLimits({ {144., 146.}, {434., 438.} },
                               { {144., 146.}, {434., 438.} }, info.version, this);
    break;
  case 0x05:
    _limits = new D878UVLimits({ {144., 146.}, {434., 437.} },
                               { {144., 146.}, {434., 437.} }, info.version, this);
    break;
  case 0x06:
    _limits = new D878UVLimits({ {136., 174.}, {446., 447.} },
                               { {136., 174.}, {446., 447.} }, info.version, this);
    break;
  case 0x07:
    _limits = new D878UVLimits({ {136., 174.}, {400., 480.} },
                               { {144., 148.}, {420., 450.} }, info.version, this);
    break;
  case 0x08:
    _limits = new D878UVLimits({ {136., 174.}, {400., 470.} },
                               { {136., 174.}, {400., 470.} }, info.version, this);
    break;
  case 0x09:
    _limits = new D878UVLimits({ {144., 146.}, {430., 432.} },
                               { {144., 146.}, {430., 432.} }, info.version, this);
    break;
  case 0x0a:
    _limits = new D878UVLimits({ {136., 174.}, {400., 480.} },
                               { {144., 148.}, {430., 450.} }, info.version, this);
    break;
  case 0x0b:
    _limits = new D878UVLimits({ {136., 174.}, {400., 520.} },
                               { {136., 174.}, {400., 520.} }, info.version, this);
    break;
  case 0x0c:
    _limits = new D878UVLimits({ {136., 174.}, {400., 490.} },
                               { {136., 174.}, {400., 490.} }, info.version, this);
    break;
  case 0x0d:
    _limits = new D878UVLimits({ {136., 174.}, {400., 480.} },
                               { {136., 174.}, {403., 470.} }, info.version, this);
    break;
  case 0x0e:
    _limits = new D878UVLimits({ {136., 174.}, {220.,225.}, {400., 520.} },
                               { {136., 174.}, {220.,225.}, {400., 520.} }, info.version, this);
    break;
  case 0x0f:
    _limits = new D878UVLimits({ {144., 148.}, {420., 520.} },
                               { {144., 148.}, {420., 520.} }, info.version, this);
    break;
  case 0x10:
    _limits = new D878UVLimits({ {144., 147.}, {430., 440.} },
                               { {144., 147.}, {430., 440.} }, info.version, this);
    break;
  case 0x11:
    _limits = new D878UVLimits({ {136., 174.}, {430., 440.} },
                               { {136., 174.} }, info.version, this);
    break;
  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Do not check frequency range.";
    _limits = new D878UVLimits({}, {}, info.version, this);
    break;
  }
}

const RadioLimits &
D878UV::limits() const {
  return *_limits;
}

RadioInfo
D878UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::D878UV, "d878uv", "AT-D878UV", "AnyTone", AnytoneInterface::interfaceInfo());
}
