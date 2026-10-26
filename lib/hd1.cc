#include "hd1.hh"
#include "hd1_interface.hh"
#include "hd1_limits.hh"
#include "logger.hh"

RadioLimits *HD1::_limits = nullptr;

HD1::HD1(HD1Interface *device, QObject *parent)
  : Radio(parent), _dev(device), _name("Ailunce HD1"), _codeplug(), _codeplugFlags(), _config(nullptr)
{
  // pass...
}

HD1::~HD1() {
  if (_dev && _dev->isOpen())
    _dev->close();
  if (_dev) {
    _dev->deleteLater();
    _dev = nullptr;
  }
}

RadioInfo
HD1::defaultRadioInfo() {
  return RadioInfo(RadioInfo::HD1, "hd1", "HD1", "Ailunce", {HD1Interface::interfaceInfo()});
}

const QString &
HD1::name() const {
  return _name;
}

const RadioLimits &
HD1::limits() const {
  if (nullptr == _limits)
    _limits = new HD1Limits();
  return *_limits;
}

const Codeplug &
HD1::codeplug() const {
  return _codeplug;
}

Codeplug &
HD1::codeplug() {
  return _codeplug;
}


bool
HD1::startDownload(const TransferFlags &flags, const ErrorStack &err) {
  if (StatusIdle != _task)
    return false;
  _task = StatusDownload;
  _errorStack = err;
  if (flags.blocking()) {
    run();
    return (StatusIdle == _task);
  }
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  start();
  return true;
}

bool
HD1::startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err) {
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
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  start();
  return true;
}

bool
HD1::startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(selection);
  errMsg(err) << "The HD1 address book is not supported yet.";
  return false;
}

bool
HD1::startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(flags);
  errMsg(err) << "The HD1 has no satellite configuration.";
  return false;
}


void
HD1::run() {
  if (StatusDownload == _task) {
    if ((nullptr == _dev) || (! _dev->isOpen())) {
      emit downloadError(this);
      return;
    }
    emit downloadStarted();
    bool ok = download(100, _errorStack);
    _dev->read_finish();
    _dev->reboot();
    _dev->close();
    if (! ok) {
      _task = StatusError;
      emit downloadError(this);
      return;
    }
    _task = StatusIdle;
    emit downloadFinished(this, &codeplug());
  } else if (StatusUpload == _task) {
    if ((nullptr == _dev) || (! _dev->isOpen())) {
      emit uploadError(this);
      return;
    }
    emit uploadStarted();
    bool ok = upload(_errorStack);
    _dev->write_finish();
    _dev->reboot();
    _dev->close();
    if (! ok) {
      _task = StatusError;
      emit uploadError(this);
      return;
    }
    _task = StatusIdle;
    emit uploadComplete(this);
  }
}


bool
HD1::download(int progressScale, const ErrorStack &err) {
  if (! _dev->read_start(0, 0, err)) {
    errMsg(err) << "Cannot start reading the codeplug.";
    return false;
  }
  QList<HD1Codeplug::Region> regions = HD1Codeplug::Region::reads();
  for (int i=0; i<regions.size(); i++) {
    const HD1Codeplug::Region &r = regions[i];
    uint8_t *ptr = _codeplug.bankData(r.bank, r.address);
    if ((nullptr == ptr) || (! _dev->read(r.bank, r.address, ptr, r.size, err))) {
      errMsg(err) << "Cannot read " << r.size << " bytes at " << Qt::hex << r.address
                  << "h of bank " << r.bank << ".";
      return false;
    }
    if ((HD1Interface::Small == r.bank) && (0xe800 == r.address)
        && (HD1Interface::model() != QByteArray(reinterpret_cast<const char *>(ptr), r.size))) {
      errMsg(err) << "Unexpected model, this is not an HD1.";
      return false;
    }
    emit downloadProgress((progressScale*(i+1))/regions.size());
  }
  return true;
}

bool
HD1::upload(const ErrorStack &err) {
  // Read the radio first, so everything qdmr does not model is written back as it was.
  if (! download(25, err)) {
    errMsg(err) << "Cannot read the codeplug before writing.";
    return false;
  }
  _dev->read_finish();
  _dev->reboot();

  if (! _codeplug.encode(_config, _codeplugFlags, err)) {
    errMsg(err) << "Cannot encode the codeplug.";
    return false;
  }

  if (! _dev->write_start(0, 0, err)) {
    errMsg(err) << "Cannot start writing the codeplug.";
    return false;
  }

  QList<HD1Codeplug::Region> regions = HD1Codeplug::Region::writes();
  unsigned int frames = 0, total = 0;
  for (const HD1Codeplug::Region &r: regions)
    total += r.size/HD1Codeplug::Region::writeFrameSize(r.bank);
  for (const HD1Codeplug::Region &r: regions) {
    uint32_t frameSize = HD1Codeplug::Region::writeFrameSize(r.bank);
    for (uint32_t offset=0; offset<r.size; offset+=frameSize) {
      // Bytes outside the allocated regions are padding: 0xff, as the original software sends.
      QByteArray chunk(frameSize, char(0xff));
      uint32_t step = (HD1Interface::Bulk == r.bank) ? HD1Interface::pageSize() : 1;
      for (uint32_t i=0; i<frameSize; i+=step) {
        if (uint8_t *ptr = _codeplug.bankData(r.bank, r.address+offset+i))
          memcpy(chunk.data()+i, ptr, step);
      }
      if (! _dev->write(r.bank, r.address+offset, reinterpret_cast<uint8_t *>(chunk.data()),
                        frameSize, err)) {
        errMsg(err) << "Cannot write the codeplug.";
        return false;
      }
      emit uploadProgress(25 + (75*++frames)/total);
    }
  }
  return true;
}
