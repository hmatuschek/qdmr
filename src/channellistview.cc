#include "channellistview.hh"
#include "ui_channellistview.h"

#include "analogchanneldialog.hh"
#include "digitalchanneldialog.hh"
#include "config.hh"
#include "settings.hh"

#include <QHeaderView>
#include <QMessageBox>


ChannelListView::ChannelListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::ChannelListView), _config(config)
{
  ui->setupUi(this);

  connect(ui->listView->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadChannelListSectionState()));
  connect(ui->listView->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeChannelListSectionState()));

  ui->listView->setModel(new ChannelListWrapper(_config->channelList(), ui->listView));

  connect(ui->addAnalogChannel, SIGNAL(clicked()), this, SLOT(onAddAnalogChannel()));
  connect(ui->addDigitalChannel, SIGNAL(clicked()), this, SLOT(onAddDigitalChannel()));
  connect(ui->cloneChannel, SIGNAL(clicked()), this, SLOT(onCloneChannel()));
  connect(ui->remChannel, SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(ui->listView, SIGNAL(doubleClicked(uint)), this, SLOT(onEditChannel(uint)));
}

ChannelListView::~ChannelListView() {
  delete ui;
}


void
ChannelListView::onAddAnalogChannel() {
  AnalogChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->channelList()->add(dialog.channel(), row);
}

void
ChannelListView::onAddDigitalChannel() {
  DigitalChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->channelList()->add(dialog.channel(), row);
}

void
ChannelListView::onCloneChannel() {
  // get selection
  int row = ui->listView->selection().first;
  if ((! ui->listView->hasSelection()) || (row != ui->listView->selection().second)) {
    QMessageBox::information(nullptr, tr("Select a single channel first"),
                             tr("To clone a channel, please select a single channel to clone."),
                             QMessageBox::Close);
    return;
  }

  // Get selected channel
  Channel *channel = _config->channelList()->channel(row);
  if (! channel)
    return;

  // Dispatch by type
  if (channel->is<AnalogChannel>()) {
    AnalogChannel *selch = channel->as<AnalogChannel>();
    // clone channel
    AnalogChannel *clone = new AnalogChannel(
          selch->name()+" clone", selch->rxFrequency(), selch->txFrequency(), selch->power(),
          selch->timeout(), selch->rxOnly(), selch->admit(), selch->squelch(),
          selch->rxTone(), selch->txTone(), selch->bandwidth(), selch->scanListObj(),
          selch->aprsSystem());
    // open editor
    AnalogChannelDialog dialog(_config, clone);
    if (QDialog::Accepted != dialog.exec()) {
      // if rejected -> destroy clone
      clone->deleteLater();
      return;
    }
    // update channel
    dialog.channel();
    // add to list (below selected one)
    _config->channelList()->add(clone, row+1);
  } else {
    DigitalChannel *selch = channel->as<DigitalChannel>();
    // clone channel
    DigitalChannel *clone = new DigitalChannel(
          selch->name()+" clone", selch->rxFrequency(), selch->txFrequency(), selch->power(),
          selch->timeout(), selch->rxOnly(), selch->admit(), selch->colorCode(),
          selch->timeSlot(), selch->groupListObj(), selch->txContactObj(), selch->aprsObj(),
          selch->scanListObj(), selch->roamingZone(), nullptr);
    // open editor
    DigitalChannelDialog dialog(_config, clone);
    if (QDialog::Accepted != dialog.exec()) {
      clone->deleteLater();
      return;
    }
    // update channel
    dialog.channel();
    // add to list (below selected one)
    _config->channelList()->add(clone, row+1);
  }
}

void
ChannelListView::onRemChannel() {
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete channel"),
          tr("Cannot delete channel: You have to select a channel first."));
    return;
  }

  // Get selection and ask for deletion
  QPair<int,int> rows = ui->listView->selection();
  int rowcount = rows.second-rows.first+1;
  if (rows.first == rows.second) {
    QString name = _config->channelList()->channel(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete channel?"), tr("Delete channel %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete channel?"), tr("Delete %1 channels?").arg(rowcount)))
      return;
  }

  // collect all selected channels
  // need to collect them first as rows change when deleting channels
  QList<Channel *> channels; channels.reserve(rowcount);
  for(int row=rows.first; row<=rows.second; row++)
    channels.push_back(_config->channelList()->channel(row));
  // remove channels
  foreach (Channel *channel, channels)
    _config->channelList()->del(channel);
}

void
ChannelListView::onEditChannel(uint row) {
  Channel *channel = _config->channelList()->channel(row);
  if (! channel)
    return;
  if (channel->is<AnalogChannel>()) {
    AnalogChannelDialog dialog(_config, channel->as<AnalogChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  } else {
    DigitalChannelDialog dialog(_config, channel->as<DigitalChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  }
}

void
ChannelListView::loadChannelListSectionState() {
  Settings settings;
  ui->listView->header()->restoreState(settings.channelListHeaderState());
}
void
ChannelListView::storeChannelListSectionState() {
  Settings settings;
  settings.setChannelListHeaderState(ui->listView->header()->saveState());
}

