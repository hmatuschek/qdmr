#include "channellistview.hh"
#include "ui_channellistview.h"

#include "analogchanneldialog.hh"
#include "digitalchanneldialog.hh"
#include "m17channeldialog.hh"
#include "config.hh"
#include "settings.hh"

#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>

ChannelListView::ChannelListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::ChannelListView), _config(config)
{
  ui->setupUi(this);

  connect(ui->listView->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadChannelListSectionState()));
  connect(ui->listView->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeChannelListSectionState()));

  ui->listView->setModel(new ChannelListWrapper(_config->channelList(), ui->listView));
  QMenu *menu = new QMenu();
  menu->addAction(ui->actionAdd_FM_Channel);
  menu->addAction(ui->actionAdd_DMR_Channel);
  menu->addAction(ui->actionAdd_M17_Channel);
  ui->addChannel->setMenu(menu);

  connect(ui->actionAdd_FM_Channel, SIGNAL(triggered(bool)), this, SLOT(onAddFMChannel()));
  connect(ui->actionAdd_DMR_Channel, SIGNAL(triggered(bool)), this, SLOT(onAddDMRChannel()));
  connect(ui->actionAdd_M17_Channel, SIGNAL(triggered(bool)), this, SLOT(onAddM17Channel()));
  connect(ui->cloneChannel, SIGNAL(clicked()), this, SLOT(onCloneChannel()));
  connect(ui->remChannel, SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(ui->listView, SIGNAL(doubleClicked(unsigned)), this, SLOT(onEditChannel(unsigned)));
}

ChannelListView::~ChannelListView() {
  delete ui;
}


void
ChannelListView::onAddFMChannel() {
  FMChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->channelList()->add(dialog.channel(), row);
}

void
ChannelListView::onAddDMRChannel() {
  DMRChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->channelList()->add(dialog.channel(), row);
}

void
ChannelListView::onAddM17Channel() {
  M17ChannelDialog dialog(_config);
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
  if (channel->is<FMChannel>()) {
    // clone channel
    FMChannel *clone = channel->clone()->as<FMChannel>();
    // open editor
    FMChannelDialog dialog(_config, clone);
    if (QDialog::Accepted != dialog.exec()) {
      // if rejected -> destroy clone
      clone->deleteLater();
      return;
    }
    // update channel
    dialog.channel();
    // add to list (below selected one)
    _config->channelList()->add(clone, row+1);
  } else if (channel->is<DMRChannel>()) {
    // clone channel
    DMRChannel *clone = channel->clone()->as<DMRChannel>();
    // open editor
    DMRChannelDialog dialog(_config, clone);
    if (QDialog::Accepted != dialog.exec()) {
      clone->deleteLater();
      return;
    }
    // update channel
    dialog.channel();
    // add to list (below selected one)
    _config->channelList()->add(clone, row+1);
  } else if (channel->is<M17Channel>()) {
    // clone channel
    M17Channel *clone = new M17Channel(*(channel->as<M17Channel>()));
    // open editor
    M17ChannelDialog dialog(_config, clone);
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
ChannelListView::onEditChannel(unsigned row) {
  Channel *channel = _config->channelList()->channel(row);
  if (! channel)
    return;
  if (channel->is<FMChannel>()) {
    FMChannelDialog dialog(_config, channel->as<FMChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  } else if (channel->is<DMRChannel>()) {
    DMRChannelDialog dialog(_config, channel->as<DMRChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  } else if (channel->is<M17Channel>()) {
    M17ChannelDialog dialog(_config, channel->as<M17Channel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  }
}

void
ChannelListView::loadChannelListSectionState() {
  Settings settings;
  ui->listView->header()->restoreState(settings.headerState("channelList"));
}
void
ChannelListView::storeChannelListSectionState() {
  Settings settings;
  settings.setHeaderState("channelList", ui->listView->header()->saveState());
}

