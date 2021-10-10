#include "zonedialog.hh"

#include "settings.hh"
#include "config.hh"
#include "zone.hh"
#include "channel.hh"
#include "channelselectiondialog.hh"


/* ********************************************************************************************* *
 * Implementation of ZoneDialog
 * ********************************************************************************************* */
ZoneDialog::ZoneDialog(Config *config, Zone *zone, QWidget *parent)
  : QDialog(parent), _config(config), _zone(""), _editZone(zone)
{
  if (_editZone)
    _zone = (*_editZone);
  construct();
}

ZoneDialog::ZoneDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _zone(""), _editZone(nullptr)
{
  _zone.clear();
  construct();
}

void
ZoneDialog::construct() {
  setupUi(this);
  Settings settings;

  if (settings.hideZoneNote())
    zoneHint->setVisible(false);

  zoneName->setText(_zone.name());
  listAView->setModel(new ChannelRefListWrapper(_zone.A()));
  listBView->setModel(new ChannelRefListWrapper(_zone.B()));

  connect(addChannelA, SIGNAL(clicked()), this, SLOT(onAddChannelA()));
  connect(remChannelA, SIGNAL(clicked()), this, SLOT(onRemChannelA()));
  connect(addChannelB, SIGNAL(clicked()), this, SLOT(onAddChannelB()));
  connect(remChannelB, SIGNAL(clicked()), this, SLOT(onRemChannelB()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(zoneHint, SIGNAL(linkActivated(QString)), this, SLOT(onHideZoneHint()));
}

void
ZoneDialog::onAddChannelA() {
  MultiChannelSelectionDialog dia(_config->channelList());
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> channels = dia.channel();
  foreach (Channel *channel, channels) {
    if (0 <= _zone.A()->indexOf(channel))
      continue;
    _zone.A()->add(channel);
  }
}

void
ZoneDialog::onRemChannelA() {
  if (! listAView->hasSelection())
    return;
  QPair<int, int> selection = listAView->selection();
  QList<Channel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_zone.A()->get(i)->as<Channel>());
  foreach (Channel *channel, channels) {
    _zone.A()->del(channel);
  }
}


void
ZoneDialog::onAddChannelB() {
  MultiChannelSelectionDialog dia(_config->channelList());
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> channels = dia.channel();
  foreach (Channel *channel, channels) {
    if (0 <= _zone.B()->indexOf(channel))
      continue;
    _zone.B()->add(channel);
  }
}

void
ZoneDialog::onRemChannelB() {
  if (! listAView->hasSelection())
    return;
  QPair<int, int> selection = listBView->selection();
  QList<Channel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_zone.B()->get(i)->as<Channel>());
  foreach (Channel *channel, channels) {
    _zone.B()->del(channel);
  }
}

void
ZoneDialog::onHideZoneHint() {
  zoneHint->setVisible(false);
  Settings settings;
  settings.setHideZoneNote(true);
}

Zone *
ZoneDialog::zone() {
  Zone *zone = _editZone;
  if (nullptr == zone)
    zone = new Zone(zoneName->text(), this);

  _zone.setName(zoneName->text());

  (*zone) = _zone;

  return zone;
}
