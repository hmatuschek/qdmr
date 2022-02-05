#include "roamingzonedialog.hh"
#include "settings.hh"
#include "config.hh"
#include "channelselectiondialog.hh"


RoamingZoneDialog::RoamingZoneDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myZone(new RoamingZone(this)), _zone(nullptr)
{
  setWindowTitle(tr("Create Roaming Zone"));
  construct();
}

RoamingZoneDialog::RoamingZoneDialog(Config *config, RoamingZone *zone, QWidget *parent)
  : QDialog(parent), _config(config), _myZone(new RoamingZone(this)), _zone(zone)
{
  setWindowTitle(tr("Set Roaming Zone"));

  if (_zone)
    _myZone->copy(*_zone);

  construct();
}


void
RoamingZoneDialog::construct() {
  setupUi(this);
  Settings settings;

  zoneName->setText(_myZone->name());
  channelListView->setModel(new ChannelRefListWrapper(_myZone->channels(), channelListView));

  extensionView->setObjectName("roamingZoneExtension");
  extensionView->setObject(_myZone);
  if (! settings.showExtensions())
    tabWidget->tabBar()->hide();

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
    if (0 <= _myZone->channels()->indexOf(channel))
      continue;
    if (channel->is<AnalogChannel>())
      continue;
    _myZone->addChannel(channel->as<DigitalChannel>());
  }
}

void
RoamingZoneDialog::onRemChannel() {
  if (! channelListView->hasSelection())
    return;
  QPair<int, int> selection = channelListView->selection();
  QList<DigitalChannel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_myZone->channels()->get(i)->as<DigitalChannel>());
  foreach (DigitalChannel *channel, channels) {
    _myZone->channels()->del(channel);
  }
}

RoamingZone *
RoamingZoneDialog::zone() {
  _myZone->setName(zoneName->text().simplified());

  RoamingZone *zone = _myZone;
  if (_zone) {
    _zone->copy(*_myZone);
    zone = _zone;
  } else {
    _myZone->setParent(nullptr);
  }

  return zone;
}
