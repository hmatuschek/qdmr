#include "anytone_radio.hh"

#include "d868uv.hh"
#include "config.hh"
#include "logger.hh"
#include "configcopyvisitor.hh"

#include "anytone_preserve.hh"
#include "config.hh"
#define RBSIZE 16
#define WBSIZE 16


AnytoneRadio::AnytoneRadio(const QString &name, AnytoneInterface *device, QObject *parent)
  : Radio(parent), _name(name), _dev(device), _codeplugFlags(), _config(nullptr),
    _codeplug(nullptr), _callsigns(nullptr), _satellites(nullptr)
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
AnytoneRadio::startDownload(const TransferFlags &flags, const ErrorStack &err) {
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
AnytoneRadio::startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;

  if (_config)
    delete _config;

  // Cannot upload null-pointer
  if (nullptr == (_config = config))
    return false;

  _task = StatusUpload;
  _codeplugFlags = flags;
  _errorStack = err;

  if (flags.blocking()) {
    run();
    return (StatusIdle == _task);
  }

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);

  // also, move config to thread
  _config->moveToThread(this);

  start();

  return true;
}


bool
AnytoneRadio::startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err) {
  _callsigns->encode(db, selection);

  _task = StatusUploadCallsigns;
  _errorStack = err;

  if (selection.blocking()) {
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
AnytoneRadio::startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err) {
  if (! _satellites->encode(db, err)) {
    errMsg(err) << "Cannot encode satellite config for AnyTone device.";
    return false;
  }

  _task = StatusUploadSatellites;
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
  } else if (StatusUploadSatellites == _task) {
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      _task = StatusError;
      emit uploadError(this);
      return;
    }

    emit uploadStarted();

    if (! uploadSatellites()) {
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

  logDebug() << "Download of " << _codeplug->image(0).numElements()-nstart << " discovered elements.";

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
    logDebug() << "Read " << Qt::hex << size <<
                  "h bytes from address " << Qt::hex << addr << ".";
    emit downloadProgress(float(n*100)/_codeplug->image(0).numElements());
  }

  return true;
}

void
AnytoneRadio::setPreservingWrite(bool enable, const QString &planFile, bool dryRun,
                                 const QString &currentYAML, bool wholeAreas) {
  _preserve = enable;
  _wholeAreas = wholeAreas;
  _planFile = planFile;
  _dryRun = dryRun;
  _currentYAML = currentYAML;
}


bool
AnytoneRadio::uploadPreserving() {
  AnytonePreservingWrite::Reader reader =
      [this](unsigned addr, uint8_t *dst, unsigned size, const ErrorStack &err) -> bool {
    return _dev->read(0, addr, dst, size, err);
  };

  AnytonePreservingWrite::Plan plan;
  AnytonePreservingWrite::Stats &stats = _planStats;
  emit uploadProgress(0);
  if (! AnytonePreservingWrite::plan(*_codeplug, reader, _config, _codeplugFlags, plan, stats,
                                     _currentYAML, nullptr, _errorStack, _wholeAreas)) {
    errMsg(_errorStack) << "Cannot plan preserving write.";
    return false;
  }
  logInfo() << "Preserving write: " << stats.blocksRead << " blocks read, "
            << stats.blocksToWrite << " blocks to write, " << stats.blocksChanged << " changed ("
            << stats.bytesChanged << " bytes change), " << stats.blocksPadding << " unchanged area padding.";
  emit uploadProgress(50);

  if (! _planFile.isEmpty()) {
    QString header = QString("radio: %1, %2").arg(_name, _dryRun ? "dry run (nothing written)" : "written");
    if (! AnytonePreservingWrite::writePlanFile(_planFile, plan, stats, header, _errorStack)) {
      errMsg(_errorStack) << "Cannot save the write plan; nothing was written.";
      return false;
    }
  }

  if (_dryRun) {
    logInfo() << "Dry run: nothing was written to the radio.";
    emit uploadProgress(100);
    return true;
  }

  // TD-008 (ham-radio-coordinator): the AnyTone erases a whole 16 KB flash block when any part
  // of it is written, so sending only the planned blocks erased channels 2 to 256 on radio B
  // (2026-11-02). Real preserving writes stay blocked until whole erase blocks are sent and
  // that fix has been tested offline and approved by Tom.
  errMsg(_errorStack) << "Preserving write to the radio is disabled (TD-008: a partial write erases "
                         "the rest of each 16 KB flash block). Nothing was written. Use --dry-run.";
  return false;

  // Write only the planned blocks
  unsigned n = 0;
  for (auto it=plan.begin(); it!=plan.end(); it++, n++) {
    if (! _dev->write(0, it.key(), reinterpret_cast<uint8_t *>(it->after.data()), WBSIZE, _errorStack)) {
      errMsg(_errorStack) << "Cannot write block 0x" << QString::number(it.key(), 16)
                          << " (" << n << " of " << plan.size() << " written).";
      return false;
    }
    emit uploadProgress(50 + float(n*40)/std::max<qsizetype>(1, plan.size()));
  }

  // No read-back in this session: on radio B (D878UVII) a block read back before END still
  // showed the old content (the CPS and qdmr never read after writing). Verify instead by
  // reading the radio again after it restarts, e.g. a dry run of the same edit: 0 blocks.
  emit uploadProgress(100);
  logInfo() << "Wrote " << plan.size() << " blocks (" << _planStats.blocksChanged
            << " changed). Verify after the radio restarts: a dry run of the same edit must plan 0 changed blocks.";
  return true;
}


bool
AnytoneRadio::upload() {
  if (nullptr == _codeplug) {
    errMsg(_errorStack) << "Cannot write codeplug: Object not created yet.";
    return false;
  }

  if (_preserve)
    return uploadPreserving();

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


bool
AnytoneRadio::uploadSatellites() {
  // Sort all elements before uploading
  _satellites->image(0).sort();

  size_t totalBlocks = _satellites->memSize()/WBSIZE;
  size_t blkWritten  = 0;
  // Upload all elements back to the device
  for (int n=0; n<_satellites->image(0).numElements(); n++) {
    unsigned addr = _satellites->image(0).element(n).address();
    unsigned size = _satellites->image(0).element(n).data().size();
    unsigned nblks = size/WBSIZE;
    for (unsigned i=0; i<nblks; i++) {
      if (! _dev->write(0, addr+i*WBSIZE, _satellites->data(addr)+i*WBSIZE, WBSIZE, _errorStack)) {
        errMsg(_errorStack) << "Cannot write satellite config.";
        _task = StatusError;
        return false;
      }
      blkWritten++;
      emit uploadProgress(float(blkWritten*100)/totalBlocks);
    }
  }

  return true;
}
