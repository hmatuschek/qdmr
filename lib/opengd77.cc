#include "opengd77.hh"

#include "logger.hh"
#include "config.hh"


#define BSIZE 32

static Radio::Features _open_gd77_features =
{
  .betaWarning = false,

  .hasDigital = true,
  .hasAnalog = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 470.} },

  .maxRadioIDs        = 1,
  .needsDefaultRadioID = true,
  .maxIntroLineLength = 16,

  .maxChannels = 1024,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 68,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 80,
  .hasABZone = false,

  .hasScanlists = false,
  .maxScanlists = 64,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 32,
  .scanListNeedsPriority = true,

  .maxContacts = 1024,
  .maxContactNameLength = 16,

  .maxGrouplists = 76,
  .maxGrouplistNameLength = 16,
  .maxContactsInGrouplist = 32,

  .hasGPS = false,
  .maxGPSSystems = 0,

  .hasAPRS = false,
  .maxAPRSSystems = 0,

  .hasRoaming = false,
  .maxRoamingChannels = 0,
  .maxRoamingZones = 0,
  .maxChannelsInRoamingZone = 0,

  .hasCallsignDB = true,
  .callsignDBImplemented = true,
  .maxCallsignsInDB = 15796
};


OpenGD77::OpenGD77(OpenGD77Interface *device, QObject *parent)
  : Radio(parent), _name("Open GD-77"), _dev(device), _config(nullptr), _codeplug(), _callsigns()
{
  if (! connect())
    return;
  logDebug() << "Connected to radio '" << _name << "'.";
}

OpenGD77::~OpenGD77() {
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

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  // start thread for download
  start();
  return true;
}


bool
OpenGD77::startUpload(Config *config, bool blocking, const CodePlug::Flags &flags) {
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

  // If non-blocking -> move device to this thread
  if (_dev && _dev->isOpen())
    _dev->moveToThread(this);
  // start thread for upload
  start();

  return true;
}

bool
OpenGD77::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  logDebug() << "Start upload to " << name() << "...";

  if (StatusIdle != _task) {
    logError() << "Cannot upload to radio, radio is not idle.";
    return false;
  }

  // Assemble call-sign db from user DB
  logDebug() << "Encode call-signs into db.";
  _callsigns.encode(db, selection);

  _task = StatusUploadCallsigns;
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


void
OpenGD77::run() {
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
    if (! connect()) {
      emit uploadError(this);
      return;
    }

    if (! uploadCallsigns()) {
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
  }
}


bool
OpenGD77::connect() {
  if (_dev && _dev->isOpen())
    return true;
  if (_dev)
    _dev->deleteLater();

  _dev = new OpenGD77Interface();
  if (! _dev->isOpen()) {
    _task = StatusError;
    _errorMessage = QString("Cannot connect to radio: %1").arg(_dev->errorString());
    logError() << _errorMessage;
    _dev->deleteLater();
    _dev = nullptr;
    return false;
  }

  return true;
}


bool
OpenGD77::download()
{
  emit downloadStarted();

  if (_codeplug.numImages() != 2) {
    _errorMessage = QString("In %1(), cannot download codeplug:\n\t"
                            " Codeplug does not contain two images.").arg(__func__);
    return false;
  }

  // Check every segment in the codeplug
  if (! _codeplug.isAligned(BSIZE)) {
    _errorMessage = QString("In %1(), cannot download codeplug:\n\t "
                            "Codeplug is not aligned with blocksize %5.").arg(__func__).arg(BSIZE);
    logError() << _errorMessage;
    return false;
  }

  size_t totb = _codeplug.memSize();

  if (! _dev->read_start(0, 0)) {
    _errorMessage = QString("in %1(), cannot start codeplug download:\n\t %2")
        .arg(__func__).arg(_dev->errorMessage());
    logError() << _errorMessage;
    _dev->close();
    return false;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      uint addr = _codeplug.image(image).element(n).address();
      uint size = _codeplug.image(image).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;

      for (uint b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE)) {
          _errorMessage = QString("In %1(), cannot read block %2:\n\t %3")
              .arg(__func__).arg(b0+b).arg(_dev->errorMessage());
          logError() << _errorMessage;
          return false;
        }
        QThread::usleep(100);
        emit downloadProgress(float(bcount*100)/totb);
      }
    }
    _dev->read_finish();
  }

  return true;
}


