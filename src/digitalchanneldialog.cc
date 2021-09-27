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
  power->setItemData(0, uint(Channel::Power::Max));
  power->setItemData(1, uint(Channel::Power::High));
  power->setItemData(2, uint(Channel::Power::Mid));
  power->setItemData(3, uint(Channel::Power::Low));
  power->setItemData(4, uint(Channel::Power::Min));
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

  if (_channel) {
    channelName->setText(_channel->name());
    rxFrequency->setText(format_frequency(_channel->rxFrequency()));
    txFrequency->setText(format_frequency(_channel->txFrequency()));
    switch (_channel->power()) {
    case Channel::Power::Max: power->setCurrentIndex(0); break;
    case Channel::Power::High: power->setCurrentIndex(1); break;
    case Channel::Power::Mid: power->setCurrentIndex(2); break;
    case Channel::Power::Low: power->setCurrentIndex(3); break;
    case Channel::Power::Min: power->setCurrentIndex(4); break;
    }
    txTimeout->setValue(_channel->timeout());
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
  }
}

DigitalChannel *
DigitalChannelDialog::channel() {
  bool ok = false;
  QString name = channelName->text();
  double rx = rxFrequency->text().toDouble(&ok);
  double tx = txFrequency->text().toDouble(&ok);
  Channel::Power pwr = Channel::Power(power->currentData().toUInt(&ok));
  uint timeout = txTimeout->text().toUInt(&ok);
  bool rxonly = rxOnly->isChecked();
  DigitalChannel::Admit admit = DigitalChannel::Admit(txAdmit->currentData().toUInt(&ok));
  uint colorcode = colorCode->value();
  DigitalChannel::TimeSlot ts = DigitalChannel::TimeSlot(timeSlot->currentData().toUInt(&ok));
  RXGroupList *rxgroup = rxGroupList->currentData().value<RXGroupList *>();
  DigitalContact *contact = txContact->currentData().value<DigitalContact *>();
  ScanList *scanlist = scanList->currentData().value<ScanList *>();
  PositioningSystem *pos = gpsSystem->currentData().value<PositioningSystem *>();
  RoamingZone *roamingZone = roaming->currentData().value<RoamingZone *>();
  RadioID *id = dmrID->currentData().value<RadioID*>();

  if (_channel) {
    _channel->setRadioIdObj(id);
    _channel->setName(name);
    _channel->setRXFrequency(rx);
    _channel->setTXFrequency(tx);
    _channel->setPower(pwr);
    _channel->setTimeout(timeout);
    _channel->setRXOnly(rxonly);
    _channel->setScanListObj(scanlist);
    _channel->setAdmit(admit);
    _channel->setColorCode(colorcode);
    _channel->setTimeSlot(ts);
    _channel->setGroupListObj(rxgroup);
    _channel->setTXContactObj(contact);
    _channel->setAPRSObj(pos);
    _channel->setRoamingZone(roamingZone);
    return _channel;
  }

  return new DigitalChannel(name, rx, tx, pwr, timeout, rxonly, admit, colorcode, ts, rxgroup,
                            contact, pos, scanlist, roamingZone, id);
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


