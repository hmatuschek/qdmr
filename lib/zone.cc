#include "zone.hh"
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
 * Implementation of Zone
 * ********************************************************************************************* */
ZoneChannelList::ZoneChannelList(QObject *parent)
  : QAbstractListModel(parent), _channels()
{
  // pass...
}

int
ZoneChannelList::count() const {
  return _channels.size();
}

void
ZoneChannelList::clear() {
  beginResetModel();
  _channels.clear();
  endResetModel();
  emit modified();
}

Channel *
ZoneChannelList::channel(int idx) const {
  if ((0>idx) || (idx>=_channels.size()))
    return nullptr;
  return _channels[idx];
}

bool
ZoneChannelList::addChannel(Channel *channel) {
  if (_channels.contains(channel) || (nullptr == channel))
    return false;
  int idx = _channels.size();
  beginInsertRows(QModelIndex(), idx, idx);
  connect(channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  _channels.append(channel);
  endInsertRows();
  emit modified();
  return true;
}

bool
ZoneChannelList::remChannel(int idx) {
  if ((0>idx) || (idx>=_channels.size()))
    return false;
  Channel *channel = _channels[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _channels.remove(idx);
  channel->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
ZoneChannelList::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

int
ZoneChannelList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _channels.size();
}

QVariant
ZoneChannelList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_channels.size()) || (0 != index.column()))
    return QVariant();
  return _channels[index.row()]->name();
}

QVariant
ZoneChannelList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Channel");
}

void
ZoneChannelList::onChannelDeleted(QObject *obj) {
  if (Channel *channel = reinterpret_cast<Channel *>(obj)) {
    remChannel(channel);
    emit modified();
  }
}



/* ********************************************************************************************* *
 * Implementation of Zone
 * ********************************************************************************************* */
Zone::Zone(const QString &name, QObject *parent)
  : ConfigObject("zone", parent), _name(name),
    _A(new ZoneChannelList(this)), _B(new ZoneChannelList(this))
{
  connect(_A, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_B, SIGNAL(modified()), this, SIGNAL(modified()));
}

const QString &
Zone::name() const {
  return _name;
}
bool
Zone::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name;
  emit modified();
  return true;
}

const ZoneChannelList *
Zone::A() const {
  return _A;
}
ZoneChannelList *
Zone::A() {
  return _A;
}

const ZoneChannelList *
Zone::B() const {
  return _B;
}
ZoneChannelList *
Zone::B() {
  return _B;
}

bool
Zone::serialize(YAML::Node &node, const Context &context) {
  if (! ConfigObject::serialize(node, context))
    return false;

  YAML::Node A = YAML::Node(YAML::NodeType::Sequence);
  A.SetStyle(YAML::EmitterStyle::Flow);
  for (int i=0; i<_A->count(); i++) {
    if (context.contains(_A->channel(i)))
      A.push_back(context.getId(_A->channel(i)).toStdString());
  }
  node["A"] = A;

  if (_B->count()) {
    YAML::Node B = YAML::Node(YAML::NodeType::Sequence);
    B.SetStyle(YAML::EmitterStyle::Flow);
    for (int i=0; i<_B->count(); i++) {
      if (context.contains(_B->channel(i)))
        B.push_back(context.getId(_B->channel(i)).toStdString());
    }
    node["B"] = B;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ZoneList
 * ********************************************************************************************* */
ZoneList::ZoneList(QObject *parent)
  : ConfigObjectList(parent)
{
  // pass...
}

Zone *
ZoneList::zone(int idx) const {
  if (ConfigObject *obj = get(idx))
    return obj->as<Zone>();
  return nullptr;
}

int
ZoneList::add(ConfigObject *obj, int row) {
  if (obj && obj->is<Zone>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

