#include "scanlist.hh"
#include "channel.hh"
#include "config.hh"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QInputDialog>


/* ********************************************************************************************* *
 * Implementation of ScanList
 * ********************************************************************************************* */
ScanList::ScanList(const QString &name, QObject *parent)
  : ConfigObject("scan", parent), _name(name), _channels(), _priorityChannel(nullptr),
    _secPriorityChannel(nullptr), _txChannel(nullptr)
{
  // pass...
}

int
ScanList::count() const {
  return _channels.size();
}

void
ScanList::clear() {
  _channels.clear();
  _name.clear();
  _priorityChannel = nullptr;
  _secPriorityChannel = nullptr;
  _txChannel = nullptr;
  emit modified();
}

const QString &
ScanList::name() const {
  return _name;
}

bool
ScanList::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name.simplified();
  emit modified();
  return true;
}

bool
ScanList::contains(Channel *channel) const {
  return _channels.contains(channel);
}

Channel *
ScanList::channel(int idx) const {
  if ((0>idx) || (idx>=_channels.size()))
    return nullptr;
  return _channels[idx];
}

bool
ScanList::addChannel(Channel *channel) {
  if (_channels.contains(channel) || (nullptr == channel))
    return false;
  connect(channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  _channels.append(channel);
  emit modified();
  return true;
}

bool
ScanList::remChannel(int idx) {
  if ((0>idx) || (idx>=_channels.size()))
    return false;
  Channel *channel = _channels[idx];
  _channels.remove(idx);
  disconnect(channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  emit modified();
  return true;
}

bool
ScanList::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

Channel *
ScanList::priorityChannel() const {
  return _priorityChannel;
}

void
ScanList::setPriorityChannel(Channel *channel) {
  if (_priorityChannel)
    disconnect(_priorityChannel, SIGNAL(destroyed(QObject*)),
               this, SLOT(onChannelDeleted(QObject*)));
  _priorityChannel = channel;
  if (_priorityChannel)
    connect(_priorityChannel, SIGNAL(destroyed(QObject *)), this, SLOT(onChannelDeleted(QObject *)));
  emit modified();
}

Channel *
ScanList::secPriorityChannel() const {
  return _secPriorityChannel;
}

void
ScanList::setSecPriorityChannel(Channel *channel) {
  if (_secPriorityChannel)
    disconnect(_secPriorityChannel, SIGNAL(destroyed(QObject*)),
               this, SLOT(onChannelDeleted(QObject*)));
  _secPriorityChannel = channel;
  if (_secPriorityChannel)
    connect(_secPriorityChannel, SIGNAL(destroyed(QObject *)), this, SLOT(onChannelDeleted(QObject *)));
  emit modified();
}

Channel *
ScanList::txChannel() const {
  return _txChannel;
}

void
ScanList::setTXChannel(Channel *channel) {
  if (_txChannel)
    disconnect(_txChannel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  _txChannel = channel;
  if (_txChannel)
    connect(_txChannel, SIGNAL(destroyed(QObject *)), this, SLOT(onChannelDeleted(QObject *)));
  emit modified();
}

void
ScanList::onChannelDeleted(QObject *obj) {
  if (Channel *channel = dynamic_cast<Channel *>(obj)) {
    remChannel(channel);
    if (_priorityChannel == channel)
      _priorityChannel = nullptr;
    if (_secPriorityChannel == channel)
      _secPriorityChannel = nullptr;
    if (_txChannel == channel)
      _txChannel = nullptr;
  }
}


/* ********************************************************************************************* *
 * Implementation of ScanLists
 * ********************************************************************************************* */
ScanLists::ScanLists(QObject *parent)
  : ConfigObjectList(parent)
{
  // pass...
}

ScanList *
ScanLists::scanlist(int idx) const {
  if (ConfigObject *obj = get(idx))
    return obj->as<ScanList>();
  return nullptr;
}

