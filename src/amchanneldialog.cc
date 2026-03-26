#include "amchanneldialog.hh"
#include "channel.hh"
#include "settings.hh"
#include "application.hh"
#include "ui_amchanneldialog.h"



AMChannelDialog::AMChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myChannel(new AMChannel(this)), _channel(nullptr)
{
  construct();
}


AMChannelDialog::AMChannelDialog(Config *config, AMChannel *channel, QWidget *parent)
  : QDialog(parent), _config(config), _myChannel(nullptr), _channel(channel)
{
  if (nullptr == _channel)
    _myChannel = new AMChannel();
  else
    _myChannel = _channel->clone()->as<AMChannel>();
  _myChannel->setParent(this);

  construct();
}


AMChannelDialog::~AMChannelDialog() {
  delete ui;
}


void
AMChannelDialog::construct() {
  ui = new Ui::AMChannelDialog();
  ui->setupUi(this);
  Settings settings;

  ui->powerValue->setItemData(0, unsigned(Channel::Power::Max));
  ui->powerValue->setItemData(1, unsigned(Channel::Power::High));
  ui->powerValue->setItemData(2, unsigned(Channel::Power::Mid));
  ui->powerValue->setItemData(3, unsigned(Channel::Power::Low));
  ui->powerValue->setItemData(4, unsigned(Channel::Power::Min));
  ui->powerDefault->setChecked(true); ui->powerValue->setEnabled(false); ui->powerValue->setCurrentIndex(1);
  ui->totDefault->setChecked(true); ui->totValue->setValue(0); ui->totValue->setEnabled(false);
  ui->scanList->addItem(tr("[None]"), QVariant::fromValue((ScanList *)nullptr));
  ui->scanList->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    ScanList *lst = _config->scanlists()->scanlist(i);
    ui->scanList->addItem(lst->name(),QVariant::fromValue(lst));
    if (_myChannel && (_myChannel->scanList() == lst) )
      ui->scanList->setCurrentIndex(i+1);
  }
  ui->squelchDefault->setChecked(true); ui->squelchValue->setValue(1); ui->squelchValue->setEnabled(false);
  ui->voxDefault->setChecked(true); ui->voxValue->setValue(0); ui->voxValue->setEnabled(false);

  ui->channelName->setText(_myChannel->name());
  ui->rxFrequency->setText(_myChannel->rxFrequency().format(Frequency::Unit::MHz));
  ui->txFrequency->setText(_myChannel->txFrequency().format(Frequency::Unit::MHz));

  ui->offsetComboBox->addItem(QIcon::fromTheme("symbol-none"), "");
  ui->offsetComboBox->setItemData(0, tr("No offset"), Qt::ToolTipRole);
  ui->offsetComboBox->addItem(QIcon::fromTheme("symbol-plus"), "");
  ui->offsetComboBox->setItemData(1, tr("Positive offset"), Qt::ToolTipRole);
  ui->offsetComboBox->addItem(QIcon::fromTheme("symbol-minus"), "");
  ui->offsetComboBox->setItemData(2, tr("Negative offset"), Qt::ToolTipRole);

  updateOffsetFrequency();

  if (! _myChannel->defaultPower()) {
    ui->powerDefault->setChecked(false); ui->powerValue->setEnabled(true);
    switch (_myChannel->power()) {
    case Channel::Power::Max: ui->powerValue->setCurrentIndex(0); break;
    case Channel::Power::High: ui->powerValue->setCurrentIndex(1); break;
    case Channel::Power::Mid: ui->powerValue->setCurrentIndex(2); break;
    case Channel::Power::Low: ui->powerValue->setCurrentIndex(3); break;
    case Channel::Power::Min: ui->powerValue->setCurrentIndex(4); break;
    }
  }
  if (! _myChannel->defaultTimeout()) {
    ui->totDefault->setChecked(false); ui->totValue->setEnabled(true);
    ui->totValue->setValue(_myChannel->timeout().seconds());
  }
  ui->rxOnly->setChecked(_myChannel->rxOnly());
  if (! _myChannel->defaultSquelch()) {
    ui->squelchDefault->setChecked(false); ui->squelchValue->setEnabled(true);
    ui->squelchValue->setValue(_myChannel->squelch().mapTo({0,10}));
  }
  if (! _myChannel->defaultVOX()) {
    ui->voxDefault->setChecked(false); ui->voxValue->setEnabled(true);
    ui->voxValue->setValue(_myChannel->vox().value());
  }

  if (! settings.showExtensions())
    ui->tabWidget->tabBar()->hide();

  ui->extensionView->setObjectName("AnalogChannelExtension");
  ui->extensionView->setObject(_myChannel, _config);

  connect(ui->powerDefault, &QCheckBox::toggled, this, &AMChannelDialog::onPowerDefaultToggled);
  connect(ui->totDefault, &QCheckBox::toggled, this, &AMChannelDialog::onTimeoutDefaultToggled);
  connect(ui->squelchDefault, &QCheckBox::toggled, this, &AMChannelDialog::onSquelchDefaultToggled);
  connect(ui->voxDefault, &QCheckBox::toggled, this, &AMChannelDialog::onVOXDefaultToggled);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AMChannelDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AMChannelDialog::reject);

  connect(ui->txFrequency, &QLineEdit::editingFinished, this, &AMChannelDialog::onTxFrequencyEdited);
  connect(ui->rxFrequency, &QLineEdit::editingFinished, this, &AMChannelDialog::onRxFrequencyEdited);
  connect(ui->offsetLineEdit, &QLineEdit::editingFinished, this, &AMChannelDialog::onOffsetFrequencyEdited);
  connect(ui->offsetComboBox, &QComboBox::currentIndexChanged, this, &AMChannelDialog::onOffsetDirectionChanged);
}


