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
#include "application.hh"
#include <QCompleter>
#include <QAbstractProxyModel>

#include <QDebug>

/* ********************************************************************************************* *
 * Implementation of Channel
 * ********************************************************************************************* */
Channel::Channel(const QString &name, double rx, double tx, Power power, uint txTimeout, bool rxOnly, ScanList *scanlist,
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

double Channel::rxFrequency() const {
  return _rxFreq;
}
bool
Channel::setRXFrequency(double freq) {
  _rxFreq = freq;
  emit modified();
  return true;
}

double
Channel::txFrequency() const {
  return _txFreq;
}
bool
Channel::setTXFrequency(double freq) {
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
  if (nullptr == list)
    return false;
  _scanlist = list;
  if (_scanlist)
    connect(_scanlist, SIGNAL(destroyed(QObject *)), this, SLOT(onScanListDeleted(QObject *)));
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
AnalogChannel::AnalogChannel(const QString &name, double rxFreq, double txFreq, Power power,
                             uint txTimeout, bool rxOnly, Admit admit, uint squelch, float rxTone,
                             float txTone, Bandwidth bw, ScanList *list, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txTimeout, rxOnly, list, parent),
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
DigitalChannel::DigitalChannel(const QString &name, double rxFreq, double txFreq, Power power,
                               uint txto, bool rxOnly, Admit admit, uint colorCode,
                               TimeSlot timeslot, RXGroupList *rxGroup, DigitalContact *txContact,
                               GPSSystem *gpsSystem, ScanList *list, QObject *parent)
  : Channel(name, rxFreq, txFreq, power, txto, rxOnly, list, parent), _admit(admit),
    _colorCode(colorCode), _timeSlot(timeslot), _rxGroup(rxGroup), _txContact(txContact),
    _gpsSystem(gpsSystem)
{
  if (_rxGroup)
    connect(_rxGroup, SIGNAL(destroyed()), this, SLOT(onRxGroupDeleted()));
  if (_txContact)
    connect(_txContact, SIGNAL(destroyed()), this, SLOT(onTxContactDeleted()));
  if (_gpsSystem)
    connect(_gpsSystem, SIGNAL(destroyed()), this, SLOT(onGPSSystemDeleted()));
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

GPSSystem *
DigitalChannel::gpsSystem() const {
  return _gpsSystem;
}

bool
DigitalChannel::setGPSSystem(GPSSystem *gps) {
  if (_gpsSystem)
    disconnect(_gpsSystem, SIGNAL(destroyed()), this, SLOT(onGPSSystemDeleted()));
  _gpsSystem = gps;
  if (_gpsSystem)
    connect(_gpsSystem, SIGNAL(destroyed()), this, SLOT(onGPSSystemDeleted()));
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

void
DigitalChannel::onGPSSystemDeleted() {
  setGPSSystem(nullptr);
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
ChannelList::addChannel(Channel *channel, int row) {
  if (_channels.contains(channel))
    return -1;
  if ((row<0) || (row>_channels.size()))
    row = _channels.size();
  beginInsertRows(QModelIndex(), row, row);
  connect(channel, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(channel, SIGNAL(destroyed(QObject *)), this, SLOT(onChannelDeleted(QObject *)));
  _channels.insert(row, channel);
  endInsertRows();
  emit modified();
  return row;
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
ChannelList::moveUp(int row) {
  if ((0>=row) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_channels[row], _channels[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
ChannelList::moveDown(int row) {
  if ((0>row) || ((row-1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_channels[row], _channels[row+1]);
  endMoveRows();
  emit modified();
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
  return 18;
}

inline QString formatFrequency(float f) {
  int val = std::round(f*10000);
  return QString::number(double(val)/10000, 'f', 4);
}

QVariant
ChannelList::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (index.row()>=_channels.size()))
    return QVariant();
  if ((Qt::DisplayRole!=role) && (Qt::EditRole!=role))
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
      return tr("-");
    }
  case 9:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      return digi->colorCode();
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 10:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      return (DigitalChannel::TimeSlot1 == digi->timeslot()) ? 1 : 2;
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 11:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->rxGroupList()) {
        return digi->rxGroupList()->name();
      } else {
        return tr("-");
      }
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 12:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->txContact())
        return digi->txContact()->name();
      else
        return tr("-");
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 13:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->gpsSystem())
        return digi->gpsSystem()->name();
      else
        return tr("-");
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 14:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (0 == analog->squelch()) {
        return tr("Off");
      } else
        return analog->squelch();
    }
    break;
  case 15:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (0 == analog->rxTone()) {
        return tr("Off");
      } else
        return analog->rxTone();
    }
    break;
  case 16:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (0 == analog->txTone()) {
        return tr("Off");
      } else
        return analog->txTone();
    }
    break;
  case 17:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
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
  case 9: return tr("CC");
  case 10: return tr("TS");
  case 11: return tr("RX Group List");
  case 12: return tr("TX Contact");
  case 13: return tr("GPS System");
  case 14: return tr("Squelch");
  case 15: return tr("Rx Tone");
  case 16: return tr("Tx Tone");
  case 17: return tr("Bandwidth");
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