bool
OpenGD77::upload()
{
  emit uploadStarted();

  if (_codeplug.numImages() != 2) {
    _errorMessage = QString("In %1(), cannot download codeplug:\n\t"
                            " Codeplug does not contain two images.").arg(__func__);
    logError() << _errorMessage;
    return false;
  }

  // Check every segment in the codeplug
  if (! _codeplug.isAligned(BSIZE)) {
    _errorMessage = QString("In %1(), cannot upload code-plug:\n\t "
                            "Codeplug is not aligned with blocksize %5.").arg(__func__).arg(BSIZE);
    logError() << _errorMessage;
    return false;
  }

  size_t totb = _codeplug.memSize();

  if (! _dev->read_start(0, 0)) {
    _errorMessage = QString("in %1(), cannot start codeplug download:\n\t %2")
        .arg(__func__).arg(_dev->errorMessage());
    logError() << _errorMessage;
    return false;
  }

  // Then download codeplug
  size_t bcount = 0;
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = ( (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH );

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      uint addr = _codeplug.image(image).element(n).address();
      uint size = _codeplug.image(image).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;
      for (uint b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->read(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE)) {
          _errorMessage = QString("In %1(), cannot read block %2:\n\t %3")
              .arg(__func__).arg(b0+b).arg(_dev->errorMessage());
          logError() << _errorMessage;
          return false;
        }
        QThread::usleep(100);
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
    return false;
  }

  // Then upload codeplug
  for (int image=0; image<_codeplug.numImages(); image++) {
    uint32_t bank = (0 == image) ? OpenGD77Codeplug::EEPROM : OpenGD77Codeplug::FLASH;

    for (int n=0; n<_codeplug.image(image).numElements(); n++) {
      uint addr = _codeplug.image(image).element(n).address();
      uint size = _codeplug.image(image).element(n).data().size();
      uint b0 = addr/BSIZE, nb = size/BSIZE;

      for (uint b=0; b<nb; b++, bcount+=BSIZE) {
        if (! _dev->write(bank, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE, image), BSIZE)) {
          _errorMessage = QString("In %1(), cannot write block %2:\n\t %3")
              .arg(__func__).arg(b0+b).arg(_dev->errorMessage());
          logError() << _errorMessage;
          return false;
        }
        QThread::usleep(100);
        emit uploadProgress(float(bcount*50)/totb);
      }
    }
    _dev->write_finish();
  }

  return true;
}


bool
OpenGD77::uploadCallsigns()
{
  emit uploadStarted();

  // Check every segment in the codeplug
  if (! _callsigns.isAligned(BSIZE)) {
    _errorMessage = QString("In %1(), cannot upload call-sign DB:\n\t "
                            "Not aligned with block-size!").arg(__func__);
    logError() << _errorMessage;
    return false;
  }

  size_t totb = _callsigns.memSize();

  if (! _dev->write_start(OpenGD77Codeplug::FLASH, 0)) {
    _errorMessage = QString("in %1(), cannot start callsign DB upload:\n\t %2")
        .arg(__func__).arg(_dev->errorMessage());
    logError() << _errorMessage;
    return false;
  }

  uint bcount = 0;
  // Then upload callsign DB
  for (int n=0; n<_callsigns.image(0).numElements(); n++) {
    uint addr = _callsigns.image(0).element(n).address();
    uint size = _callsigns.image(0).element(n).data().size();
    uint b0 = addr/BSIZE, nb = size/BSIZE;
    for (uint b=0; b<nb; b++, bcount+=BSIZE) {
      if (! _dev->write(OpenGD77Codeplug::FLASH, (b0+b)*BSIZE,
                        _callsigns.data((b0+b)*BSIZE, 0), BSIZE))
      {
        _errorMessage = QString("In %1(), cannot write block %2:\n\t %3")
            .arg(__func__).arg(b0+b).arg(_dev->errorMessage());
        logError() << _errorMessage;
        return false;
      }
      emit uploadProgress(float(bcount*100)/totb);
    }
  }

  _dev->write_finish();
  return true;
}