AMChannel *
AMChannelDialog::channel()
{
  _myChannel->setName(ui->channelName->text());
  _myChannel->setRXFrequency(Frequency::fromString(ui->rxFrequency->text()));
  _myChannel->setTXFrequency(Frequency::fromString(ui->txFrequency->text()));
  if (ui->powerDefault->isChecked()) {
    _myChannel->setDefaultPower();
  } else {
    _myChannel->setPower(Channel::Power(ui->powerValue->currentData().toUInt()));
  }
  if (ui->totDefault->isChecked())
    _myChannel->setDefaultTimeout();
  else
    _myChannel->setTimeout(Interval::fromSeconds(ui->totValue->value()));
  _myChannel->setRXOnly(ui->rxOnly->isChecked());
  if (ui->squelchDefault->isChecked())
    _myChannel->setSquelchDefault();
  else
    _myChannel->setSquelch(Level::fromValue(ui->squelchValue->value()));
  _myChannel->setScanList(ui->scanList->currentData().value<ScanList *>());
  if (ui->voxDefault->isChecked())
    _myChannel->setVOXDefault();
  else
    _myChannel->setVOX(Level::fromValue(ui->voxValue->value()));

  auto channel = _myChannel;
  if (nullptr == _channel) {
    _myChannel->setParent(nullptr);
    _myChannel = nullptr;
  } else {
    _channel->copy(*_myChannel);
    channel = _channel;
  }

  return channel;
}

void
AMChannelDialog::onPowerDefaultToggled(bool checked) {
  ui->powerValue->setEnabled(!checked);
}

void
AMChannelDialog::onTimeoutDefaultToggled(bool checked) {
  ui->totValue->setEnabled(!checked);
}

void
AMChannelDialog::onSquelchDefaultToggled(bool checked) {
  ui->squelchValue->setEnabled(! checked);
}

void
AMChannelDialog::onVOXDefaultToggled(bool checked) {
  ui->voxValue->setEnabled(! checked);
}

void
AMChannelDialog::onTxFrequencyEdited() {
  _myChannel->setTXFrequency(Frequency::fromString(ui->txFrequency->text()));
  ui->txFrequency->setText(_myChannel->txFrequency().format());
  updateOffsetFrequency();
}

void
AMChannelDialog::onRxFrequencyEdited() {
  _myChannel->setRXFrequency(Frequency::fromString(ui->rxFrequency->text()));
  ui->rxFrequency->setText(_myChannel->rxFrequency().format());

  if (_myChannel->txFrequency().isZero()) {
    // If no previous txFrequency set, match rx frequency.
    _myChannel->setTXFrequency(Frequency::fromString(ui->rxFrequency->text()));
    ui->txFrequency->setText(_myChannel->txFrequency().format());
  }
  updateOffsetFrequency();
}

void
AMChannelDialog::onOffsetFrequencyEdited() {
  FrequencyOffset offsetFrequency = FrequencyOffset::fromString(ui->offsetLineEdit->text()).abs();
  Frequency txFreq = _myChannel->rxFrequency();

  switch (ui->offsetComboBox->currentIndex()) {
  case 0: break;
  case 1: txFreq = _myChannel->rxFrequency() + offsetFrequency; break;
  case 2: txFreq = _myChannel->rxFrequency() + offsetFrequency.invert(); break;
  }

  ui->offsetLineEdit->setText(offsetFrequency.format());
  ui->txFrequency->setText(txFreq.format());
  _myChannel->setTXFrequency(txFreq);
}

void
AMChannelDialog::onOffsetDirectionChanged(int index) {
  Frequency txFreq = _myChannel->rxFrequency();
  FrequencyOffset offsetFrequency = FrequencyOffset::fromString(ui->offsetLineEdit->text()).abs();

  switch (index) {
  case 0:
    ui->offsetLineEdit->setEnabled(false);
    txFreq = _myChannel->rxFrequency();
    break;
  case 1:
    ui->offsetLineEdit->setEnabled(true);
    txFreq = _myChannel->rxFrequency() + offsetFrequency;
    break;
  case 2:
    ui->offsetLineEdit->setEnabled(true);
    txFreq = _myChannel->rxFrequency() + offsetFrequency.invert();
    break;
  }

  _myChannel->setTXFrequency(txFreq);
  ui->txFrequency->setText(txFreq.format());
}

void
AMChannelDialog::updateOffsetFrequency() {
  FrequencyOffset offsetFrequency = _myChannel->offsetFrequency();
  // Show absolute value
  ui->offsetLineEdit->setText(offsetFrequency.abs().format());
  // Use combo box to indicate direction
  updateComboBox();
}

void
AMChannelDialog::updateComboBox() {
  switch (_myChannel->offsetShift()) {
  case Channel::OffsetShift::None:
    ui->offsetComboBox->setCurrentIndex(0);
    ui->offsetLineEdit->setEnabled(false);
    break;
  case Channel::OffsetShift::Positive:
    ui->offsetComboBox->setCurrentIndex(1);
    ui->offsetLineEdit->setEnabled(true);
    break;
  case Channel::OffsetShift::Negative:
    ui->offsetComboBox->setCurrentIndex(2);
    ui->offsetLineEdit->setEnabled(true);
    break;
  }
}
