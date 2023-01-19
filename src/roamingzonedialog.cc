#include "roamingzonedialog.hh"
#include "settings.hh"
#include "config.hh"
#include "channelselectiondialog.hh"
#include "roamingchannelselectiondialog.hh"
#include <QMessageBox>


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
  channelListView->setModel(new RoamingChannelRefListWrapper(_myZone->channels(), channelListView));

  extensionView->setObjectName("roamingZoneExtension");
  extensionView->setObject(_myZone, _config);
  if (! settings.showExtensions())
    tabWidget->tabBar()->hide();

  connect(addRoam, SIGNAL(clicked(bool)), this, SLOT(onAddRoamingChannel()));
  connect(addDMR, SIGNAL(clicked(bool)), this, SLOT(onAddDMRChannel()));
  connect(rem, SIGNAL(clicked(bool)), this, SLOT(onRemChannel()));
}


void
RoamingZoneDialog::onAddDMRChannel() {
  MultiChannelSelectionDialog dia(_config->channelList(), false, true);
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> lst = dia.channel();
  foreach (Channel *channel, lst) {
    if (0 <= _myZone->channels()->indexOf(channel))
      continue;
    if (! channel->is<DMRChannel>())
      continue;
    RoamingChannel *rch = RoamingChannel::fromDMRChannel(channel->as<DMRChannel>());
    _config->roamingChannels()->add(rch);
    _myZone->addChannel(rch);
  }
}

void
RoamingZoneDialog::onAddRoamingChannel() {
  MultiRoamingChannelSelectionDialog dia(_config);
  if (QDialog::Accepted != dia.exec())
    return;

  QList<RoamingChannel *> lst = dia.channels();
  foreach (RoamingChannel *channel, lst) {
    if (0 <= _myZone->channels()->indexOf(channel))
      continue;
    _myZone->addChannel(channel);
  }
}

void
RoamingZoneDialog::onRemChannel() {
  if (! channelListView->hasSelection()) {
    QMessageBox::information(nullptr, tr("Cannot remove channels."),
                             tr("Cannot remove channels. Select at least one channel first."),
                             QMessageBox::Close, QMessageBox::Close);
    return;
  }

  QPair<int, int> selection = channelListView->selection();
  QList<RoamingChannel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_myZone->channels()->get(i)->as<RoamingChannel>());
  foreach (RoamingChannel *channel, channels) {
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
