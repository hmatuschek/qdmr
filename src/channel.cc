#include "channel.hh"
#include "contact.hh"
#include "ctcssbox.hh"
#include "rxgrouplist.hh"
#include "config.hh"
#include "scanlist.hh"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <cmath>


/* ********************************************************************************************* *
 * Implementation of Channel
 * ********************************************************************************************* */
Channel::Channel(const QString &name, float rx, float tx, Power power, uint txTimeout, bool rxOnly, ScanList *scanlist,
                 QObject *parent)
  : QObject(parent), _name(name), _rxFreq(rx), _txFreq(tx), _power(power), _txTimeOut(txTimeout),
    _rxOnly(rxOnly), _scanlist(scanlist)
{
  // pass..
}

const QString &
Channel::name() const {
  return _name;
}
bool
Channel::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name;
  emit modified();
  return true;
}

float
Channel::rxFrequency() const {
  return _rxFreq;
}
bool
Channel::setRXFrequency(float freq) {
  _rxFreq = freq;
  emit modified();
  return true;
}

float
Channel::txFrequency() const {
  return _txFreq;
}
bool
Channel::setTXFrequency(float freq) {
  _txFreq = freq;
  emit modified();
  return true;
}

Channel::Power
Channel::power() const {
  return _power;
}
void
Channel::setPower(Power power) {
  _power = power;
  emit modified();
}

uint
Channel::txTimeout() const {
  return _txTimeOut;
}
bool
Channel::setTimeout(uint dur) {
  _txTimeOut = dur;
  emit modified();
  return true;
}

bool
Channel::rxOnly() const {
  return _rxOnly;
}
bool
Channel::setRXOnly(bool enable) {
  _rxOnly = enable;
  emit modified();
  return true;
}

ScanList *
Channel::scanList() const {
  return _scanlist;
}
bool
Channel::setScanList(ScanList *list) {
  _scanlist = list;
  if (_scanlist)
    connect(_scanlist, SIGNAL(destroyed()), this, SLOT(onScanListDeleted(QObject *)));
  emit modified();
  return true;
}

void
Channel::onScanListDeleted(QObject *obj) {
  ScanList *scanlist = reinterpret_cast<ScanList *>(obj);
  if (_scanlist == scanlist)
    _scanlist = nullptr;
}


/* ********************************************************************************************* *
 * Implementation of AnalogChannel
 * ********************************************************************************************* */
AnalogChannel::AnalogChannel(const QString &name, float rxFreq, float txFreq, Power power,
                             uint txTimout, bool txOnly, Admit admit, uint squelch, float rxTone,
                             float txTone, Bandwidth bw, ScanList *list, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txTimout, txOnly, list, parent),
    _admit(admit), _squelch(squelch), _rxTone(rxTone), _txTone(txTone), _bw(bw)
{
  // pass...
}

AnalogChannel::Admit
AnalogChannel::admit() const {
  return _admit;
}
void
AnalogChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified();
}

uint
AnalogChannel::squelch() const {
  return _squelch;
}
bool
AnalogChannel::setSquelch(uint val) {
  _squelch = val;
  emit modified();
  return true;
}

float
AnalogChannel::rxTone() const {
  return _rxTone;
}
bool
AnalogChannel::setRXTone(float val) {
  _rxTone = val;
  emit modified();
  return true;
}

float
AnalogChannel::txTone() const {
  return _txTone;
}
bool
AnalogChannel::setTXTone(float val) {
  _txTone = val;
  emit modified();
  return true;
}

AnalogChannel::Bandwidth
AnalogChannel::bandwidth() const {
  return _bw;
}
bool
AnalogChannel::setBandwidth(Bandwidth bw) {
  _bw = bw;
  emit modified();
  return true;
}


/* ********************************************************************************************* *
 * Implementation of DigitalChannel
 * ********************************************************************************************* */
DigitalChannel::DigitalChannel(const QString &name, float rxFreq, float txFreq, Power power,
                               uint txto, bool rxOnly, Admit admit, uint colorCode,
                               TimeSlot timeslot, RXGroupList *rxGroup, DigitalContact *txContact,
                               ScanList *list, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txto, rxOnly, list, parent), _admit(admit),
    _colorCode(colorCode), _timeSlot(timeslot), _rxGroup(rxGroup), _txContact(txContact)
{
  // pass...
}

