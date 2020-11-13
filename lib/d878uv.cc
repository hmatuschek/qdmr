#include "d878uv.hh"
#include "config.hh"
#include "logger.hh"

#define RBSIZE 64
#define WBSIZE 16

static Radio::Features _d878uv_features =
{
  true,  // hasDigital
  true,  // hasAnalog
  true,  // hasGPS

  16,    // maxNameLength
  10,    // maxIntroLineLength

  3000,  // maxChannels;
  16,    // maxChannelNameLength;

  250,   // maxZones;
  16,    // maxZoneNameLength;
  64,    // maxChannelsInZone;
  true,  // hasABZones

  250,   // maxScanlists;
  16,    // maxScanlistNameLength;
  31,    // maxChannelsInScanlist;
  false, // scanListNeedPriority;

  10000, // maxContacts;
  16,    // maxContactNameLength;

  250,   // maxGrouplists;
  16,    // maxGrouplistNameLength;
  32     // maxContactsInGrouplist;
};


D878UV::D878UV(QObject *parent)
  : Radio(parent), _name("Anytone AT-D878UV"), _dev(nullptr), _codeplugUpdate(true), _config(nullptr)
{
  // pass...
}

const QString &
D878UV::name() const {
  return _name;
}

const Radio::Features &
D878UV::features() const {
  return _d878uv_features;
}

const CodePlug &
D878UV::codeplug() const {
  return _codeplug;
}

CodePlug &
D878UV::codeplug() {
  return _codeplug;
}

bool
D878UV::startDownload(Config *config, bool blocking) {
  if (StatusIdle != _task)
    return false;

  _config = config;
  if (!_config)
    return false;

  _dev = new AnytoneInterface(this);
  if (! _dev->isOpen()) {
    _errorMessage = QString("Cannot open device: %1").arg(_dev->errorMessage());
    _dev->deleteLater();
    return false;
  }

  _task = StatusDownload;

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
D878UV::startUpload(Config *config, bool blocking, bool update) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;

  _dev = new AnytoneInterface(this);
  if (!_dev->isOpen()) {
    _errorMessage = QString("Cannot open device: %1").arg(_dev->errorMessage());
    _dev->deleteLater();
    return false;
  }

  _task = StatusUpload;
  _codeplugUpdate = update;
  if (blocking) {
    this->run();
    return (StatusIdle == _task);
  }

  this->start();
  return true;
}

void
D878UV::run() {
  if (StatusDownload == _task) {
    emit downloadStarted();
    logDebug() << "Download of " << _codeplug.image(0).numElements() << " bitmaps.";

    // Then download bitmaps
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      logDebug() << "Download of block " << n << " " << hex << addr << ":" << size;
      uint b0 = addr/RBSIZE, nb = size/RBSIZE;
      for (uint b=0; b<nb; b++) {
        if (! _dev->read(0, (b0+b)*RBSIZE, _codeplug.data((b0+b)*RBSIZE), RBSIZE)) {
          _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          logError() << _errorMessage;
          _task = StatusError;
          _dev->reboot();
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
      }
    }

    // Allocate remaining memory sections
    uint nstart = _codeplug.image(0).numElements();
    _codeplug.allocateFromBitmaps();

    // Check every segment in the remaining codeplug
    size_t totb = 0;
    for (int n=nstart; n<_codeplug.image(0).numElements(); n++) {
      if (! _codeplug.image(0).element(n).isAligned(RBSIZE)) {
        _errorMessage = QString("%1 Cannot download codeplug: Codeplug element %2 (addr=%3, size=%4) "
                                "is not aligned with blocksize %5.").arg(__func__)
            .arg(n).arg(_codeplug.image(0).element(n).address())
            .arg(_codeplug.image(0).element(n).data().size()).arg(RBSIZE);
        logError() << _errorMessage;
        _task = StatusError;
        _dev->reboot();
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }
      totb += _codeplug.image(0).element(n).data().size()/RBSIZE;
    }

    // Download remaining memory sections
    size_t bcount = 0;
    for (int n=nstart; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      logDebug() << "Download of block " << n << " " << hex << addr << ":" << size;
      uint b0 = addr/RBSIZE, nb = size/RBSIZE;
      for (uint b=0; b<nb; b++, bcount++) {
        if (! _dev->read(0, (b0+b)*RBSIZE, _codeplug.data((b0+b)*RBSIZE), RBSIZE)) {
          _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          logError() << _errorMessage;
          _task = StatusError;
          _dev->reboot();
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
        emit downloadProgress(float(bcount*100)/totb);
      }
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();
    emit downloadFinished(this, &_codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    emit uploadStarted();
    logDebug() << "Upload " << _codeplug.image(0).numElements() << " elements.";

    // Check every segment in the codeplug
    size_t totb = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      logDebug() << "Check element " << (n+1) << " of " << _codeplug.image(0).numElements() << ".";
      if (! _codeplug.image(0).element(n).isAligned(WBSIZE)) {
        _errorMessage = QString("%1 Cannot upload codeplug: Codeplug element %2 (addr=%3, size=%4) "
                                "is not aligned with blocksize %5.").arg(__func__)
            .arg(n).arg(_codeplug.image(0).element(n).address())
            .arg(_codeplug.image(0).element(n).data().size()).arg(WBSIZE);
        logError() << _errorMessage;
        _task = StatusError;
        _dev->reboot();
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }
      totb += _codeplug.image(0).element(n).data().size()/WBSIZE;
    }

    size_t bcount = 0;

    // If codeplug gets updated, download codeplug from device first:
    if (_codeplugUpdate) {
      for (int n=0; n<_codeplug.image(0).numElements(); n++) {
        uint addr = _codeplug.image(0).element(n).address();
        uint size = _codeplug.image(0).element(n).data().size();
        uint b0 = addr/RBSIZE, nb = size/RBSIZE;
        for (uint b=0; b<nb; b++, bcount++) {
          if (! _dev->read(0, (b0+b)*RBSIZE, _codeplug.data((b0+b)*RBSIZE), RBSIZE)) {
            _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
                .arg(_dev->errorMessage());
            logError() << _errorMessage;
            _task = StatusError;
            _dev->reboot();
            _dev->close();
            _dev->deleteLater();
            emit downloadError(this);
            return;
          }
          emit uploadProgress(float(bcount*50)/totb);
        }
      }
    }

    // Encode config into codeplug
    _codeplug.encode(_config);

    // then erase memory
    //_dev->erase(0, 0xd0000);

    // then, upload modified codeplug
    bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      uint b0 = addr/WBSIZE, nb = size/WBSIZE;
      for (size_t b=0; b<nb; b++,bcount++) {
        if (! _dev->write(0, (b0+b)*WBSIZE, _codeplug.data((b0+b)*WBSIZE), WBSIZE)) {
          _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          logError() << _errorMessage;
          _task = StatusError;
          _dev->reboot();
          _dev->close();
          _dev->deleteLater();
          emit uploadError(this);
          return;
        }
        emit uploadProgress(50+float(bcount*50)/totb);
      }
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();

    emit uploadComplete(this);
  }
}



