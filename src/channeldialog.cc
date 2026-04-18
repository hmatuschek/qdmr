#include "channeldialog.hh"
#include "channel.hh"
#include "settings.hh"
#include "application.hh"
#include "ui_channeldialog.h"



ChannelDialog::ChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _channel()
{
  ui = new Ui::ChannelDialog();
  ui->setupUi(this);
  Settings settings;

  if (settings.hideChannelNote()) {
    ui->hintLabel->setVisible(false);
    layout()->invalidate();
    adjustSize();
  }
  connect(ui->hintLabel, &QLabel::linkActivated,
    this, &ChannelDialog::onHideChannelHint);

  ui->offsetComboBox->addItem(QIcon::fromTheme("symbol-none"), "");
  ui->offsetComboBox->setItemData(0, tr("No offset"), Qt::ToolTipRole);
  ui->offsetComboBox->addItem(QIcon::fromTheme("symbol-plus"), "");
  ui->offsetComboBox->setItemData(1, tr("Positive offset"), Qt::ToolTipRole);
  ui->offsetComboBox->addItem(QIcon::fromTheme("symbol-minus"), "");
  ui->offsetComboBox->setItemData(2, tr("Negative offset"), Qt::ToolTipRole);
  connect(ui->txFrequency, &QLineEdit::editingFinished,
    this, &ChannelDialog::onTxFrequencyEdited);
  connect(ui->rxFrequency, &QLineEdit::editingFinished,
    this, &ChannelDialog::onRxFrequencyEdited);
  connect(ui->offsetLineEdit, &QLineEdit::editingFinished,
    this, &ChannelDialog::onOffsetFrequencyEdited);
  connect(ui->offsetComboBox, &QComboBox::currentIndexChanged,
    this, &ChannelDialog::onOffsetDirectionChanged);

  ui->powerValue->setItemData(0, unsigned(Channel::Power::Max));
  ui->powerValue->setItemData(1, unsigned(Channel::Power::High));
  ui->powerValue->setItemData(2, unsigned(Channel::Power::Mid));
  ui->powerValue->setItemData(3, unsigned(Channel::Power::Low));
  ui->powerValue->setItemData(4, unsigned(Channel::Power::Min));
  ui->powerDefault->setChecked(true); ui->powerValue->setEnabled(false); ui->powerValue->setCurrentIndex(1);
  connect(ui->powerDefault, &QCheckBox::toggled, [this](bool checked) { this->ui->powerValue->setEnabled(! checked); });

  ui->totDefault->setChecked(true); ui->totValue->setValue(0); ui->totValue->setEnabled(false);
  connect(ui->totDefault, &QCheckBox::toggled, [this](bool checked) { this->ui->totValue->setEnabled(! checked); });

  ui->scanList->addItem(tr("[None]"), QVariant::fromValue<ScanList *>(nullptr));
  for (int i=0; i<_config->scanlists()->count(); i++) {
    auto lst = _config->scanlists()->scanlist(i);
    ui->scanList->addItem(lst->name(), QVariant::fromValue(lst));
  }

  ui->voxDefault->setChecked(true); ui->voxValue->setValue(0); ui->voxValue->setEnabled(false);
  connect(ui->voxDefault, &QCheckBox::toggled, [this](bool checked) { this->ui->voxValue->setEnabled(! checked); });

  updateOffsetFrequency();

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ChannelDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ChannelDialog::reject);
}


ChannelDialog::~ChannelDialog() {
  delete ui;
}


void
ChannelDialog::setChannel(Channel *channel) {
  _channel = channel;
  if (_channel.isNull())
    return;

  ui->channelName->setText(_channel->name());
  ui->rxFrequency->setText(_channel->rxFrequency().format(Frequency::Unit::MHz));
  ui->txFrequency->setText(_channel->txFrequency().format(Frequency::Unit::MHz));
  updateOffsetFrequency();

  if (! _channel->defaultPower()) {
    ui->powerDefault->setChecked(false); ui->powerValue->setEnabled(true);
    switch (_channel->power()) {
    case Channel::Power::Max: ui->powerValue->setCurrentIndex(0); break;
    case Channel::Power::High: ui->powerValue->setCurrentIndex(1); break;
    case Channel::Power::Mid: ui->powerValue->setCurrentIndex(2); break;
    case Channel::Power::Low: ui->powerValue->setCurrentIndex(3); break;
    case Channel::Power::Min: ui->powerValue->setCurrentIndex(4); break;
    }
  }

  if (! _channel->defaultTimeout()) {
    ui->totDefault->setChecked(false); ui->totValue->setEnabled(true);
    ui->totValue->setValue(_channel->timeout().seconds());
  }
  ui->rxOnly->setChecked(_channel->rxOnly());
  if (! _channel->defaultVOX()) {
    ui->voxDefault->setChecked(false); ui->voxValue->setEnabled(true);
    ui->voxValue->setValue(_channel->vox().value());
  }

  ui->extensionView->setObjectName("ChannelExtension");
  ui->extensionView->setObject(_channel, _config);

  for (int i=0; i<ui->scanList->count(); i++) {
    if (ui->scanList->itemData(i).value<ScanList*>() == _channel->scanList())
      ui->scanList->setCurrentIndex(i);
  }
}


