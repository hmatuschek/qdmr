#include "opengd77.hh"
#include "opengd77_limits.hh"
#include "logger.hh"
#include "config.hh"


#define BSIZE 32

RadioLimits *OpenGD77::_limits = nullptr;

OpenGD77::OpenGD77(OpenGD77Interface *device, QObject *parent)
  : Radio(parent), _name("Open GD-77"), _dev(device), _config(nullptr), _codeplug(), _callsigns()
{
  // pass...
}

OpenGD77::~OpenGD77() {
  if (_dev && _dev->isOpen())  {
    logDebug() << "Closing device.";
    _dev->reboot();
    _dev->close();
  }
  if (_dev) {
    logDebug() << "Deleting device.";
    _dev->deleteLater();
    _dev = nullptr;
  }
}

const QString &
OpenGD77::name() const {
  return _name;
}

const RadioLimits &
OpenGD77::limits() const {
  if (nullptr == _limits)
    _limits = new OpenGD77Limits();
  return *_limits;
}

const Codeplug &
OpenGD77::codeplug() const {
  return _codeplug;
}

Codeplug &
OpenGD77::codeplug() {
  return _codeplug;
}

RadioInfo
OpenGD77::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::OpenGD77, "opengd77", "OpenGD77", "OpenGD77 Project", OpenGD77Interface::interfaceInfo());
}


bool
OpenGD77::startDownload(bool blocking, const ErrorStack &err) {
  if (StatusIdle != _task) {
    logError() << "Cannot download from radio, radio is not idle.";
    return false;
  }

  _task = StatusDownload;
  _errorStack = err;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  // start thread for download
  start();
  return true;
}


bool
OpenGD77::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags)

  logDebug() << "Start upload to " << name() << "...";

  if (StatusIdle != _task) {
    logError() << "Cannot upload to radio, radio is not idle.";
    return false;
  }

  if (_config)
    delete _config;

  if (! (_config = config)) {
    logError() << "Cannot upload to radio, no config given.";
    return false;
  }
  _config->setParent(this);

  _task = StatusUpload;
  _errorStack = err;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  // start thread for upload
  start();

  return true;
}

bool
OpenGD77::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection, const ErrorStack &err) {
  logDebug() << "Start upload to " << name() << "...";

  if (StatusIdle != _task) {
    logError() << "Cannot upload to radio, radio is not idle.";
    return false;
  }

  // Assemble call-sign db from user DB
  logDebug() << "Encode call-signs into db.";
  _callsigns.encode(db, selection);

  _task = StatusUploadCallsigns;
  _errorStack = err;
  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  // start thread for upload
  start();

  return true;
}


void
OpenGD77::run() {
  if (StatusDownload == _task) {
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      emit downloadError(this);
      return;
    }

    if (! download()) {
      _task = StatusError;
      _dev->read_finish();
      _dev->reboot();
      _dev->close();
      emit downloadError(this);
      return;
    }

    _dev->read_finish();
    _dev->reboot();
    _dev->close();
    _task = StatusIdle;
    emit downloadFinished(this, &_codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    if ((nullptr==_dev) || (! _dev->isOpen())) {
      emit uploadError(this);
      return;
    }

    if (! upload()) {
      _task = StatusError;
      _dev->write_finish();
      _dev->reboot();
      _dev->close();
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

    if (! uploadCallsigns()) {
      _task = StatusError;
      _dev->write_finish();
      _dev->reboot();
      _dev->close();
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
OpenGD77::download()
{
  emit downloadStarted();

  if (_codeplug.numImages() != 2) {
    errMsg(_errorStack) << "Cannot download codeplug: Codeplug does not contain two images.";
    return false;
  }

  // Check every segment in the codeplug
  if (! _codeplug.isAligned(BSIZE)) {
    errMsg(_errorStack) << "Cannot download codeplug: Codeplug is not aligned with blocksize " << BSIZE << ".";
    return false;
  }

  size_t totb = _codeplug.memSize();

  if (! _dev->read_start(0, 0, _errorStack)) {
    errMsg(_errorStack) << "Cannot start codeplug download.";
    _dev->close();
    return false;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      unsigned addr = _codeplug.image(image).element(n).address();
      unsigned size = _codeplug.image(image).element(n).data().size();
      unsigned b0 = addr/BSIZE, nb = size/BSIZE;

      for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE, _errorStack)) {
          errMsg(_errorStack) << "Cannot read block " << (b0+b) << ".";
          return false;
        }
        QThread::usleep(100);
        emit downloadProgress(float(bcount*100)/totb);
      }
    }
    _dev->read_finish(_errorStack);
  }

  return true;
}


