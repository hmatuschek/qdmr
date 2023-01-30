#include "dr1801uv.hh"
#include "dr1801uv_interface.hh"
#include "logger.hh"


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
    _limits = new DR1801UVLimits();
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
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;
  _codeplugFlags = flags;

  _task = StatusUpload;
  _errorStack = err;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
DR1801UV::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(blocking); Q_UNUSED(selection);
  errMsg(err) << "This device does not support a call-sign DB.";
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

  if (! _device->readCodeplug(_codeplug, [this](unsigned int n, unsigned int total){
                              emit downloadProgress(float(n*100)/total); }, _errorStack)) {
    errMsg(_errorStack) << "Cannot read codeplug from device.";
    return false;
  }

  emit downloadFinished(this, &_codeplug);
  return true;
}

bool
DR1801UV::upload() {
  emit uploadStarted();

  // First, read codeplug from the device
  if (! _device->readCodeplug(_codeplug, [this](unsigned int n, unsigned int total) {
                              emit uploadProgress(float(n*50)/total); }, _errorStack))
  {
    errMsg(_errorStack) << "Cannot read codeplug.";
    return false;
  }

  // Encode config into codeplug
  _codeplug.encode(_config, _codeplugFlags);

  // Write codeplug back to the device
  if (! _device->writeCodeplug(_codeplug, [this](unsigned int n, unsigned int total) {
                               emit uploadProgress(50+float(n*50)/total); }, _errorStack)) {
    errMsg(_errorStack) << "Cannot write codeplug to the device.";
  }

  return true;
}
