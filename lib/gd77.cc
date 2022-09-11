#include "gd77.hh"
#include "gd77_limits.hh"

#include "logger.hh"
#include "config.hh"


#define BSIZE           32

RadioLimits * GD77::_limits = nullptr;


GD77::GD77(RadioddityInterface *device, QObject *parent)
  : RadioddityRadio(device, parent), _name("Radioddity GD-77"), _codeplug(), _callsigns()
{
  // pass...
}

const QString &
GD77::name() const {
  return _name;
}

const RadioLimits &
GD77::limits() const {
  if (nullptr == _limits)
    _limits = new GD77Limits();
  return *_limits;
}

const Codeplug &
GD77::codeplug() const {
  return _codeplug;
}

Codeplug &
GD77::codeplug() {
  return _codeplug;
}

RadioInfo
GD77::defaultRadioInfo() {
  return RadioInfo(
        RadioInfo::GD77, "gd77", "GD-77", "Radioddity", RadioddityInterface::interfaceInfo());
}


bool
GD77::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection, const ErrorStack &err) {
  logDebug() << "Start call-sign DB upload to " << name() << "...";
  _errorStack = err;

  if (StatusIdle != _task) {
    errMsg(err) << "Cannot upload to radio, radio is not idle.";
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
    errMsg(_errorStack) << "Cannot upload call-sign DB: Not aligned with block-size " << BSIZE << ".";
    return false;
  }

  logDebug() << "Call-sign DB upload started...";

  size_t totb = _callsigns.memSize();
  unsigned bcount = 0;
  for (int n=0; n<_callsigns.image(0).numElements(); n++) {
    unsigned addr = _callsigns.image(0).element(n).address();
    unsigned size = _callsigns.image(0).element(n).data().size();
    unsigned b0 = addr/BSIZE, nb = size/BSIZE;
    for (unsigned b=0; b<nb; b++, bcount+=BSIZE) {
      RadioddityInterface::MemoryBank bank = (
            (0x10000 > (b0+b)*BSIZE) ? RadioddityInterface::MEMBANK_CALLSIGN_LOWER : RadioddityInterface::MEMBANK_CALLSIGN_UPPER );
      if (! _dev->write(bank, ((b0+b)*BSIZE)&0xffff,
                        _callsigns.data((b0+b)*BSIZE, 0), BSIZE, _errorStack))
      {
        errMsg(_errorStack) << "Cannot write block " << (b0+b) << ".";
        return false;
      }
      emit uploadProgress(float(bcount*100)/totb);
    }
  }


  _dev->write_finish();
  return true;
}


