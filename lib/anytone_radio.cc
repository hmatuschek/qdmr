#include "anytone_radio.hh"

#include "d868uv.hh"
#include "config.hh"
#include "logger.hh"
#include "configcopyvisitor.hh"

#define RBSIZE 16
#define WBSIZE 16


AnytoneRadio::AnytoneRadio(const QString &name, AnytoneInterface *device, QObject *parent)
  : Radio(parent), _name(name), _dev(device), _codeplugFlags(), _config(nullptr),
    _codeplug(nullptr), _callsigns(nullptr)
{
  // Check if device is open
  if ((nullptr==_dev) || (! _dev->isOpen())) {
    _task = StatusError;
    return;
  }  
}

AnytoneRadio::~AnytoneRadio() {
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
AnytoneRadio::name() const {
  return _name;
}

const Codeplug &
AnytoneRadio::codeplug() const {
  return *_codeplug;
}

Codeplug &
AnytoneRadio::codeplug() {
  return *_codeplug;
}

bool
AnytoneRadio::startDownload(bool blocking, const ErrorStack &err) {
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
AnytoneRadio::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;

  if (_config)
    delete _config;

  // Cannot upload null-pointer
  if (nullptr == (_config = config))
    return false;
  _config->setParent(this);

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
  _config->moveToThread(this);

  start();

  return true;
}

bool
AnytoneRadio::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection, const ErrorStack &err) {
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
AnytoneRadio::run() {
  if (StatusDownload == _task) {
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      _task = StatusError;
      emit downloadError(this);
      return;
    }

    emit downloadStarted();

    if (! download()) {
      _dev->reboot();
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
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    emit uploadStarted();

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
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    emit uploadStarted();

    if (! uploadCallsigns()) {
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
  }
}

bool
AnytoneRadio::download() {
  if (nullptr == _codeplug) {
    errMsg(_errorStack) << "Cannot download codeplug: Object not created yet.";
    return false;
  }

  logDebug() << "Download of " << _codeplug->image(0).numElements() << " bitmaps.";

  // Download bitmaps
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size, _errorStack)) {
      errMsg(_errorStack) << "Cannot download codeplug.";
      return false;
    }
    emit downloadProgress(float(n*100)/_codeplug->image(0).numElements());
  }

  // Allocate remaining memory sections
  unsigned nstart = _codeplug->image(0).numElements();
  _codeplug->allocateForDecoding();

  // Check every segment in the remaining codeplug
  for (int n=nstart; n<_codeplug->image(0).numElements(); n++) {
    if (! _codeplug->image(0).element(n).isAligned(RBSIZE)) {
      errMsg(_errorStack) << "Cannot download codeplug: Codeplug element " << n
                          << " (addr=" << _codeplug->image(0).element(n).address()
                          << ", size=" << _codeplug->image(0).element(n).data().size()
                          << ") is not aligned with blocksize " << RBSIZE << ".";
      return false;
    }
  }

  // Download remaining memory sections
  for (int n=nstart; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size, _errorStack)) {
      errMsg(_errorStack) << "Cannot download codeplug.";
      return false;
    }
    emit downloadProgress(float(n*100)/_codeplug->image(0).numElements());
  }

  return true;
}

bool
AnytoneRadio::upload() {
  if (nullptr == _codeplug) {
    errMsg(_errorStack) << "Cannot write codeplug: Object not created yet.";
    return false;
  }

  // Download bitmaps first
  size_t nbitmaps = _codeplug->numImages();
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size, _errorStack)) {
      errMsg(_errorStack) << "Cannot read codeplug for update.";
      return false;
    }
    emit uploadProgress(float(n*25)/_codeplug->image(0).numElements());
  }

  // Allocate all memory sections that must be read first
  // and written back to the device more or less untouched
  _codeplug->allocateUpdated();

  // Download new memory sections for update
  for (int n=nbitmaps; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size, _errorStack)) {
      errMsg(_errorStack) << "Cannot read codeplug for update.";
      return false;
    }
    emit uploadProgress(25+float(n*25)/_codeplug->image(0).numElements());
  }

  // Update binary codeplug from config
  if (! _codeplug->encode(_config, _codeplugFlags, _errorStack)) {
    errMsg(_errorStack) << "Cannot encode codeplug.";
    return false;
  }

  // Sort all elements before uploading
  _codeplug->image(0).sort();

  // Upload all elements back to the device
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->write(0, addr, _codeplug->data(addr), size, _errorStack)) {
      errMsg(_errorStack) << "Cannot write codeplug.";
      return false;
    }
    emit uploadProgress(50+float(n*50)/_codeplug->image(0).numElements());
  }

  return true;
}


bool
AnytoneRadio::uploadCallsigns() {
  // Sort all elements before uploading
  _callsigns->image(0).sort();

  size_t totalBlocks = _callsigns->memSize()/WBSIZE;
  size_t blkWritten  = 0;
  // Upload all elements back to the device
  for (int n=0; n<_callsigns->image(0).numElements(); n++) {
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
  }

  return true;
}
