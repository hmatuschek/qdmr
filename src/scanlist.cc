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
  : QAbstractListModel(parent), _name(name), _channels()
{
  // pass...
}

int
ScanList::count() const {
  return _channels.size();
}

void
ScanList::clear() {
  beginResetModel();
  _channels.clear();
  endResetModel();
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

Channel *
ScanList::channel(int idx) const {
  if ((0>idx) || (idx>=_channels.size()))
    return nullptr;
  return _channels[idx];
}

bool
ScanList::addChannel(Channel *channel) {
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
ScanList::remChannel(int idx) {
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
ScanList::remChannel(Channel *channel) {
  if (! _channels.contains(channel))
    return false;
  int idx = _channels.indexOf(channel);
  return remChannel(idx);
}

int
ScanList::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _channels.size();
}

QVariant
ScanList::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_channels.size()) || (0 != index.column()))
    return QVariant();
  return _channels[index.row()]->name();
}

QVariant
ScanList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Channel");
}

void
ScanList::onChannelDeleted(QObject *obj) {
  if (Channel *channel = reinterpret_cast<Channel *>(obj))
    remChannel(channel);
}


/* ********************************************************************************************* *
 * Implementation of ScanLists
 * ********************************************************************************************* */
ScanLists::ScanLists(QObject *parent)
  : QAbstractListModel(parent), _scanlists()
{
  // pass...
}

int
ScanLists::count() const {
  return _scanlists.size();
}

int
ScanLists::indexOf(ScanList *list) const {
  if (! _scanlists.contains(list))
    return -1;
  return _scanlists.indexOf(list);
}

void
ScanLists::clear() {
  for (int i=0; i<count(); i++)
    _scanlists[i]->deleteLater();
}

ScanList *
ScanLists::scanlist(int idx) const {
  if ((0>idx) || (idx>=_scanlists.size()))
    return nullptr;
  return _scanlists[idx];
}

bool
ScanLists::addScanList(ScanList *list) {
  if (_scanlists.contains(list))
    return false;
  int idx = _scanlists.size();
  beginInsertRows(QModelIndex(), idx, idx);
  list->setParent(this);
  connect(list, SIGNAL(destroyed(QObject*)), this, SLOT(onScanListDeleted(QObject*)));
  connect(list, SIGNAL(modified()), this, SIGNAL(modified()));
  _scanlists.append(list);
  endInsertRows();
  emit modified();
  return true;
}

bool
ScanLists::remScanList(int idx) {
  if ((0>idx) || (idx>=_scanlists.size()))
    return false;
  ScanList *scanlist = _scanlists[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _scanlists.remove(idx);
  scanlist->deleteLater();
  endRemoveRows();
  emit modified();
  return true;
}

bool
ScanLists::remScanList(ScanList *scanlist) {
  if (! _scanlists.contains(scanlist))
    return false;
  int idx = _scanlists.indexOf(scanlist);
  return remScanList(idx);
}

int
ScanLists::rowCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return _scanlists.size();
}

QVariant
ScanLists::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_scanlists.size()) || (0 != index.column()))
    return QVariant();
  return _scanlists[index.row()]->name();
}

QVariant
ScanLists::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Zone");
}

void
ScanLists::onScanListDeleted(QObject *obj) {
  if (ScanList *scanlist = reinterpret_cast<ScanList *>(obj))
    remScanList(scanlist);
}


/* ********************************************************************************************* *
 * Implementation of ScanListView
 * ********************************************************************************************* */
ScanListsView::ScanListsView(Config *config, QWidget *parent)
  : QWidget(parent), _config(config)
{
  _view = new QListView();
  _view->setModel(_config->scanlists());
  connect(_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditScanList(const QModelIndex &)));

  QPushButton *add = new QPushButton(tr("Add Scanlist"));
  QPushButton *rem = new QPushButton(tr("Delete Scanlist"));
  connect(add, SIGNAL(clicked()), this, SLOT(onAddScanList()));
  connect(rem, SIGNAL(clicked()), this, SLOT(onRemScanList()));

  QHBoxLayout *bbox = new QHBoxLayout();
  bbox->addWidget(add);
  bbox->addWidget(rem);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_view);
  layout->addLayout(bbox);

  setLayout(layout);
}

void
ScanListsView::onAddScanList() {
  ScanListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  ScanList *scanlist = dialog.scanlist();
  _config->scanlists()->addScanList(scanlist);
}

void
ScanListsView::onRemScanList() {
  QModelIndex idx = _view->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(nullptr, tr("Cannot delete scanlist"),
                             tr("Cannot delete scanlist: You have to select a scanlist first."));
    return;
  }

  QString name = _config->scanlists()->scanlist(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(nullptr, tr("Delete scanlist?"), tr("Delete scanlist %1?").arg(name)))
    return;

  _config->scanlists()->remScanList(idx.row());
}

void
ScanListsView::onEditScanList(const QModelIndex &idx) {
  if (idx.row()>=_config->scanlists()->count())
    return;

  ScanListDialog dialog(_config, _config->scanlists()->scanlist(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.scanlist();

  emit _view->model()->dataChanged(idx,idx);
}


/* ********************************************************************************************* *
 * Implementation of ScanListDialog
 * ********************************************************************************************* */
ScanListDialog::ScanListDialog(Config *config, ScanList *scanlist, QWidget *parent)
  : QDialog(parent), _config(config), _scanlist(scanlist)
{
  construct();
}

ScanListDialog::ScanListDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _scanlist(nullptr)
{
  construct();
}

void
ScanListDialog::construct() {
  QLabel *label = new QLabel(tr("Name"));
  _name = new QLineEdit("name");
  if (_scanlist)
    _name->setText(_scanlist->name());
  label->setBuddy(_name);

  _list = new QListWidget();
  if (_scanlist) {
    for (int i=0; i<_scanlist->rowCount(QModelIndex()); i++) {
      Channel *channel = _scanlist->channel(i);
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

  QPushButton *up = new QPushButton(QIcon("://icons/up.png"),"");
  up->setToolTip(tr("Up"));
  QPushButton *down = new QPushButton(QIcon("://icons/down.png"), "");
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
ScanListDialog::onAddChannel() {
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
ScanListDialog::onRemChannel() {
  if (0 == _list->selectedItems().size())
    return;
  QListWidgetItem *item = _list->takeItem(_list->currentRow());
  delete item;
}

void
ScanListDialog::onChannelUp() {
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
ScanListDialog::onChannelDown() {
  if (0 == _list->selectedItems().size())
    return;
  int idx = _list->currentRow();
  if ((_list->count()-1) == idx)
    return;
  QListWidgetItem *item = _list->takeItem(idx);
  _list->insertItem(idx+1, item);
  _list->setCurrentRow(idx+1);
}

ScanList *
ScanListDialog::scanlist() {
  if (_scanlist) {
    _scanlist->setName(_name->text().simplified());
    _scanlist->clear();
    for (int i=0; i<_list->count(); i++)
      _scanlist->addChannel(_list->item(i)->data(Qt::UserRole).value<Channel*>());
    return _scanlist;
  }
  ScanList *scanlist = new ScanList(_name->text(), this);
  for (int i=0; i<_list->count(); i++)
    scanlist->addChannel(_list->item(i)->data(Qt::UserRole).value<Channel *>());
  return scanlist;
}

