#include "gpssystemdialog.hh"

GPSSystemDialog::GPSSystemDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _gpsSystem(nullptr)
{
  construct();
}

GPSSystemDialog::GPSSystemDialog(Config *config, GPSSystem *gps, QWidget *parent)
  : QDialog(parent), _config(config), _gpsSystem(gps)
{
  construct();
}

void
GPSSystemDialog::construct() {
  setupUi(this);

  // setup name entry
  if (_gpsSystem)
    name->setText(_gpsSystem->name());
  // setup contact entry
  for (int i=0; i<_config->contacts()->digitalCount(); i++) {
    destination->addItem(_config->contacts()->digitalContact(i)->name(),
                         QVariant::fromValue(_config->contacts()->digitalContact(i)));
    if (_gpsSystem && (_gpsSystem->contactObj() == _config->contacts()->digitalContact(i)))
      destination->setCurrentIndex(i);
  }
  // setup period
  if (_gpsSystem)
    period->setValue(_gpsSystem->period());
  else
    period->setValue(300);
  // setup revert channel
  revChannel->addItem(tr("[Selected]"), QVariant::fromValue((DigitalChannel *)nullptr));
  for (int i=0,j=0; j<_config->channelList()->count(); j++) {
    if (! _config->channelList()->channel(j)->is<DigitalChannel>())
      continue;
    revChannel->addItem(_config->channelList()->channel(j)->name(),
                        QVariant::fromValue(_config->channelList()->channel(j)->as<DigitalChannel>()));
    if (_gpsSystem && (_gpsSystem->revertChannel()==_config->channelList()->channel(j)->as<DigitalChannel>()))
      revChannel->setCurrentIndex(i);
    i++;
  }
}

GPSSystem *
GPSSystemDialog::gpsSystem() {
  if (_gpsSystem) {
    _gpsSystem->setName(name->text().simplified());
    _gpsSystem->setContact(destination->currentData().value<DigitalContact*>());
    _gpsSystem->setPeriod(period->value());
    _gpsSystem->setRevertChannel(revChannel->currentData().value<DigitalChannel*>());
    return _gpsSystem;
  }

  return new GPSSystem(name->text().simplified(), destination->currentData().value<DigitalContact*>(),
                       revChannel->currentData().value<DigitalChannel*>(), period->value());
}
