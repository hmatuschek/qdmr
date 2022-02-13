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
  _callsigns = new D868UVCallsignDB(this);

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info; _dev->getInfo(info);
  switch (info.bands) {
  case 0x00:
    _limits = new D868UVLimits({ {136., 174.}, {400., 480.} }, info.version, this);
    break;
  case 0x01:
    _limits = new D868UVLimits({ {144., 148.}, {420., 450.} }, info.version, this);
    break;
  case 0x02:
    _limits = new D868UVLimits({ {136., 174.}, {430., 450.} }, info.version, this);
    break;
  case 0x03:
    _limits = new D868UVLimits({ {144., 146.}, {430., 440.} }, info.version, this);
    break;
  case 0x04:
    _limits = new D868UVLimits({ {136., 174.}, {440., 480.} }, info.version, this);
    break;
  case 0x05:
    _limits = new D868UVLimits({ {144., 146.}, {440., 480.} }, info.version, this);
    break;
  case 0x06:
    _limits = new D868UVLimits({ {136., 174.}, {446., 447.} }, info.version, this);
    break;
  case 0x07:
    _limits = new D868UVLimits({ {144., 146.}, {446., 447.} }, info.version, this);
    break;
  case 0x08:
    _limits = new D868UVLimits({ {136., 174.}, {440., 470.} }, info.version, this);
    break;
  case 0x09:
    _limits = new D868UVLimits({ {144., 146.}, {430., 432.} }, info.version, this);
    break;
  case 0x0a:
    _limits = new D868UVLimits({ {144., 148.}, {430., 450.} }, info.version, this);
    break;
  case 0x0b:
    _limits = new D868UVLimits({ {144., 146.}, {430., 440.} }, info.version, this);
    break;
  case 0x0c:
    _limits = new D868UVLimits({ {136., 174.}, {403., 470.} }, info.version, this);
    break;

  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Set freq range to 136-174MHz and 400-480MHz.";
    _limits = new D868UVLimits({ {136., 174.}, {403., 470.} }, info.version, this);
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
          RadioInfo(RadioInfo::DMR6X2, "dmr6x2", "DMR-6X2", "BTech", AnytoneInterface::interfaceInfo()),
          RadioInfo(RadioInfo::D868UV, "d868uv", "AT-D868UV", "AnyTone", AnytoneInterface::interfaceInfo())
        });
}
