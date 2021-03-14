#include "d878uv.hh"
#include "config.hh"
#include "logger.hh"

#define RBSIZE 16
#define WBSIZE 16


static Radio::Features _d878uv_features =
{
  true,  // show beta-warning

  true,  // hasDigital
  true,  // hasAnalog
  true,  // hasGPS

  16,    // maxNameLength
  14,    // maxIntroLineLength

  4000,  // maxChannels;
  16,    // maxChannelNameLength;

  250,   // maxZones;
  16,    // maxZoneNameLength;
  250,   // maxChannelsInZone;
  false, // hasABZones

  true,  // hasScanlists;
  250,   // maxScanlists;
  16,    // maxScanlistNameLength;
  31,    // maxChannelsInScanlist;
  false, // scanListNeedPriority;

  10000, // maxContacts;
  16,    // maxContactNameLength;

  250,   // maxGrouplists;
  16,    // maxGrouplistNameLength;
  32,    // maxContactsInGrouplist;

  true,  // hasCallsignDB
  false, // callsignDBImplemented
  0      // maxCallsignsInDB
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
D878UV::startDownload(bool blocking) {
  if (StatusIdle != _task)
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

bool
D878UV::startUploadCallsignDB(UserDatabase *db, bool blocking) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _errorMessage = tr("RD5R does not support a callsign DB.");

  return false;
}

void
D878UV::run() {
  if (StatusDownload == _task)
  {
    emit downloadStarted();

    if (! download())
      return;

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();
    emit downloadFinished(this, &_codeplug);
    _config = nullptr;
  } else if (StatusUpload == _task) {
    emit uploadStarted();

    if (! upload())
      return;

    _task = StatusIdle;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();

    emit uploadComplete(this);
  }
}

bool D878UV::download() {
  logDebug() << "Download of " << _codeplug.image(0).numElements() << " bitmaps.";

  // Download bitmaps
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).data().size();
    logDebug() << "Download of block " << n << " " << Qt::hex << addr << ":" << size;
    if (! _dev->read(0, addr, _codeplug.data(addr), size)) {
      _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      emit downloadError(this);
      return false;
    }
    emit downloadProgress(float(n*100)/_codeplug.image(0).numElements());
  }

  // Allocate remaining memory sections
  uint nstart = _codeplug.image(0).numElements();
  _codeplug.allocateForDecoding();

  // Check every segment in the remaining codeplug
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
      return false;
    }
  }

  // Download remaining memory sections
  for (int n=nstart; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).data().size();
    logDebug() << "Download of block " << n << " " << Qt::hex << addr << ":" << size;
    if (! _dev->read(0, addr, _codeplug.data(addr), size)) {
      _errorMessage = QString("%1 Cannot download codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      emit downloadError(this);
      return false;
    }
    emit downloadProgress(float(n*100)/_codeplug.image(0).numElements());
  }

  return true;
}

bool
D878UV::upload() {
  // Download bitmaps first
  size_t nbitmaps = _codeplug.numImages();
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).data().size();
    logDebug() << "Download of block " << n << " " << Qt::hex << addr << ":" << size;
    if (! _dev->read(0, addr, _codeplug.data(addr), size)) {
      _errorMessage = QString("%1 Cannot read codeplug for update: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      emit uploadError(this);
      return false;
    }
    emit uploadProgress(float(n*25)/_codeplug.image(0).numElements());
  }

  // Allocate all memory sections that must be read first
  // and written back to the device more or less untouched
  _codeplug.allocateUntouched();
  // Download new memory sections for update
  for (int n=nbitmaps; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).data().size();
    logDebug() << "Download of block " << n << " " << Qt::hex << addr << ":" << size;
    if (! _dev->read(0, addr, _codeplug.data(addr), size)) {
      _errorMessage = QString("%1 Cannot read codeplug for update: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      emit uploadError(this);
      return false;
    }
    emit uploadProgress(25+float(n*25)/_codeplug.image(0).numElements());
  }

  // Update bitmaps for all elements representing the common Config
  _codeplug.setBitmaps(_config);
  // Allocate all memory elements representing the common config
  _codeplug.allocateForEncoding();

  // Update binary codeplug from config
  if (! _codeplug.encode(_config)) {
    _errorMessage = QString("Cannot encode codeplug: %1").arg(_codeplug.errorMessage());
    logError() << _errorMessage;
    _task = StatusError;
    _dev->reboot();
    _dev->close();
    _dev->deleteLater();
    emit uploadError(this);
    return false;
  }

  // Sort all elements before uploading
  _codeplug.image(0).sort();

  // Upload all elements back to the device
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).data().size();
    if (! _dev->write(0, addr, _codeplug.data(addr), size)) {
      _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
          .arg(_dev->errorMessage());
      logError() << _errorMessage;
      _task = StatusError;
      _dev->reboot();
      _dev->close();
      _dev->deleteLater();
      emit uploadError(this);
      return false;
    }
    emit uploadProgress(50+float(n*50)/_codeplug.image(0).numElements());
  }
  //_codeplug.write("debug_codeplug.dfu");
  return true;
}

