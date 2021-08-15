#include "rd5r.hh"
#include "config.hh"

#define BSIZE 128


static Radio::Features _rd5r_features =
{
  .betaWarning = false,

  .hasDigital = true,
  .hasAnalog  = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 470.} },

  .maxRadioIDs        = 1,
  .maxNameLength      = 8,
  .maxIntroLineLength = 16,

  .maxChannels = 1024,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 250,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 16,
  .hasABZone = false,

  .hasScanlists = true,
  .maxScanlists = 250,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 31,
  .scanListNeedsPriority = true,

  .maxContacts = 256,
  .maxContactNameLength = 16,

  .maxGrouplists = 64,
  .maxGrouplistNameLength = 16,
  .maxContactsInGrouplist = 16,

  .hasGPS = false,
  .maxGPSSystems = 0,

  .hasAPRS = false,
  .maxAPRSSystems = 0,

  .hasRoaming = false,
  .maxRoamingChannels = 0,
  .maxRoamingZones = 0,
  .maxChannelsInRoamingZone = 0,

  .hasCallsignDB = false,
  .callsignDBImplemented = false,
  .maxCallsignsInDB = 0
};


RD5R::RD5R(HID *device, QObject *parent)
  : Radio(parent), _name("Baofeng/Radioddity RD-5R"), _dev(device), _codeplugFlags(),
    _config(nullptr), _codeplug()
{
  if (! connect())
    return;
}

RD5R::~RD5R() {
  if (_dev && _dev->isOpen()) {
    _dev->reboot();
    _dev->close();
  }
  if (_dev) {
    _dev->deleteLater();
    _dev = nullptr;
  }
}

const QString &
RD5R::name() const {
  return _name;
}

const Radio::Features &
RD5R::features() const {
  return _rd5r_features;
}

const CodePlug &
RD5R::codeplug() const {
  return _codeplug;
}

CodePlug &
RD5R::codeplug() {
  return _codeplug;
}

bool
RD5R::startDownload(bool blocking) {
  _task = StatusDownload;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
RD5R::startUpload(Config *config, bool blocking, const CodePlug::Flags &flags) {
  _config = config;
  if (!_config)
    return false;

  _task = StatusUpload;
  _codeplugFlags = flags;
  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
RD5R::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _errorMessage = tr("RD5R does not support a callsign DB.");

  return false;
}

void
RD5R::run()
{
  if (StatusDownload == _task) {
    if (! connect()) {
      _errorMessage = tr("%1(): Cannot download codeplug: %2")
          .arg(__func__).arg(_dev->errorMessage());
      _task = StatusError;
      emit downloadError(this);
      return;
    }

    if (! download()) {
      _task = StatusError;
      _dev->read_finish();
      _dev->reboot();
      _dev->close();
      emit downloadError(this);
    }

    _dev->reboot();
    _dev->close();
    _task = StatusIdle;
    emit downloadFinished(this, &_codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    if (! connect()) {
      _errorMessage = tr("%1(): Cannot upload codeplug: %2")
          .arg(__func__).arg(_dev->errorMessage());
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    if (! upload()) {
      _task = StatusError;
      _dev->write_finish();
      _dev->reboot();
      _dev->close();
      emit uploadError(this);
      return;
    }

    _dev->write_finish();
    _dev->reboot();
    _dev->close();

    _task = StatusIdle;
    emit uploadComplete(this);
  }
}


bool
RD5R::connect() {
  // If connected -> done
  if (_dev && _dev->isOpen())
    return true;
  // If not open -> reconnect
  if (_dev)
    _dev->deleteLater();
  _dev = new HID(0x15a2, 0x0073);
  if (! _dev->isOpen()) {
    _errorMessage = tr("%1(): Cannot connect to RD5R: %2")
        .arg(__func__).arg(_dev->errorMessage());
    _dev->deleteLater();
    _dev = nullptr;
    _task = StatusError;
    return false;
  }
  return true;
}

bool
RD5R::download() {
  emit downloadStarted();

  uint btot = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    btot += _codeplug.image(0).element(n).data().size()/BSIZE;
  }

  uint bcount = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    int b0 = _codeplug.image(0).element(n).address()/BSIZE;
    int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
    for (int i=0; i<nb; i++, bcount++) {
      // Select bank by addr
      uint32_t addr = (b0+i)*BSIZE;
      HID::MemoryBank bank = (
            (0x10000 > addr) ? HID::MEMBANK_CODEPLUG_LOWER : HID::MEMBANK_CODEPLUG_UPPER );
      // read
      if (! _dev->read(bank, (b0+i)*BSIZE, _codeplug.data((b0+i)*BSIZE), BSIZE)) {
        _errorMessage = tr("%1: Cannot download codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        return false;
      }
      emit downloadProgress(float(bcount*100)/btot);
    }
  }

  _dev->read_finish();
  return true;
}

bool
RD5R::upload() {
  emit uploadStarted();

  uint btot = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    btot += _codeplug.image(0).element(n).data().size()/BSIZE;
  }

  uint bcount = 0;
  if (_codeplugFlags.updateCodePlug) {
    // If codeplug gets updated, download codeplug from device first:
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      int b0 = _codeplug.image(0).element(n).address()/BSIZE;
      int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
      for (int i=0; i<nb; i++, bcount++) {
        // Select bank by addr
        uint32_t addr = (b0+i)*BSIZE;
        HID::MemoryBank bank = (
              (0x10000 > addr) ? HID::MEMBANK_CODEPLUG_LOWER : HID::MEMBANK_CODEPLUG_UPPER );
        // read
        if (! _dev->read(bank, addr, _codeplug.data(addr), BSIZE)) {
          _errorMessage = tr("%1: Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          return false;
        }
        emit uploadProgress(float(bcount*50)/btot);
      }
    }
  }

  // Encode config into codeplug
  if (! _codeplug.encode(_config, _codeplugFlags)) {
    _errorMessage = tr("%1(): Upload failed: %2")
        .arg(__func__).arg(_codeplug.errorMessage());
    return false;
  }

  // then, upload modified codeplug
  bcount = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    int b0 = _codeplug.image(0).element(n).address()/BSIZE;
    int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
    for (int i=0; i<nb; i++, bcount++) {
      // Select bank by addr
      uint32_t addr = (b0+i)*BSIZE;
      HID::MemoryBank bank = (
            (0x10000 > addr) ? HID::MEMBANK_CODEPLUG_LOWER : HID::MEMBANK_CODEPLUG_UPPER );
      // write block
      if (! _dev->write(bank, addr, _codeplug.data(addr), BSIZE)) {
        _errorMessage = tr("%1: Cannot upload codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        return false;
      }
      emit uploadProgress(50+float(bcount*50)/btot);
    }
  }

  return true;
}