DigitalChannel::Admit
DigitalChannel::admit() const {
  return _admit;
}
void
DigitalChannel::setAdmit(Admit admit) {
  _admit = admit;
  emit modified();
}

uint
DigitalChannel::colorCode() const {
  return _colorCode;
}
bool
DigitalChannel::setColorCode(uint cc) {
  _colorCode = cc;
  emit modified();
  return true;
}

DigitalChannel::TimeSlot
DigitalChannel::timeslot() const {
  return _timeSlot;
}
bool
DigitalChannel::setTimeSlot(TimeSlot slot) {
  _timeSlot = slot;
  emit modified();
  return true;
}

RXGroupList *
DigitalChannel::rxGroupList() const {
  return _rxGroup;
}
bool
DigitalChannel::setRXGroupList(RXGroupList *g) {
  if (_rxGroup)
    disconnect(_rxGroup, SIGNAL(destroyed()), this, SLOT(onRxGroupDeleted()));
  _rxGroup = g;
  if (_rxGroup)
    connect(_rxGroup, SIGNAL(destroyed()), this, SLOT(onRxGroupDeleted()));
  emit modified();
  return true;
}

DigitalContact *
DigitalChannel::txContact() const {
  return _txContact;
}
bool
DigitalChannel::setTXContact(DigitalContact *c) {
  if (_txContact)
    disconnect(_txContact, SIGNAL(destroyed()), this, SLOT(onTxContactDeleted()));
  _txContact = c;
  if (_txContact)
    connect(_txContact, SIGNAL(destroyed()), this, SLOT(onTxContactDeleted()));
  emit modified();
  return true;
}

void
DigitalChannel::onRxGroupDeleted() {
  setRXGroupList(nullptr);
}

void
DigitalChannel::onTxContactDeleted() {
  setTXContact(nullptr);
}


/* ********************************************************************************************* *
 * Implementation of ChannelList
 * ********************************************************************************************* */
ChannelList::ChannelList(QObject *parent)
  : QAbstractTableModel(parent), _channels()
{
  connect(this, SIGNAL(modified()), this, SLOT(onChannelEdited()));
}

int
ChannelList::count() const {
  return _channels.size();
}

void
ChannelList::clear() {
  for (int i=0; i<count(); i++)
    _channels[i]->deleteLater();
}

int
ChannelList::indexOf(Channel *channel) const {
  if (! _channels.contains(channel))
    return -1;
  return _channels.indexOf(channel);
}

Channel *
ChannelList::channel(int idx) const {
  if ((0>idx) || (idx >= _channels.size()))
    return nullptr;
  return _channels.at(idx);
}

int
ChannelList::addChannel(Channel *channel) {
  if (_channels.contains(channel))
    return -1;
  int idx = _channels.size();
  beginInsertRows(QModelIndex(), idx, idx);
  connect(channel, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(channel, SIGNAL(destroyed(QObject *)), this, SLOT(onChannelDeleted(QObject *)));
  _channels.append(channel);
  endInsertRows();
  emit modified();
  return idx;
}

bool
ChannelList::remChannel(int idx) {
  if ((0>idx) || (idx >= _channels.size()))
    return false;
  beginRemoveRows(QModelIndex(), idx, idx);
  Channel *channel = _channels.at(idx);
  _channels.remove(idx);
  channel->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
ChannelList::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

bool
ChannelList::moveUp(int idx) {
  if ((0>=idx) || (idx>=count()))
    return false;
  qSwap(_channels[idx],_channels[idx-1]);
  QModelIndex tl=index(idx-1,0), br=index(idx, columnCount(QModelIndex()));
  emit dataChanged(tl, br);
  return true;
}

bool
ChannelList::moveDown(int idx) {
  if ((0>idx) || ((idx+1)>=count()))
    return false;
  qSwap(_channels[idx],_channels[idx+1]);
  QModelIndex tl=index(idx-1,0), br=index(idx, columnCount(QModelIndex()));
  emit dataChanged(tl, br);
  return true;
}

int
ChannelList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _channels.size();
}
int
ChannelList::columnCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return 17;
}

