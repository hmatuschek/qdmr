#include "dr1801uv.hh"
#include "dr1801uv_interface.hh"

RadioLimits * DR1801UV::_limits = nullptr;

DR1801UV::DR1801UV(DR1801UVInterface *device, QObject *parent)
  : Radio(parent), _device(device), _name("Baofeng DR-1801UV")
{
  // Check if device is open
  if ((nullptr==_device) || (! _device->isOpen())) {
    _task = StatusError;
    return;
  }
}

RadioInfo
DR1801UV::defaultRadioInfo() {
  return RadioInfo(RadioInfo::DR1801UV, "dr1801uv",
                   "DR-1801UV", "Baofeng",
                   DR1801UVInterface::interfaceInfo());
}


const QString &
DR1801UV::name() const {
  return _name;
}

const Codeplug &
DR1801UV::codeplug() const {

}

Codeplug &
DR1801UV::codeplug() {

}

const RadioLimits &
DR1801UV::limits() const {
  if (nullptr == _limits)
    _limits = nullptr;
  return *_limits;
}

bool
DR1801UV::startDownload(bool blocking, const ErrorStack &err) {

}

bool
DR1801UV::startUpload(Config *config, bool blocking, const Codeplug::Flags &flags, const ErrorStack &err) {

}

bool
DR1801UV::startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection, const ErrorStack &err) {
  return false;
}
