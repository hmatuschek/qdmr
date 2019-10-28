#include "rd5r.hh"
#include "config.hh"
#include <QDebug>

#define BSIZE 128


static Radio::Features _rd5r_features =
{
  true,  // hasDigital
  true,  // hasAnalog
  false, // hasGPS
  8,     // maxNameLength
  16,    // maxIntroLineLength

  1024,  // maxChannels;
  16,    // maxChannelNameLength;

  250,   // maxZones;
  16,    // maxZoneNameLength;
  16,    // maxChannelsInZone;
  false, // hasABZone

  250,   // maxScanlists;
  16,    // maxScanlistNameLength;
  31,    // maxChannelsInScanlist;
  true,  // scanListNeedPriority;

  256,   // maxContacts;
  16,    // maxContactNameLength;

  64,    // maxGrouplists;
  16,    // maxGrouplistNameLength;
  16     // maxContactsInGrouplist;
};


RD5R::RD5R(QObject *parent)
  : Radio(parent), _name("Baofeng/Radiodity RD-5R"), _dev(nullptr), _config(nullptr), _codeplug()
{
  connect(this, SIGNAL(downloadFinished()), this, SLOT(onDonwloadFinished()));
}

const QString &
RD5R::name() const {
  return _name;
}

const Radio::Features &
RD5R::features() const {
  return _rd5r_features;
}

const CodePlug &
RD5R::codeplug() const {
  return _codeplug;
}

CodePlug &
RD5R::codeplug() {
  return _codeplug;
}

bool
RD5R::startDownload(Config *config) {
  _config = config;
  if (!_config)
    return false;

  _dev = new HID(0x15a2, 0x0073);
  if (! _dev->isOpen()) {
    _dev->deleteLater();
    return false;
  }

  _task = StatusDownload;
  _config->reset();

  start();

  return true;
}


void
RD5R::run()
{
  if (StatusDownload == _task) {
    emit downloadStarted();
    qDebug() << __FILE__ << "," << __LINE__ << ":" << __func__ << " Start download.";
    uint btot = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      btot += _codeplug.image(0).element(n).data().size()/BSIZE;
    }

    uint bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      int b0 = _codeplug.image(0).element(n).address()/BSIZE;
      int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
      for (int i=0; i<nb; i++, bcount++) {
        if (! _dev->read_block(b0+i, _codeplug.data((b0+i)*BSIZE), BSIZE)) {
          _task = StatusError;
          _dev->read_finish();
          _dev->close();
          _dev->deleteLater();
          emit downloadError(this);
          return;
        }
        emit downloadProgress(float(bcount*100)/btot);
      }
    }
    _task = StatusIdle;
    _dev->read_finish();
    _dev->close();
    _dev->deleteLater();

    emit downloadFinished();
  } else if (StatusUpload == _task) {
    emit uploadStarted();

    uint btot = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      btot += _codeplug.image(0).element(n).data().size()/BSIZE;
    }

    uint bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      int b0 = _codeplug.image(0).element(n).address()/BSIZE;
      int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
      for (int i=0; i<nb; i++, bcount++) {
        if (! _dev->read_block(b0+i, _codeplug.data((b0+i)*BSIZE), BSIZE)) {
          _task = StatusError;
          _dev->read_finish();
          _dev->close();
          _dev->deleteLater();
          emit uploadError(this);
          return;
        }
        emit uploadProgress(float(bcount*50)/btot);
      }
    }

    // Send encode config into codeplug
    _codeplug.encode(_config);

    // then, upload modified codeplug
    bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      int b0 = _codeplug.image(0).element(n).address()/BSIZE;
      int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
      for (int i=0; i<nb; i++, bcount++) {
        if (! _dev->write_block(b0+i, _codeplug.data((b0+i)*BSIZE), BSIZE)) {
          _task = StatusError;
          _dev->write_finish();
          _dev->close();
          _dev->deleteLater();
          emit uploadError(this);
          return;
        }
        emit uploadProgress(50+float(bcount*50)/btot);
      }
    }
    _dev->write_finish();

    _task = StatusIdle;
    _dev->close();
    _dev->deleteLater();

    emit uploadComplete(this);
  }
}

void
RD5R::onDonwloadFinished() {
  if (_codeplug.decode(_config))
    emit downloadComplete(this, _config);
  else
    emit downloadError(this);
  _config = nullptr;
}

bool
RD5R::startUpload(Config *config) {
  _config = config;
  if (!_config)
    return false;

  _dev = new HID(0x15a2, 0x0073);
  if (!_dev->isOpen()) {
    _dev->deleteLater();
    return false;
  }

  _task = StatusUpload;
  start();

  return true;
}
