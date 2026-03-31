#include "channellistview.hh"
#include "ui_channellistview.h"

#include "fmchanneldialog.hh"
#include "amchanneldialog.hh"
#include "dmrchanneldialog.hh"
#include "config.hh"
#include "settings.hh"

#include <QMenu>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>

ChannelListView::ChannelListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::ChannelListView), _config(config)
{
  ui->setupUi(this);

  connect(ui->channelTableView->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadChannelListSectionState()));
  connect(ui->channelTableView->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeChannelListSectionState()));

  ui->channelTableView->setModel(new ChannelListWrapper(_config->channelList(), ui->channelTableView));

  auto menu = new QMenu();
  menu->addAction(ui->actionAddDMRChannel);
  menu->addAction(ui->actionAddFMChannel);
  menu->addAction(ui->actionAddAMChannel);
  ui->addChannel->setMenu(menu);

  connect(ui->actionAddFMChannel, &QAction::triggered, this, &ChannelListView::onAddFMChannel);
  connect(ui->actionAddAMChannel, &QAction::triggered, this, &ChannelListView::onAddAMChannel);
  connect(ui->actionAddDMRChannel, &QAction::triggered, this, &ChannelListView::onAddDMRChannel);
  connect(ui->cloneChannel, SIGNAL(clicked()), this, SLOT(onCloneChannel()));
  connect(ui->remChannel, SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(ui->channelTableView, SIGNAL(doubleClicked(unsigned)), this, SLOT(onEditChannel(unsigned)));
}

ChannelListView::~ChannelListView() {
  delete ui;
}


void
ChannelListView::onAddFMChannel() {
  FMChannelDialog dialog(_config, parentWidget());
  auto newCh = new FMChannel();
  dialog.setChannel(newCh);
  if (QDialog::Accepted != dialog.exec()) {
    delete newCh;
    return;
  }

  int row=-1;
  if (ui->channelTableView->hasSelection())
    row = ui->channelTableView->selection().second+1;
  _config->channelList()->add(newCh, row);
}

void
ChannelListView::onAddAMChannel() {
  AMChannelDialog dialog(_config, parentWidget());
  auto newCh = new AMChannel();
  dialog.setChannel(newCh);

  if (QDialog::Accepted != dialog.exec()) {
    delete newCh;
    return;
  }

  int row=-1;
  if (ui->channelTableView->hasSelection())
    row = ui->channelTableView->selection().second+1;
  _config->channelList()->add(newCh, row);
}

void
ChannelListView::onAddDMRChannel() {
  DMRChannelDialog dialog(_config, parentWidget());
  auto newCh = new DMRChannel();
  dialog.setChannel(newCh);
  if (QDialog::Accepted != dialog.exec()) {
    delete newCh;
    return;
  }

  int row=-1;
  if (ui->channelTableView->hasSelection())
    row = ui->channelTableView->selection().second+1;
  _config->channelList()->add(newCh, row);
}

void
ChannelListView::onCloneChannel() {
  // get selection
  int row = ui->channelTableView->selection().first;
  if ((! ui->channelTableView->hasSelection()) || (row != ui->channelTableView->selection().second)) {
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
    FMChannelDialog dialog(_config);
    dialog.setChannel(clone);
    if (QDialog::Accepted != dialog.exec()) {
      // if rejected -> destroy clone
      clone->deleteLater();
      return;
    }
    // add to list (below selected one)
    _config->channelList()->add(clone, row+1);
  } else if (channel->is<AMChannel>()) {
    // clone channel
    auto clone = channel->clone()->as<AMChannel>();
    // open editor
    AMChannelDialog dialog(_config);
    dialog.setChannel(clone);
    if (QDialog::Accepted != dialog.exec()) {
      // if rejected -> destroy clone
      clone->deleteLater();
      return;
    }
    // add to list (below selected one)
    _config->channelList()->add(clone, row+1);
  } else if (channel->is<DMRChannel>()) {
    // clone channel
    DMRChannel *clone = channel->clone()->as<DMRChannel>();
    // open editor
    DMRChannelDialog dialog(_config);
    dialog.setChannel(clone);
    if (QDialog::Accepted != dialog.exec()) {
      clone->deleteLater();
      return;
    }
    // add to list (below selected one)
    _config->channelList()->add(clone, row+1);
  }
}

void
ChannelListView::onRemChannel() {
  if (! ui->channelTableView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete channel"),
          tr("Cannot delete channel: You have to select a channel first."));
    return;
  }

  // Get selection and ask for deletion
  QPair<int,int> rows = ui->channelTableView->selection();
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
    FMChannelDialog dialog(_config);
    dialog.setChannel(channel->as<FMChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
  } else if (channel->is<DMRChannel>()) {
    DMRChannelDialog dialog(_config);
    dialog.setChannel(channel->as<DMRChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
  } else if (channel->is<AMChannel>()) {
    AMChannelDialog dialog(_config);
    dialog.setChannel(channel->as<AMChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
  }
}

void
ChannelListView::loadChannelListSectionState() {
  Settings settings;
  ui->channelTableView->header()->restoreState(settings.headerState("channelList"));
}
void
ChannelListView::storeChannelListSectionState() {
  Settings settings;
  settings.setHeaderState("channelList", ui->channelTableView->header()->saveState());
}

