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
  _name = name.simplified();
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
 * Implementation of ZoneListView
 * ********************************************************************************************* */
ZoneListView::ZoneListView(Config *config, QWidget *parent)
  : QWidget(parent), _config(config)
{
  _view = new QListView();
  _view->setModel(_config->zones());
  connect(_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditZone(const QModelIndex &)));

  QPushButton *add = new QPushButton(tr("Add Zone"));
  QPushButton *rem = new QPushButton(tr("Delete Zone"));
  connect(add, SIGNAL(clicked()), this, SLOT(onAddZone()));
  connect(rem, SIGNAL(clicked()), this, SLOT(onRemZone()));

  QHBoxLayout *bbox = new QHBoxLayout();
  bbox->addWidget(add);
  bbox->addWidget(rem);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_view);
  layout->addLayout(bbox);

  setLayout(layout);
}

void
ZoneListView::onAddZone() {
  ZoneDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  Zone *zone = dialog.zone();
  _config->zones()->addZone(zone);
}

void
ZoneListView::onRemZone() {
  QModelIndex idx = _view->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(nullptr, tr("Cannot delete zone"),
                             tr("Cannot delete zone: You have to select a zone first."));
    return;
  }

  QString name = _config->zones()->zone(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(nullptr, tr("Delete zone?"), tr("Delete zone %1?").arg(name)))
    return;

  _config->zones()->remZone(idx.row());
}

void
ZoneListView::onEditZone(const QModelIndex &idx) {
  if (idx.row()>=_config->zones()->rowCount(QModelIndex()))
    return;

  ZoneDialog dialog(_config, _config->zones()->zone(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.zone();

  emit _view->model()->dataChanged(idx,idx);
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
  QLabel *label = new QLabel(tr("Name"));
  _name = new QLineEdit("name");
  if (_zone)
    _name->setText(_zone->name());
  label->setBuddy(_name);

  _list = new QListWidget();
  if (_zone) {
    for (int i=0; i<_zone->rowCount(QModelIndex()); i++) {
      Channel *channel = _zone->channel(i);
      if (channel->is<AnalogChannel>()) {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        _list->addItem(item);
      } else {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        _list->addItem(item);
      }
    }
  }

  QPushButton *add = new QPushButton(tr("Add Channel"));
  QPushButton *rem = new QPushButton(tr("Remove Channel"));
  connect(add, SIGNAL(clicked()), this, SLOT(onAddChannel()));
  connect(rem, SIGNAL(clicked()), this, SLOT(onRemChannel()));

  QPushButton *up = new QPushButton(QIcon("://icons/up.png"), "");
  up->setToolTip(tr("Up"));
  QPushButton *down = new QPushButton(QIcon("://icons/down.png"),"");
  down->setToolTip(tr("Down"));
  connect(up, SIGNAL(clicked()), this, SLOT(onChannelUp()));
  connect(down, SIGNAL(clicked()), this, SLOT(onChannelDown()));

  QVBoxLayout *layout = new QVBoxLayout();

  QHBoxLayout *nb = new QHBoxLayout();
  nb->addWidget(label);
  nb->addWidget(_name);
  layout->addLayout(nb);

  QVBoxLayout *ud = new QVBoxLayout();
  ud->addWidget(up); ud->addWidget(down);
  QHBoxLayout *lb = new QHBoxLayout();
  lb->addWidget(_list, 1);
  lb->addLayout(ud);
  layout->addLayout(lb);

  QHBoxLayout *bbox = new QHBoxLayout();
  bbox->addWidget(add);
  bbox->addWidget(rem);
  layout->addLayout(bbox);

  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
  connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(bb);

  setLayout(layout);
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
    _list->addItem(item);
  } else {
    QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
    item->setData(Qt::UserRole, QVariant::fromValue(channel));
    _list->addItem(item);
  }
}

void
ZoneDialog::onRemChannel() {
  if (0 == _list->selectedItems().size())
    return;
  QListWidgetItem *item = _list->takeItem(_list->currentRow());
  delete item;
}

void
ZoneDialog::onChannelUp() {
  if (0 == _list->selectedItems().size())
    return;
  int idx = _list->currentRow();
  if (0 == idx)
    return;
  QListWidgetItem *item = _list->takeItem(idx);
  _list->insertItem(idx-1, item);
  _list->setCurrentRow(idx-1);
}

void
ZoneDialog::onChannelDown() {
  if (0 == _list->selectedItems().size())
    return;
  int idx = _list->currentRow();
  if ((_list->count()-1) == idx)
    return;
  QListWidgetItem *item = _list->takeItem(idx);
  _list->insertItem(idx+1, item);
  _list->setCurrentRow(idx+1);
}

Zone *
ZoneDialog::zone() {
  if (_zone) {
    _zone->setName(_name->text().simplified());
    _zone->clear();
    for (int i=0; i<_list->count(); i++)
      _zone->addChannel(_list->item(i)->data(Qt::UserRole).value<Channel*>());
    return _zone;
  }
  Zone *zone = new Zone(_name->text(), this);
  for (int i=0; i<_list->count(); i++)
    zone->addChannel(_list->item(i)->data(Qt::UserRole).value<Channel *>());
  return zone;
}