inline QString formatFrequency(float f) {
  int val = std::round(f*10000);
  return QString::number(double(val)/10000, 'f', 4);
}

QVariant
ChannelList::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (Qt::DisplayRole!=role) || (index.row()>=_channels.size()))
    return QVariant();
  Channel *channel = _channels[index.row()];

  switch (index.column()) {
    case 0:
      if (channel->is<AnalogChannel>())
        return tr("Analog");
      else
        return tr("Digital");
    case 1:
      return channel->name();
    case 2:
      return formatFrequency(channel->rxFrequency());
    case 3:
      if (channel->txFrequency()<channel->rxFrequency())
        return formatFrequency(channel->txFrequency()-channel->rxFrequency());
      else
        return formatFrequency(channel->txFrequency());
    case 4:
      return (Channel::HighPower == channel->power()) ? tr("High") : tr("Low");
    case 5:
      if (0 == channel->txTimeout())
        return tr("-");
      return QString::number(channel->txTimeout());
    case 6:
      return channel->rxOnly() ? tr("On") : tr("Off");
    case 7:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        switch (digi->admit()) {
          case DigitalChannel::AdmitNone: return tr("Always");
          case DigitalChannel::AdmitFree: return tr("Free");
          case DigitalChannel::AdmitColorCode: return tr("Color");
        }
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        switch (analog->admit()) {
          case AnalogChannel::AdmitNone: return tr("Always");
          case AnalogChannel::AdmitFree: return tr("Free");
          case AnalogChannel::AdmitTone: return tr("Tone");
        }
      }
      break;
    case 8:
      if (channel->scanList()) {
        return channel->scanList()->name();
      } else {
        return tr("[None]");
      }
    case 9:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        return digi->colorCode();
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        return tr("-");
      }
      break;
    case 10:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        return (DigitalChannel::TimeSlot1 == digi->timeslot()) ? 1 : 2;
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        return tr("-");
      }
      break;
    case 11:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        if (digi->rxGroupList()) {
          return digi->rxGroupList()->name();
        } else {
          return tr("[None]");
        }
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        return tr("[None]");
      }
      break;
    case 12:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        if (digi->txContact())
          return digi->txContact()->name();
        else
          return tr("[None]");
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        return tr("[None]");
      }
      break;
    case 13:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        return tr("[None]");
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        if (0 == analog->squelch()) {
          return tr("Off");
        } else
          return analog->squelch();
      }
      break;
    case 14:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        return tr("[None]");
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        if (0 == analog->rxTone()) {
          return tr("Off");
        } else
          return analog->rxTone();
      }
      break;
    case 15:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        return tr("[None]");
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        if (0 == analog->txTone()) {
          return tr("Off");
        } else
          return analog->txTone();
      }
      break;
    case 16:
      if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
        return tr("Narrow");
      } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
        if (AnalogChannel::BWWide == analog->bandwidth()) {
          return tr("Wide");
        } else
          return tr("Narrow");
      }
      break;

    default:
      break;
  }

  return QVariant();
}

QVariant
ChannelList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation))
    return QVariant();
  switch (section) {
    case 0: return tr("Type");
    case 1: return tr("Name");
    case 2: return tr("Rx Frequency");
    case 3: return tr("Tx Frequency");
    case 4: return tr("Power");
    case 5: return tr("Timeout");
    case 6: return tr("Rx Only");
    case 7: return tr("Admit");
    case 8: return tr("Scanlist");
    case 9: return tr("Colorcode");
    case 10: return tr("Timeslot");
    case 11: return tr("RX Group List");
    case 12: return tr("TX Contact");
    case 13: return tr("Squelch");
    case 14: return tr("Rx Tone");
    case 15: return tr("Tx Tone");
    case 16: return tr("Bandwidth");
    default:
      break;
  }
  return QVariant();
}

void
ChannelList::onChannelDeleted(QObject *obj) {
  if (Channel *channel = reinterpret_cast<Channel *>(obj))
    remChannel(channel);
}

