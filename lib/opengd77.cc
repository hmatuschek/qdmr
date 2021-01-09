#include "opengd77.hh"

#include "logger.hh"
#include "config.hh"


#define BSIZE 32

static Radio::Features _open_gd77_features =
{
  false, // isBeta

  true,  // hasDigital
  true,  // hasAnalog
  false, // hasGPS
  8,     // maxNameLength
  16,    // maxIntroLineLength

  1024,  // maxChannels
  16,    // maxChannelNameLength

  32,    // maxZones
  16,    // maxZoneNameLength
  80,    // maxChannelsInZone
  false, // hasABZone

  false, // hasScanlists
  64,    // maxScanlists
  16,    // maxScanlistNameLength
  32,    // maxChannelsInScanlist
  true,  // scanListNeedPriority

  256,   // maxContacts
  16,    // maxContactNameLength

  76,   // maxGrouplists
  16,   // maxGrouplistNameLength
  32    // maxContactsInGrouplist
};


OpenGD77::OpenGD77(QObject *parent)
  : Radio(parent), _name("Open GD-77"), _dev(nullptr), _config(nullptr)
{
  // pass...
}

OpenGD77::~OpenGD77() {
  // pass...
}

const QString &
OpenGD77::name() const {
  return _name;
}

const Radio::Features &
OpenGD77::features() const {
  return _open_gd77_features;
}

const CodePlug &
OpenGD77::codeplug() const {
  return _codeplug;
}

CodePlug &
OpenGD77::codeplug() {
  return _codeplug;
}


bool
OpenGD77::startDownload(bool blocking) {
  if (StatusIdle != _task) {
    logError() << "Cannot download from radio, radio is not idle.";
    return false;
  }

  _task = StatusDownload;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  // start thread for download
  start();
  return true;
}


bool
OpenGD77::startUpload(Config *config, bool blocking, bool update) {
  logDebug() << "Start upload to " << name() << "...";

  if (StatusIdle != _task) {
    logError() << "Cannot upload to radio, radio is not idle.";
    return false;
  }

  if (! (_config = config)) {
    logError() << "Cannot upload to radio, no config given.";
    return false;
  }

  _task = StatusUpload;
  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  // start thread for upload
  start();
  return true;
}


void
OpenGD77::run() {
  if (StatusDownload == _task) {
    download();
  } else if (StatusUpload == _task) {
    upload();
  }
}


void
OpenGD77::download()
{
  _dev = new OpenGD77Interface();
  if (! _dev->isOpen()) {
    _task = StatusError;
    _errorMessage = tr("In %1(), cannot open OpenGD77 device:\n\t%2").arg(__func__).arg(_dev->errorMessage());
    logError() << _errorMessage;
    _dev->deleteLater();
    _dev = nullptr;
    emit downloadError(this);
    return;
  }

  emit downloadStarted();

  if (_codeplug.numImages() != 2) {
    _errorMessage = QString("In %1(), cannot download codeplug:\n\t"
                            " Codeplug does not contain two images.").arg(__func__);
    _task = StatusError;
    _dev->close();
    _dev->deleteLater();
    _dev = nullptr;
    emit downloadError(this);
    return;
  }

  // Check every segment in the codeplug
  size_t totb = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      if (! _codeplug.image(0).element(n).isAligned(BSIZE)) {
        _errorMessage = QString("In %1(), cannot download codeplug:\n\t Codeplug element %2 (addr=%3, size=%4) "
                                "is not aligned with blocksize %5.").arg(__func__)
            .arg(n).arg(_codeplug.image(0).element(n).address())
            .arg(_codeplug.image(0).element(n).data().size()).arg(BSIZE);
        logError() << _errorMessage;
        _task = StatusError;
        _dev->close();
        _dev->deleteLater();
        _dev = nullptr;
        emit downloadError(this);
        return;
      }
      totb += _codeplug.image(0).element(n).data().size()/BSIZE;
    }
  }

  if (! _dev->read_start(0, 0)) {
    _errorMessage = QString("in %1(), cannot start codeplug download:\n\t %2")
        .arg(__func__).arg(_dev->errorMessage());
    logError() << _errorMessage;
    _task = StatusError;
    _dev->close();
    _dev->deleteLater();
    _dev = nullptr;
    emit downloadError(this);
    return;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      uint addr = _codeplug.image(image).element(n).address();
      uint size = _codeplug.image(image).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;

      for (uint b=0; b<nb; b++, bcount++) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE)) {
          _errorMessage = QString("In %1(), cannot read block %2:\n\t %3")
              .arg(__func__).arg(b0+b).arg(_dev->errorMessage());
          logError() << _errorMessage;
          _task = StatusError;
          _dev->read_finish();
          _dev->close();
          _dev->deleteLater();
          _dev = nullptr;
          emit downloadError(this);
          return;
        }
        emit downloadProgress(float(bcount*100)/totb);
      }
    }
    _dev->read_finish();
  }

  _dev->reboot();

  _task = StatusIdle;
  _dev->close();
  _dev->deleteLater();
  _dev = nullptr;
  emit downloadFinished(this, &_codeplug);
  _config = nullptr;
}


