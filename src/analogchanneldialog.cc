#include "analogchanneldialog.hh"
#include "application.hh"
#include <QCompleter>
#include "ctcssbox.hh"
#include "utils.hh"
#include "settings.hh"
#include "repeaterbookcompleter.hh"

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

  Application *app = qobject_cast<Application *>(qApp);
  FMRepeaterFilter *filter = new FMRepeaterFilter(app->repeater(), app->position(), this);
  filter->setSourceModel(app->repeater());
  QCompleter *completer = new RepeaterBookCompleter(2, app->repeater(), this);
  completer->setModel(filter);
  channelName->setCompleter(completer);
  connect(completer, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(onRepeaterSelected(const QModelIndex &)));

  rxFrequency->setValidator(new QDoubleValidator(0,500,5));
  txFrequency->setValidator(new QDoubleValidator(0,500,5));
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
  populateCTCSSBox(rxTone, (nullptr != _myChannel ? _myChannel->rxTone() : Signaling::SIGNALING_NONE));
  populateCTCSSBox(txTone, (nullptr != _myChannel ? _myChannel->txTone() : Signaling::SIGNALING_NONE));
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
  rxFrequency->setText(format_frequency(_myChannel->rxFrequency()));
  txFrequency->setText(format_frequency(_myChannel->txFrequency()));
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
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

FMChannel *
AnalogChannelDialog::channel()
{
  _myChannel->setName(channelName->text());
  _myChannel->setRXFrequency(rxFrequency->text().toDouble());
  _myChannel->setTXFrequency(txFrequency->text().toDouble());
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
  _myChannel->setRXTone(Signaling::Code(rxTone->currentData().toUInt()));
  _myChannel->setTXTone(Signaling::Code(txTone->currentData().toUInt()));
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
  double rx = app->repeater()->repeater(src.row())->rxFrequency();
  double tx = app->repeater()->repeater(src.row())->txFrequency();
  int idx = rxTone->findData(app->repeater()->repeater(src.row())->rxTone());
  if (0 <= idx)
    rxTone->setCurrentIndex(idx);
  idx = txTone->findData(app->repeater()->repeater(src.row())->txTone());
  if (0 <= idx)
    txTone->setCurrentIndex(idx);
  txFrequency->setText(QString::number(tx, 'f'));
  rxFrequency->setText(QString::number(rx, 'f'));
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

