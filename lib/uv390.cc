#include "uv390.hh"
#include "config.hh"
#include "logger.hh"
#include "utils.hh"

#define BSIZE 1024

static Radio::Features _uv390_features =
{
  .betaWarning = false,

  .hasDigital = true,
  .hasAnalog = true,

  .maxNameLength = 16,
  .maxIntroLineLength = 10,

  .maxChannels = 3000,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 250,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 64,
  .hasABZone = true,

  .hasScanlists = true,
  .maxScanlists = 250,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 31,
  .scanListNeedsPriority = false,

  .maxContacts = 10000,
  .maxContactNameLength = 16,

  .maxGrouplists = 250,
  .maxGrouplistNameLength = 16,
  .maxContactsInGrouplist = 32,

  .hasGPS = true,
  .maxGPSSystems = 16,

  .hasAPRS = false,
  .maxAPRSSystems = 0,

  .hasRoaming = false,
  .maxRoamingChannels = 0,
  .maxRoamingZones = 0,
  .maxChannelsInRoamingZone = 0,

  .hasCallsignDB = true,
  .callsignDBImplemented = true,
  .maxCallsignsInDB = 122197
};


UV390::UV390(QObject *parent)
  : Radio(parent), _name("TYT MD-UV390"), _dev(nullptr), _codeplugFlags(), _config(nullptr)
{
  // pass...
}

const QString &
UV390::name() const {
  return _name;
}

const Radio::Features &
UV390::features() const {
  return _uv390_features;
}

const CodePlug &
UV390::codeplug() const {
  return _codeplug;
}

CodePlug &
UV390::codeplug() {
  return _codeplug;
}

bool
UV390::startDownload(bool blocking) {
  if (StatusIdle != _task)
    return false;

  _task = StatusDownload;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
UV390::startUpload(Config *config, bool blocking, const CodePlug::Flags &flags) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;

  _task = StatusUpload;
  _codeplugFlags = flags;
  if (blocking) {
    this->run();
    return (StatusIdle == _task);
  }

  this->start();
  return true;
}

bool
UV390::startUploadCallsignDB(UserDatabase *db, bool blocking) {
  if (StatusIdle != _task)
    return false;

  _callsigns.encode(db);

  _task = StatusUploadCallsigns;
  if (blocking) {
    this->run();
    return (StatusIdle == _task);
  }

  this->start();
  return true;
}

void
UV390::run() {
  if (StatusDownload == _task) {
    download();
  } else if (StatusUpload == _task) {
    upload();
  } else if (StatusUploadCallsigns == _task) {
    uploadCallsigns();
  }
}


void
UV390::download() {
  emit downloadStarted();
  logDebug() << "Download of " << _codeplug.image(0).numElements() << " elements.";

  _dev = new DFUDevice(0x0483, 0xdf11, this);
  if (!_dev->isOpen()) {
    _errorMessage = QString("Cannot open device at 0483:DF11: %1").arg(_dev->errorMessage());
    _dev->deleteLater();
    _task = StatusError;
    emit downloadError(this);
    return;
  }

  // Check every segment in the codeplug
  size_t totb = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    if (! _codeplug.image(0).element(n).isAligned(BSIZE)) {
      _errorMessage = QString("%1 Cannot download codeplug: Codeplug element %2 (addr=%3, size=%4) "
                              "is not aligned with blocksize %5.").arg(__func__)
          .arg(n).arg(_codeplug.image(0).element(n).address())
          .arg(_codeplug.image(0).element(n).data().size()).arg(BSIZE);
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      emit downloadError(this);
      return;
    }
    totb += _codeplug.image(0).element(n).data().size()/BSIZE;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).data().size();
    uint b0 = addr/BSIZE, nb = size/BSIZE;
    for (uint b=0; b<nb; b++, bcount++) {
      if (! _dev->read(0, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
        _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        logError() << _errorMessage;
        _task = StatusError;
        _dev->reboot();
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }
      emit downloadProgress(float(bcount*100)/totb);
    }
  }

  _task = StatusIdle;
  _dev->reboot();
  _dev->close();
  _dev->deleteLater();
  emit downloadFinished(this, &_codeplug);
  _config = nullptr;
}

