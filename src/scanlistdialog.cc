#include "scanlistdialog.hh"
#include "config.hh"
#include "channelselectiondialog.hh"



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
  setupUi(this);

  priorityChannel1->addItem(tr("[None]"), QVariant::fromValue(nullptr));
  priorityChannel1->addItem(tr("[Selected]"), QVariant::fromValue(SelectedChannel::get()));
  priorityChannel2->addItem(tr("[None]"), QVariant::fromValue(nullptr));
  priorityChannel2->addItem(tr("[Selected]"), QVariant::fromValue(SelectedChannel::get()));
  transmitChannel->addItem(tr("[Last]"), QVariant::fromValue(nullptr));
  transmitChannel->addItem(tr("[Selected]"), QVariant::fromValue(SelectedChannel::get()));

  for (int i=0; i<_config->channelList()->count(); i++) {
    Channel *channel = _config->channelList()->channel(i);
    priorityChannel1->addItem(channel->name(), QVariant::fromValue(channel));
    priorityChannel2->addItem(channel->name(), QVariant::fromValue(channel));
    transmitChannel->addItem(channel->name(), QVariant::fromValue(channel));
  }

  if (_scanlist) {
    // set name
    scanListName->setText(_scanlist->name());
    // set priority channel
    if (_scanlist->priorityChannel())
      priorityChannel1->setCurrentIndex(_config->channelList()->indexOf(_scanlist->priorityChannel())+2);
    // set secondary priority channel
    if (_scanlist->secPriorityChannel())
      priorityChannel2->setCurrentIndex(_config->channelList()->indexOf(_scanlist->secPriorityChannel())+2);
    if (_scanlist->txChannel())
      transmitChannel->setCurrentIndex(_config->channelList()->indexOf(_scanlist->txChannel())+2);
    // fill channel list
    for (int i=0; i<_scanlist->count(); i++) {
      Channel *channel = _scanlist->channel(i);
      if (channel->is<AnalogChannel>()) {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelList->addItem(item);
      } else if(channel->is<DigitalChannel>()) {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelList->addItem(item);
      } else if (SelectedChannel::get() == channel) {
        QListWidgetItem *item = new QListWidgetItem(tr("[Selected Channel]"));
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
ScanListDialog::onAddChannel() {
  MultiChannelSelectionDialog dia(_config->channelList(), true);
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> channels = dia.channel();
  foreach (Channel *channel, channels) {
    if (channel->is<AnalogChannel>()) {
      QString name = tr("%1 (Analog)").arg(channel->name());
      if (channelList->findItems(name, Qt::MatchExactly).size())
        continue;
      QListWidgetItem *item = new QListWidgetItem(name);
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channelList->addItem(item);
    } else if (channel->is<DigitalChannel>()) {
      QString name = tr("%1 (Digital)").arg(channel->name());
      if (channelList->findItems(name, Qt::MatchExactly).size())
        continue;
      QListWidgetItem *item = new QListWidgetItem(name);
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channelList->addItem(item);
    } else if (SelectedChannel::get() == channel) {
      QString name = tr("[Selected Channel]");
      if (channelList->findItems(name, Qt::MatchExactly).size())
        continue;
      QListWidgetItem *item = new QListWidgetItem(name);
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channelList->addItem(item);
    }
  }
}

void
ScanListDialog::onRemChannel() {
  QList<QListWidgetItem *> selection = channelList->selectedItems();
  if (selection.isEmpty())
    return;

  foreach (QListWidgetItem *item, selection) {
    int row = channelList->row(item);
    channelList->takeItem(row);
    delete item;
  }
}

void
ScanListDialog::onChannelUp() {
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
ScanListDialog::onChannelDown() {
  if (0 == channelList->selectedItems().size())
    return;
  int idx = channelList->currentRow();
  if ((channelList->count()-1) == idx)
    return;
  QListWidgetItem *item = channelList->takeItem(idx);
  channelList->insertItem(idx+1, item);
  channelList->setCurrentRow(idx+1);
}

ScanList *
ScanListDialog::scanlist() {
  ScanList *scanlist = _scanlist;
  if (! scanlist) {
    scanlist = new ScanList(scanListName->text().simplified(), this);
  } else {
    scanlist->clear();
    scanlist->setName(scanListName->text().simplified());
  }

  for (int i=0; i<channelList->count(); i++)
    scanlist->addChannel(channelList->item(i)->data(Qt::UserRole).value<Channel*>());

  // Set priority and transmit channels
  scanlist->setPriorityChannel(priorityChannel1->currentData(Qt::UserRole).value<Channel *>());
  scanlist->setSecPriorityChannel(priorityChannel2->currentData(Qt::UserRole).value<Channel *>());
  scanlist->setTXChannel(transmitChannel->currentData(Qt::UserRole).value<Channel *>());

  return scanlist;
}