bool
OpenGD77::upload()
{
  emit uploadStarted();

  if (_codeplug.numImages() != 2) {
    errMsg(_errorStack) << "Cannot download codeplug: Codeplug does not contain two images.";
    return false;
  }

  // Check every segment in the codeplug
  if (! _codeplug.isAligned(BSIZE)) {
    errMsg(_errorStack) << "Cannot upload code-plug: Codeplug is not aligned with blocksize " << BSIZE << ".";
    return false;
  }

  size_t totb = _codeplug.memSize();

  if (! _dev->read_start(0, 0, _errorStack)) {
    errMsg(_errorStack) << "Cannot start codeplug download.";
    return false;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = ( (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH );

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      unsigned addr = _codeplug.image(image).element(n).address();
      unsigned size = _codeplug.image(image).element(n).data().size();
      unsigned b0 = addr/BSIZE, nb = size/BSIZE;
      for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE, _errorStack)) {
          errMsg(_errorStack) << "Cannot read block " << (b0+b) << ".";
          return false;
        }
        QThread::usleep(100);
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
    _dev->read_finish();
  }

  // Encode config into codeplug
  _codeplug.encode(_config);

  if (! _dev->write_start(0,0, _errorStack)) {
    errMsg(_errorStack) << "Cannot start codeplug upload.";
    return false;
  }

  // Then upload codeplug
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      unsigned addr = _codeplug.image(image).element(n).address();
      unsigned size = _codeplug.image(image).element(n).data().size();
      unsigned b0 = addr/BSIZE, nb = size/BSIZE;

      for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->write(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE, _errorStack)) {
          errMsg(_errorStack) << "Cannot write block " << (b0+b) << ".";
          return false;
        }
        QThread::usleep(100);
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
    _dev->write_finish();
  }

  return true;
}


bool
OpenGD77::uploadCallsigns()
{
  emit uploadStarted();

  // Check every segment in the codeplug
  if (! _callsigns.isAligned(BSIZE)) {
    errMsg(_errorStack) << "Cannot upload call-sign DB: Not aligned with block-size " << BSIZE << "!";
    return false;
  }

  size_t totb = _callsigns.memSize();

  if (! _dev->write_start(OpenGD77Codeplug::FLASH, 0, _errorStack)) {
    errMsg(_errorStack) << "Cannot start callsign DB upload.";
    return false;
  }

  unsigned bcount = 0;
  // Then upload callsign DB
  for (int n=0; n<_callsigns.image(0).numElements(); n++) {
    unsigned addr = _callsigns.image(0).element(n).address();
    unsigned size = _callsigns.image(0).element(n).data().size();
    unsigned b0 = addr/BSIZE, nb = size/BSIZE;
    for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
      if (! _dev->write(OpenGD77Codeplug::FLASH, (b0+b)*BSIZE,
                        _callsigns.data((b0+b)*BSIZE, 0), BSIZE, _errorStack))
      {
        errMsg(_errorStack) << "Cannot write block " << (b0+b) << ".";
        return false;
      }
      emit uploadProgress(float(bcount*100)/totb);
    }
  }

  _dev->write_finish();
  return true;
}