void
ChannelDialog::accept() {
  if (_channel.isNull())
    return;

  _channel->setName(ui->channelName->text().simplified());
  _channel->setRXFrequency(Frequency::fromString(ui->rxFrequency->text()));
  _channel->setTXFrequency(Frequency::fromString(ui->txFrequency->text()));
  if (ui->powerDefault->isChecked()) {
    _channel->setDefaultPower();
  } else {
    _channel->setPower(Channel::Power(ui->powerValue->currentData().toUInt()));
  }
  if (ui->totDefault->isChecked())
    _channel->setDefaultTimeout();
  else
    _channel->setTimeout(Interval::fromSeconds(ui->totValue->value()));
  _channel->setRXOnly(ui->rxOnly->isChecked());
  _channel->setScanList(ui->scanList->currentData().value<ScanList *>());
  if (ui->voxDefault->isChecked())
    _channel->setVOXDefault();
  else
    _channel->setVOX(Level::fromValue(ui->voxValue->value()));

  QDialog::accept();
}


void
ChannelDialog::onTxFrequencyEdited() {
  // Normalize frequency format
  ui->txFrequency->setText(Frequency::fromString(ui->txFrequency->text()).format());
  updateOffsetFrequency();
}

void
ChannelDialog::onRxFrequencyEdited() {
  auto rx = Frequency::fromString(ui->rxFrequency->text());
  // normalize RX frequency
  ui->rxFrequency->setText(rx.format());
  // if no previous txFrequency set, match rx frequency.
  if (ui->txFrequency->text().isEmpty())
    ui->txFrequency->setText(rx.format());
  updateOffsetFrequency();
}

void
ChannelDialog::onOffsetFrequencyEdited() {
  auto offsetFrequency = FrequencyOffset::fromString(ui->offsetLineEdit->text()).abs();
  // By default use same as RX frequency
  auto rx = Frequency::fromString(ui->rxFrequency->text()), tx = rx;

  switch (ui->offsetComboBox->currentIndex()) {
  case 0: break;
  case 1: tx = rx + offsetFrequency; break;
  case 2: tx = rx + offsetFrequency.invert(); break;
  }

  ui->offsetLineEdit->setText(offsetFrequency.format());
  ui->txFrequency->setText(tx.format());
}

void
ChannelDialog::onOffsetDirectionChanged(int index) {
  auto rx = Frequency::fromString(ui->rxFrequency->text()), tx = rx;
  FrequencyOffset offsetFrequency = FrequencyOffset::fromString(ui->offsetLineEdit->text()).abs();

  switch (index) {
  case 0:
    ui->offsetLineEdit->setEnabled(false);
    break;
  case 1:
    ui->offsetLineEdit->setEnabled(true);
    tx = rx + offsetFrequency;
    break;
  case 2:
    ui->offsetLineEdit->setEnabled(true);
    tx = rx + offsetFrequency.invert();
    break;
  }

  ui->txFrequency->setText(tx.format());
}

void
ChannelDialog::updateOffsetFrequency() {
  Frequency rx,tx;
  if (!rx.parse(ui->rxFrequency->text().simplified()) ||
      !tx.parse(ui->txFrequency->text().simplified()))
    return;
  // Show absolute value
  ui->offsetLineEdit->setText((tx-rx).abs().format());
  // Use combo box to indicate direction
  updateComboBox();
}

void
ChannelDialog::updateComboBox() {
  auto rx = Frequency::fromString(ui->rxFrequency->text()),
    tx = Frequency::fromString(ui->txFrequency->text());
  if (tx == rx) {
    ui->offsetComboBox->setCurrentIndex(0);
    ui->offsetLineEdit->setEnabled(false);
  } else if (tx > rx) {
    ui->offsetComboBox->setCurrentIndex(1);
    ui->offsetLineEdit->setEnabled(true);
  } else {
    ui->offsetComboBox->setCurrentIndex(2);
    ui->offsetLineEdit->setEnabled(true);
  }
}


void
ChannelDialog::onHideChannelHint() {
  Settings settings;
  settings.setHideChannelNote(true);
  ui->hintLabel->setVisible(false);
  layout()->invalidate();
  adjustSize();
}
