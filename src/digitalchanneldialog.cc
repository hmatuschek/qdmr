#include "digitalchanneldialog.hh"
#include "application.hh"
#include <QCompleter>
#include "rxgrouplistdialog.hh"
#include "repeaterdatabase.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of DigitalChannelDialog
 * ********************************************************************************************* */
DigitalChannelDialog::DigitalChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _channel(nullptr)
{
  construct();
}

DigitalChannelDialog::DigitalChannelDialog(Config *config, DigitalChannel *channel, QWidget *parent)
  : QDialog(parent), _config(config), _channel(channel)
{
  construct();
}

void
DigitalChannelDialog::construct() {
  setupUi(this);

  Application *app = qobject_cast<Application *>(qApp);
  DMRRepeaterFilter *filter = new DMRRepeaterFilter(this);
  filter->setSourceModel(app->repeater());
  QCompleter *completer = new QCompleter(filter, this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setCompletionColumn(0);
  completer->setCompletionRole(Qt::EditRole);
  channelName->setCompleter(completer);
  connect(completer, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(onRepeaterSelected(const QModelIndex &)));

  rxFrequency->setValidator(new QDoubleValidator(0,500,5));
  txFrequency->setValidator(new QDoubleValidator(0,500,5));
  powerValue->setItemData(0, uint(Channel::Power::Max));
  powerValue->setItemData(1, uint(Channel::Power::High));
  powerValue->setItemData(2, uint(Channel::Power::Mid));
  powerValue->setItemData(3, uint(Channel::Power::Low));
  powerValue->setItemData(4, uint(Channel::Power::Min));
  powerDefault->setChecked(true); powerValue->setCurrentIndex(1); powerValue->setEnabled(false);
  totDefault->setChecked(true); totValue->setValue(0); totValue->setEnabled(false);
  scanList->addItem(tr("[None]"), QVariant::fromValue((ScanList *)(nullptr)));
  scanList->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    scanList->addItem(_config->scanlists()->scanlist(i)->name(),
                      QVariant::fromValue(_config->scanlists()->scanlist(i)));
    if (_channel && (_channel->scanListObj() == _config->scanlists()->scanlist(i)) )
      scanList->setCurrentIndex(i+1);
  }
  txAdmit->setItemData(0, uint(DigitalChannel::Admit::Always));
  txAdmit->setItemData(1, uint(DigitalChannel::Admit::Free));
  txAdmit->setItemData(2, uint(DigitalChannel::Admit::ColorCode));
  timeSlot->setItemData(0, uint(DigitalChannel::TimeSlot::TS1));
  timeSlot->setItemData(1, uint(DigitalChannel::TimeSlot::TS2));
  populateRXGroupListBox(rxGroupList, _config->rxGroupLists(),
                         (nullptr != _channel ? _channel->groupListObj() : nullptr));
  txContact->addItem(tr("[None]"), QVariant::fromValue(nullptr));
  if (_channel && (nullptr == _channel->txContactObj()))
    txContact->setCurrentIndex(0);
  for (int i=0; i<_config->contacts()->count(); i++) {
    txContact->addItem(_config->contacts()->contact(i)->name(),
                       QVariant::fromValue(_config->contacts()->contact(i)));
    if (_channel && (_channel->txContactObj() == _config->contacts()->contact(i)) )
      txContact->setCurrentIndex(i+1);
  }
  gpsSystem->addItem(tr("[None]"), QVariant::fromValue((GPSSystem *)nullptr));
  for (int i=0; i<_config->posSystems()->count(); i++) {
    PositioningSystem *sys = _config->posSystems()->system(i);
    gpsSystem->addItem(sys->name(), QVariant::fromValue(sys));
    if (_channel && (_channel->aprsObj() == sys))
      gpsSystem->setCurrentIndex(i+1);
  }
  roaming->addItem(tr("[None]"), QVariant::fromValue((RoamingZone *)nullptr));
  roaming->addItem(tr("[Default]"), QVariant::fromValue(DefaultRoamingZone::get()));
  if (_channel && (_channel->roamingZone() == DefaultRoamingZone::get()))
    roaming->setCurrentIndex(1);
  for (int i=0; i<_config->roaming()->count(); i++) {
    RoamingZone *zone = _config->roaming()->zone(i);
    roaming->addItem(zone->name(), QVariant::fromValue(zone));
    if (_channel && (_channel->roamingZone() == zone))
      roaming->setCurrentIndex(i+2);
  }
  dmrID->addItem(tr("[Default]"), QVariant::fromValue(DefaultRadioID::get()));
  dmrID->setCurrentIndex(0);
  for (int i=0; i<_config->radioIDs()->count(); i++) {
    dmrID->addItem(_config->radioIDs()->getId(i)->name(),
                   QVariant::fromValue(_config->radioIDs()->getId(i)));
    if (_channel && (_config->radioIDs()->getId(i) == _channel->radioIdObj())) {
      dmrID->setCurrentIndex(i+1);
    }
  }
  voxDefault->setChecked(true); voxValue->setValue(0); voxValue->setEnabled(false);

  if (_channel) {
    channelName->setText(_channel->name());
    rxFrequency->setText(format_frequency(_channel->rxFrequency()));
    txFrequency->setText(format_frequency(_channel->txFrequency()));
    if (! _channel->defaultPower()) {
      powerDefault->setChecked(false); powerValue->setEnabled(true);
      switch (_channel->power()) {
      case Channel::Power::Max: powerValue->setCurrentIndex(0); break;
      case Channel::Power::High: powerValue->setCurrentIndex(1); break;
      case Channel::Power::Mid: powerValue->setCurrentIndex(2); break;
      case Channel::Power::Low: powerValue->setCurrentIndex(3); break;
      case Channel::Power::Min: powerValue->setCurrentIndex(4); break;
      }
    }
    if (! _channel->defaultTimeout()) {
      totDefault->setChecked(false); totValue->setEnabled(true);
      totValue->setValue(_channel->timeout());
    }
    rxOnly->setChecked(_channel->rxOnly());
    switch (_channel->admit()) {
      case DigitalChannel::Admit::Always: txAdmit->setCurrentIndex(0); break;
      case DigitalChannel::Admit::Free: txAdmit->setCurrentIndex(1); break;
      case DigitalChannel::Admit::ColorCode: txAdmit->setCurrentIndex(2); break;
    }
    colorCode->setValue(_channel->colorCode());
    if (DigitalChannel::TimeSlot::TS1 == _channel->timeSlot())
      timeSlot->setCurrentIndex(0);
    else if (DigitalChannel::TimeSlot::TS2 == _channel->timeSlot())
      timeSlot->setCurrentIndex(1);
    if (! _channel->defaultVOX()) {
      voxDefault->setChecked(false); voxValue->setEnabled(true);
      voxValue->setValue(_channel->vox());
    }
  }

  connect(powerDefault, SIGNAL(toggled(bool)), this, SLOT(onPowerDefaultToggled(bool)));
  connect(totDefault, SIGNAL(toggled(bool)), this, SLOT(onTimeoutDefaultToggled(bool)));
  connect(voxDefault, SIGNAL(toggled(bool)), this, SLOT(onVOXDefaultToggled(bool)));
}

