#include "gpssystemdialog.hh"
#include "settings.hh"


GPSSystemDialog::GPSSystemDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myGPSSystem(new DMRAPRSSystem(this)), _gpsSystem(nullptr)
{
  setWindowTitle(tr("Create DMR APRS System"));
  construct();
}

GPSSystemDialog::GPSSystemDialog(Config *config, DMRAPRSSystem *gps, QWidget *parent)
  : QDialog(parent), _config(config), _myGPSSystem(new DMRAPRSSystem(this)), _gpsSystem(gps)
{
  setWindowTitle(tr("Edit DMR APRS System"));
  if (_gpsSystem)
    _myGPSSystem->copy(*_gpsSystem);
  construct();
}

void
GPSSystemDialog::construct() {
  setupUi(this);
  Settings settings;

  // setup name entry
  name->setText(_myGPSSystem->name());

  // setup contact entry
  for (int i=0; i<_config->contacts()->count(); i++) {
    if (! _config->contacts()->get(i)->is<DMRContact>())
      continue;
    auto cont = _config->contacts()->get(i)->as<DMRContact>();
    destination->addItem(cont->name(), QVariant::fromValue(cont));
    if (_myGPSSystem->contact() == cont)
      destination->setCurrentIndex(i);
  }

  // setup period
  if (_myGPSSystem->periodDisabled())
    period->setValue(0);
  else
    period->setValue(_myGPSSystem->period().seconds());

  // setup revert channel
  revChannel->addItem(tr("[Selected]"), QVariant::fromValue<DMRChannel *>(nullptr));
  if (! _myGPSSystem->hasRevertChannel())
    revChannel->setCurrentIndex(0);
  for (int i=1,j=0; j<_config->channelList()->count(); j++) {
    if (! _config->channelList()->channel(j)->is<DMRChannel>())
      continue;
    revChannel->addItem(_config->channelList()->channel(j)->name(),
                        QVariant::fromValue(_config->channelList()->channel(j)->as<DMRChannel>()));
    if (_myGPSSystem->hasRevertChannel() &&
        (_myGPSSystem->revertChannel() == _config->channelList()->channel(j)->as<DMRChannel>()))
      revChannel->setCurrentIndex(i);
    i++;
  }

  extensionView->setObjectName("dmrAPRSSystemExtension");
  extensionView->setObject(_myGPSSystem, _config);
}

DMRAPRSSystem *
GPSSystemDialog::gpsSystem() {
  _myGPSSystem->setName(name->text().simplified());
  _myGPSSystem->setContact(destination->currentData().value<DMRContact*>());
  if (0 == period->value())
    _myGPSSystem->disablePeriod();
  else
    _myGPSSystem->setPeriod(Interval::fromSeconds(period->value()));
  if (revChannel->currentData().isNull())
    _myGPSSystem->resetRevertChannel();
  else
    _myGPSSystem->setRevertChannel(revChannel->currentData().value<DMRChannel*>());

  DMRAPRSSystem *sys = _myGPSSystem;
  if (_gpsSystem) {
    _gpsSystem->copy(*_myGPSSystem);
    sys = _gpsSystem;
  } else {
    _myGPSSystem->setParent(nullptr);
  }

  return sys;
}