void
ChannelList::onChannelEdited() {
  if (0 == count())
    return;
  QModelIndex tl = index(0,0), br = index(count()-1, columnCount(QModelIndex()));
  emit dataChanged(tl, br);
}


/* ********************************************************************************************* *
 * Implementation of ChannelListView
 * ********************************************************************************************* */
ChannelListView::ChannelListView(Config *config, QWidget *parent)
  : QWidget(parent), _config(config), _list(config->channelList()), _view(nullptr)
{
  QPushButton *up   = new QPushButton(QIcon("://icons/up.png"),"");
  QPushButton *down = new QPushButton(QIcon("://icons/down.png"), "");
  up->setToolTip(tr("Move channel up in list."));
  down->setToolTip(tr("Move channel down in list."));
  QPushButton *adda = new QPushButton(tr("Add Analog Channel"));
  QPushButton *addd = new QPushButton(tr("Add Digital Channel"));
  QPushButton *rem  = new QPushButton(tr("Delete Channel"));

  _view = new QTableView();
  _view->setModel(_list);
  _view->setColumnWidth(0, 60);
  _view->setColumnWidth(1, 120);
  _view->setColumnWidth(2, 80);
  _view->setColumnWidth(3, 80);
  _view->setColumnWidth(4, 50);
  _view->setColumnWidth(5, 60);
  _view->setColumnWidth(6, 50);
  _view->setColumnWidth(7, 60);
  _view->setColumnWidth(8, 120);
  _view->setColumnWidth(9, 30);
  _view->setColumnWidth(10, 30);
  _view->setColumnWidth(11, 120);
  _view->setColumnWidth(12, 120);
  _view->setColumnWidth(13, 60);
  _view->setColumnWidth(14, 60);
  _view->setColumnWidth(15, 60);
  _view->setColumnWidth(16, 60);

  QVBoxLayout *layout = new QVBoxLayout();

  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addWidget(_view,1);
  QVBoxLayout *vbox = new QVBoxLayout();
  vbox->addWidget(up);
  vbox->addWidget(down);
  hbox->addLayout(vbox);
  layout->addLayout(hbox);

  hbox = new QHBoxLayout();
  hbox->addWidget(adda);
  hbox->addWidget(addd);
  hbox->addWidget(rem);
  layout->addLayout(hbox);
  setLayout(layout);

  connect(up,   SIGNAL(clicked()), this, SLOT(onChannelUp()));
  connect(down, SIGNAL(clicked()), this, SLOT(onChannelDown()));
  connect(adda, SIGNAL(clicked()), this, SLOT(onAddAnalogChannel()));
  connect(addd, SIGNAL(clicked()), this, SLOT(onAddDigitalChannel()));
  connect(rem,  SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditChannel(const QModelIndex &)));
}

void
ChannelListView::onChannelUp() {
  QModelIndex selected =_view->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0==selected.row()))
    return;
  if (_config->channelList()->moveUp(selected.row()))
    _view->selectRow(selected.row()-1);
}

void
ChannelListView::onChannelDown() {
  QModelIndex selected =_view->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (_config->channelList()->count()==(selected.row()+1)))
    return;
  if (_config->channelList()->moveDown(selected.row()))
    _view->selectRow(selected.row()+1);
}

void
ChannelListView::onAddAnalogChannel() {
  AnalogChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  _list->addChannel(dialog.channel());
}

void
ChannelListView::onAddDigitalChannel() {
  DigitalChannelDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  _list->addChannel(dialog.channel());
}

void
ChannelListView::onRemChannel() {
  QModelIndex selected =_view->selectionModel()->currentIndex();
  if (! selected.isValid()) {
    QMessageBox::information(nullptr, tr("Cannot delete channel"),
                             tr("Cannot delete channel: You have to select a channel first."));
    return;
  }

  QString name = _list->channel(selected.row())->name();
  if (QMessageBox::No == QMessageBox::question(nullptr, tr("Delete channel?"), tr("Delete channel %1?").arg(name)))
    return;

  _list->remChannel(selected.row());
}

