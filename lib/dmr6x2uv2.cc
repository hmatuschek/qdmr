#include "gpssystem.hh"
#include "userdatabase.hh"
#include "dmr6x2uv2.hh"
#include "dmr6x2uv_codeplug.hh"
#include "dmr6x2uv_limits.hh"
#include "d878uv2_callsigndb.hh"
#include "logger.hh"

DMR6X2UV2::DMR6X2UV2(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("BTECH DMR-6X2UV PRO", device, parent), _limits(nullptr)
{
  _codeplug = new DMR6X2UVCodeplug(this);
  _codeplug->clear();
  _callsigns = new D878UV2CallsignDB(this);

  // Get device info and determine supported TX frequency bands
  AnytoneInterface::RadioVariant info;
  if (_dev)
    _dev->getInfo(info);

  switch (info.bands) {
  case 0x00:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                                 { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} }, info.version, this);
    break;
  case 0x01:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                                 { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(420.), Frequency::fromMHz(450.)} }, info.version, this);
    break;
  case 0x02:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                                 { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x03:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                                 { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x04:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} },
                                 { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} }, info.version, this);
    break;
  case 0x05:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} },
                                 { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(440.), Frequency::fromMHz(480.)} }, info.version, this);
    break;
  case 0x06:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} },
                                 { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} }, info.version, this);
    break;
  case 0x07:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} },
                                 { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(446.), Frequency::fromMHz(447.)} }, info.version, this);
    break;
  case 0x08:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(400.), Frequency::fromMHz(470.)} },
                                 { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(400.), Frequency::fromMHz(470.)} }, info.version, this);
    break;
  case 0x09:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(432.)} },
                                 { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(432.)} }, info.version, this);
    break;
  case 0x0a:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                                 { {Frequency::fromMHz(144.), Frequency::fromMHz(148.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(450.)} }, info.version, this);
    break;
  case 0x0b:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(400.), Frequency::fromMHz(480.)} },
                                 { {Frequency::fromMHz(144.), Frequency::fromMHz(146.)},
                                   {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  case 0x0c:
    _limits = new DMR6X2UVLimits({ {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(403.), Frequency::fromMHz(470.)} },
                                 { {Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                   {Frequency::fromMHz(403.), Frequency::fromMHz(470.)} }, info.version, this);
    break;
  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Do not check frequency range.";
    _limits = new DMR6X2UVLimits({}, {}, info.version, this);
    break;
  }
}

const RadioLimits &
DMR6X2UV2::limits() const {
  return *_limits;
}

RadioInfo
DMR6X2UV2::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::DMR6X2UV2, "dmr6x2uv2", "DMR-6X2UV PRO", "BTECH", {AnytoneGD32Interface::interfaceInfo()});
}

