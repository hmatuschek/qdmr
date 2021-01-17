#include "aprssystemdialog.hh"
#include "ui_aprssystemdialog.h"

APRSSystemDialog::APRSSystemDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _aprs(nullptr), ui(new Ui::aprssystemdialog)
{
  construct();
}

APRSSystemDialog::APRSSystemDialog(Config *config, APRSSystem *aprs, QWidget *parent)
  : QDialog(parent), _config(config), _aprs(aprs), ui(new Ui::aprssystemdialog)
{
  construct();
}

APRSSystemDialog::~APRSSystemDialog() {
  delete ui;
}


void
APRSSystemDialog::construct() {
  // Construct UI
  ui->setupUi(this);

  // Setup name
  if (_aprs)
    ui->name->setText(_aprs->name());

  // Setup analog channels
  for (int i=0, j=0; i<_config->channelList()->count(); i++) {
    if (! _config->channelList()->channel(i)->is<AnalogChannel>())
      continue;
    AnalogChannel *ch = _config->channelList()->channel(i)->as<AnalogChannel>();
    ui->channel->addItem(ch->name(), QVariant::fromValue(ch));
    if (_aprs && (_aprs->channel() == ch))
      ui->channel->setCurrentIndex(j);
    j++;
  }

  // Setup source
  if (_aprs) {
    ui->source->setText(_aprs->source());
    ui->srcSSID->setValue(_aprs->srcSSID());
  } else {
    ui->source->setText(_config->name());
  }

  // Setup dest
  if (_aprs) {
    ui->destination->setText(_aprs->destination());
    ui->destSSID->setValue(_aprs->destSSID());
  }

  // Setup icons
  ui->icon->addItem(tr("[None]"), uint(APRSSystem::APRS_ICON_NO_SYMBOL));

  // Setup update period
  if (_aprs)
    ui->updatePeriod->setValue(_aprs->period());

  // Setup message
  if (_aprs)
    ui->message->setText(_aprs->message());
}

APRSSystem *
APRSSystemDialog::aprsSystem() {
  if (_aprs) {
    _aprs->setName(ui->name->text().simplified());
    _aprs->setChannel(ui->channel->currentData().value<AnalogChannel*>());
    _aprs->setSource(ui->source->text().simplified(), ui->srcSSID->value());
    _aprs->setDestination(ui->destination->text().simplified(), ui->destSSID->value());
    _aprs->setIcon(APRSSystem::Icon(ui->icon->currentData().toUInt()));
    _aprs->setPeriod(ui->updatePeriod->value());
    _aprs->setMessage(ui->message->text().simplified());
    return _aprs;
  }

  return new APRSSystem(ui->name->text().simplified(), ui->channel->currentData().value<AnalogChannel *>(),
                        ui->destination->text().simplified(), ui->destSSID->value(),
                        ui->source->text().simplified(), ui->srcSSID->value(),
                        APRSSystem::Icon(ui->icon->currentData().toUInt()),
                        ui->message->text().simplified());
}