void
UV390::upload() {
  emit uploadStarted();

  _dev = new DFUDevice(0x0483, 0xdf11, this);
  if (!_dev->isOpen()) {
    _errorMessage = QString("Cannot open device at 0483:DF11: %1").arg(_dev->errorMessage());
    _dev->deleteLater();
    _task = StatusError;
    emit uploadError(this);
    return;
  }

  // Check every segment in the codeplug
  if (! _codeplug.isAligned(BSIZE)) {
    _errorMessage = QString("%1 Cannot upload codeplug: "
                            "Codeplug is not aligned with blocksize %5.").arg(__func__).arg(BSIZE);
    logError() << _errorMessage;
    _task = StatusError;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();
    emit downloadError(this);
    return;
  }

  size_t totb = _codeplug.memSize();

  size_t bcount = 0;
  // If codeplug gets updated, download codeplug from device first:
  if (_codeplugFlags.updateCodePlug) {
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;
      for (uint b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(0, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          logError() << _errorMessage;
          _task = StatusError;
          _dev->reboot();
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
  }

  // Encode config into codeplug
  logDebug() << "Encode call-sign DB.";
  _codeplug.encode(_config, _codeplugFlags);

  // then erase memory
  for (int i=0; i<_codeplug.image(0).numElements(); i++)
    _dev->erase(_codeplug.image(0).element(i).address(), _codeplug.image(0).element(i).memSize());

  logDebug() << "Upload " << _codeplug.image(0).numElements() << " elements.";
  // then, upload modified codeplug
  bcount = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).memSize();
    uint b0 = addr/BSIZE, nb = size/BSIZE;
    for (size_t b=0; b<nb; b++,bcount+=BSIZE) {
      if (! _dev->write(0, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
        _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        logError() << _errorMessage;
        _task = StatusError;
        _dev->reboot();
        _dev->close();
        _dev->deleteLater();
        emit uploadError(this);
        return;
      }
      emit uploadProgress(50+float(bcount*50)/totb);
    }
  }

  _task = StatusIdle;
  _dev->reboot();
  _dev->close();
  _dev->deleteLater();

  emit uploadComplete(this);
}

void
UV390::uploadCallsigns() {
  emit uploadStarted();

  _dev = new DFUDevice(0x0483, 0xdf11, this);
  if (!_dev->isOpen()) {
    _errorMessage = QString("Cannot open device at 0483:DF11: %1").arg(_dev->errorMessage());
    _dev->deleteLater();
    _task = StatusError;
    emit uploadError(this);
    return;
  }

  logDebug() << "Check alignment.";
  // Check alignment in the codeplug
  if (! _callsigns.isAligned(BSIZE)) {
    _errorMessage = QString("%1 Cannot upload callsign db: Callsign DB is not aligned with blocksize %5.").arg(__func__);
    logError() << _errorMessage;
    _task = StatusError;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();
    emit downloadError(this);
    return;
  }

  // then erase memory
  logDebug() << "Erase memory section for call-sign DB.";
  _dev->erase(_callsigns.image(0).element(0).address(),
              _callsigns.image(0).element(0).memSize(),
              [](uint percent, void *ctx) { emit ((UV390 *)ctx)->uploadProgress(percent/2); }, this);

  logDebug() << "Upload " << _callsigns.image(0).numElements() << " elements.";
  // Total amount of data to transfer
  size_t totb = _callsigns.memSize();
  // Upload callsign DB
  uint addr = _callsigns.image(0).element(0).address();
  uint size = _callsigns.image(0).element(0).memSize();
  uint b0 = addr/BSIZE, nb = size/BSIZE;
  for (size_t b=0, bcount=0; b<nb; b++,bcount+=BSIZE) {
    if (! _dev->write(0, (b0+b)*BSIZE, _callsigns.data((b0+b)*BSIZE), BSIZE)) {
      _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      emit uploadError(this);
      return;
    }
    emit uploadProgress(50+float(bcount*50)/totb);
  }

  _task = StatusIdle;
  _dev->reboot();
  _dev->close();
  _dev->deleteLater();

  emit uploadComplete(this);
}
