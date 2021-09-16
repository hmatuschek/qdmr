#include "gd77.hh"

#include "logger.hh"
#include "config.hh"


#define BSIZE           32

static Radio::Features _gd77_features = {
  .betaWarning = true,

  .hasDigital = true,
  .hasAnalog = true,

  .frequencyLimits = QVector<Radio::Features::FrequencyRange>{ {136., 174.}, {400., 470.} },

  .maxRadioIDs        = 1,
  .needsDefaultRadioID = true,
  .maxIntroLineLength = 16,

  .maxChannels = 1024,
  .maxChannelNameLength = 16,
  .allowChannelNoDefaultContact = true,

  .maxZones = 250,
  .maxZoneNameLength = 16,
  .maxChannelsInZone = 16,
  .hasABZone = false,

  .hasScanlists = true,
  .maxScanlists = 64,
  .maxScanlistNameLength = 15,
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
  .maxCallsignsInDB = 10920
};


GD77::GD77(RadioddityInterface *device, QObject *parent)
  : RadioddityRadio(device, parent), _name("Radioddity GD-77"), _codeplug(), _callsigns()
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
GD77::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection) {
  logDebug() << "Start call-sign DB upload to " << name() << "...";

  if (StatusIdle != _task) {
    logError() << "Cannot upload to radio, radio is not idle.";
    return false;
  }

  // Assemble call-sign db from user DB
  logDebug() << "Encode call-signs into db.";
  _callsigns.encode(db, selection);

  _task = StatusUploadCallsigns;
  if (blocking) {
    logDebug() << "Upload call-sign DB in this thread (blocking).";
    run();
    return (StatusIdle == _task);
  }

  //if (_dev && _dev->isOpen())
  // _dev->moveToThread(this);

  // start thread for upload
  logDebug() << "Upload call-sign DB in separate thread.";
  start();

  return true;
}

bool
GD77::uploadCallsigns()
{
  emit uploadStarted();

  // Check every segment in the codeplug
  if (! _callsigns.isAligned(BSIZE)) {
    _errorMessage = QString("In %1(), cannot upload call-sign DB:\n\t "
                            "Not aligned with block-size!").arg(__func__);
    logError() << _errorMessage;
    return false;
  }

  logDebug() << "Call-sign DB upload started...";

  size_t totb = _callsigns.memSize();
  uint bcount = 0;
  // Then upload callsign DB
  for (int n=0; n<_callsigns.image(0).numElements(); n++) {
    uint addr = _callsigns.image(0).element(n).address();
    uint size = _callsigns.image(0).element(n).data().size();
    uint b0 = addr/BSIZE, nb = size/BSIZE;
    RadioddityInterface::MemoryBank bank = (
          (0x10000 > addr) ? RadioddityInterface::MEMBANK_CALLSIGN_LOWER : RadioddityInterface::MEMBANK_CALLSIGN_UPPER );
    for (uint b=0; b<nb; b++, bcount+=BSIZE) {
      if (! _dev->write(bank, (b0+b)*BSIZE,
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


