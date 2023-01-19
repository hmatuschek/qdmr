#include "zonedialog.hh"

#include "settings.hh"
#include "config.hh"
#include "zone.hh"
#include "channel.hh"
#include "channelselectiondialog.hh"
#include "settings.hh"
#include <QMessageBox>


/* ********************************************************************************************* *
 * Implementation of ZoneDialog
 * ********************************************************************************************* */
ZoneDialog::ZoneDialog(Config *config, Zone *zone, QWidget *parent)
  : QDialog(parent), _config(config), _myZone(new Zone(this)), _zone(zone)
{
  setWindowTitle(tr("Edit Zone"));
  if (_zone)
    _myZone->copy(*_zone);

  construct();
}

ZoneDialog::ZoneDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myZone(new Zone(this)), _zone(nullptr)
{
  setWindowTitle(tr("Create Zone"));
  construct();
}

void
ZoneDialog::construct() {
  setupUi(this);
  Settings settings;

  if (settings.hideZoneNote())
    zoneHint->setVisible(false);

  zoneName->setText(_myZone->name());
  listAView->setModel(new ChannelRefListWrapper(_myZone->A()));
  listBView->setModel(new ChannelRefListWrapper(_myZone->B()));

  extensionView->setObjectName("zoneExtension");
  extensionView->setObject(_myZone, _config);
  if (! settings.showExtensions()) {
    tabWidget->tabBar()->hide();
  }

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
    if (0 <= _myZone->A()->indexOf(channel))
      continue;
    _myZone->A()->add(channel);
  }
}

void
ZoneDialog::onRemChannelA() {
  if (! listAView->hasSelection()) {
    QMessageBox::information(nullptr, tr("Cannot remove channel"),
                             tr("Select at least one channel first."));
    return;
  }
  QPair<int, int> selection = listAView->selection();
  QList<Channel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_myZone->A()->get(i)->as<Channel>());
  foreach (Channel *channel, channels) {
    _myZone->A()->del(channel);
  }
}


void
ZoneDialog::onAddChannelB() {
  MultiChannelSelectionDialog dia(_config->channelList());
  if (QDialog::Accepted != dia.exec())
    return;

  QList<Channel *> channels = dia.channel();
  foreach (Channel *channel, channels) {
    if (0 <= _myZone->B()->indexOf(channel))
      continue;
    _myZone->B()->add(channel);
  }
}

void
ZoneDialog::onRemChannelB() {
  if (! listBView->hasSelection()) {
    QMessageBox::information(nullptr, tr("Cannot remove channel"),
                             tr("Select at least one channel first."));
    return;
  }
  QPair<int, int> selection = listBView->selection();
  QList<Channel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_myZone->B()->get(i)->as<Channel>());
  foreach (Channel *channel, channels) {
    _myZone->B()->del(channel);
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
  _myZone->setName(zoneName->text());

  Zone *zone = _myZone;
  if (_zone) {
    _zone->copy(*_myZone);
    zone = _zone;
  } else {
    _myZone->setParent(nullptr);
  }

  return zone;
}
