#include "anytone_radio.hh"

#include "d868uv.hh"
#include "config.hh"
#include "logger.hh"

#define RBSIZE 16
#define WBSIZE 16


AnytoneRadio::AnytoneRadio(const QString &name, AnytoneInterface *device, QObject *parent)
  : Radio(parent), _name(name), _dev(device), _codeplugFlags(), _config(nullptr),
    _codeplug(nullptr), _callsigns(nullptr), _supported_version(), _version()
{
  // Open device to radio if not already present
  if (! connect()) {
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


VerifyIssue::Type
AnytoneRadio::verifyConfig(Config *config, QList<VerifyIssue> &issues, const VerifyFlags &flags) {
  VerifyIssue::Type issue = Radio::verifyConfig(config, issues, flags);

  if (_supported_version.isEmpty() || _version.isEmpty())
    return issue;

  if (_supported_version < _version) {
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("You are likely using a newer radio reversion (%1) than supported (%2) by qdmr. "
                       "The codeplug might be incompatible. "
                       "Notify the developers of qdmr about the new reversion.").arg(_version, _supported_version)));
    issue = std::max(issue, VerifyIssue::WARNING);
  } else if (_supported_version > _version) {
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("You are likely using an older hardware reversion (%1) than supported (%2) by qdmr. "
                       "The codeplug might be incompatible.").arg(_version, _supported_version)));
    issue = std::max(issue, VerifyIssue::WARNING);
  }
  return issue;
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

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  start();

  return true;
}

bool
AnytoneRadio::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;

  _task = StatusUpload;
  _codeplugFlags = flags;
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
AnytoneRadio::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _callsigns->encode(db, selection);

  _task = StatusUploadCallsigns;
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
    if (! connect()) {
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
    if (! connect()) {
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
    if (! connect()) {
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
AnytoneRadio::connect() {
  // Check if there is a connection
  if ((nullptr != _dev) && (_dev->isOpen()))
    return true;

  // If there is a connection but it is not open -> close it.
  if (nullptr != _dev)
    _dev->deleteLater();

  // If no connection -> open one.
  _dev = new AnytoneInterface();
  if (! _dev->isOpen()) {
    pushErrorMessage(_dev->errorMessages());
    errMsg() << "Cannot connect to device.";
    _task = StatusError;
    _dev->deleteLater();
    _dev = nullptr;
    return false;
  }

  return true;
}

bool
AnytoneRadio::download() {
  if (nullptr == _codeplug) {
    errMsg() << "Cannot download codeplug: Object not created yet.";
    return false;
  }

  logDebug() << "Download of " << _codeplug->image(0).numElements() << " bitmaps.";

  // Download bitmaps
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      pushErrorMessage(_dev->errorMessages());
      errMsg() << "Cannot download codeplug.";
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
      errMsg() << "Cannot download codeplug: Codeplug element " << n
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
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      pushErrorMessage(_dev->errorMessages());
      errMsg() << "Cannot download codeplug.";
      return false;
    }
    emit downloadProgress(float(n*100)/_codeplug->image(0).numElements());
  }

  return true;
}

bool
AnytoneRadio::upload() {
  if (nullptr == _codeplug) {
    errMsg() << "Cannot write codeplug: Object not created yet.";
    return false;
  }

  // Download bitmaps first
  size_t nbitmaps = _codeplug->numImages();
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      pushErrorMessage(_dev->errorMessages());
      errMsg() << "Cannot read codeplug for update.";
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
    if (! _dev->read(0, addr, _codeplug->data(addr), size)) {
      pushErrorMessage(_dev->errorMessages());
      errMsg() << "Cannot read codeplug for update.";
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
    pushErrorMessage(_codeplug->errorMessages());
    errMsg() << "Cannot encode codeplug.";
    return false;
  }

  // Sort all elements before uploading
  _codeplug->image(0).sort();

  // Upload all elements back to the device
  for (int n=0; n<_codeplug->image(0).numElements(); n++) {
    unsigned addr = _codeplug->image(0).element(n).address();
    unsigned size = _codeplug->image(0).element(n).data().size();
    if (! _dev->write(0, addr, _codeplug->data(addr), size)) {
      pushErrorMessage(_dev->errorMessages());
      errMsg() << "Cannot write codeplug.";
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
      if (! _dev->write(0, addr+i*WBSIZE, _callsigns->data(addr)+i*WBSIZE, WBSIZE)) {
        pushErrorMessage(_dev->errorMessages());
        errMsg() << "Cannot write callsign db.";
        _task = StatusError;
        return false;
      }
      blkWritten++;
      emit uploadProgress(float(blkWritten*100)/totalBlocks);
    }
  }

  return true;
}
