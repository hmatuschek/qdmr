#include "scanlistdialog.hh"
#include "config.hh"
#include "channelselectiondialog.hh"



/* ********************************************************************************************* *
 * Implementation of ScanListDialog
 * ********************************************************************************************* */
ScanListDialog::ScanListDialog(Config *config, ScanList *scanlist, QWidget *parent)
  : QDialog(parent), _config(config), _scanList(""), _editScanlist(scanlist)
{
  if (_editScanlist)
    _scanList = *_editScanlist;
  construct();
}

ScanListDialog::ScanListDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _scanList(""), _editScanlist(nullptr)
{
  _scanList.clear();
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

  scanListName->setText(_scanList.name());
  // set priority channel
  if (_scanList.primaryChannel())
    priorityChannel1->setCurrentIndex(_config->channelList()->indexOf(_scanList.primaryChannel())+2);
  // set secondary priority channel
  if (_scanList.secondaryChannel())
    priorityChannel2->setCurrentIndex(_config->channelList()->indexOf(_scanList.secondaryChannel())+2);
  if (_scanList.revertChannel())
    transmitChannel->setCurrentIndex(_config->channelList()->indexOf(_scanList.revertChannel())+2);

  channelListView->setModel(new ChannelRefListWrapper(_scanList.channels(), channelListView));

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
    if (0 <= _scanList.channels()->indexOf(channel))
      continue;
    _scanList.channels()->add(channel);
  }
}

void
ScanListDialog::onRemChannel() {
  if (! channelListView->hasSelection())
    return;

  QPair<int,int> selection = channelListView->selection();
  QList<Channel *> channels;
  for (int i=selection.first; i<=selection.second; i++)
    channels.push_back(_scanList.channels()->get(i)->as<Channel>());
  foreach (Channel *channel, channels)
    _scanList.channels()->del(channel);
}


ScanList *
ScanListDialog::scanlist() {
  ScanList *scanlist = _editScanlist;
  if (! scanlist)
    scanlist = new ScanList(scanListName->text().simplified(), this);

  _scanList.setName(scanListName->text().simplified());
  // Set priority and transmit channels
  _scanList.setPrimaryChannel(priorityChannel1->currentData(Qt::UserRole).value<Channel *>());
  _scanList.setSecondaryChannel(priorityChannel2->currentData(Qt::UserRole).value<Channel *>());
  _scanList.setRevertChannel(transmitChannel->currentData(Qt::UserRole).value<Channel *>());

  (*scanlist) = _scanList;

  return scanlist;
}

