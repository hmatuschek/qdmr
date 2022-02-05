#include "scanlistdialog.hh"
#include "config.hh"
#include "channelselectiondialog.hh"
#include "settings.hh"


/* ********************************************************************************************* *
 * Implementation of ScanListDialog
 * ********************************************************************************************* */
ScanListDialog::ScanListDialog(Config *config, ScanList *scanlist, QWidget *parent)
  : QDialog(parent), _config(config), _myScanList(new ScanList(this)), _scanlist(scanlist)
{
  setWindowTitle(tr("Edit Scan List"));

  if (_scanlist)
    _myScanList->copy(*_scanlist);

  construct();
}

ScanListDialog::ScanListDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myScanList(new ScanList(this)), _scanlist(nullptr)
{
  setWindowTitle(tr("Create Scan List"));

  construct();
}

void
ScanListDialog::construct() {
  setupUi(this);
  Settings settings;

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

  scanListName->setText(_myScanList->name());
  // set priority channel
  if (_myScanList->primaryChannel())
    priorityChannel1->setCurrentIndex(_config->channelList()->indexOf(_myScanList->primaryChannel())+2);
  // set secondary priority channel
  if (_myScanList->secondaryChannel())
    priorityChannel2->setCurrentIndex(_config->channelList()->indexOf(_myScanList->secondaryChannel())+2);
  if (_myScanList->revertChannel())
    transmitChannel->setCurrentIndex(_config->channelList()->indexOf(_myScanList->revertChannel())+2);

  channelListView->setModel(new ChannelRefListWrapper(_myScanList->channels(), channelListView));

  extensionView->setObjectName("scanListExtension");
  extensionView->setObject(_myScanList);
  if (! settings.showExtensions())
    tabWidget->tabBar()->hide();

  connect(addChannel, SIGNAL(clicked()), this, SLOT(onAddChannel()));
  connect(remChannel, SIGNAL(clicked()), this, SLOT(onRemChannel()));
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
    if (0 <= _myScanList->channels()->indexOf(channel))
      continue;
    _myScanList->channels()->add(channel);
  }
}

void
ScanListDialog::onRemChannel() {
  if (! channelListView->hasSelection())
    return;

  QPair<int,int> selection = channelListView->selection();
  QList<Channel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_myScanList->channels()->get(i)->as<Channel>());
  foreach (Channel *channel, channels)
    _myScanList->channels()->del(channel);
}


ScanList *
ScanListDialog::scanlist() {
  _myScanList->setName(scanListName->text().simplified());
  // Set priority and transmit channels
  _myScanList->setPrimaryChannel(priorityChannel1->currentData(Qt::UserRole).value<Channel *>());
  _myScanList->setSecondaryChannel(priorityChannel2->currentData(Qt::UserRole).value<Channel *>());
  _myScanList->setRevertChannel(transmitChannel->currentData(Qt::UserRole).value<Channel *>());

  ScanList *scanlist = _myScanList;
  if (_scanlist) {
    _scanlist->copy(*_myScanList);
    scanlist = _scanlist;
  } else {
    _myScanList->setParent(nullptr);
  }

  return scanlist;
}
