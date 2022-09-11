#include "openrtx.hh"

#include "openrtx_interface.hh"
#include "logger.hh"
#include "config.hh"


#define BSIZE 32

OpenRTX::OpenRTX(OpenRTXInterface *device, QObject *parent)
  : Radio(parent), _name("Open RTX"), _dev(device), _config(nullptr), _codeplug()
{
  if (! connect())
    return;
  logDebug() << "Connected to radio '" << _name << "'.";
}

OpenRTX::~OpenRTX() {
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
OpenRTX::name() const {
  return _name;
}

const Codeplug &
OpenRTX::codeplug() const {
  return _codeplug;
}

Codeplug &
OpenRTX::codeplug() {
  return _codeplug;
}

RadioInfo
OpenRTX::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::OpenRTX, "openrtx", "OpenRTX", USBDeviceInfo());
}


bool
OpenRTX::startDownload(bool blocking, const ErrorStack &err) {
  if (StatusIdle != _task) {
    errMsg(err) << "Cannot download from radio, radio is not idle.";
    return false;
  }

  _task = StatusDownload;

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
OpenRTX::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags)

  logDebug() << "Start upload to " << name() << "...";

  if (StatusIdle != _task) {
    errMsg(err) << "Cannot upload to radio, radio is not idle.";
    return false;
  }

  if (! (_config = config)) {
    errMsg(err) << "Cannot upload to radio, no config given.";
    return false;
  }

  _task = StatusUpload;
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
OpenRTX::startUploadCallsignDB(UserDatabase *db, bool blocking,
                               const CallsignDB::Selection &selection,const ErrorStack &err)
{
  Q_UNUSED(db); Q_UNUSED(blocking); Q_UNUSED(selection)
  errMsg(err) << "OpenRTX has no call-sign DB implemented.";
  return false;
}


void
OpenRTX::run() {
  if (StatusDownload == _task) {
    if (! connect()) {
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
    if (! connect()) {
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
    emit uploadError(this);
    return;
  }
}


bool
OpenRTX::connect(const ErrorStack &err) {
  if (_dev && _dev->isOpen())
    return true;
  if (_dev)
    _dev->deleteLater();

  _dev = new OpenRTXInterface(USBDeviceDescriptor());
  if (! _dev->isOpen()) {
    _task = StatusError;
    errMsg(err) << "Cannot connect to radio.";
    _dev->deleteLater();
    _dev = nullptr;
    return false;
  }

  return true;
}


bool
OpenRTX::download(const ErrorStack &err)
{
  emit downloadStarted();

  if (_codeplug.numImages() != 2) {
    errMsg(err) << "Cannot download codeplug: Codeplug does not contain two images.";
    return false;
  }

  // Check every segment in the codeplug
  if (! _codeplug.isAligned(BSIZE)) {
    errMsg(err) << "Cannot download codeplug: Codeplug is not aligned with blocksize "
                << BSIZE << ".";
    return false;
  }

  size_t totb = _codeplug.memSize();

  if (! _dev->read_start(0, 0, err)) {
    errMsg(err) << "Cannot start codeplug download.";
    _dev->close();
    return false;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = 0;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      unsigned addr = _codeplug.image(image).element(n).address();
      unsigned size = _codeplug.image(image).element(n).data().size();
      unsigned b0 = addr/BSIZE, nb = size/BSIZE;

      for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE, err)) {
          errMsg(err) << "Cannot read block "<< (b0+b) <<".";
          return false;
        }
        QThread::usleep(100);
        emit downloadProgress(float(bcount*100)/totb);
      }
    }
    _dev->read_finish(err);
  }

  return true;
}


bool
OpenRTX::upload(const ErrorStack &err)
{
  emit uploadStarted();

  if (_codeplug.numImages() != 2) {
    errMsg(err) << "Cannot download codeplug: Codeplug does not contain two images.";
    return false;
  }

  // Check every segment in the codeplug
  if (! _codeplug.isAligned(BSIZE)) {
    errMsg(err) << "Cannot upload code-plug: Codeplug is not aligned with blocksize "
                << BSIZE << ".";
    return false;
  }

  size_t totb = _codeplug.memSize();

  if (! _dev->read_start(0, 0, err)) {
    errMsg(err) << "Cannot start codeplug download.";
    return false;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = 0;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      unsigned addr = _codeplug.image(image).element(n).address();
      unsigned size = _codeplug.image(image).element(n).data().size();
      unsigned b0 = addr/BSIZE, nb = size/BSIZE;
      for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE, err)) {
          errMsg(err) << "Cannot read block " << (b0+b) << ".";
          return false;
        }
        QThread::usleep(100);
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
    _dev->read_finish(err);
  }

  // Encode config into codeplug
  _codeplug.encode(_config, Codeplug::Flags(), err);

  if (! _dev->write_start(0,0, err)) {
    errMsg(err) << "Cannot start codeplug upload.";
    return false;
  }

  // Then upload codeplug
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = 0;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      unsigned addr = _codeplug.image(image).element(n).address();
      unsigned size = _codeplug.image(image).element(n).data().size();
      unsigned b0 = addr/BSIZE, nb = size/BSIZE;

      for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->write(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE, err)) {
          errMsg(err) << "Cannot write block " << (b0+b) << ".";
          return false;
        }
        QThread::usleep(100);
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
    _dev->write_finish(err);
  }

  return true;
}

