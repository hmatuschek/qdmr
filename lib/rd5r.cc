#include "rd5r.hh"
#include "config.hh"

#define BSIZE 128


static Radio::Features _rd5r_features =
{
  .betaWarning = false,

  .hasDigital = true,
  .hasAnalog  = true,

  .maxNameLength = 8,
  .maxIntroLineLength = 16,

  .maxChannels = 1024,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 250,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 16,
  .hasABZone = false,

  .hasScanlists = true,
  .maxScanlists = 250,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 31,
  .scanListNeedsPriority = true,

  .maxContacts = 256,
  .maxContactNameLength = 16,

  .maxGrouplists = 64,
  .maxGrouplistNameLength = 16,
  .maxContactsInGrouplist = 16,

  .hasGPS = false,
  .maxGPSSystems = 0,

  .hasAPRS = false,
  .maxAPRSSystems = 0,

  .hasRoaming = false,
  .maxRoamingChannels = 0,
  .maxRoamingZones = 0,
  .maxChannelsInRoamingZone = 0,

  .hasCallsignDB = false,
  .callsignDBImplemented = false,
  .maxCallsignsInDB = 0
};


RD5R::RD5R(QObject *parent)
  : Radio(parent), _name("Baofeng/Radioddity RD-5R"), _dev(nullptr), _codeplugUpdate(true),
    _config(nullptr), _codeplug()
{
  // pass...
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
RD5R::startDownload(bool blocking) {
  _dev = new HID(0x15a2, 0x0073);
  if (! _dev->isOpen()) {
    _errorMessage = tr("%1(): Cannot open Download codeplug: %2")
        .arg(__func__).arg(_dev->errorMessage());
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
RD5R::startUpload(Config *config, bool blocking, bool update) {
  _config = config;
  if (!_config)
    return false;

  _dev = new HID(0x15a2, 0x0073);
  if (! _dev->isOpen()) {
    _dev->deleteLater();
    return false;
  }

  _task = StatusUpload;
  _codeplugUpdate = update;
  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
RD5R::startUploadCallsignDB(UserDatabase *db, bool blocking) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _errorMessage = tr("RD5R does not support a callsign DB.");

  return false;
}

void
RD5R::run()
{
  if (StatusDownload == _task) {
    emit downloadStarted();
    uint btot = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      btot += _codeplug.image(0).element(n).data().size()/BSIZE;
    }

    uint bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      int b0 = _codeplug.image(0).element(n).address()/BSIZE;
      int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
      for (int i=0; i<nb; i++, bcount++) {
        if (! _dev->read(0, (b0+i)*BSIZE, _codeplug.data((b0+i)*BSIZE), BSIZE)) {
          _errorMessage = tr("%1: Cannot download codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
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
    emit downloadFinished(this, &_codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    emit uploadStarted();

    uint btot = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      btot += _codeplug.image(0).element(n).data().size()/BSIZE;
    }

    uint bcount = 0;
    if (_codeplugUpdate) {
      // If codeplug gets updated, download codeplug from device first:
      for (int n=0; n<_codeplug.image(0).numElements(); n++) {
        int b0 = _codeplug.image(0).element(n).address()/BSIZE;
        int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
        for (int i=0; i<nb; i++, bcount++) {
          if (! _dev->read(0, (b0+i)*BSIZE, _codeplug.data((b0+i)*BSIZE), BSIZE)) {
            _errorMessage = tr("%1: Cannot upload codeplug: %2").arg(__func__)
                .arg(_dev->errorMessage());
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
    }

    // Encode config into codeplug
    if (! _codeplug.encode(_config, _codeplugUpdate)) {
      _errorMessage = tr("%1(): Upload failed: %2")
          .arg(__func__).arg(_codeplug.errorMessage());
      _task = StatusError;
      _dev->read_finish();
      _dev->close();
      _dev->deleteLater();
      emit uploadError(this);
      return;
    }

    // then, upload modified codeplug
    bcount = 0;
    for (int n=0; n<_codeplug.image(0).numElements(); n++) {
      int b0 = _codeplug.image(0).element(n).address()/BSIZE;
      int nb = _codeplug.image(0).element(n).data().size()/BSIZE;
      for (int i=0; i<nb; i++, bcount++) {
        if (! _dev->write(0, (b0+i)*BSIZE, _codeplug.data((b0+i)*BSIZE), BSIZE)) {
          _errorMessage = tr("%1: Cannot upload codeplug: %2").arg(__func__)
              .arg(_dev->errorMessage());
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
