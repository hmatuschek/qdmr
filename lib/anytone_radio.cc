#include "anytone_radio.hh"

#include "d868uv.hh"
#include "config.hh"
#include "logger.hh"

#define RBSIZE 16
#define WBSIZE 16


AnytoneRadio::AnytoneRadio(const QString &name, AnytoneInterface *device, QObject *parent)
  : Radio(parent), _name(name), _dev(device), _codeplugFlags(), _config(nullptr),
    _codeplug(nullptr), _callsigns(nullptr)
{
  // Open device to radio if not already present
  if (nullptr != _dev) {
    _dev->setParent(this);
  } else if (! connect()) {
    _errorMessage = tr("Cannot connect to AnyTone radio: %1").arg(_errorMessage);
    _task = StatusError;
    return;
  }  
}

AnytoneRadio::~AnytoneRadio() {
  // pass...
}

const QString &
AnytoneRadio::name() const {
  return _name;
}

const CodePlug &
AnytoneRadio::codeplug() const {
  return *_codeplug;
}

CodePlug &
AnytoneRadio::codeplug() {
  return *_codeplug;
}

bool
AnytoneRadio::startDownload(bool blocking) {
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
AnytoneRadio::startUpload(Config *config, bool blocking, const CodePlug::Flags &flags) {
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
AnytoneRadio::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _callsigns->encode(db, selection);

  _task = StatusUploadCallsigns;
  if (blocking) {
    this->run();
    return (StatusIdle == _task);
  }

  this->start();
  return true;
}

void
AnytoneRadio::run() {
  if (StatusDownload == _task) {
    emit downloadStarted();

    if (! download()) {
      emit downloadError(this);
      return;
    }
    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    //_dev->deleteLater();

    emit downloadFinished(this, _codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    emit uploadStarted();

    if (! upload()) {
      emit uploadError(this);
      return;
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    //_dev->deleteLater();

    emit uploadComplete(this);
  } else if (StatusUploadCallsigns == _task) {
    emit uploadStarted();

    if (! uploadCallsigns()) {
      emit uploadError(this);
      return;
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();

    emit uploadComplete(this);
  }
}

bool
AnytoneRadio::connect() {
  // Check if there is a connection
  if ((nullptr != _dev) && (_dev->isOpen()))
    return true;

  // If there is a connection but it is not open -> close it.
  if (nullptr != _dev)
    _dev->deleteLater();

  // If no connection -> open one.
  _dev = new AnytoneInterface(this);
  if (! _dev->isOpen()) {
    _errorMessage = QString("Cannot open device: %1").arg(_dev->errorMessage());
    _dev->deleteLater();
    return false;
  }

  return true;
}

bool
AnytoneRadio::download() {
  if (nullptr == _codeplug) {
    _errorMessage = tr("Cannot download codeplug: Object not created yet.");
    return false;
  }

  logDebug() << "Download of " << _codeplug->image(0).numElements() << " bitmaps.";

  // The first thing happening within the thread is creating the interface to the device.
  // For some reason this object cannot be created outside of the thread.
  if (! connect()) {
    _task = StatusError;
    return false;
  }

  // Download bitmaps
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    uint addr = _codeplug->image(0).element(n).address();
    uint size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      return false;
    }
    emit downloadProgress(float(n*100)/_codeplug->image(0).numElements());
  }

  // Allocate remaining memory sections
  uint nstart = _codeplug->image(0).numElements();
  _codeplug->allocateForDecoding();

  // Check every segment in the remaining codeplug
  for (int n=nstart; n<_codeplug->image(0).numElements(); n++) {
    if (! _codeplug->image(0).element(n).isAligned(RBSIZE)) {
      _errorMessage = QString("%1 Cannot download codeplug: Codeplug element %2 (addr=%3, size=%4) "
                              "is not aligned with blocksize %5.").arg(__func__)
          .arg(n).arg(_codeplug->image(0).element(n).address())
          .arg(_codeplug->image(0).element(n).data().size()).arg(RBSIZE);
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      return false;
    }
  }

  // Download remaining memory sections
  for (int n=nstart; n<_codeplug->image(0).numElements(); n++) {
    uint addr = _codeplug->image(0).element(n).address();
    uint size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      return false;
    }
    emit downloadProgress(float(n*100)/_codeplug->image(0).numElements());
  }

  return true;
}

bool
AnytoneRadio::upload() {
  if (nullptr == _codeplug) {
    _errorMessage = tr("Cannot upload codeplug: Object not created yet.");
    return false;
  }

  // The first thing happening within the thread is creating the interface to the device.
  // For some reason this object cannot be created outside of the thread.
  if (! connect()) {
    _task = StatusError;
    return false;
  }

  // Download bitmaps first
  size_t nbitmaps = _codeplug->numImages();
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    uint addr = _codeplug->image(0).element(n).address();
    uint size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      _errorMessage = QString("%1 Cannot read codeplug for update: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      return false;
    }
    emit uploadProgress(float(n*25)/_codeplug->image(0).numElements());
  }

  // Allocate all memory sections that must be read first
  // and written back to the device more or less untouched
  _codeplug->allocateUpdated();

  // Download new memory sections for update
  for (int n=nbitmaps; n<_codeplug->image(0).numElements(); n++) {
    uint addr = _codeplug->image(0).element(n).address();
    uint size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      _errorMessage = QString("%1 Cannot read codeplug for update: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      return false;
    }
    emit uploadProgress(25+float(n*25)/_codeplug->image(0).numElements());
  }

  // Update bitmaps for all elements representing the common Config
  _codeplug->setBitmaps(_config);
  // Allocate all memory elements representing the common config
  _codeplug->allocateForEncoding();

  // Update binary codeplug from config
  if (! _codeplug->encode(_config, _codeplugFlags)) {
    _errorMessage = QString("Cannot encode codeplug: %1").arg(_codeplug->errorMessage());
    logError() << _errorMessage;
    _task = StatusError;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();
    return false;
  }

  // Sort all elements before uploading
  _codeplug->image(0).sort();

  // Upload all elements back to the device
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    uint addr = _codeplug->image(0).element(n).address();
    uint size = _codeplug->image(0).element(n).data().size();
    if (! _dev->write(0, addr, _codeplug->data(addr), size)) {
      _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      return false;
    }
    emit uploadProgress(50+float(n*50)/_codeplug->image(0).numElements());
  }

  //_codeplug.write("debug_codeplug.dfu");
  return true;
}


bool
AnytoneRadio::uploadCallsigns() {
  // Connect to radio
  if (! connect()) {
    _task = StatusError;
    return false;
  }

  // Sort all elements before uploading
  _callsigns->image(0).sort();

  size_t totalBlocks = _callsigns->memSize()/WBSIZE;
  size_t blkWritten  = 0;
  // Upload all elements back to the device
  for (int n=0; n<_callsigns->image(0).numElements(); n++) {
    uint addr = _callsigns->image(0).element(n).address();
    uint size = _callsigns->image(0).element(n).data().size();
    uint nblks = size/WBSIZE;
    for (uint i=0; i<nblks; i++) {
      if (! _dev->write(0, addr+i*WBSIZE, _callsigns->data(addr)+i*WBSIZE, WBSIZE)) {
        _errorMessage = QString("%1 Cannot upload callsign db: %2").arg(__func__)
            .arg(_dev->errorMessage());
        logError() << _errorMessage;
        _task = StatusError;
        _dev->reboot();
        _dev->close();
        _dev->deleteLater();
        return false;
      }
      blkWritten++;
      emit uploadProgress(float(blkWritten*100)/totalBlocks);
    }
  }

  return true;
}
