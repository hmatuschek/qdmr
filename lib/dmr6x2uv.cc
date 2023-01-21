#include "dmr6x2uv.hh"
#include "dmr6x2uv_codeplug.hh"
#include "dmr6x2uv_limits.hh"
#include "d868uv_callsigndb.hh"
#include "logger.hh"

DMR6X2UV::DMR6X2UV(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("BTECH DMR-6X2UV", device, parent), _limits(nullptr)
{
  _codeplug = new DMR6X2UVCodeplug(this);
  _codeplug->clear();
  _callsigns = new D868UVCallsignDB(this);

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info;
  if (_dev)
    _dev->getInfo(info);

  switch (info.bands) {
  case 0x00:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {400., 480.} },
                                 { {136., 174.}, {400., 480.} }, info.version, this);
    break;
  case 0x01:
    _limits = new DMR6X2UVLimits({ {144., 146.}, {400., 480.} },
                                 { {144., 146.}, {420., 450.} }, info.version, this);
    break;
  case 0x02:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {430., 440.} },
                                 { {136., 174.}, {430., 440.} }, info.version, this);
    break;
  case 0x03:
    _limits = new DMR6X2UVLimits({ {144., 146.}, {430., 440.} },
                                 { {144., 146.}, {430., 440.} }, info.version, this);
    break;
  case 0x04:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {440., 480.} },
                                 { {136., 174.}, {440., 480.} }, info.version, this);
    break;
  case 0x05:
    _limits = new DMR6X2UVLimits({ {144., 146.}, {440., 480.} },
                                 { {144., 146.}, {440., 480.} }, info.version, this);
    break;
  case 0x06:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {446., 447.} },
                                 { {136., 174.}, {446., 447.} }, info.version, this);
    break;
  case 0x07:
    _limits = new DMR6X2UVLimits({ {144., 146.}, {446., 447.} },
                                 { {144., 146.}, {446., 447.} }, info.version, this);
    break;
  case 0x08:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {400., 470.} },
                                 { {136., 174.}, {400., 470.} }, info.version, this);
    break;
  case 0x09:
    _limits = new DMR6X2UVLimits({ {144., 146.}, {430., 432.} },
                                 { {144., 146.}, {430., 432.} }, info.version, this);
    break;
  case 0x0a:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {400., 480.} },
                                 { {144., 148.}, {430., 450.} }, info.version, this);
    break;
  case 0x0b:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {400., 480.} },
                                 { {144., 146.}, {430., 440.} }, info.version, this);
    break;
  case 0x0c:
    _limits = new DMR6X2UVLimits({ {136., 174.}, {403., 470.} },
                                 { {136., 174.}, {403., 470.} }, info.version, this);
    break;
  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Do not check frequency range.";
    _limits = new DMR6X2UVLimits({}, {}, info.version, this);
    break;
  }
}

const RadioLimits &
DMR6X2UV::limits() const {
  return *_limits;
}

RadioInfo
DMR6X2UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::DMR6X2UV, "dmr6x2uv", "DMR-6X2UV", "BTECH", AnytoneInterface::interfaceInfo());
}