void
ChannelListView::onEditChannel(const QModelIndex &index) {
  Channel *channel = _config->channelList()->channel(index.row());
  if (! channel)
    return;
  if (channel->is<AnalogChannel>()) {
    AnalogChannelDialog dialog(_config, channel->as<AnalogChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  } else {
    DigitalChannelDialog dialog(_config, channel->as<DigitalChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  }
}


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
  _name = new QLineEdit();
  if (_channel)
    _name->setText(_channel->name());

  _rx = new QLineEdit();
  _rx->setValidator(new QDoubleValidator(0,500,5));
  if (_channel)
    _rx->setText(QString::number(_channel->rxFrequency()));

  _tx = new QLineEdit();
  _tx->setValidator(new QDoubleValidator(0,500,5));
  if (_channel)
    _tx->setText(QString::number(_channel->txFrequency()));

  _power = new QComboBox();
  _power->addItem(tr("High"), uint(Channel::HighPower));
  _power->addItem(tr("Low"), uint(Channel::LowPower));
  if (_channel && (Channel::HighPower==_channel->power()))
    _power->setCurrentIndex(0);
  else if (_channel && (Channel::LowPower==_channel->power()))
    _power->setCurrentIndex(1);

  _timeout = new QSpinBox();
  _timeout->setValue(0);
  _timeout->setRange(0, 555);
  _timeout->setSingleStep(15);
  if (_channel)
    _timeout->setValue(_channel->txTimeout());

  _rxOnly = new QCheckBox();
  if (_channel)
    _rxOnly->setChecked(_channel->rxOnly());

  _scanlist = new QComboBox();
  _scanlist->addItem(tr("[None]"), QVariant::fromValue((ScanList *)(nullptr)));
  _scanlist->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    _scanlist->addItem(_config->scanlists()->scanlist(i)->name(),
                       QVariant::fromValue(_config->scanlists()->scanlist(i)));
    if (_channel && (_channel->scanList() == _config->scanlists()->scanlist(i)) )
      _scanlist->setCurrentIndex(i);
  }

  _admit = new QComboBox();
  _admit->addItem(tr("Always"), uint(AnalogChannel::AdmitNone));
  _admit->addItem(tr("Channel free"), uint(AnalogChannel::AdmitFree));
  _admit->addItem(tr("Tone"), uint(AnalogChannel::AdmitTone));
  if (_channel) {
    switch (_channel->admit()) {
      case AnalogChannel::AdmitNone: _admit->setCurrentIndex(0); break;
      case AnalogChannel::AdmitFree: _admit->setCurrentIndex(1); break;
      case AnalogChannel::AdmitTone: _admit->setCurrentIndex(2); break;
    }
  }

  _squelch = new QSpinBox();
  _squelch->setValue(1);
  _squelch->setRange(0, 9);
  if (_channel)
    _squelch->setValue(_channel->squelch());

  _rxTone = new CTCSSBox();
  _txTone = new CTCSSBox();

  _bw = new QComboBox();
  _bw->addItem(tr("Narrow (12.5kHz)"), uint(AnalogChannel::BWNarrow));
  _bw->addItem(tr("Wide (25kHz)"), uint(AnalogChannel::BWWide));
  if (_channel && (AnalogChannel::BWNarrow == _channel->bandwidth()))
    _bw->setCurrentIndex(0);
  else if (_channel && (AnalogChannel::BWWide == _channel->bandwidth()))
    _bw->setCurrentIndex(1);

  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);
  connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

  QHBoxLayout *hbox = new QHBoxLayout();
  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Name"), _name);
  form->addRow(tr("Rx Frequency [MHz]"), _rx);
  form->addRow(tr("Tx Frequency [MHz]"), _tx);
  form->addRow(tr("Power"), _power);
  form->addRow(tr("TX Timeout"), _timeout);
  form->addRow(tr("RX Only"), _rxOnly);
  form->addRow(tr("Scanlist"), _scanlist);
  hbox->addLayout(form);
  form = new QFormLayout();
  form->addRow(tr("TX Admit"), _admit);
  form->addRow(tr("Squelch"), _squelch);
  form->addRow(tr("RX Tone"), _rxTone);
  form->addRow(tr("TX Tone"), _txTone);
  form->addRow(tr("Bandwidth"), _bw);
  hbox->addLayout(form);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(hbox);
  layout->addWidget(bb);

  setLayout(layout);
}

