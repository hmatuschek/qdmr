#include "analogchanneldialog.hh"
#include "application.hh"
#include <QCompleter>
#include "ctcssbox.hh"
#include "repeaterdatabase.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of AnalogChannelDialog
 * ********************************************************************************************* */
AnalogChannelDialog::AnalogChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _channel(nullptr)
{
  construct();
}

AnalogChannelDialog::AnalogChannelDialog(Config *config, AnalogChannel *channel, QWidget *parent)
  : QDialog(parent), _config(config), _channel(channel)
{
  construct();
}

void
AnalogChannelDialog::construct() {
  setupUi(this);

  Application *app = qobject_cast<Application *>(qApp);
  FMRepeaterFilter *filter = new FMRepeaterFilter(this);
  filter->setSourceModel(app->repeater());
  QCompleter *completer = new QCompleter(filter, this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setCompletionColumn(0);
  channelName->setCompleter(completer);
  connect(completer, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(onRepeaterSelected(const QModelIndex &)));

  rxFrequency->setValidator(new QDoubleValidator(0,500,5));
  txFrequency->setValidator(new QDoubleValidator(0,500,5));
  power->setItemData(0, uint(Channel::MaxPower));
  power->setItemData(1, uint(Channel::HighPower));
  power->setItemData(2, uint(Channel::MidPower));
  power->setItemData(3, uint(Channel::LowPower));
  power->setItemData(4, uint(Channel::MinPower));
  scanList->addItem(tr("[None]"), QVariant::fromValue((ScanList *)nullptr));
  scanList->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    ScanList *lst = _config->scanlists()->scanlist(i);
    scanList->addItem(lst->name(),QVariant::fromValue(lst));
    if (_channel && (_channel->scanList() == lst) )
      scanList->setCurrentIndex(i+1);
  }
  txAdmit->setItemData(0, uint(AnalogChannel::AdmitNone));
  txAdmit->setItemData(1, uint(AnalogChannel::AdmitFree));
  txAdmit->setItemData(2, uint(AnalogChannel::AdmitTone));
  populateCTCSSBox(rxTone, (nullptr != _channel ? _channel->rxTone() : Signaling::SIGNALING_NONE));
  populateCTCSSBox(txTone, (nullptr != _channel ? _channel->txTone() : Signaling::SIGNALING_NONE));
  bandwidth->setItemData(0, uint(AnalogChannel::BWNarrow));
  bandwidth->setItemData(1, uint(AnalogChannel::BWWide));
  aprsList->addItem(tr("[None]"), QVariant::fromValue((APRSSystem *)nullptr));
  aprsList->setCurrentIndex(0);
  for (int i=0; i<_config->posSystems()->aprsCount(); i++) {
    APRSSystem *sys = _config->posSystems()->aprsSystem(i);
    aprsList->addItem(sys->name(),QVariant::fromValue(sys));
    if (_channel && (_channel->aprsSystem() == sys))
      aprsList->setCurrentIndex(i+1);
  }

  if (_channel) {
    channelName->setText(_channel->name());
    rxFrequency->setText(format_frequency(_channel->rxFrequency()));
    txFrequency->setText(format_frequency(_channel->txFrequency()));
    if (Channel::HighPower==_channel->power())
      power->setCurrentIndex(0);
    else if (Channel::LowPower==_channel->power())
      power->setCurrentIndex(1);
    txTimeout->setValue(_channel->txTimeout());
    rxOnly->setChecked(_channel->rxOnly());
    switch (_channel->admit()) {
      case AnalogChannel::AdmitNone: txAdmit->setCurrentIndex(0); break;
      case AnalogChannel::AdmitFree: txAdmit->setCurrentIndex(1); break;
      case AnalogChannel::AdmitTone: txAdmit->setCurrentIndex(2); break;
    }
    squelch->setValue(_channel->squelch());
    if (AnalogChannel::BWNarrow == _channel->bandwidth())
      bandwidth->setCurrentIndex(0);
    else if (AnalogChannel::BWWide == _channel->bandwidth())
      bandwidth->setCurrentIndex(1);
  }

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

AnalogChannel *
AnalogChannelDialog::channel() {
  bool ok = false;
  QString name = channelName->text();
  double rx = rxFrequency->text().toDouble(&ok);
  double tx = txFrequency->text().toDouble(&ok);
  Channel::Power pwr = Channel::Power(power->currentData().toUInt(&ok));
  uint timeout = txTimeout->text().toUInt(&ok);
  bool rxonly = rxOnly->isChecked();
  AnalogChannel::Admit admit = AnalogChannel::Admit(txAdmit->currentData().toUInt(&ok));
  uint squ = squelch->text().toUInt(&ok);
  Signaling::Code rxtone = Signaling::Code(rxTone->currentData().toUInt(&ok));
  Signaling::Code txtone = Signaling::Code(txTone->currentData().toUInt(&ok));
  AnalogChannel::Bandwidth bw = AnalogChannel::Bandwidth(bandwidth->currentData().toUInt());
  ScanList *scanlist = scanList->currentData().value<ScanList *>();
  APRSSystem *aprs = aprsList->currentData().value<APRSSystem *>();

  if (_channel) {
    _channel->setName(name);
    _channel->setRXFrequency(rx);
    _channel->setTXFrequency(tx);
    _channel->setPower(pwr);
    _channel->setTimeout(timeout);
    _channel->setRXOnly(rxonly);
    _channel->setAdmit(admit);
    _channel->setSquelch(squ);
    _channel->setRXTone(rxtone);
    _channel->setTXTone(txtone);
    _channel->setBandwidth(bw);
    _channel->setScanList(scanlist);
    _channel->setAPRSSystem(aprs);
    return _channel;
  } else {
    return new AnalogChannel(name, rx, tx, pwr, timeout, rxonly, admit, squ,
                             rxtone, txtone, bw, scanlist, aprs);
  }
}

void
AnalogChannelDialog::onRepeaterSelected(const QModelIndex &index) {
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



