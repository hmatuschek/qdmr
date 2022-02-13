#include "d578uv.hh"
#include "config.hh"
#include "logger.hh"

#include "d578uv_limits.hh"
#include "d578uv_codeplug.hh"
// uses same callsign db as 878
#include "d878uv2_callsigndb.hh"

#define RBSIZE 16
#define WBSIZE 16


D578UV::D578UV(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("Anytone AT-D578UV", device, parent), _limits(nullptr)
{
  _codeplug = new D578UVCodeplug(this);
  _callsigns = new D878UV2CallsignDB(this);

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info; _dev->getInfo(info);
  switch (info.bands) {
  case 0x00:
  case 0x01:
  case 0x04:
    _limits = new D578UVLimits({ {136., 174.}, {400., 480.} }, info.version, this);
    break;
  case 0x02:
    _limits = new D578UVLimits({ {136., 174.}, {430., 440.} }, info.version, this);
    break;
  case 0x03:
  case 0x05:
    _limits = new D578UVLimits({ {144., 146.}, {430., 440.} }, info.version, this);
    break;
  case 0x06:
    _limits = new D578UVLimits({ {136., 174.}, {446., 447.} }, info.version, this);
    break;
  case 0x07:
    _limits = new D578UVLimits({ {144., 148.}, {420., 450.} }, info.version, this);
    break;
  case 0x08:
    _limits = new D578UVLimits({ {136., 174.}, {400., 470.} }, info.version, this);
    break;
  case 0x09:
    _limits = new D578UVLimits({ {144., 146.}, {430., 432.} }, info.version, this);
    break;
  case 0x0a:
    _limits = new D578UVLimits({ {144., 148.}, {430., 450.} }, info.version, this);
    break;
  case 0x0b:
    _limits = new D578UVLimits({ {136., 174.}, {400., 520.} }, info.version, this);
    break;
  case 0x0c:
    _limits = new D578UVLimits({ {136., 174.}, {400., 490.} }, info.version, this);
    break;
  case 0x0d:
    _limits = new D578UVLimits({ {136., 174.}, {403., 470.} }, info.version, this);
    break;
  case 0x0e:
    _limits = new D578UVLimits({ {136., 174.}, {220.,225.}, {400., 520.} }, info.version, this);
    break;
  case 0x0f:
    _limits = new D578UVLimits({ {144., 148.}, {400., 520.} }, info.version, this);
    break;
  case 0x10:
    _limits = new D578UVLimits({ {144., 147.}, {430., 440.} }, info.version, this);
    break;
  case 0x11:
    _limits = new D578UVLimits({ {136., 174.} }, info.version, this);
    break;
  default:
    logInfo() << "Unknown band-code 0x" << QString::number(int(info.bands), 16)
              << ": Set freq range to 136-174MHz and 400-480MHz.";
    _limits = new D578UVLimits({ {136., 174.}, {400., 480.} }, info.version, this);
    break;
  }
}

const RadioLimits &
D578UV::limits() const {
  return *_limits;
}

RadioInfo
D578UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::D578UV, "d578uv", "AT-D578UV", "AnyTone", AnytoneInterface::interfaceInfo());
}
