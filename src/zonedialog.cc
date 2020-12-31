#include "zonedialog.hh"

#include "config.hh"
#include "zone.hh"
#include "channel.hh"
#include "channelselectiondialog.hh"


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
    for (int i=0; i<_zone->A()->count(); i++) {
      Channel *channel = _zone->A()->channel(i);
      if (channel->is<AnalogChannel>()) {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelListA->addItem(item);
      } else {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelListA->addItem(item);
      }
    }
    for (int i=0; i<_zone->B()->count(); i++) {
      Channel *channel = _zone->B()->channel(i);
      if (channel->is<AnalogChannel>()) {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelListB->addItem(item);
      } else {
        QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(channel));
        channelListB->addItem(item);
      }
    }
  }

  connect(addChannelA, SIGNAL(clicked()), this, SLOT(onAddChannelA()));
  connect(remChannelA, SIGNAL(clicked()), this, SLOT(onRemChannelA()));
  connect(channelAUp, SIGNAL(clicked()), this, SLOT(onChannelAUp()));
  connect(channelADown, SIGNAL(clicked()), this, SLOT(onChannelADown()));
  connect(addChannelB, SIGNAL(clicked()), this, SLOT(onAddChannelB()));
  connect(remChannelB, SIGNAL(clicked()), this, SLOT(onRemChannelB()));
  connect(channelBUp, SIGNAL(clicked()), this, SLOT(onChannelBUp()));
  connect(channelBDown, SIGNAL(clicked()), this, SLOT(onChannelBDown()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void
ZoneDialog::onAddChannelA() {
  MultiChannelSelectionDialog dia(_config->channelList());
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> channels = dia.channel();
  foreach (Channel *channel, channels) {
    if (channel->is<AnalogChannel>()) {
      QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channelListA->addItem(item);
    } else {
      QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channelListA->addItem(item);
    }
  }
}

void
ZoneDialog::onRemChannelA() {
  QList<QListWidgetItem*> selection = channelListA->selectedItems();
  // Nothing is selected -> done.
  if (0 == selection.size())
    return;
  // Remove every selected item from list
  foreach(QListWidgetItem *item, selection) {
    channelListA->takeItem(channelListA->row(item));
    delete item;
  }
}

void
ZoneDialog::onChannelAUp() {
  if (1 != channelListA->selectedItems().size())
    return;
  int idx = channelListA->currentRow();
  if (0 == idx)
    return;
  QListWidgetItem *item = channelListA->takeItem(idx);
  channelListA->insertItem(idx-1, item);
  channelListA->setCurrentRow(idx-1);
}

void
ZoneDialog::onChannelADown() {
  if (1 != channelListA->selectedItems().size())
    return;
  int idx = channelListA->currentRow();
  if ((channelListA->count()-1) <= idx)
    return;
  QListWidgetItem *item = channelListA->takeItem(idx);
  channelListA->insertItem(idx+1, item);
  channelListA->setCurrentRow(idx+1);
}

void
ZoneDialog::onAddChannelB() {
  MultiChannelSelectionDialog dia(_config->channelList());
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> channels = dia.channel();
  foreach (Channel *channel, channels) {
    if (channel->is<AnalogChannel>()) {
      QListWidgetItem *item = new QListWidgetItem(tr("%1 (Analog)").arg(channel->name()));
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channelListB->addItem(item);
    } else {
      QListWidgetItem *item = new QListWidgetItem(tr("%1 (Digital)").arg(channel->name()));
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channelListB->addItem(item);
    }
  }
}

void
ZoneDialog::onRemChannelB() {
  QList<QListWidgetItem*> selection = channelListB->selectedItems();
  // Nothing is selected -> done.
  if (0 == selection.size())
    return;
  // Remove every selected item from list
  foreach(QListWidgetItem *item, selection) {
    channelListB->takeItem(channelListB->row(item));
    delete item;
  }
}

void
ZoneDialog::onChannelBUp() {
  if (1 != channelListB->selectedItems().size())
    return;
  int idx = channelListB->currentRow();
  if (0 == idx)
    return;
  QListWidgetItem *item = channelListB->takeItem(idx);
  channelListB->insertItem(idx-1, item);
  channelListB->setCurrentRow(idx-1);
}

void
ZoneDialog::onChannelBDown() {
  if (1 != channelListB->selectedItems().size())
    return;
  int idx = channelListB->currentRow();
  if ((channelListB->count()-1) <= idx)
    return;
  QListWidgetItem *item = channelListB->takeItem(idx);
  channelListB->insertItem(idx+1, item);
  channelListB->setCurrentRow(idx+1);
}

Zone *
ZoneDialog::zone() {
  if (_zone) {
    _zone->setName(zoneName->text().simplified());
    _zone->A()->clear();
    for (int i=0; i<channelListA->count(); i++)
      _zone->A()->addChannel(channelListA->item(i)->data(Qt::UserRole).value<Channel*>());
    _zone->B()->clear();
    for (int i=0; i<channelListB->count(); i++)
      _zone->B()->addChannel(channelListB->item(i)->data(Qt::UserRole).value<Channel*>());
    return _zone;
  }
  Zone *zone = new Zone(zoneName->text(), this);
  for (int i=0; i<channelListA->count(); i++)
    zone->A()->addChannel(channelListA->item(i)->data(Qt::UserRole).value<Channel *>());
  for (int i=0; i<channelListB->count(); i++)
    zone->B()->addChannel(channelListB->item(i)->data(Qt::UserRole).value<Channel *>());
  return zone;
}
