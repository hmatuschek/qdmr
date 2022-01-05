#include "kydera_radio.hh"
#include "kydera_interface.hh"

#include "config.hh"
#include "logger.hh"

#define BLOCK_SIZE 0x800 // = 2048 bytes


KyderaRadio::KyderaRadio(const QString &name, KyderaInterface *device, QObject *parent)
  : Radio(parent), _name(name), _dev(device), _codeplugFlags(), _config(nullptr),
    _codeplug(nullptr), _callsigns(nullptr)
{
  // Open device to radio if not already present
  if (! connect()) {
    _task = StatusError;
    return;
  }  
}

KyderaRadio::~KyderaRadio() {
  if (_dev && _dev->isOpen()) {
    _dev->close();
  }
  if (_dev) {
    _dev->deleteLater();
    _dev = nullptr;
  }
}

const QString &
KyderaRadio::name() const {
  return _name;
}

const Codeplug &
KyderaRadio::codeplug() const {
  return *_codeplug;
}

Codeplug &
KyderaRadio::codeplug() {
  return *_codeplug;
}


bool
KyderaRadio::startDownload(bool blocking, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;

  _task = StatusDownload;
  _errorStack = err;

  if (blocking) {
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
KyderaRadio::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;

  _task = StatusUpload;
  _codeplugFlags = flags;
  _errorStack = err;

  if (blocking) {
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
KyderaRadio::startUploadCallsignDB(UserDatabase *db, bool blocking,
                                   const CallsignDB::Selection &selection, const ErrorStack &err) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _callsigns->encode(db, selection);

  _task = StatusUploadCallsigns;
  _errorStack = err;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  start();

  return true;
}

void
KyderaRadio::run() {
  if (StatusDownload == _task) {
    if (! connect()) {
      _task = StatusError;
      emit downloadError(this);
      return;
    }

    emit downloadStarted();

    if (! download()) {
      _dev->close();
      _task = StatusError;
      emit downloadError(this);
      return;
    }

    _dev->close();
    _task = StatusIdle;
    emit downloadFinished(this, _codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    if (! connect()) {
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    emit uploadStarted();

    if (! upload()) {
      _dev->close();
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    _dev->close();
    _task = StatusIdle;
    emit uploadComplete(this);
  } else if (StatusUploadCallsigns == _task) {
    if (! connect()) {
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    emit uploadStarted();

    if (! uploadCallsigns()) {
      _dev->close();
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    _dev->close();
    _task = StatusIdle;
    emit uploadComplete(this);
  }
}

bool
KyderaRadio::connect() {
  // Check if there is a connection
  if ((nullptr != _dev) && (_dev->isOpen()))
    return true;

  // If there is a connection but it is not open -> close it.
  if (nullptr != _dev)
    _dev->deleteLater();

  // If no connection -> open one.
  _dev = new KyderaInterface(_errorStack);
  if (! _dev->isOpen()) {
    errMsg(_errorStack) << "Cannot connect to device.";
    _task = StatusError;
    _dev->deleteLater();
    _dev = nullptr;
    return false;
  }

  return true;
}

bool
KyderaRadio::download() {
  if (nullptr == _codeplug) {
    errMsg(_errorStack) << "Cannot read codeplug: Object not created yet.";
    return false;
  }

  logDebug() << "Read entire codeplug (" << _codeplug->image(0).element(0).size() << "b).";

  // Check alignment of codeplug
  if (! _codeplug->image(0).element(0).isAligned(BLOCK_SIZE)) {
    errMsg(_errorStack) << "Cannot read codeplug: Codeplug "
                        << " (addr=" << _codeplug->image(0).element(0).address()
                        << ", size=" << _codeplug->image(0).element(0).data().size()
                        << ") is not aligned with blocksize " << BLOCK_SIZE << ".";
    return false;
  }

  if (! _dev->read_start(0, _codeplug->image(0).element(0).size(), _errorStack)) {
    errMsg(_errorStack) << "Cannot read codeplug from device.";
    return false;
  }

  // Download codeplug
  for (unsigned byte=0; byte<_codeplug->image(0).element(0).size(); byte+=BLOCK_SIZE) {
    if (! _dev->read(0, byte, _codeplug->data(byte), BLOCK_SIZE, _errorStack)) {
      errMsg(_errorStack) << "Cannot read codeplug.";
      return false;
    }
    emit downloadProgress(float(byte*100)/_codeplug->image(0).element(0).size());
  }

  if (! _dev->read_finish(_errorStack)) {
    errMsg(_errorStack) << "Cannot read codeplug from device.";
    return false;
  }

  return true;
}

bool
KyderaRadio::upload() {
  if (nullptr == _codeplug) {
    errMsg(_errorStack) << "Cannot write codeplug: Object not created yet.";
    return false;
  }

  logDebug() << "Read entire codeplug (" << _codeplug->image(0).element(0).size() << "b) for update.";

  // Check alignment of codeplug
  if (! _codeplug->image(0).element(0).isAligned(BLOCK_SIZE)) {
    errMsg(_errorStack) << "Cannot read codeplug: Codeplug "
                        << " (addr=" << _codeplug->image(0).element(0).address()
                        << ", size=" << _codeplug->image(0).element(0).data().size()
                        << ") is not aligned with blocksize " << BLOCK_SIZE << ".";
    return false;
  }

  if (! _dev->read_start(0, _codeplug->image(0).element(0).size(), _errorStack)) {
    errMsg(_errorStack) << "Cannot read codeplug from device for update.";
    return false;
  }

  // Download codeplug
  for (unsigned byte=0; byte<_codeplug->image(0).element(0).size(); byte+=BLOCK_SIZE) {
    if (! _dev->read(0, byte, _codeplug->data(byte), BLOCK_SIZE, _errorStack)) {
      errMsg(_errorStack) << "Cannot read codeplug.";
      return false;
    }
    emit uploadProgress(float(byte*50)/_codeplug->image(0).element(0).size());
  }

  if (! _dev->read_finish(_errorStack)) {
    errMsg(_errorStack) << "Cannot read codeplug from device for update.";
    return false;
  }

  // Update binary codeplug from config
  if (! _codeplug->encode(_config, _codeplugFlags, _errorStack)) {
    errMsg(_errorStack) << "Cannot encode codeplug.";
    return false;
  }

  if (! _dev->write_start(0, _codeplug->image(0).element(0).size(), _errorStack)) {
    errMsg(_errorStack) << "Cannot write codeplug.";
    return false;
  }

  // Write codeplug back to the device
  for (unsigned byte=0; byte<_codeplug->image(0).element(0).size(); byte+=BLOCK_SIZE) {
    if (! _dev->write(0, byte, _codeplug->data(byte), BLOCK_SIZE, _errorStack)) {
      errMsg(_errorStack) << "Cannot write codeplug.";
      return false;
    }
    emit uploadProgress(50+float(byte*50)/_codeplug->image(0).element(0).size());
  }

  if (! _dev->write_finish(_errorStack)) {
    errMsg(_errorStack) << "Cannot write codeplug.";
    return false;
  }

  return true;
}


bool
KyderaRadio::uploadCallsigns() {
  // Sort all elements before uploading
  _callsigns->image(0).sort();

  /*size_t totalBlocks = _callsigns->memSize()/WBSIZE;
  size_t blkWritten  = 0;*/
  // Upload all elements back to the device
  /*for (int n=0; n<_callsigns->image(0).numElements(); n++) {
    unsigned addr = _callsigns->image(0).element(n).address();
    unsigned size = _callsigns->image(0).element(n).data().size();
    unsigned nblks = size/WBSIZE;
    for (unsigned i=0; i<nblks; i++) {
      if (! _dev->write(0, addr+i*WBSIZE, _callsigns->data(addr)+i*WBSIZE, WBSIZE, _errorStack)) {
        errMsg(_errorStack) << "Cannot write callsign db.";
        _task = StatusError;
        return false;
      }
      blkWritten++;
      emit uploadProgress(float(blkWritten*100)/totalBlocks);
    }
  }*/

  return true;
}
