#include "radioddity_radio.hh"
#include "config.hh"
#include "logger.hh"
#include "utils.hh"

#define BSIZE           32


RadioddityRadio::RadioddityRadio(RadioddityInterface *device, QObject *parent)
  : Radio(parent), _dev(device), _codeplugFlags(), _config(nullptr)
{
  if (! connect())
    return;
}

RadioddityRadio::~RadioddityRadio() {
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
RadioddityRadio::startDownload(bool blocking) {
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
RadioddityRadio::startUpload(Config *config, bool blocking, const CodePlug::Flags &flags) {
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
RadioddityRadio::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _errorMessage = tr("Radio does not support a callsign DB.");

  return false;
}

void
RadioddityRadio::run() {
  if (StatusDownload == _task) {
    if (! connect()) {
      emit downloadError(this);
      return;
    }

    if (! download()) {
      _dev->read_finish();
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
      _dev->write_finish();
      _dev->reboot();
      _dev->close();
      _task = StatusError;
      emit uploadError(this);
      return;
    }
    _dev->write_finish();
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
RadioddityRadio::connect() {
  // If connected -> done
  if (_dev && _dev->isOpen())
    return true;
  // If not open -> reconnect
  if (_dev)
    _dev->deleteLater();
  _dev = new RadioddityInterface(0x15a2, 0x0073);
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
RadioddityRadio::download() {
  emit downloadStarted();

  uint btot = 0;
  for (int n=0; n<codeplug().image(0).numElements(); n++) {
    btot += codeplug().image(0).element(n).data().size()/BSIZE;
  }

  uint bcount = 0;
  for (int n=0; n<codeplug().image(0).numElements(); n++) {
    int b0 = codeplug().image(0).element(n).address()/BSIZE;
    int nb = codeplug().image(0).element(n).data().size()/BSIZE;
    for (int i=0; i<nb; i++, bcount++) {
      // Select bank by addr
      uint32_t addr = (b0+i)*BSIZE;
      RadioddityInterface::MemoryBank bank = (
            (0x10000 > addr) ? RadioddityInterface::MEMBANK_CODEPLUG_LOWER : RadioddityInterface::MEMBANK_CODEPLUG_UPPER );
      // read
      if (! _dev->read(bank, (b0+i)*BSIZE, codeplug().data((b0+i)*BSIZE), BSIZE)) {
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
RadioddityRadio::upload() {
  emit uploadStarted();

  uint btot = 0;
  for (int n=0; n<codeplug().image(0).numElements(); n++) {
    btot += codeplug().image(0).element(n).data().size()/BSIZE;
  }

  uint bcount = 0;
  if (_codeplugFlags.updateCodePlug) {
    // If codeplug gets updated, download codeplug from device first:
    for (int n=0; n<codeplug().image(0).numElements(); n++) {
      int b0 = codeplug().image(0).element(n).address()/BSIZE;
      int nb = codeplug().image(0).element(n).data().size()/BSIZE;
      for (int i=0; i<nb; i++, bcount++) {
        // Select bank by addr
        uint32_t addr = (b0+i)*BSIZE;
        RadioddityInterface::MemoryBank bank = (
              (0x10000 > addr) ? RadioddityInterface::MEMBANK_CODEPLUG_LOWER : RadioddityInterface::MEMBANK_CODEPLUG_UPPER );
        // read
        if (! _dev->read(bank, addr, codeplug().data(addr), BSIZE)) {
          _errorMessage = tr("%1: Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          return false;
        }
        emit uploadProgress(float(bcount*50)/btot);
      }
    }
  }

  // Encode config into codeplug
  if (! codeplug().encode(_config, _codeplugFlags)) {
    _errorMessage = tr("%1(): Upload failed: %2")
        .arg(__func__).arg(codeplug().errorMessage());
    return false;
  }

  // then, upload modified codeplug
  bcount = 0;
  for (int n=0; n<codeplug().image(0).numElements(); n++) {
    int b0 = codeplug().image(0).element(n).address()/BSIZE;
    int nb = codeplug().image(0).element(n).data().size()/BSIZE;
    for (int i=0; i<nb; i++, bcount++) {
      // Select bank by addr
      uint32_t addr = (b0+i)*BSIZE;
      RadioddityInterface::MemoryBank bank = (
            (0x10000 > addr) ? RadioddityInterface::MEMBANK_CODEPLUG_LOWER : RadioddityInterface::MEMBANK_CODEPLUG_UPPER );
      // write block
      if (! _dev->write(bank, addr, codeplug().data(addr), BSIZE)) {
        _errorMessage = tr("%1: Cannot upload codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        return false;
      }
      emit uploadProgress(50+float(bcount*50)/btot);
    }
  }

  return true;
}

bool
RadioddityRadio::uploadCallsigns() {
  return false;
}