DigitalChannel *
DigitalChannelDialog::channel()
{
  DigitalChannel *channel = nullptr;
  if (_channel) {
    channel = _channel;
  } else {
    channel = new DigitalChannel(
          "", 0, 0, Channel::Power::Low, 0, false, DigitalChannel::Admit::Always, 1,
          DigitalChannel::TimeSlot::TS1, nullptr, nullptr, nullptr, nullptr, nullptr,
          DefaultRadioID::get());
  }

  channel->setRadioIdObj(dmrID->currentData().value<RadioID*>());
  channel->setName(channelName->text());
  channel->setRXFrequency(rxFrequency->text().toDouble());
  channel->setTXFrequency(txFrequency->text().toDouble());
  if (powerDefault->isChecked())
    channel->setDefaultPower();
  else
    channel->setPower(Channel::Power(powerValue->currentData().toUInt()));
  if (totDefault->isChecked())
    channel->setDefaultTimeout();
  else
    channel->setTimeout(totValue->value());
  channel->setRXOnly(rxOnly->isChecked());
  channel->setScanListObj(scanList->currentData().value<ScanList *>());
  channel->setAdmit(DigitalChannel::Admit(txAdmit->currentData().toUInt()));
  channel->setColorCode(colorCode->value());
  channel->setTimeSlot(DigitalChannel::TimeSlot(timeSlot->currentData().toUInt()));
  channel->setGroupListObj(rxGroupList->currentData().value<RXGroupList *>());
  channel->setTXContactObj(txContact->currentData().value<DigitalContact *>());
  channel->setAPRSObj(gpsSystem->currentData().value<PositioningSystem *>());
  channel->setRoamingZone(roaming->currentData().value<RoamingZone *>());
  if (voxDefault->isChecked())
    channel->setVOXDefault();
  else
    channel->setVOX(voxValue->value());

  return _channel;
}

void
DigitalChannelDialog::onRepeaterSelected(const QModelIndex &index) {
  Application *app = qobject_cast<Application *>(qApp);

  QModelIndex src = qobject_cast<QAbstractProxyModel*>(
        channelName->completer()->completionModel())->mapToSource(index);
  src = qobject_cast<QAbstractProxyModel*>(
        channelName->completer()->model())->mapToSource(src);
  double rx = app->repeater()->repeater(src.row()).value("tx").toDouble();
  double tx = app->repeater()->repeater(src.row()).value("rx").toDouble();
  txFrequency->setText(QString::number(tx, 'f'));
  rxFrequency->setText(QString::number(rx, 'f'));
}

void
DigitalChannelDialog::onPowerDefaultToggled(bool checked) {
  powerValue->setEnabled(!checked);
}

void
DigitalChannelDialog::onTimeoutDefaultToggled(bool checked) {
  totValue->setEnabled(!checked);
}

void
DigitalChannelDialog::onVOXDefaultToggled(bool checked) {
  voxValue->setEnabled(! checked);
}

