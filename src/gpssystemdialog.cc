#include "gpssystemdialog.hh"
#include "settings.hh"


GPSSystemDialog::GPSSystemDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myGPSSystem(new GPSSystem(this)), _gpsSystem(nullptr)
{
  setWindowTitle(tr("Create DMR APRS Sysmtem"));
  construct();
}

GPSSystemDialog::GPSSystemDialog(Config *config, GPSSystem *gps, QWidget *parent)
  : QDialog(parent), _config(config), _myGPSSystem(new GPSSystem(this)), _gpsSystem(gps)
{
  setWindowTitle(tr("Edit DMR APRS Sysmtem"));
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
  for (int i=0; i<_config->contacts()->digitalCount(); i++) {
    destination->addItem(_config->contacts()->digitalContact(i)->name(),
                         QVariant::fromValue(_config->contacts()->digitalContact(i)));
    if (_myGPSSystem->contactObj() == _config->contacts()->digitalContact(i))
      destination->setCurrentIndex(i);
  }

  // setup period
  period->setValue(_myGPSSystem->period());

  // setup revert channel
  revChannel->addItem(tr("[Selected]"), QVariant::fromValue((DigitalChannel *)nullptr));
  for (int i=0,j=0; j<_config->channelList()->count(); j++) {
    if (! _config->channelList()->channel(j)->is<DigitalChannel>())
      continue;
    revChannel->addItem(_config->channelList()->channel(j)->name(),
                        QVariant::fromValue(_config->channelList()->channel(j)->as<DigitalChannel>()));
    if (_myGPSSystem->revertChannel()==_config->channelList()->channel(j)->as<DigitalChannel>())
      revChannel->setCurrentIndex(i);
    i++;
  }

  extensionView->setObject(_myGPSSystem);
  if (! settings.showCommercialFeatures())
    return tabWidget->tabBar()->hide();
}

GPSSystem *
GPSSystemDialog::gpsSystem() {
  _myGPSSystem->setName(name->text().simplified());
  _myGPSSystem->setContactObj(destination->currentData().value<DigitalContact*>());
  _myGPSSystem->setPeriod(period->value());
  _myGPSSystem->setRevertChannel(revChannel->currentData().value<DigitalChannel*>());

  GPSSystem *sys = _myGPSSystem;
  if (_gpsSystem) {
    _gpsSystem->copy(*_myGPSSystem);
    sys = _gpsSystem;
  } else {
    _myGPSSystem->setParent(nullptr);
  }

  return sys;
}
