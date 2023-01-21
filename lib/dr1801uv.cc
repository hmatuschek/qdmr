#include "dr1801uv.hh"
#include "dr1801uv_interface.hh"

RadioLimits * DR1801UV::_limits = nullptr;

DR1801UV::DR1801UV(DR1801UVInterface *device, QObject *parent)
  : Radio(parent), _device(device), _name("Baofeng DR-1801UV")
{
  // Check if device is open
  if ((nullptr==_device) || (! _device->isOpen())) {
    _task = StatusError;
    return;
  }
}

RadioInfo
DR1801UV::defaultRadioInfo() {
  return RadioInfo(RadioInfo::DR1801UV, "dr1801uv",
                   "DR-1801UV", "Baofeng",
                   DR1801UVInterface::interfaceInfo());
}


const QString &
DR1801UV::name() const {
  return _name;
}

const Codeplug &
DR1801UV::codeplug() const {
  return _codeplug;
}

Codeplug &
DR1801UV::codeplug() {
  return _codeplug;
}

const RadioLimits &
DR1801UV::limits() const {
  if (nullptr == _limits)
    _limits = nullptr;
  return *_limits;
}

bool
DR1801UV::startDownload(bool blocking, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;

  _task = StatusDownload;
  _errorStack = err;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
DR1801UV::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags, const ErrorStack &err) {
  return false;
}

bool
DR1801UV::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection, const ErrorStack &err) {
  return false;
}

void
DR1801UV::run() {
  if (StatusDownload == _task) {
    if ((nullptr==_device) || (! _device->isOpen())) {
      emit downloadError(this);
      return;
    }

    if (! download()) {
      _device->read_finish();
      _device->reboot();
      _device->close();
      _task = StatusError;
      emit downloadError(this);
      return;
    }

    _task = StatusIdle;
    _device->reboot();
    _device->close();
    emit downloadFinished(this, &codeplug());
    //_config = nullptr;
  } else if (StatusUpload == _task) {
    if ((nullptr==_device) || (! _device->isOpen())) {
      emit uploadError(this);
      return;
    }

    if (! upload()) {
      _device->write_finish();
      _device->reboot();
      _device->close();
      _task = StatusError;
      emit uploadError(this);
      return;
    }
    _device->write_finish();
    _device->reboot();
    _device->close();
    _task = StatusIdle;
    emit uploadComplete(this);
  } else if (StatusUploadCallsigns == _task) {
    // Not implemented.
    emit uploadError(this);
    return;
  }
}

bool
DR1801UV::download() {
  emit downloadStarted();

  // Init download
  if (! _device->read_start(0, 0, _errorStack)) {
    errMsg(_errorStack) << "Cannot initialize codeplug read.";
    _device->read_finish(_errorStack);
    return false;
  }

  uint32_t offset = 0;
  uint32_t bytesToTransfer = _device->bytesToTransfer(), total = bytesToTransfer;
  if (_codeplug.image(0).element(0).memSize() != bytesToTransfer) {
    errMsg(_errorStack) << "Codeplug size mismatch! Expected " << _codeplug.image(0).element(0).memSize()
                        << " radio sends " << bytesToTransfer << ".";
    return false;
  }

  while (DR1801UVInterface::READ_THROUGH == _device->state()) {
    uint8_t buffer[128];
    uint32_t n = std::min(128U, bytesToTransfer);
    if (! _device->read(0, offset, buffer, n, _errorStack)) {
      errMsg(_errorStack) << "Cannot read codeplug from device.";
      return false;
    }
    memcpy(_codeplug.data(offset), buffer, n);
    offset += n; bytesToTransfer -= n;
    emit downloadProgress(float(offset*100)/total);
  }

  if (! _device->read_finish(_errorStack)) {
    errMsg(_errorStack) << "Cannot finish read operation properly. Partial read?";
  }

  emit downloadFinished(this, &_codeplug);
  return true;
}

bool
DR1801UV::upload() {
  return false;
}
