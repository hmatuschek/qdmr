#include "analogchanneldialog.hh"
#include "application.hh"
#include <QCompleter>
#include "utils.hh"
#include "settings.hh"
#include "repeatercompleter.hh"
#include "repeaterdatabase.hh"


/* ********************************************************************************************* *
 * Implementation of AnalogChannelDialog
 * ********************************************************************************************* */
AnalogChannelDialog::AnalogChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myChannel(new FMChannel(this)), _channel(nullptr)
{
  construct();
}

AnalogChannelDialog::AnalogChannelDialog(Config *config, FMChannel *channel, QWidget *parent)
  : QDialog(parent), _config(config), _myChannel(nullptr), _channel(channel)
{
  if (nullptr == _channel)
    _myChannel = new FMChannel();
  else
    _myChannel = _channel->clone()->as<FMChannel>();
  _myChannel->setParent(this);

  construct();
}

void
AnalogChannelDialog::construct() {
  setupUi(this);
  Settings settings;

  if (settings.hideChannelNote()) {
    hintLabel->setVisible(false);
    layout()->invalidate();
    adjustSize();
  }

  Application *app = qobject_cast<Application *>(qApp);
  FMRepeaterFilter *filter = new FMRepeaterFilter(app->repeater(), app->position(), this);
  filter->setSourceModel(app->repeater());
  QCompleter *completer = new RepeaterCompleter(2, app->repeater(), this);
  completer->setModel(filter);
  channelName->setCompleter(completer);
  connect(completer, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(onRepeaterSelected(const QModelIndex &)));

  powerValue->setItemData(0, unsigned(Channel::Power::Max));
  powerValue->setItemData(1, unsigned(Channel::Power::High));
  powerValue->setItemData(2, unsigned(Channel::Power::Mid));
  powerValue->setItemData(3, unsigned(Channel::Power::Low));
  powerValue->setItemData(4, unsigned(Channel::Power::Min));
  powerDefault->setChecked(true); powerValue->setEnabled(false); powerValue->setCurrentIndex(1);
  totDefault->setChecked(true); totValue->setValue(0); totValue->setEnabled(false);
  scanList->addItem(tr("[None]"), QVariant::fromValue((ScanList *)nullptr));
  scanList->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    ScanList *lst = _config->scanlists()->scanlist(i);
    scanList->addItem(lst->name(),QVariant::fromValue(lst));
    if (_myChannel && (_myChannel->scanList() == lst) )
      scanList->setCurrentIndex(i+1);
  }
  txAdmit->setItemData(0, unsigned(FMChannel::Admit::Always));
  txAdmit->setItemData(1, unsigned(FMChannel::Admit::Free));
  txAdmit->setItemData(2, unsigned(FMChannel::Admit::Tone));
  squelchDefault->setChecked(true); squelchValue->setValue(1); squelchValue->setEnabled(false);
  if (_myChannel) {
    rxTone->setSelectiveCall(_myChannel->rxTone());
    txTone->setSelectiveCall(_myChannel->txTone());
  }
  bandwidth->setItemData(0, unsigned(FMChannel::Bandwidth::Narrow));
  bandwidth->setItemData(1, unsigned(FMChannel::Bandwidth::Wide));
  aprsList->addItem(tr("[None]"), QVariant::fromValue((APRSSystem *)nullptr));
  aprsList->setCurrentIndex(0);
  for (int i=0; i<_config->posSystems()->aprsCount(); i++) {
    APRSSystem *sys = _config->posSystems()->aprsSystem(i);
    aprsList->addItem(sys->name(),QVariant::fromValue(sys));
    if (_myChannel && (_myChannel->aprsSystem() == sys))
      aprsList->setCurrentIndex(i+1);
  }
  voxDefault->setChecked(true); voxValue->setValue(0); voxValue->setEnabled(false);

  channelName->setText(_myChannel->name());
  rxFrequency->setText(_myChannel->rxFrequency().format(Frequency::Unit::MHz));
  txFrequency->setText(_myChannel->txFrequency().format(Frequency::Unit::MHz));

  offsetComboBox->addItem(QIcon::fromTheme("symbol-none"), "");
  offsetComboBox->setItemData(0, tr("No offset"), Qt::ToolTipRole);
  offsetComboBox->addItem(QIcon::fromTheme("symbol-plus"), "");
  offsetComboBox->setItemData(1, tr("Positive offset"), Qt::ToolTipRole);
  offsetComboBox->addItem(QIcon::fromTheme("symbol-minus"), "");
  offsetComboBox->setItemData(2, tr("Negative offset"), Qt::ToolTipRole);

  updateOffsetFrequency();

  if (! _myChannel->defaultPower()) {
    powerDefault->setChecked(false); powerValue->setEnabled(true);
    switch (_myChannel->power()) {
    case Channel::Power::Max: powerValue->setCurrentIndex(0); break;
    case Channel::Power::High: powerValue->setCurrentIndex(1); break;
    case Channel::Power::Mid: powerValue->setCurrentIndex(2); break;
    case Channel::Power::Low: powerValue->setCurrentIndex(3); break;
    case Channel::Power::Min: powerValue->setCurrentIndex(4); break;
    }
  }
  if (! _myChannel->defaultTimeout()) {
    totDefault->setChecked(false); totValue->setEnabled(true);
    totValue->setValue(_myChannel->timeout());
  }
  rxOnly->setChecked(_myChannel->rxOnly());
  switch (_myChannel->admit()) {
  case FMChannel::Admit::Always: txAdmit->setCurrentIndex(0); break;
  case FMChannel::Admit::Free: txAdmit->setCurrentIndex(1); break;
  case FMChannel::Admit::Tone: txAdmit->setCurrentIndex(2); break;
  }
  if (! _myChannel->defaultSquelch()) {
    squelchDefault->setChecked(false); squelchValue->setEnabled(true);
    squelchValue->setValue(_myChannel->squelch());
  }
  if (FMChannel::Bandwidth::Narrow == _myChannel->bandwidth())
    bandwidth->setCurrentIndex(0);
  else if (FMChannel::Bandwidth::Wide == _myChannel->bandwidth())
    bandwidth->setCurrentIndex(1);
  if (! _myChannel->defaultVOX()) {
    voxDefault->setChecked(false); voxValue->setEnabled(true);
    voxValue->setValue(_myChannel->vox());
  }

  if (! settings.showExtensions())
    tabWidget->tabBar()->hide();

  extensionView->setObjectName("AnalogChannelExtension");
  extensionView->setObject(_myChannel, _config);

  connect(powerDefault, SIGNAL(toggled(bool)), this, SLOT(onPowerDefaultToggled(bool)));
  connect(totDefault, SIGNAL(toggled(bool)), this, SLOT(onTimeoutDefaultToggled(bool)));
  connect(squelchDefault, SIGNAL(toggled(bool)), this, SLOT(onSquelchDefaultToggled(bool)));
  connect(voxDefault, SIGNAL(toggled(bool)), this, SLOT(onVOXDefaultToggled(bool)));
  connect(hintLabel, SIGNAL(linkActivated(QString)), this, SLOT(onHideChannelHint()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(txFrequency, &QLineEdit::editingFinished, this, &AnalogChannelDialog::onTxFrequencyEdited);
  connect(rxFrequency, &QLineEdit::editingFinished, this, &AnalogChannelDialog::onRxFrequencyEdited);
  connect(offsetLineEdit, &QLineEdit::editingFinished, this, &AnalogChannelDialog::onOffsetFrequencyEdited);
  connect(offsetComboBox, &QComboBox::currentIndexChanged, this, &AnalogChannelDialog::onOffsetDirectionChanged);
}

FMChannel *
AnalogChannelDialog::channel()
{
  _myChannel->setName(channelName->text());
  _myChannel->setRXFrequency(Frequency::fromString(rxFrequency->text()));
  _myChannel->setTXFrequency(Frequency::fromString(txFrequency->text()));
  if (powerDefault->isChecked()) {
    _myChannel->setDefaultPower();
  } else {
    _myChannel->setPower(Channel::Power(powerValue->currentData().toUInt()));
  }
  if (totDefault->isChecked())
    _myChannel->setDefaultTimeout();
  else
    _myChannel->setTimeout(totValue->value());
  _myChannel->setRXOnly(rxOnly->isChecked());
  _myChannel->setAdmit(FMChannel::Admit(txAdmit->currentData().toUInt()));
  if (squelchDefault->isChecked())
    _myChannel->setSquelchDefault();
  else
    _myChannel->setSquelch(squelchValue->value());
  _myChannel->setRXTone(rxTone->selectiveCall());
  _myChannel->setTXTone(txTone->selectiveCall());
  _myChannel->setBandwidth(FMChannel::Bandwidth(bandwidth->currentData().toUInt()));
  _myChannel->setScanList(scanList->currentData().value<ScanList *>());
  _myChannel->setAPRSSystem(aprsList->currentData().value<APRSSystem *>());
  if (voxDefault->isChecked())
    _myChannel->setVOXDefault();
  else
    _myChannel->setVOX(voxValue->value());

  FMChannel *channel = _myChannel;
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
AnalogChannelDialog::onRepeaterSelected(const QModelIndex &index) {
  Application *app = qobject_cast<Application *>(qApp);

  QModelIndex src = qobject_cast<QAbstractProxyModel*>(
        channelName->completer()->completionModel())->mapToSource(index);
  src = qobject_cast<QAbstractProxyModel*>(
        channelName->completer()->model())->mapToSource(src);
  Frequency rx = app->repeater()->get(src.row()).rxFrequency();
  Frequency tx = app->repeater()->get(src.row()).txFrequency();
  rxTone->setSelectiveCall(app->repeater()->get(src.row()).rxTone());
  txTone->setSelectiveCall(app->repeater()->get(src.row()).txTone());
  rxFrequency->setText(rx.format());
  txFrequency->setText(tx.format());

  _myChannel->setRXFrequency(Frequency::fromString(rxFrequency->text()));
  _myChannel->setTXFrequency(Frequency::fromString(txFrequency->text()));
  updateOffsetFrequency();
}

void
AnalogChannelDialog::onPowerDefaultToggled(bool checked) {
  powerValue->setEnabled(!checked);
}

void
AnalogChannelDialog::onTimeoutDefaultToggled(bool checked) {
  totValue->setEnabled(!checked);
}

void
AnalogChannelDialog::onSquelchDefaultToggled(bool checked) {
  squelchValue->setEnabled(! checked);
}

void
AnalogChannelDialog::onVOXDefaultToggled(bool checked) {
  voxValue->setEnabled(! checked);
}

void
AnalogChannelDialog::onHideChannelHint() {
  Settings settings;
  settings.setHideChannelNote(true);
  hintLabel->setVisible(false);
  layout()->invalidate();
  adjustSize();
}

void
AnalogChannelDialog::onTxFrequencyEdited() {
  _myChannel->setTXFrequency(Frequency::fromString(txFrequency->text()));
  txFrequency->setText(_myChannel->txFrequency().format());
  updateOffsetFrequency();
}

void
AnalogChannelDialog::onRxFrequencyEdited() {
  _myChannel->setRXFrequency(Frequency::fromString(rxFrequency->text()));
  rxFrequency->setText(_myChannel->rxFrequency().format());

  if (_myChannel->txFrequency().isZero()) {
    // If no previous txFrequency set, match rx frequency.
    _myChannel->setTXFrequency(Frequency::fromString(rxFrequency->text()));
    txFrequency->setText(_myChannel->txFrequency().format());
  }
  updateOffsetFrequency();
}

void
AnalogChannelDialog::onOffsetFrequencyEdited() {
  FrequencyOffset offsetFrequency = FrequencyOffset::fromString(offsetLineEdit->text()).abs();
  Frequency txFreq = _myChannel->rxFrequency();

  switch (offsetComboBox->currentIndex()) {
  case 0: break;
  case 1: txFreq = _myChannel->rxFrequency() + offsetFrequency; break;
  case 2: txFreq = _myChannel->rxFrequency() + offsetFrequency.invert(); break;
  }

  offsetLineEdit->setText(offsetFrequency.format());
  txFrequency->setText(txFreq.format());
  _myChannel->setTXFrequency(txFreq);
}

void
AnalogChannelDialog::onOffsetDirectionChanged(int index) {
  Frequency txFreq = _myChannel->rxFrequency();
  FrequencyOffset offsetFrequency = FrequencyOffset::fromString(offsetLineEdit->text()).abs();

  switch (index) {
  case 0:
    offsetLineEdit->setEnabled(false);
    txFreq = _myChannel->rxFrequency();
  break;
  case 1:
    offsetLineEdit->setEnabled(true);
    txFreq = _myChannel->rxFrequency() + offsetFrequency;
  break;
  case 2:
    offsetLineEdit->setEnabled(true);
    txFreq = _myChannel->rxFrequency() + offsetFrequency.invert();
  break;
  }

  _myChannel->setTXFrequency(txFreq);
  txFrequency->setText(txFreq.format());
}

void
AnalogChannelDialog::updateOffsetFrequency() {
  FrequencyOffset offsetFrequency = _myChannel->offsetFrequency();
  // Show absolute value
  offsetLineEdit->setText(offsetFrequency.abs().format());
  // Use combo box to indicate direction
  updateComboBox();
}

void
AnalogChannelDialog::updateComboBox() {
  switch (_myChannel->offsetShift()) {
  case Channel::OffsetShift::None:
    offsetComboBox->setCurrentIndex(0);
    offsetLineEdit->setEnabled(false);
  break;
  case Channel::OffsetShift::Positive:
    offsetComboBox->setCurrentIndex(1);
    offsetLineEdit->setEnabled(true);
  break;
  case Channel::OffsetShift::Negative:
    offsetComboBox->setCurrentIndex(2);
    offsetLineEdit->setEnabled(true);
  break;
  }
}