AnalogChannel *
AnalogChannelDialog::channel() {
  bool ok = false;
  QString name = _name->text();
  double rx = _rx->text().toDouble(&ok);
  double tx = _tx->text().toDouble(&ok);
  Channel::Power power = Channel::Power(_power->currentData().toUInt(&ok));
  uint timeout = _timeout->text().toUInt(&ok);
  bool rxOnly = _rxOnly->isChecked();
  AnalogChannel::Admit admit = AnalogChannel::Admit(_admit->currentData().toUInt(&ok));
  uint squelch = _squelch->text().toUInt(&ok);
  double rxTone = _rxTone->currentData().toDouble(&ok);
  double txTone = _rxTone->currentData().toDouble(&ok);
  AnalogChannel::Bandwidth bw = AnalogChannel::Bandwidth(_bw->currentData().toUInt());
  ScanList *scanlist = _scanlist->currentData().value<ScanList *>();

  if (_channel) {
    _channel->setName(name);
    _channel->setRXFrequency(rx);
    _channel->setTXFrequency(tx);
    _channel->setPower(power);
    _channel->setTimeout(timeout);
    _channel->setRXOnly(rxOnly);
    _channel->setAdmit(admit);
    _channel->setSquelch(squelch);
    _channel->setRXTone(rxTone);
    _channel->setTXTone(txTone);
    _channel->setBandwidth(bw);
    _channel->setScanList(scanlist);
    return _channel;
  } else {
    return new AnalogChannel(name, rx, tx, power, timeout, rxOnly, admit, squelch,
                             rxTone, txTone, bw, scanlist);
  }
}


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
  _name = new QLineEdit();
  if (_channel)
    _name->setText(_channel->name());

  _rx = new QLineEdit();
  _rx->setValidator(new QDoubleValidator(0,500,5));
  if (_channel)
    _rx->setText(QString::number(_channel->rxFrequency()));

  _tx = new QLineEdit();
  _tx->setValidator(new QDoubleValidator(0,500,5));
  if (_channel)
    _tx->setText(QString::number(_channel->txFrequency()));

  _power = new QComboBox();
  _power->addItem(tr("High"));
  _power->addItem(tr("Low"));
  if (_channel && (Channel::HighPower==_channel->power()))
    _power->setCurrentIndex(0);
  else if (_channel && (Channel::LowPower==_channel->power()))
    _power->setCurrentIndex(1);

  _timeout = new QSpinBox();
  _timeout->setValue(0);
  _timeout->setRange(0, 555);
  _timeout->setSingleStep(15);
  if (_channel)
    _timeout->setValue(_channel->txTimeout());

  _rxOnly = new QCheckBox();
  if (_channel)
    _rxOnly->setChecked(_channel->rxOnly());

  _scanlist = new QComboBox();
  _scanlist->addItem(tr("[None]"), QVariant::fromValue((ScanList *)(nullptr)));
  _scanlist->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    _scanlist->addItem(_config->scanlists()->scanlist(i)->name(),
                       QVariant::fromValue(_config->scanlists()->scanlist(i)));
    if (_channel && (_channel->scanList() == _config->scanlists()->scanlist(i)) )
      _scanlist->setCurrentIndex(i);
  }

  _admit = new QComboBox();
  _admit->addItem(tr("Always"));
  _admit->addItem(tr("Channel free"));
  _admit->addItem(tr("Color code"));
  if (_channel) {
    switch(_channel->admit()) {
      case DigitalChannel::AdmitNone: _admit->setCurrentIndex(0); break;
      case DigitalChannel::AdmitFree: _admit->setCurrentIndex(1); break;
      case DigitalChannel::AdmitColorCode: _admit->setCurrentIndex(2); break;
    }
  }
  _colorCode = new QSpinBox();
  _colorCode->setValue(1);
  _colorCode->setRange(0, 15);
  if (_channel)
    _colorCode->setValue(_channel->colorCode());

  _slot = new QComboBox();
  _slot->addItem(tr("TS 1"), uint(DigitalChannel::TimeSlot1));
  _slot->addItem(tr("TS 2"), uint(DigitalChannel::TimeSlot2));
  if (_channel && (DigitalChannel::TimeSlot1 == _channel->timeslot()))
    _slot->setCurrentIndex(0);
  else if (_channel && (DigitalChannel::TimeSlot2 == _channel->timeslot()))
    _slot->setCurrentIndex(1);

  _rxGroup = new RXGroupListBox(_config->rxGroupLists());
  if (_channel && _channel->rxGroupList())
    _rxGroup->setCurrentIndex(_config->rxGroupLists()->indexOf(_channel->rxGroupList()));
  _contact = new DigitalContactBox(_config->contacts());
  if (_channel && _channel->txContact())
    _contact->setCurrentIndex(_config->contacts()->indexOf(_channel->txContact()));

  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);
  connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

  QHBoxLayout *hbox = new QHBoxLayout();
  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Name"), _name);
  form->addRow(tr("Rx Frequency [MHz]"), _rx);
  form->addRow(tr("Tx Frequency [MHz]"), _tx);
  form->addRow(tr("Power"), _power);
  form->addRow(tr("TX Timeout"), _timeout);
  form->addRow(tr("RX Only"), _rxOnly);
  form->addRow(tr("Scanlist"), _scanlist);
  hbox->addLayout(form);
  form = new QFormLayout();
  form->addRow(tr("TX Admit"), _admit);
  form->addRow(tr("Color code"), _colorCode);
  form->addRow(tr("Time Slot"), _slot);
  form->addRow(tr("RX Group List"), _rxGroup);
  form->addRow(tr("Contact"), _contact);
  hbox->addLayout(form);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(hbox);
  layout->addWidget(bb);

  setLayout(layout);
}

