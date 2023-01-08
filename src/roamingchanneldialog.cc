#include "roamingchanneldialog.hh"
#include "ui_roamingchanneldialog.h"
#include "roamingchannel.hh"


RoamingChannelDialog::RoamingChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), ui(new Ui::RoamingChannelDialog), _myChannel(new RoamingChannel(this)),
    _channel(nullptr)
{
  Q_UNUSED(config)

  ui->setupUi(this);

  construct();
}

RoamingChannelDialog::RoamingChannelDialog(Config *config, RoamingChannel *channel, QWidget *parent)
  : QDialog(parent), ui(new Ui::RoamingChannelDialog), _myChannel(new RoamingChannel(this)),
    _channel(channel)
{
  Q_UNUSED(config)

  ui->setupUi(this);
  if (_channel)
    _myChannel->copy(*_channel);

  construct();
}

RoamingChannelDialog::~RoamingChannelDialog() {
  delete ui;
}

void
RoamingChannelDialog::construct() {
  ui->name->setText(_myChannel->name());
  ui->rxFrequency->setText(QString::number(_myChannel->rxFrequency(), 'f'));
  ui->txFrequency->setText(QString::number(_myChannel->txFrequency(), 'f'));
  ui->timeSlot->addItem(tr("TS 1"), QVariant::fromValue(DMRChannel::TimeSlot::TS1));
  ui->timeSlot->addItem(tr("TS 2"), QVariant::fromValue(DMRChannel::TimeSlot::TS2));
  ui->timeSlot->setCurrentIndex(
        (DMRChannel::TimeSlot::TS1 == _myChannel->timeSlot()) ? 0 : 1);
  if (! _myChannel->timeSlotOverridden()) {
    ui->overrideTimeSlot->setChecked(true);
    ui->timeSlot->setEnabled(false);
  }
  ui->colorCode->setValue(_myChannel->colorCode());
  if (! _myChannel->colorCodeOverridden()) {
    ui->overrideColorCode->setChecked(true);
    ui->colorCode->setEnabled(false);
  }

  connect(ui->overrideTimeSlot, SIGNAL(toggled(bool)),
          this, SLOT(onOverrideTimeSlotToggled(bool)));
  connect(ui->overrideColorCode, SIGNAL(toggled(bool)),
          this, SLOT(onOverrideColorCodeToggled(bool)));
}


void
RoamingChannelDialog::onOverrideTimeSlotToggled(bool override) {
  ui->timeSlot->setEnabled(! override);
}

void
RoamingChannelDialog::onOverrideColorCodeToggled(bool override) {
  ui->colorCode->setEnabled(! override);
}

RoamingChannel *
RoamingChannelDialog::channel() {
  _myChannel->setName(ui->name->text().simplified());
  _myChannel->setRXFrequency(ui->rxFrequency->text().toDouble());
  _myChannel->setTXFrequency(ui->txFrequency->text().toDouble());
  _myChannel->setTimeSlot(ui->timeSlot->currentData().value<DMRChannel::TimeSlot>());
  _myChannel->overrideTimeSlot(! ui->overrideTimeSlot->isChecked());
  _myChannel->setColorCode(ui->colorCode->value());
  _myChannel->overrideColorCode(! ui->overrideColorCode->isChecked());

  if (_channel) {
    _channel->copy(*_myChannel);
    return _channel;
  }

  _myChannel->setParent(nullptr);
  return _myChannel;
}
