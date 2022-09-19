#include "m17channeldialog.hh"
#include "ui_m17channeldialog.h"
#include "rxgrouplistdialog.hh"
#include "channel.hh"
#include "config.hh"
#include "utils.hh"


M17ChannelDialog::M17ChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), ui(new Ui::M17ChannelDialog), _config(config), _channel(nullptr),
    _myChannel(new M17Channel(this))
{
  construct();
}

M17ChannelDialog::M17ChannelDialog(Config *config, M17Channel *channel, QWidget *parent)
  : QDialog(parent), ui(new Ui::M17ChannelDialog), _config(config), _channel(channel),
    _myChannel(new M17Channel(this))
{
  if (_channel)
    _myChannel->copy(*_channel);

  construct();
}

M17ChannelDialog::~M17ChannelDialog()
{
  delete ui;
}

void
M17ChannelDialog::construct() {
  ui->setupUi(this);

  if (_channel) {
    setWindowTitle(tr("Edit M17 Channel"));
  } else {
    setWindowTitle(tr("Create M17 Channel"));
  }

  ui->rxFrequency->setValidator(new QDoubleValidator(0,500,5));
  ui->txFrequency->setValidator(new QDoubleValidator(0,500,5));
  ui->powerValue->setItemData(0, unsigned(Channel::Power::Max));
  ui->powerValue->setItemData(1, unsigned(Channel::Power::High));
  ui->powerValue->setItemData(2, unsigned(Channel::Power::Mid));
  ui->powerValue->setItemData(3, unsigned(Channel::Power::Low));
  ui->powerValue->setItemData(4, unsigned(Channel::Power::Min));
  ui->powerDefault->setChecked(true); ui->powerValue->setCurrentIndex(1);
  ui->powerValue->setEnabled(false);
  ui->totDefault->setChecked(true); ui->totValue->setValue(0); ui->totValue->setEnabled(false);
  ui->voxDefault->setChecked(true); ui->voxValue->setValue(0); ui->voxValue->setEnabled(false);
  ui->scanList->addItem(tr("[None]"), QVariant::fromValue((ScanList *)(nullptr)));
  ui->scanList->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    ui->scanList->addItem(_config->scanlists()->scanlist(i)->name(),
                          QVariant::fromValue(_config->scanlists()->scanlist(i)));
    if (_channel && (_myChannel->scanList() == _config->scanlists()->scanlist(i)) )
      ui->scanList->setCurrentIndex(i+1);
  }
  ui->txContact->addItem(tr("[None]"), QVariant::fromValue(nullptr));
  if (_channel && (nullptr == _myChannel->contact()))
    ui->txContact->setCurrentIndex(0);
  for (int i=0; i<_config->contacts()->count(); i++) {
    if (! _config->contacts()->contact(i)->is<M17Contact>())
      continue;
    ui->txContact->addItem(_config->contacts()->contact(i)->name(),
                           QVariant::fromValue(_config->contacts()->contact(i)));
    if (_channel && (_myChannel->contact() == _config->contacts()->contact(i)) )
      ui->txContact->setCurrentIndex(i+1);
  }
  ui->channelMode->setItemData(0, QVariant::fromValue(M17Channel::Mode::Voice));
  ui->channelMode->setItemData(1, QVariant::fromValue(M17Channel::Mode::Data));
  ui->channelMode->setItemData(2, QVariant::fromValue(M17Channel::Mode::VoiceAndData));
  ui->sendGPS->setChecked(false);

  if (_channel) {
    ui->channelName->setText(_myChannel->name());
    ui->rxFrequency->setText(format_frequency(_myChannel->rxFrequency()));
    ui->txFrequency->setText(format_frequency(_myChannel->txFrequency()));
    if (! _myChannel->defaultPower()) {
      ui->powerDefault->setChecked(false); ui->powerValue->setEnabled(true);
      switch (_myChannel->power()) {
      case Channel::Power::Max:  ui->powerValue->setCurrentIndex(0); break;
      case Channel::Power::High: ui->powerValue->setCurrentIndex(1); break;
      case Channel::Power::Mid:  ui->powerValue->setCurrentIndex(2); break;
      case Channel::Power::Low:  ui->powerValue->setCurrentIndex(3); break;
      case Channel::Power::Min:  ui->powerValue->setCurrentIndex(4); break;
      }
    }
    if (! _myChannel->defaultTimeout()) {
      ui->totDefault->setChecked(false); ui->totValue->setEnabled(true);
      ui->totValue->setValue(_channel->timeout());
    }
    ui->rxOnly->setChecked(_myChannel->rxOnly());
    if (! _myChannel->defaultVOX()) {
      ui->voxDefault->setChecked(false); ui->voxValue->setEnabled(true);
      ui->voxValue->setValue(_channel->vox());
    }
    switch (_myChannel->mode()) {
    case M17Channel::Mode::Voice: ui->channelMode->setCurrentIndex(0); break;
    case M17Channel::Mode::Data: ui->channelMode->setCurrentIndex(1); break;
    case M17Channel::Mode::VoiceAndData: ui->channelMode->setCurrentIndex(2); break;
    }
    ui->accessNumber->setValue(_myChannel->accessNumber());
    ui->sendGPS->setChecked(_myChannel->gpsEnabled());
  }

  ui->extensionView->setObjectName("m17ChannelExtension");
  ui->extensionView->setObject(_myChannel, _config);
}

M17Channel *
M17ChannelDialog::channel() {
  _myChannel->setName(ui->channelName->text());
  _myChannel->setRXFrequency(ui->rxFrequency->text().toDouble());
  _myChannel->setTXFrequency(ui->txFrequency->text().toDouble());
  if (ui->powerDefault->isChecked())
    _myChannel->setDefaultPower();
  else
    _myChannel->setPower(Channel::Power(ui->powerValue->currentData().toUInt()));
  if (ui->totDefault->isChecked())
    _myChannel->setDefaultTimeout();
  else
    _myChannel->setTimeout(ui->totValue->value());
  _myChannel->setRXOnly(ui->rxOnly->isChecked());
  _myChannel->setScanList(ui->scanList->currentData().value<ScanList *>());
  _myChannel->setMode(ui->channelMode->currentData().value<M17Channel::Mode>());
  _myChannel->setAccessNumber(ui->accessNumber->value());
  _myChannel->setContact(ui->txContact->currentData().value<M17Contact *>());
  _myChannel->enableGPS(ui->sendGPS->isChecked());

  if (_channel) {
    _channel->copy(*_myChannel);
    return _channel;
  }
  _myChannel->setParent(nullptr);
  return _myChannel;
}