void
OpenGD77::upload() {
  _dev = new OpenGD77Interface();
  if (!_dev->isOpen()) {
    _task = StatusError;
    _errorMessage = QString("Cannot upload to radio, device is not open: %1").arg(_dev->errorString());
    logError() << _errorMessage;
    _dev->deleteLater();
    _dev = nullptr;
    emit uploadError(this);
    return;
  }


  emit uploadStarted();

  if (_codeplug.numImages() != 2) {
    _errorMessage = QString("In %1(), cannot download codeplug:\n\t"
                            " Codeplug does not contain two images.").arg(__func__);
    logError() << _errorMessage;
    _task = StatusError;
    _dev->close();
    _dev->deleteLater();
    _dev = nullptr;
    emit uploadError(this);
    return;
  }

  // Check every segment in the codeplug
  size_t totb = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      if (! _codeplug.image(0).element(n).isAligned(BSIZE)) {
        _errorMessage = QString("In %1(), cannot download codeplug:\n\t Codeplug element %2 (addr=%3, size=%4) "
                                "is not aligned with blocksize %5.").arg(__func__)
            .arg(n).arg(_codeplug.image(0).element(n).address())
            .arg(_codeplug.image(0).element(n).data().size()).arg(BSIZE);
        logError() << _errorMessage;
        _task = StatusError;
        _dev->close();
        _dev->deleteLater();
        _dev = nullptr;
        emit uploadError(this);
        return;
      }
      totb += _codeplug.image(0).element(n).data().size()/BSIZE;
    }
  }

  if (! _dev->read_start(0, 0)) {
    _errorMessage = QString("in %1(), cannot start codeplug download:\n\t %2")
        .arg(__func__).arg(_dev->errorMessage());
    logError() << _errorMessage;
    _task = StatusError;
    _dev->close();
    _dev->deleteLater();
    _dev = nullptr;
    emit uploadError(this);
    return;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      uint addr = _codeplug.image(image).element(n).address();
      uint size = _codeplug.image(image).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;

      for (uint b=0; b<nb; b++, bcount++) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE)) {
          _errorMessage = QString("In %1(), cannot read block %2:\n\t %3")
              .arg(__func__).arg(b0+b).arg(_dev->errorMessage());
          logError() << _errorMessage;
          _task = StatusError;
          _dev->read_finish();
          _dev->close();
          _dev->deleteLater();
          _dev = nullptr;
          emit uploadError(this);
          return;
        }
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
    _dev->read_finish();
  }

  // Encode config into codeplug
  _codeplug.encode(_config);

  if (! _dev->write_start(0,0)) {
    _errorMessage = QString("in %1(), cannot start codeplug upload:\n\t %2")
        .arg(__func__).arg(_dev->errorMessage());
    logError() << _errorMessage;
    _task = StatusError;
    _dev->close();
    _dev->deleteLater();
    _dev = nullptr;
    emit uploadError(this);
    return;
  }

  // Then upload codeplug
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      uint addr = _codeplug.image(image).element(n).address();
      uint size = _codeplug.image(image).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;

      for (uint b=0; b<nb; b++, bcount++) {
        if (! _dev->write(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE)) {
          _errorMessage = QString("In %1(), cannot write block %2:\n\t %3")
              .arg(__func__).arg(b0+b).arg(_dev->errorMessage());
          logError() << _errorMessage;
          _task = StatusError;
          _dev->write_finish();
          _dev->close();
          _dev->deleteLater();
          _dev = nullptr;
          emit uploadError(this);
          return;
        }
        emit uploadProgress(50.0+float(bcount*50)/totb);
      }
    }
    _dev->write_finish();
  }

  _task = StatusIdle;
  _dev->reboot();
  _dev->close();
  _dev->deleteLater();
  _dev = nullptr;

  emit uploadComplete(this);
}

