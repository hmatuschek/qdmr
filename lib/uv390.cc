#include "uv390.hh"
#include "config.hh"


static Radio::Features _uv390_features =
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
  true, // scanListNeedPriority;

  256,  // maxContacts;
  16,   // maxContactNameLength;

  64,   // maxGrouplists;
  16,   // maxGrouplistNameLength;
  16    // maxContactsInGrouplist;
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

    for (size_t bno=1; bno<(_codeplug.size()/1024); bno++)
    {
      if (! _dev->read_block(bno, _codeplug.data(bno*1024), 1024)) {
        _task = StatusError;
        _dev->close();
        _dev->deleteLater();
        emit downloadError(this);
        return;
      }

      emit downloadProgress(float(bno*100)/(_codeplug.size()/1024));
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();

    emit downloadFinished();
  } else if (StatusUpload == _task) {
    emit uploadStarted();

    // First download codeplug from device:
    for (size_t bno=1; bno<(_codeplug.size()/1024); bno++)
    {
      if (! _dev->read_block(bno, _codeplug.data(bno*1024), 1024)) {
        _task = StatusError;
        _dev->close();
        _dev->deleteLater();
        emit uploadError(this);
        return;
      }

      emit uploadProgress(float(bno*50)/(_codeplug.size()/1024));
    }

    // Send encode config into codeplug
    _codeplug.encode(_config);

    // then, upload modified codeplug
    for (size_t bno=1; bno<(_codeplug.size()/1024); bno++)
    {
      if (! _dev->write_block(bno, _codeplug.data(bno*1024), 1024)) {
        _task = StatusError;
        _dev->close();
        _dev->deleteLater();
        emit uploadError(this);
        return;
      }

      emit uploadProgress(50+float(bno*100)/(_codeplug.size()/1024));
    }

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();

    emit uploadComplete(this);
  }
}



