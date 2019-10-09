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
  ChannelSelectionDialog dia(_config->channelList());
  if (QDialog::Accepted != dia.exec())
    return;

  Channel *channel = dia.channel();
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
