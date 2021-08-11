#include "gd77.hh"

#include "logger.hh"
#include "config.hh"


#define BSIZE 128

static Radio::Features _gd77_features = {
  .betaWarning = true,

  .hasDigital = true,
  .hasAnalog = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 470.} },

  .maxRadioIDs        = 1,
  .maxNameLength      = 8,
  .maxIntroLineLength = 16,

  .maxChannels = 1024,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 32,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 16,
  .hasABZone = false,

  .hasScanlists = true,
  .maxScanlists = 64,
  .maxScanlistNameLength = 16,
  .maxChannelsInScanlist = 32,
  .scanListNeedsPriority = true,

  .maxContacts = 256,
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
  .callsignDBImplemented = false,
  .maxCallsignsInDB = 0
};


GD77::GD77(HID *device, QObject *parent)
  : Radio(parent), _name("Radioddity GD-77"), _dev(device), _config(nullptr)
{
  if (! connect())
    return;
}

GD77::~GD77() {
  if (_dev && _dev->isOpen()) {
    _dev->reboot();
    _dev->close();
  }
  if (_dev) {
    _dev->deleteLater();
    _dev = nullptr;
  }
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
GD77::startDownload(bool blocking) {
  if (StatusIdle != _task)
    return false;

  _task = StatusDownload;
  if (blocking) {
    run();
    return (StatusIdle == _task);
  }
  start();
  return true;
}

bool
GD77::startUpload(Config *config, bool blocking, const CodePlug::Flags &flags) {
  if (StatusIdle != _task)
    return false;

  if (! (_config = config))
    return false;

  _task = StatusUpload;
  if (blocking) {
    run();
    return (StatusIdle == _task);
  }

  start();
  return true;
}

bool
GD77::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  Q_UNUSED(db);
  Q_UNUSED(blocking);

  _errorMessage = tr("Call-sign DB support for GD77 is not implemented yet.");

  return false;
}

bool
GD77::connect() {
  // If connected -> done
  if (_dev && _dev->isOpen())
    return true;
  // If not open -> reconnect
  if (_dev)
    _dev->deleteLater();
  // connect
  _dev = new HID(0x15a2, 0x0073);
  if (! _dev->isOpen()) {
    _task = StatusError;
    _errorMessage = tr("%1(): Cannot connect to radio: %2").arg(__func__).arg(_dev->errorMessage());
    _dev->deleteLater();
    _dev = nullptr;
    return false;
  }
  return true;
}

void
GD77::run() {
  if (StatusDownload == _task) {
    // ensure connection
    if (! connect()) {
      emit downloadError(this);
      return;
    }
    // try download
    if (! download()) {
      _task = StatusError;
      _dev->read_finish();
      _dev->close();
      emit downloadError(this);
      return;
    }

    _task = StatusIdle;
    _dev->read_finish();
    _dev->reboot();
    _dev->close();
    emit downloadFinished(this, &_codeplug);
  } else if (StatusUpload == _task) {
    if (! connect()) {
      emit uploadError(this);
      return;
    }

    if (! upload()) {
      _task = StatusError;
      _dev->write_finish();
      _dev->close();
      emit uploadError(this);
      return;
    }

    _task = StatusIdle;
    _dev->write_finish();
    _dev->reboot();
    _dev->close();
    emit uploadComplete(this);
  }
}


bool
GD77::download() {
  emit downloadStarted();

  // Check every segment in the codeplug
  size_t totb = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    if (! _codeplug.image(0).element(n).isAligned(BSIZE)) {
      _errorMessage = QString("%1 Cannot download codeplug: Codeplug element %2 (addr=%3, size=%4) "
                              "is not aligned with blocksize %5.").arg(__func__)
          .arg(n).arg(_codeplug.image(0).element(n).address())
          .arg(_codeplug.image(0).element(n).data().size()).arg(BSIZE);
      return false;
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
        return false;
      }
      logDebug() << "Read block " << (b0+b) << ".";
      emit downloadProgress(float(bcount*100)/totb);
    }
  }

  return true;
}


bool
GD77::upload() {
  emit uploadStarted();

  // Check every segment in the codeplug
  size_t totb = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    if (! _codeplug.image(0).element(n).isAligned(BSIZE)) {
      _errorMessage = QString("%1 Cannot upload codeplug: Codeplug element %2 (addr=%3, size=%4) "
                              "is not aligned with blocksize %5.").arg(__func__)
          .arg(n).arg(_codeplug.image(0).element(n).address())
          .arg(_codeplug.image(0).element(n).data().size()).arg(BSIZE);
      return false;
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
      if (! _dev->read(0, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
        _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        _task = StatusError;
        return false;
      }
      emit uploadProgress(float(bcount*50)/totb);
    }
  }

  logDebug() << "Encode config into binary codeplug.";

  // Encode config into codeplug
  _codeplug.encode(_config);

  logDebug() << "Write binary codeplug into device.";

  // then, upload modified codeplug
  bcount = 0;
  for (int n=0; n<_codeplug.image(0).numElements(); n++) {
    uint addr = _codeplug.image(0).element(n).address();
    uint size = _codeplug.image(0).element(n).data().size();
    uint b0 = addr/BSIZE, nb = size/BSIZE;
    for (size_t b=0; b<nb; b++,bcount++) {
      logDebug() << "Write " << BSIZE << "bytes to 0x" << QString::number((b0+b)*BSIZE,16) << ".";
      if (! _dev->write(0, (b0+b)*BSIZE, _codeplug.data((b0+b)*BSIZE), BSIZE)) {
        _errorMessage = QString("%1 Cannot upload codeplug: %2").arg(__func__)
            .arg(_dev->errorMessage());
        return false;
      }
      emit uploadProgress(50+float(bcount*50)/totb);
    }
  }

  _task = StatusIdle;

  return true;
}

