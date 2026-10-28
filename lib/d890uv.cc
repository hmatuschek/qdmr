#include "d890uv.hh"
#include "logger.hh"

#include "d890uv_limits.hh"
#include "d890uv_codeplug.hh"


D890UV::D890UV(AnytoneInterface *device, QObject *parent)
  : AnytoneRadio("Anytone AT-D890UV", device, parent), _limits(nullptr)
{
  _codeplug = new D890UVCodeplug(this);
  _codeplug->clear();

  AnytoneInterface::RadioVariant info;
  if (_dev)
    _dev->getInfo(info);
  logDebug() << "AT-D890UV reports band code 0x" << QString::number(int(info.bands), 16)
             << ", HW rev. '" << info.version << "'.";

  // Band-code table. TX limits follow the vendor CPS's own table (a 16-way jump table on the
  // band code, byte-identical in the D878UV and D168UV modules of the same CPS). Its format
  // distinguishes RX from TX where they differ, e.g., code 0x0a reads
  // "UHF{Rx(400 - 480 MHz) Tx(420 - 450 MHz)}", so a single range means RX == TX. Where qdmr's
  // D878UV table already agreed on TX but modelled a wider RX, the wider RX is kept.
  switch (info.bands) {
  case 0x00:
    // CPS: UHF 400-480, VHF 136-174
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} }, info.version, this);
    break;
  case 0x01:
    // CPS: UHF 400-480, VHF 144-146 -- was a duplicate of 0x00, VHF TX too wide
    _limits = new D890UVLimits({ {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} }, info.version, this);
    break;
  case 0x02:
    // CPS: UHF 430-440, VHF 136-174
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(430.0), Frequency::fromMHz(440.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(430.0), Frequency::fromMHz(440.0)} }, info.version, this);
    break;
  case 0x03:
    // CPS: UHF 430-440, VHF 144-146. TX already agreed; qdmr's wider RX kept.
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(430.0), Frequency::fromMHz(440.0)} }, info.version, this);
    break;
  case 0x04:
    // CPS: UHF 440-480, VHF 136-174 -- qdmr had 144-146/434-438, a different variant
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(440.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(440.0), Frequency::fromMHz(480.0)} }, info.version, this);
    break;
  case 0x05:
    // CPS: UHF 440-480, VHF 144-146 -- qdmr had 144-146/434-437
    _limits = new D890UVLimits({ {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(440.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(440.0), Frequency::fromMHz(480.0)} }, info.version, this);
    break;
  case 0x06:
    // CPS: UHF 446-447, VHF 136-174
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(446.0), Frequency::fromMHz(447.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(446.0), Frequency::fromMHz(447.0)} }, info.version, this);
    break;
  case 0x07:
    // CPS: UHF 446-447, VHF 144-146 -- qdmr had 144-148/420-450. TX follows the CPS. RX is kept
    // wide, as with 0x03/0x0d, since the 144-146/446-447 RX range would reject receive-only
    // channels such as NOAA weather (162 MHz). RX range not verified on hardware.
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(446.0), Frequency::fromMHz(447.0)} }, info.version, this);
    break;
  case 0x08:
    // CPS: UHF 400-470, VHF 136-174
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(470.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(470.0)} }, info.version, this);
    break;
  case 0x09:
    // CPS: UHF 430-432, VHF 144-146
    _limits = new D890UVLimits({ {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(430.0), Frequency::fromMHz(432.0)} },
                               { {Frequency::fromMHz(144.0), Frequency::fromMHz(146.0)}, {Frequency::fromMHz(430.0), Frequency::fromMHz(432.0)} }, info.version, this);
    break;
  case 0x0a:
    // CPS gives an explicit split: RX 136-174/400-480, TX 144-148/420-450
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(144.0), Frequency::fromMHz(148.0)}, {Frequency::fromMHz(420.0), Frequency::fromMHz(450.0)} }, info.version, this);
    break;
  case 0x0b:
    // CPS: UHF 400-520, VHF 136-174
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(520.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(520.0)} }, info.version, this);
    break;
  case 0x0c:
    // CPS: UHF 400-490, VHF 136-174
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(490.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(490.0)} }, info.version, this);
    break;
  case 0x0d:
    // CPS: UHF 403-470, VHF 136-174. TX already agreed; qdmr's wider RX kept.
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(480.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(403.0), Frequency::fromMHz(470.0)} }, info.version, this);
    break;
  case 0x0e:
    // CPS: UHF 400-520, MHF 220-225, VHF 136-174
    _limits = new D890UVLimits({ {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(220.0), Frequency::fromMHz(225.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(520.0)} },
                               { {Frequency::fromMHz(136.0), Frequency::fromMHz(174.0)}, {Frequency::fromMHz(220.0), Frequency::fromMHz(225.0)}, {Frequency::fromMHz(400.0), Frequency::fromMHz(520.0)} }, info.version, this);
    break;
  case 0x0f:
    // CPS: UHF 420-450, VHF 144-148 -- qdmr had UHF TX up to 520
    _limits = new D890UVLimits({ {Frequency::fromMHz(144.0), Frequency::fromMHz(148.0)}, {Frequency::fromMHz(420.0), Frequency::fromMHz(450.0)} },
                               { {Frequency::fromMHz(144.0), Frequency::fromMHz(148.0)}, {Frequency::fromMHz(420.0), Frequency::fromMHz(450.0)} }, info.version, this);
    break;
  case 0x10:
  case 0x11:
    // Not in the CPS table, which stops at 0x0f. Kept as inherited from the D878UV rather than
    // dropped: without them these codes fall through to the default and get no check at all.
    _limits = new D890UVLimits({ {Frequency::fromMHz(144.), Frequency::fromMHz(147.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} },
                               { {Frequency::fromMHz(144.), Frequency::fromMHz(147.)},
                                 {Frequency::fromMHz(430.), Frequency::fromMHz(440.)} }, info.version, this);
    break;
  default:
    logInfo() << "Unknown band-code" << QString::number(int(info.bands), 16)
              << ": Do not check frequency range.";
    _limits = new D890UVLimits({}, {}, info.version, this);
    break;
  }

}

const RadioLimits &
D890UV::limits() const {
  return *_limits;
}

bool
D890UV::startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err) {
  if (! flags.updateCodeplug()) {
    errMsg(err) << tr("The codeplug of the AT-D890UV can only be updated, not initialized.");
    return false;
  }
  return AnytoneRadio::startUpload(config, flags, err);
}

bool
D890UV::startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(selection);
  errMsg(err) << tr("Writing the call-sign DB to the AT-D890UV is not implemented yet.");
  return false;
}

bool
D890UV::startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(flags);
  errMsg(err) << tr("Writing the satellite config to the AT-D890UV is not implemented yet.");
  return false;
}

RadioInfo
D890UV::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::D890UV, "d890uv", "AT-D890UV", "AnyTone", {D890UVInterface::interfaceInfo()});
}
