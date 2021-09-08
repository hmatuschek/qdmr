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
  return _channels.count();
}

void
ScanList::clear() {
  _channels.clear();
  _name.clear();
  _priorityChannel = nullptr;
  _secPriorityChannel = nullptr;
  _txChannel = nullptr;
  emit modified(this);
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
  emit modified(this);
  return true;
}

bool
ScanList::contains(Channel *channel) const {
  return (0 <= _channels.indexOf(channel));
}

Channel *
ScanList::channel(int idx) const {
  return _channels.get(idx)->as<Channel>();
}

int
ScanList::addChannel(Channel *channel, int idx) {
  idx = _channels.add(channel, idx);
  if (0 > idx)
    return idx;
  return idx;
}

bool
ScanList::remChannel(int idx) {
  return _channels.del(_channels.get(idx));
  emit modified(this);
  return true;
}

bool
ScanList::remChannel(Channel *channel) {
  return _channels.del(channel);
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
  emit modified(this);
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
  emit modified(this);
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
  emit modified(this);
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

bool
ScanList::serialize(YAML::Node &node, const Context &context) {
  if (! ConfigObject::serialize(node, context))
    return false;

  QStringList pchannels;
  if (_priorityChannel && context.contains(_priorityChannel))
    pchannels.append(context.getId(_priorityChannel));
  if (_secPriorityChannel && context.contains(_secPriorityChannel))
    pchannels.append(context.getId(_secPriorityChannel));
  if (pchannels.count()) {
    YAML::Node priority = YAML::Node(YAML::NodeType::Sequence);
    priority.SetStyle(YAML::EmitterStyle::Flow);
    foreach (QString id, pchannels) {
      priority.push_back(id.toStdString());
    }
    node["priority"] = priority;
  }

  if (_txChannel && context.contains(_txChannel)) {
    node["revert"] = context.getId(_txChannel).toStdString();
  }

  return true;
}

const ChannelRefList *
ScanList::channels() const {
  return &_channels;
}

ChannelRefList *
ScanList::channels() {
  return &_channels;
}


/* ********************************************************************************************* *
 * Implementation of ScanLists
 * ********************************************************************************************* */
ScanLists::ScanLists(QObject *parent)
  : ConfigObjectList(ScanList::staticMetaObject, parent)
{
  // pass...
}

ScanList *
ScanLists::scanlist(int idx) const {
  if (ConfigObject *obj = get(idx))
    return obj->as<ScanList>();
  return nullptr;
}

int
ScanLists::add(ConfigObject *obj, int row) {
  if (obj && obj->is<ScanList>())
    return ConfigObjectList::add(obj, row);
  return -1;
}
