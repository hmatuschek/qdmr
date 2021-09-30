#include "tyt_radio.hh"
#include "config.hh"
#include "logger.hh"
#include "utils.hh"

#define BSIZE 1024


TyTRadio::TyTRadio(DFUDevice *device, QObject *parent)
  : Radio(parent), _dev(device), _codeplugFlags(), _config(nullptr)
{
  if (! connect())
    return;
}

TyTRadio::~TyTRadio() {
  if (_dev && _dev->isOpen()) {
    logDebug() << "Reboot and close connection to radio.";
    _dev->reboot();
    _dev->close();
  }
  if (_dev) {
    _dev->deleteLater();
    _dev = nullptr;
  }
}

bool
TyTRadio::startDownload(bool blocking) {
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
TyTRadio::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags) {
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
TyTRadio::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  if (StatusIdle != _task)
    return false;

  logDebug() << "Encode call-sign DB.";
  if (nullptr == callsignDB()) {
    _errorMessage = tr("Cannot upload callsign DB. DB not created.");
    return false;
  }
  callsignDB()->encode(db, selection);

  _task = StatusUploadCallsigns;
  if (blocking) {
    this->run();
    return (StatusIdle == _task);
  }

  this->start();
  return true;
}

void
TyTRadio::run() {
  if (StatusDownload == _task) {
    if (! connect()) {
      emit downloadError(this);
      return;
    }

    if (! download()) {
      _dev->reboot();
      _dev->close();
      _task = StatusError;
      emit downloadError(this);
      return;
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    emit downloadFinished(this, &codeplug());
    _config = nullptr;
  } else if (StatusUpload == _task) {
    if (! connect()) {
      emit uploadError(this);
      return;
    }

    if (! upload()) {
      _dev->reboot();
      _dev->close();
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    _dev->reboot();
    _dev->close();
    _task = StatusIdle;
    emit uploadComplete(this);
  } else if (StatusUploadCallsigns == _task) {
    if (! connect()) {
      emit uploadError(this);
      return;
    }

    if(! uploadCallsigns()) {
      _dev->reboot();
      _dev->close();
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    emit uploadComplete(this);

  }
}

bool
TyTRadio::connect() {
  if (_dev && _dev->isOpen())
    return true;

  // Connected but not open
  if (_dev)
    _dev->deleteLater();

  _dev = new DFUDevice(0x0483, 0xdf11);
  if (!_dev->isOpen()) {
    _errorMessage = QString("Cannot open device at 0483:DF11: %1").arg(_dev->errorMessage());
    _dev->deleteLater();
    _dev = nullptr;
    _task = StatusError;
    return false;
  }

  return true;
}

bool
TyTRadio::download() {
  emit downloadStarted();
  logDebug() << "Download of " << codeplug().image(0).numElements() << " elements.";

  // Check every segment in the codeplug
  size_t totb = 0;
  for (int n=0; n<codeplug().image(0).numElements(); n++) {
    if (! codeplug().image(0).element(n).isAligned(BSIZE)) {
      _errorMessage = QString("%1 Cannot download codeplug: Codeplug element %2 (addr=%3, size=%4) "
                              "is not aligned with blocksize %5.").arg(__func__)
          .arg(n).arg(codeplug().image(0).element(n).address())
          .arg(codeplug().image(0).element(n).data().size()).arg(BSIZE);
      logError() << _errorMessage;
      return false;
    }
    totb += codeplug().image(0).element(n).data().size()/BSIZE;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int n=0; n<codeplug().image(0).numElements(); n++) {
    unsigned addr = codeplug().image(0).element(n).address();
    unsigned size = codeplug().image(0).element(n).data().size();
    unsigned b0 = addr/BSIZE, nb = size/BSIZE;
    for (unsigned b=0; b<nb; b++, bcount++) {
      if (! _dev->read(0, (b0+b)*BSIZE, codeplug().data((b0+b)*BSIZE), BSIZE)) {
        _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        logError() << _errorMessage;
        return false;
      }
      emit downloadProgress(float(bcount*100)/totb);
    }
  }

  return true;
}

bool
TyTRadio::upload() {
  emit uploadStarted();

  // Check every segment in the codeplug
  if (! codeplug().isAligned(BSIZE)) {
    _errorMessage = QString("%1 Cannot upload codeplug: "
                            "Codeplug is not aligned with blocksize %5.").arg(__func__).arg(BSIZE);
    logError() << _errorMessage;
    return false;
  }

  size_t totb = codeplug().memSize();

  size_t bcount = 0;
  // If codeplug gets updated, download codeplug from device first:
  if (_codeplugFlags.updateCodePlug) {
    for (int n=0; n<codeplug().image(0).numElements(); n++) {
      unsigned addr = codeplug().image(0).element(n).address();
      unsigned size = codeplug().image(0).element(n).data().size();
      unsigned b0 = addr/BSIZE, nb = size/BSIZE;
      for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(0, (b0+b)*BSIZE, codeplug().data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          logError() << _errorMessage;
          return false;
        }
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
  }

  // Encode config into codeplug
  logDebug() << "Encode codeplug.";
  codeplug().encode(_config, _codeplugFlags);

  // then erase memory
  for (int i=0; i<codeplug().image(0).numElements(); i++)
    _dev->erase(codeplug().image(0).element(i).address(), codeplug().image(0).element(i).memSize());

  logDebug() << "Upload " << codeplug().image(0).numElements() << " elements.";
  // then, upload modified codeplug
  bcount = 0;
  for (int n=0; n<codeplug().image(0).numElements(); n++) {
    unsigned addr = codeplug().image(0).element(n).address();
    unsigned size = codeplug().image(0).element(n).memSize();
    unsigned b0 = addr/BSIZE, nb = size/BSIZE;
    for (size_t b=0; b<nb; b++,bcount+=BSIZE) {
      if (! _dev->write(0, (b0+b)*BSIZE, codeplug().data((b0+b)*BSIZE), BSIZE)) {
        _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        logError() << _errorMessage;
        return false;
      }
      emit uploadProgress(50+float(bcount*50)/totb);
    }
  }

  return true;
}

bool
TyTRadio::uploadCallsigns() {
  emit uploadStarted();

  logDebug() << "Check alignment.";
  // Check alignment in the codeplug
  if (! callsignDB()->isAligned(BSIZE)) {
    _errorMessage = QString("%1 Cannot upload callsign db: Callsign DB is not aligned with blocksize %5.").arg(__func__);
    logError() << _errorMessage;
    return false;
  }

  // then erase memory
  logDebug() << "Erase memory section for call-sign DB.";
  _dev->erase(callsignDB()->image(0).element(0).address(),
              callsignDB()->image(0).element(0).memSize(),
              [](unsigned percent, void *ctx) { emit ((TyTRadio *)ctx)->uploadProgress(percent/2); }, this);

  logDebug() << "Upload " << callsignDB()->image(0).numElements() << " elements.";
  // Total amount of data to transfer
  size_t totb = callsignDB()->memSize();
  // Upload callsign DB
  unsigned addr = callsignDB()->image(0).element(0).address();
  unsigned size = callsignDB()->image(0).element(0).memSize();
  unsigned b0 = addr/BSIZE, nb = size/BSIZE;
  for (size_t b=0, bcount=0; b<nb; b++,bcount+=BSIZE) {
    if (! _dev->write(0, (b0+b)*BSIZE, callsignDB()->data((b0+b)*BSIZE), BSIZE)) {
      _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      return false;
    }
    emit uploadProgress(50+float(bcount*50)/totb);
  }

  return true;
}
