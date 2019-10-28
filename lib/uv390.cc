#include "uv390.hh"
#include "config.hh"
#include <QDebug>


#define BSIZE 1024

static Radio::Features _uv390_features =
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


UV390::UV390(QObject *parent)
  : Radio(parent), _name("TYT MD-UV390"), _dev(nullptr), _config(nullptr)
{
  // pass...
}

const QString &
UV390::name() const {
  return _name;
}

const Radio::Features &
UV390::features() const {
  return _uv390_features;
}

const CodePlug &
UV390::codeplug() const {
  return _codeplug;
}

CodePlug &
UV390::codeplug() {
  return _codeplug;
}

bool
UV390::startDownload(Config *config) {
  if (StatusIdle != _task)
    return false;

  _config = config;
  if (!_config)
    return false;

  _dev = new DFUDevice(0x0483, 0xdf11, this);
  if (! _dev->isOpen()) {
    _dev->deleteLater();
    return false;
  }

  _task = StatusDownload;
  _config->reset();

  start();

  return true;
}

bool
UV390::startUpload(Config *config) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;

  _dev = new DFUDevice(0x15a2, 0x0073, this);
  if (!_dev->isOpen()) {
    _dev->deleteLater();
    return false;
  }

  _task = StatusUpload;
  start();

  return true;
}

void
UV390::onDonwloadFinished() {
  if (_codeplug.decode(_config))
    emit downloadComplete(this, _config);
  else
    emit downloadError(this);
  _config = nullptr;
}

void
UV390::run() {
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
        if (! _dev->read_block(b0+b, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          _task = StatusError;
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
        qDebug() << "Read block" << (b0+b);
        emit downloadProgress(float(bcount*100)/totb);
      }
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();

    emit downloadFinished();
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
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }
      totb += _codeplug.image(0).element(n).data().size()/BSIZE;
    }

    // First download codeplug from device:
    size_t bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;
      for (uint b=0; b<nb; b++, bcount++) {
        if (! _dev->read_block(b0+b, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          _task = StatusError;
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
        emit uploadProgress(float(bcount*50)/totb);
      }
    }

    // Send encode config into codeplug
    _codeplug.encode(_config);

    // then, upload modified codeplug
    bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      uint addr = _codeplug.image(0).element(n).address();
      uint size = _codeplug.image(0).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;
      for (size_t b=1; b<nb; b++,bcount++) {
        if (! _dev->write_block(b, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
          _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
          _task = StatusError;
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



