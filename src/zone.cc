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
Zone::Zone(const QString &name, QObject *parent)
  : QAbstractListModel(parent), _name(name), _channels()
{
  // pass...
}

int
Zone::count() const {
  return _channels.size();
}

void
Zone::clear() {
  beginResetModel();
  _channels.clear();
  endResetModel();
  emit modified();
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

Channel *
Zone::channel(int idx) const {
  if ((0>idx) || (idx>=_channels.size()))
    return nullptr;
  return _channels[idx];
}

bool
Zone::addChannel(Channel *channel) {
  if (_channels.contains(channel))
    return false;
  int idx = _channels.size();
  beginInsertRows(QModelIndex(), idx, idx);
  channel->setParent(this);
  connect(channel, SIGNAL(destroyed(QObject*)), this, SLOT(onChannelDeleted(QObject*)));
  _channels.append(channel);
  endInsertRows();
  emit modified();
  return true;
}

bool
Zone::remChannel(int idx) {
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
Zone::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

int
Zone::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _channels.size();
}

QVariant
Zone::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_channels.size()) || (0 != index.column()))
    return QVariant();
  return _channels[index.row()]->name();
}

QVariant
Zone::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Channel");
}

void
Zone::onChannelDeleted(QObject *obj) {
  if (Channel *channel = reinterpret_cast<Channel *>(obj))
    remChannel(channel);
}


/* ********************************************************************************************* *
 * Implementation of ZoneList
 * ********************************************************************************************* */
ZoneList::ZoneList(QObject *parent)
  : QAbstractListModel(parent), _zones()
{
  // pass...
}

int
ZoneList::count() const {
  return _zones.size();
}

void
ZoneList::clear() {
  for (int i=0; i<count(); i++)
    _zones[i]->deleteLater();
}

Zone *
ZoneList::zone(int idx) const {
  if ((0>idx) || (idx>=_zones.size()))
    return nullptr;
  return _zones[idx];
}

bool
ZoneList::addZone(Zone *zone) {
  if (_zones.contains(zone))
    return false;
  int idx = _zones.size();
  beginInsertRows(QModelIndex(), idx, idx);
  zone->setParent(this);
  connect(zone, SIGNAL(destroyed(QObject*)), this, SLOT(onZoneDeleted(QObject*)));
  connect(zone, SIGNAL(modified()), this, SIGNAL(modified()));
  _zones.append(zone);
  endInsertRows();
  emit modified();
  return true;
}

bool
ZoneList::remZone(int idx) {
  if ((0>idx) || (idx>=_zones.size()))
    return false;
  Zone *zone = _zones[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _zones.remove(idx);
  zone->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
ZoneList::remZone(Zone *zone) {
  if (! _zones.contains(zone))
    return false;
  int idx = _zones.indexOf(zone);
  return remZone(idx);
}

bool
ZoneList::moveUp(int row) {
  if ((0>=row) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_zones[row], _zones[row-1]);
  endMoveRows();
  emit modified();
  return true;
}

bool
ZoneList::moveDown(int row) {
  if ((0>row) || ((row-1)>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_zones[row], _zones[row+1]);
  endMoveRows();
  emit modified();
  return true;
}

int
ZoneList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _zones.size();
}

QVariant
ZoneList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_zones.size()) || (0 != index.column()))
    return QVariant();
  return _zones[index.row()]->name();
}

QVariant
ZoneList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Zone");
}

void
ZoneList::onZoneDeleted(QObject *obj) {
  if (Zone *zone = reinterpret_cast<Zone *>(obj))
    remZone(zone);
}


/* ********************************************************************************************* *
 * Implementation of ZoneDialog
 * ********************************************************************************************* */
ZoneDialog::ZoneDialog(Config *config, Zone *zone, QWidget *parent)
  : QDialog(parent), _config(config), _zone(zone)
{
  construct();
}

ZoneDialog::ZoneDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _zone(nullptr)
{
  construct();
}

void
ZoneDialog::construct() {
  setupUi(this);

  if (_zone) {
    zoneName->setText(_zone->name());
    for (int i=0; i<_zone->rowCount(QModelIndex()); i++) {
      Channel *channel = _zone->channel(i);
      if (channel->is<AnalogChannel>()) {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelList->addItem(item);
      } else {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelList->addItem(item);
      }
    }
  }

  connect(addChannel, SIGNAL(clicked()), this, SLOT(onAddChannel()));
  connect(remChannel, SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(channelUp, SIGNAL(clicked()), this, SLOT(onChannelUp()));
  connect(channelDown, SIGNAL(clicked()), this, SLOT(onChannelDown()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void
ZoneDialog::onAddChannel() {
  QStringList names;
  QVector<Channel *> channels;
  ChannelList *lst = _config->channelList();
  for (int i=0; i<lst->rowCount(QModelIndex()); i++) {
    Channel *channel = lst->channel(i);
    if (channel->is<AnalogChannel>())
      names.append(tr("%1 (Analog)").arg(channel->name()));
    else
      names.append(tr("%1 (Digital)").arg(channel->name()));
    channels.append(lst->channel(i));
  }

  bool ok=false;
  QString name  = QInputDialog::getItem(0, tr("Select channel"),
                                        tr("Select a channel to add:"), names, 0, false, &ok);
  if ((! ok) || (! names.contains(name)))
    return;

  Channel *channel = channels[names.indexOf(name)];
  if (channel->is<AnalogChannel>()) {
    QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
    item->setData(Qt::UserRole, QVariant::fromValue(channel));
    channelList->addItem(item);
  } else {
    QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
    item->setData(Qt::UserRole, QVariant::fromValue(channel));
    channelList->addItem(item);
  }
}

void
ZoneDialog::onRemChannel() {
  if (0 == channelList->selectedItems().size())
    return;
  QListWidgetItem *item = channelList->takeItem(channelList->currentRow());
  delete item;
}

void
ZoneDialog::onChannelUp() {
  if (0 == channelList->selectedItems().size())
    return;
  int idx = channelList->currentRow();
  if (0 == idx)
    return;
  QListWidgetItem *item = channelList->takeItem(idx);
  channelList->insertItem(idx-1, item);
  channelList->setCurrentRow(idx-1);
}

void
ZoneDialog::onChannelDown() {
  if (0 == channelList->selectedItems().size())
    return;
  int idx = channelList->currentRow();
  if ((channelList->count()-1) <= idx)
    return;
  QListWidgetItem *item = channelList->takeItem(idx);
  channelList->insertItem(idx+1, item);
  channelList->setCurrentRow(idx+1);
}

Zone *
ZoneDialog::zone() {
  if (_zone) {
    _zone->setName(zoneName->text().simplified());
    _zone->clear();
    for (int i=0; i<channelList->count(); i++)
      _zone->addChannel(channelList->item(i)->data(Qt::UserRole).value<Channel*>());
    return _zone;
  }
  Zone *zone = new Zone(zoneName->text(), this);
  for (int i=0; i<channelList->count(); i++)
    zone->addChannel(channelList->item(i)->data(Qt::UserRole).value<Channel *>());
  return zone;
}
