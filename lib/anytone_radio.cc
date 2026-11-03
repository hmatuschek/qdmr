#include "anytone_radio.hh"

#include "d868uv.hh"
#include "config.hh"
#include "logger.hh"
#include "configcopyvisitor.hh"

#include "anytone_preserve.hh"
#include "anytone_writeall.hh"
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
                                 const QString &currentYAML, bool wholeAreas,
                                 const QString &expectFile) {
  _preserve = enable;
  _expectFile = expectFile;
  _wholeAreas = wholeAreas;
  _planFile = planFile;
  _dryRun = dryRun;
  _currentYAML = currentYAML;
}


bool
AnytoneRadio::readAddresses(const QList<QPair<unsigned, unsigned>> &ranges,
                            AnytonePreservingWrite::Memory &mem, unsigned &failedAddr,
                            const ErrorStack &err) {
  if ((StatusIdle != _task) || (nullptr == _dev) || (! _dev->isOpen())) {
    errMsg(err) << "Cannot read addresses: radio not ready.";
    return false;
  }
  if (! _dev->read_start(0, 0, err)) {
    errMsg(err) << "Cannot read addresses: cannot enter programming mode.";
    return false;
  }
  unsigned total = 0, done = 0;
  for (const auto &r : ranges)
    total += (r.second - r.first) / RBSIZE;
  for (const auto &r : ranges) {
    for (unsigned addr = r.first; addr < r.second; addr += RBSIZE, done++) {
      QByteArray block(RBSIZE, '\0');
      if (! _dev->read(0, addr, reinterpret_cast<uint8_t *>(block.data()), RBSIZE, err)) {
        failedAddr = addr;
        errMsg(err) << "Radio did not answer the read of block 0x" << QString::number(addr, 16)
                    << " (" << done << " of " << total << " blocks read).";
        _dev->read_finish(err);
        _dev->reboot();   // after a failed read, as download() does on error
        _dev->close();
        return false;
      }
      mem[addr] = block;
      if (0 == (done % 64))
        emit downloadProgress(float(done*100)/std::max(1u, total));
    }
  }
  _dev->read_finish(err);
  _dev->close();      // end the session exactly as qdmr's own read (download) does
  emit downloadProgress(100);
  return true;
}


bool
AnytoneRadio::uploadPreserving() {
  // D-113 (ham-radio-coordinator): write everything like the CPS. The old write of changed
  // blocks only erased channels 2 to 256 on radio B (TD-008) and is gone.
  const AnytoneWriteAll::Map *map = AnytoneWriteAll::forCodeplug(*_codeplug);
  if (nullptr == map) {
    errMsg(_errorStack) << "No write map for " << _name << " yet: a model needs a USB capture of "
                           "the vendor CPS writing it first (INV-020). Nothing was written.";
    return false;
  }

  AnytonePreservingWrite::Reader reader =
      [this](unsigned addr, uint8_t *dst, unsigned size, const ErrorStack &err) -> bool {
    return _dev->read(0, addr, dst, size, err);
  };
  if (! _dev->read_start(0, 0, _errorStack)) {
    errMsg(_errorStack) << "Cannot enter programming mode; nothing was written.";
    return false;
  }

  emit uploadProgress(0);
  AnytoneWriteAll::Result res;
  if (! AnytoneWriteAll::build(*_codeplug, *map, reader, _config, _codeplugFlags, res, _currentYAML,
                               [this](int p) { emit uploadProgress(p/2); }, _errorStack)) {
    errMsg(_errorStack) << "Cannot plan the write; nothing was written.";
    return false;
  }
  _planStats = res.planStats;
  logInfo() << "Write everything (" << map->model << "): " << res.pages.size() << " pages of "
            << map->pageSize/1024 << " KB read (" << res.blocksRead << " blocks), " << res.blocksChanged
            << " blocks changed (" << res.bytesChanged << " bytes), " << res.send.size()
            << " blocks to send (" << res.blocksSentNotCps << " beyond the CPS's own), "
            << res.pagesAdded << " pages added by the edit.";

  QString what = _dryRun ? "dry run (nothing written)" : "written";
  if (! _planFile.isEmpty()) {
    AnytonePreservingWrite::Stats s = res.planStats;
    s.blocksToWrite = res.send.size();
    s.blocksChanged = res.blocksChanged;
    s.bytesChanged = res.bytesChanged;
    s.blocksPadding = res.send.size() - res.blocksChanged;
    QString header = QString("radio: %1, %2; write everything: %3 pages, %4 blocks sent, %5 changed "
                             "(only changed blocks are listed)")
        .arg(_name, what).arg(res.pages.size()).arg(res.send.size()).arg(res.blocksChanged);
    if (! AnytonePreservingWrite::writePlanFile(_planFile, res.plan, s, header, _errorStack)) {
      errMsg(_errorStack) << "Cannot save the write plan; nothing was written.";
      return false;
    }
  }
  if (! _expectFile.isEmpty()) {
    if ((! AnytonePreservingWrite::writeMemoryFile(_expectFile, res.expected, _errorStack))
        || (! AnytoneWriteAll::writeRangesFile(_expectFile + ".ranges", res.pages, map->pageSize,
                                               QString("pages of %1, %2").arg(_name, what), _errorStack))
        || (! AnytonePreservingWrite::writeMemoryFile(_expectFile + ".before", res.image, _errorStack))) {
      errMsg(_errorStack) << "Cannot save the expected radio content; nothing was written.";
      return false;
    }
  }

  if (_dryRun) {
    logInfo() << "Dry run: nothing was written to the radio.";
    emit uploadProgress(100);
    return true;
  }

  // Send in ascending address order, as the CPS and qdmr do.
  unsigned n = 0;
  for (auto it = res.send.cbegin(); it != res.send.cend(); it++, n++) {
    if (! _dev->write(0, it.key(), reinterpret_cast<uint8_t *>(const_cast<char *>(it->constData())),
                      WBSIZE, _errorStack)) {
      errMsg(_errorStack) << "Cannot write block 0x" << QString::number(it.key(), 16) << " (" << n
                          << " of " << res.send.size() << " written). The radio may be incomplete: "
                          "restore it from the CPS backup.";
      return false;
    }
    if (0 == (n % 64))
      emit uploadProgress(50 + float(n*50)/std::max<qsizetype>(1, res.send.size()));
  }
  emit uploadProgress(100);
  logInfo() << "Sent " << res.send.size() << " blocks. Verify after the radio restarts: read the pages "
               "again (read-addr with --expect) and compare.";
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
