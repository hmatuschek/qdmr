#include "roamingzonedialog.hh"
#include "settings.hh"
#include "config.hh"
#include "channelselectiondialog.hh"


RoamingZoneDialog::RoamingZoneDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _zone(nullptr)
{
  construct();
}

RoamingZoneDialog::RoamingZoneDialog(Config *config, RoamingZone *zone, QWidget *parent)
  : QDialog(parent), _config(config), _zone(zone)
{
  construct();
}


void
RoamingZoneDialog::construct() {
  setupUi(this);
  Settings settings;

  if (_zone) {
    zoneName->setText(_zone->name());
    for (int i=0; i<_zone->count(); i++) {
      DigitalChannel *channel = _zone->channel(i);
      QListWidgetItem *item = new QListWidgetItem(channel->name());
      item->setData(Qt::UserRole, QVariant::fromValue(channel));
      channels->addItem(item);
    }
  }

  connect(add, SIGNAL(clicked(bool)), this, SLOT(onAddChannel()));
  connect(rem, SIGNAL(clicked(bool)), this, SLOT(onRemChannel()));
  connect(up, SIGNAL(clicked(bool)), this, SLOT(onChannelUp()));
  connect(down, SIGNAL(clicked(bool)), this, SLOT(onChannelDown()));
}


void
RoamingZoneDialog::onAddChannel() {
  MultiChannelSelectionDialog dia(_config->channelList(), false, true);
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> lst = dia.channel();
  foreach (Channel *channel, lst) {
    if (channels->findItems(channel->name(), Qt::MatchExactly).size())
      continue;
    if (channel->is<AnalogChannel>())
      continue;
    QListWidgetItem *item = new QListWidgetItem(channel->name());
    item->setData(Qt::UserRole, QVariant::fromValue(channel->as<DigitalChannel>()));
    channels->addItem(item);
  }
}

void
RoamingZoneDialog::onRemChannel() {
  QList<QListWidgetItem*> selection = channels->selectedItems();
  // Nothing is selected -> done.
  if (0 == selection.size())
    return;
  // Remove every selected item from list
  foreach(QListWidgetItem *item, selection) {
    channels->takeItem(channels->row(item));
    delete item;
  }
}

void
RoamingZoneDialog::onChannelUp() {
  if (1 != channels->selectedItems().size())
    return;
  int idx = channels->currentRow();
  if (0 == idx)
    return;
  QListWidgetItem *item = channels->takeItem(idx);
  channels->insertItem(idx-1, item);
  channels->setCurrentRow(idx-1);
}

void
RoamingZoneDialog::onChannelDown() {
  if (1 != channels->selectedItems().size())
    return;
  int idx = channels->currentRow();
  if ((channels->count()-1) <= idx)
    return;
  QListWidgetItem *item = channels->takeItem(idx);
  channels->insertItem(idx+1, item);
  channels->setCurrentRow(idx+1);
}


RoamingZone *
RoamingZoneDialog::zone() {
  if (_zone) {
    _zone->setName(zoneName->text().simplified());
    _zone->clear();
    for (int i=0; i<channels->count(); i++)
      _zone->addChannel(channels->item(i)->data(Qt::UserRole).value<DigitalChannel*>());
    return _zone;
  }
  RoamingZone *zone = new RoamingZone(zoneName->text(), this);
  for (int i=0; i<channels->count(); i++)
    zone->addChannel(channels->item(i)->data(Qt::UserRole).value<DigitalChannel *>());
  return zone;
}
