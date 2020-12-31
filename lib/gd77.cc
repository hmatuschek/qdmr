#include "gd77.hh"

#include "logger.hh"
#include "config.hh"


#define BSIZE 1024

static Radio::Features _gd77_features =
{
  true,  // hasDigital
  true,  // hasAnalog
  false, // hasGPS
  8,     // maxNameLength
  16,    // maxIntroLineLength

  1024,  // maxChannels
  16,    // maxChannelNameLength

  32,    // maxZones
  16,    // maxZoneNameLength
  16,    // maxChannelsInZone
  false, // hasABZone

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


GD77::GD77(QObject *parent)
  : Radio(parent), _name("Radioddity GD-77"), _dev(nullptr), _config(nullptr)
{
  // pass...
}

const QString &
GD77::name() const {
  return _name;
}

const Radio::Features &
GD77::features() const {
  return _gd77_features;
}

const CodePlug &
GD77::codeplug() const {
  return _codeplug;
}

CodePlug &
GD77::codeplug() {
  return _codeplug;
}

bool
GD77::startDownload(Config *config, bool blocking) {
  if (StatusIdle != _task)
    return false;

  _config = config;
  if (!_config)
    return false;

  _dev = new HID(0x0483, 0xdf11, this);
  if (! _dev->isOpen()) {
    _dev->deleteLater();
    return false;
  }

  _task = StatusDownload;
  _config->reset();

  if (blocking) {
    run();
    return (StatusIdle == _task);
  }
  start();
  return true;
}

bool
GD77::startUpload(Config *config, bool blocking, bool update) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;

  _dev = new HID(0x15a2, 0x0073, this);
  if (!_dev->isOpen()) {
    _dev->deleteLater();
    return false;
  }

  _task = StatusUpload;
  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

void
GD77::run() {
  if (StatusDownload == _task) {
    emit downloadStarted();

    // Check every segment in the codeplug
    size_t totb = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      if (! _codeplug.image(0).element(n).isAligned(BSIZE)) {
        _errorMessage = QString("%1 Cannot download codeplug: Codeplug element %2 (addr=%3, size=%4) "
                                "is not aligned with blocksize %5.").arg(__func__)
            .arg(n).arg(_codeplug.image(0).element(n).address())
            .arg(_codeplug.image(0).element(n).data().size()).arg(BSIZE);
        _task = StatusError;
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }
      totb += _codeplug.image(0).element(n).data().size()/BSIZE;
    }

    // Then download codeplug
    size_t bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;
      for (uint b=0; b<nb; b++, bcount++) {
        if (! _dev->read(0, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          _task = StatusError;
          _dev->read_finish();
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
        logDebug() << "Read block " << (b0+b) << ".";
        emit downloadProgress(float(bcount*100)/totb);
      }
    }
    _dev->read_finish();

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();
    emit downloadFinished(this, &_codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    emit uploadStarted();

    // Check every segment in the codeplug
    size_t totb = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      if (! _codeplug.image(0).element(n).isAligned(BSIZE)) {
        _errorMessage = QString("%1 Cannot upload codeplug: Codeplug element %2 (addr=%3, size=%4) "
                                "is not aligned with blocksize %5.").arg(__func__)
            .arg(n).arg(_codeplug.image(0).element(n).address())
            .arg(_codeplug.image(0).element(n).data().size()).arg(BSIZE);
        _task = StatusError;
        _dev->read_finish();
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }
      totb += _codeplug.image(0).element(n).data().size()/BSIZE;
    }
    _dev->read_finish();

    // First download codeplug from device:
    size_t bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;
      for (uint b=0; b<nb; b++, bcount++) {
        if (! _dev->read(0, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          _task = StatusError;
          _dev->read_finish();
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
        emit uploadProgress(float(bcount*50)/totb);
      }
    }

    // Encode config into codeplug
    _codeplug.encode(_config);

    // then, upload modified codeplug
    bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;
      for (size_t b=1; b<nb; b++,bcount++) {
        if (! _dev->write(0, b*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          _task = StatusError;
          _dev->write_finish();
          _dev->close();
          _dev->deleteLater();
          emit uploadError(this);
          return;
        }
        emit uploadProgress(50+float(bcount*50)/totb);
      }
    }

    _task = StatusIdle;
    _dev->write_finish();
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();

    emit uploadComplete(this);
  }
}



