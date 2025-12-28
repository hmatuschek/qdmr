#include "dm32uv.hh"
#include "dm32uv_limits.hh"
#include "dm32uv_interface.hh"


/* ********************************************************************************************* *
 * Implementation of Baofeng DM32UV weird address mapping
 * ********************************************************************************************* */
DM32UV::AddressMap::AddressMap()
  : _toVirtual(), _toPhysical()
{
  // pass...
}

void
DM32UV::AddressMap::map(uint32_t phys, uint32_t virt) {
  phys >>= 12; virt >>= 12;
  _toVirtual[phys] = virt;
  _toPhysical[virt] = phys;
}

bool
DM32UV::AddressMap::virtualIsMapped(uint32_t addr) const {
  return _toPhysical.contains(addr>>12);
}

bool
DM32UV::AddressMap::physicalIsMapped(uint32_t addr) const {
  return _toVirtual.contains(addr>>12);
}

uint32_t
DM32UV::AddressMap::toPhysical(uint32_t virt) const {
  uint32_t offset = virt & 0xfff;
  if (! _toPhysical.contains(virt>>12))
    return std::numeric_limits<uint32_t>::max();
  return (_toPhysical[virt>>12]<<12) | offset;
}

uint32_t
DM32UV::AddressMap::toVirtual(uint32_t phys) const {
  uint32_t offset = phys & 0xfff;
  if (! _toVirtual.contains(phys>>12))
    return std::numeric_limits<uint32_t>::max();
  return (_toVirtual[phys>>12]<<12) | offset;
}



/* ********************************************************************************************* *
 * Implementation of Baofeng DM32UV
 * ********************************************************************************************* */
RadioLimits *DM32UV::_limits = nullptr;

DM32UV::DM32UV(DM32UVInterface *dev, QObject *parent)
  : Radio{parent}, _dev(dev), _radioName("Baofeng DM-32UV"), _codeplug()
{
  // pass...
}

RadioInfo
DM32UV::defaultRadioInfo() {
  return RadioInfo(
    RadioInfo::DM32UV, "dm32uv", "DM-32UV", "Baofeng", {DM32UVInterface::interfaceInfo()});
}

const QString &
DM32UV::name() const {
  return _radioName;
}

const RadioLimits &
DM32UV::limits() const {
  if (nullptr == _limits)
    _limits = new DM32UVLimits();
  return *_limits;
}

const Codeplug &
DM32UV::codeplug() const {
  return _codeplug;
}

Codeplug &
DM32UV::codeplug() {
  return _codeplug;
}


bool
DM32UV::startDownload(const TransferFlags &flags, const ErrorStack &err) {
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
DM32UV::startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err) {
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
DM32UV::startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(selection);

  errMsg(err) << "This device does not support a call-sign DB.";

  return false;
}


bool
DM32UV::startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err) {
  Q_UNUSED(db); Q_UNUSED(flags);

  errMsg(err) << "Satellite config upload is not implemented yet.";

  return false;
}


void
DM32UV::run() {
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
    // Not implemented.
    emit uploadError(this);
    return;
  }
}


bool
DM32UV::download(const ErrorStack &err) {
  // First, we need to get the codeplug address map
  DM32UV::AddressMap addressMap;
  if (! _dev->getAddressMap(addressMap, err)) {
    errMsg(err) << "Cannot read codeplug from device: Cannot get address map.";
    return false;
  }

  // Allocate all mapped memory regions:
  foreach (uint32_t virtualBlockAddress, addressMap.mappedVirtual()) {
    _codeplug.image(0).addElement(virtualBlockAddress, Offset::blockSize());
  }

  if (! _dev->read_start(0, 0, err)) {
    errMsg(err) << "Cannot start reading codeplug.";
    return false;
  }

  // Read all allocated memory regions (sorted by physical address).
  uint32_t blockCount = 0;
  foreach (uint32_t physicalBlockAddress, addressMap.mappedPhysical()) {
    emit downloadProgress(100*blockCount/addressMap.mappedPhysical().count());
    uint32_t virtualBlockAddress = addressMap.toVirtual(physicalBlockAddress);
    if (! _dev->read(0, physicalBlockAddress, _codeplug.data(virtualBlockAddress), Offset::blockSize())) {
      errMsg(err) << "Cannot read codeplug block from "
                  << Qt::hex << physicalBlockAddress << "h (virt. "
                  << Qt::hex << virtualBlockAddress << "h).";
      return false;
    }
  }

  if (! _dev->read_finish(err)) {
    errMsg(err) << "Cannot finish codeplug download.";
    return false;
  }

  return true;
}


bool
DM32UV::upload(const ErrorStack &err) {
  errMsg(err) << "Not implemented yet.";
  return false;
}



