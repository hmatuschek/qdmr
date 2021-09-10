#include "roamingzonedialog.hh"
#include "settings.hh"
#include "config.hh"
#include "channelselectiondialog.hh"


RoamingZoneDialog::RoamingZoneDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _zone(""), _editZone(nullptr)
{
  _zone.clear();
  construct();
}

RoamingZoneDialog::RoamingZoneDialog(Config *config, RoamingZone *zone, QWidget *parent)
  : QDialog(parent), _config(config), _zone(""), _editZone(zone)
{
  if (_editZone)
    _zone = (*_editZone);
  construct();
}


void
RoamingZoneDialog::construct() {
  setupUi(this);
  Settings settings;

  zoneName->setText(_zone.name());
  channelListView->setModel(new ChannelRefListWrapper(_zone.channels(), channelListView));

  connect(add, SIGNAL(clicked(bool)), this, SLOT(onAddChannel()));
  connect(rem, SIGNAL(clicked(bool)), this, SLOT(onRemChannel()));
}


void
RoamingZoneDialog::onAddChannel() {
  MultiChannelSelectionDialog dia(_config->channelList(), false, true);
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> lst = dia.channel();
  foreach (Channel *channel, lst) {
    if (0 <= _zone.channels()->indexOf(channel))
      continue;
    if (channel->is<AnalogChannel>())
      continue;
    _zone.addChannel(channel->as<DigitalChannel>());
  }
}

void
RoamingZoneDialog::onRemChannel() {
  if (! channelListView->hasSelection())
    return;
  QPair<int, int> selection = channelListView->selection();
  QList<DigitalChannel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_zone.channels()->get(i)->as<DigitalChannel>());
  foreach (DigitalChannel *channel, channels) {
    _zone.channels()->del(channel);
  }
}

RoamingZone *
RoamingZoneDialog::zone() {
  RoamingZone *zone = _editZone;
  if (nullptr == zone)
    zone = new RoamingZone(zoneName->text(), this);

  _zone.setName(zoneName->text().simplified());

  (*zone) = _zone;

  return zone;
}
