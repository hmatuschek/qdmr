//
// Created by hannes on 23.06.26.
//

#include "rt4d.hh"
#include "rt4d_interface.hh"


RT4D::RT4D(RT4DInterface *dev, QObject *parent)
  : Radio{parent}, _radioName("Radtel RT-4D"), _dev(dev), _codeplugFlags(), _codeplug(),
    _config(nullptr)
{
  // pass...
}

RT4D::~RT4D() {
  if (_dev && _dev->isOpen())
    _dev->close();
  if (_dev) {
    _dev->deleteLater();
    _dev = nullptr;
  }
}


RadioInfo
RT4D::defaultRadioInfo() {
  return RadioInfo(RadioInfo::RT4D, "rt4d", "RT-4D",
    "Radtel", {RT4DInterface::interfaceInfo()});
}


const QString &
RT4D::name() const {
  return _radioName;
}

const RadioLimits &
RT4D::limits() const {
  /*if (nullptr == _limits)
    _limits = new DM32UVLimits();*/
  return *(const RadioLimits*)nullptr; //*_limits;
}

const Codeplug &
RT4D::codeplug() const {
  return _codeplug;
}

Codeplug &
RT4D::codeplug() {
  return _codeplug;
}


bool
RT4D::startDownload(const TransferFlags &flags, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;

  _task = StatusDownload;
  _errorStack = err;

  if (flags.blocking()) {
    run();
    return (StatusIdle == _task);
  }

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);

  start();

  return true;
}


bool
RT4D::startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;
  _codeplugFlags = flags;

  _task = StatusUpload;
  _errorStack = err;

  if (flags.blocking()) {
    run();
    return (StatusIdle == _task);
  }

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  start();
  return true;
}


bool
RT4D::startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(selection);

  errMsg(err) << "Callsign DB upload is not implemented yet.";

  return false;
}


bool
RT4D::startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(flags);

  errMsg(err) << "Satellite config upload is not implemented yet.";

  return false;
}


void
RT4D::run() {
  if (StatusDownload == _task) {
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      emit downloadError(this);
      return;
    }

    emit downloadStarted();

    if (! download(_errorStack)) {
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
    //_config = nullptr;
  } else if (StatusUpload == _task) {
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      emit uploadError(this);
      return;
    }

    emit uploadStarted();

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
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      emit uploadError(this);
      return;
    }

    emit uploadStarted();

    if (! uploadCallsigns()) {
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
  }
}


bool
RT4D::download(const ErrorStack &err) {
  if (! _dev->read_start(0, 0, err)) {
    errMsg(err) << "Cannot start reading codeplug.";
    return false;
  }

  // Read all allocated memory regions (sorted by physical address).
  uint32_t byteCount = 0, byteTotal = _codeplug.size();
  for (int i=0; i<_codeplug.image(0).numElements(); i++) {
    auto element = _codeplug.image(0).element(i);
    auto el_address = element.address();
    auto pages = element.size()/0x400;
    for (unsigned int page = 0; page < pages; page++) {
      if (! _dev->read(0, el_address, _codeplug.data(el_address), 0x400, err)) {
        errMsg(err) << "Cannot read codeplug.";
        return false;
      }
      el_address += 0x400; byteCount += 0x400;
      emit downloadProgress((100*byteCount)/byteTotal);
    }
  }

  if (! _dev->read_finish(err)) {
    errMsg(err) << "Cannot finish codeplug download.";
    return false;
  }

  return true;
}


bool
RT4D::upload(const ErrorStack &err) {
  if (! _dev->read_start(0, 0, err)) {
    errMsg(err) << "Cannot start reading codeplug.";
    return false;
  }

  // Read all allocated memory regions (sorted by physical address).
  uint32_t byteCount = 0, byteTotal = _codeplug.size();
  for (int i=0; i<_codeplug.image(0).numElements(); i++) {
    auto element = _codeplug.image(0).element(i);
    auto el_address = element.address();
    auto pages = element.size()/0x400;
    for (unsigned int page = 0; page < pages; page++) {
      if (! _dev->read(0, el_address, _codeplug.data(el_address), 0x400, err)) {
        errMsg(err) << "Cannot read codeplug.";
        return false;
      }
      el_address += 0x400; byteCount += 0x400;
      emit uploadProgress((50*byteCount)/byteTotal);
    }
  }

  if (! _dev->read_finish(err)) {
    errMsg(err) << "Cannot finish codeplug read.";
    return false;
  }

  // Now, encode codeplug in physical address space, may override large portions and may also add
  // new portions to the codeplug.
  if (! _codeplug.encode(_config, _codeplugFlags, err)) {
    errMsg(err) << "Cannot encode codeplug from config.";
    return false;
  }

  if (! _dev->write_start(0,0,err)) {
    errMsg(err) << "Cannot start codeplug write´.";
    return false;
  }

  byteCount = 0;
  for (int i=0; i<_codeplug.image(0).numElements(); i++) {
    auto element = _codeplug.image(0).element(i);
    auto el_address = element.address();
    auto pages = element.size()/0x400;
    for (unsigned int page = 0; page < pages; page++) {
      if (! _dev->write(0, el_address, _codeplug.data(el_address), 0x400, err)) {
        errMsg(err) << "Cannot write codeplug.";
        return false;
      }
      el_address += 0x400; byteCount += 0x400;
      emit uploadProgress(50 + (50*byteCount)/byteTotal);
    }
  }

  if (! _dev->write_finish(err)) {
    errMsg(err) << "Cannot finish codeplug write.";
    return false;
  }

  return true;
}


bool
RT4D::uploadCallsigns(const ErrorStack &err) {
  errMsg(err) << "Not implemented yet.";
  return false;
}