DigitalChannel *
DigitalChannelDialog::channel() {
  bool ok = false;
  QString name = _name->text();
  double rx = _rx->text().toDouble(&ok);
  double tx = _tx->text().toDouble(&ok);
  Channel::Power power = Channel::Power(_power->currentData().toUInt(&ok));
  uint timeout = _timeout->text().toUInt(&ok);
  bool rxOnly = _rxOnly->isChecked();
  DigitalChannel::Admit admit = DigitalChannel::Admit(_admit->currentData().toUInt(&ok));
  uint colorCode = _colorCode->value();
  DigitalChannel::TimeSlot ts = DigitalChannel::TimeSlot(_slot->currentData().toUInt(&ok));
  RXGroupList *rxgroup = _rxGroup->currentData().value<RXGroupList *>();
  DigitalContact *contact = _contact->currentData().value<DigitalContact *>();
  ScanList *scanlist = _scanlist->currentData().value<ScanList *>();

  if (_channel) {
    _channel->setName(name);
    _channel->setRXFrequency(rx);
    _channel->setTXFrequency(tx);
    _channel->setPower(power);
    _channel->setTimeout(timeout);
    _channel->setRXOnly(rxOnly);
    _channel->setScanList(scanlist);
    _channel->setAdmit(admit);
    _channel->setColorCode(colorCode);
    _channel->setTimeSlot(ts);
    _channel->setRXGroupList(rxgroup);
    _channel->setTXContact(contact);
    return _channel;
  } else {
    return new DigitalChannel(name, rx, tx, power, timeout, rxOnly, admit, colorCode, ts, rxgroup,
                              contact, scanlist);
  }
}


/* ********************************************************************************************* *
 * Implementation of ChannelComboBox
 * ********************************************************************************************* */
ChannelComboBox::ChannelComboBox(ChannelList *list, QWidget *parent)
  : QComboBox(parent)
{
  for (int i=0; i<list->rowCount(QModelIndex()); i++) {
    if (list->channel(i)->is<AnalogChannel>())
      addItem(tr("%1 (Analog)").arg(list->channel(i)->name()),
              QVariant::fromValue(list->channel(i)));
    else
      addItem(tr("%1 (Digital)").arg(list->channel(i)->name()),
              QVariant::fromValue(list->channel(i)));
  }
}

Channel *
ChannelComboBox::channel() const {
  return currentData().value<Channel*>();
}
