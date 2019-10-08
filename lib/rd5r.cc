#include "rd5r.hh"
#include "config.hh"

static Radio::Features _rd5r_features =
{
  true, // hasDigital
  true, // hasAnalog

  8,    // maxNameLength
  16,   // maxIntroLineLength

  1024, // maxChannels;
  16,   // maxChannelNameLength;

  250,  // maxZones;
  16,   // maxZoneNameLength;
  16,   // maxChannelsInZone;

  250,  // maxScanlists;
  16,   // maxScanlistNameLength;
  31,   // maxChannelsInScanlist;

  256,  // maxContacts;
  16,   // maxContactNameLength;

  64,   // maxGrouplists;
  16,   // maxGrouplistNameLength;
  16    // maxContactsInGrouplist;
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

  _task = TaskDownload;
  _config->reset();

  start();

  return true;
}


void
RD5R::run()
{
  if (TaskDownload == _task) {
    emit downloadStarted();

    for (int bno=1; bno<966; bno++)
    {
      // Skip range 0x7c00...0x8000.
      if (bno >= 248 && bno < 256)
        continue;

      if (! _dev->readBlock(bno, &_codeplug.data()[bno*128], 128)) {
        _task = TaskNone;
        _dev->readFinish();
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }

      emit downloadProgress(float(bno*100)/966);
    }

    _task = TaskNone;
    _dev->readFinish();
    _dev->close();
    _dev->deleteLater();

    emit downloadFinished();
  } else if (TaskUpload == _task) {
    emit uploadStarted();

    for (int bno=1; bno<966; bno++)
    {
      if (bno >= 248 && bno < 256) {
        // Skip range 0x7c00...0x8000.
        continue;
      }

      if (! _dev->readBlock(bno, &_codeplug.data()[bno*128], 128)) {
        _task = TaskNone;
        _dev->readFinish();
        _dev->close();
        _dev->deleteLater();
        emit uploadError(this);
        return;
      }

      emit uploadProgress(float(bno*50)/966);
    }

    _codeplug.encode(_config);

    for (int bno=1; bno<966; bno++)
    {
      if (bno >= 248 && bno < 256) {
        // Skip range 0x7c00...0x8000.
        continue;
      }

      if (! _dev->writeBlock(bno, &_codeplug.data()[bno*128], 128)) {
        _task = TaskNone;
        _dev->readFinish();
        _dev->close();
        _dev->deleteLater();
        emit uploadError(this);
        return;
      }

      emit uploadProgress(50+float(bno*100)/966);
    }
    _dev->writeFinish();

    _task = TaskNone;
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

  _task = TaskUpload;
  start();

  return true;
}
