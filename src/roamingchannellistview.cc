#include "roamingchannellistview.hh"
#include "ui_roamingchannellistview.h"
#include "roamingchanneldialog.hh"
#include "configitemwrapper.hh"
#include "config.hh"
#include "settings.hh"

#include <QMessageBox>

RoamingChannelListView::RoamingChannelListView(Config *config, QWidget *parent) :
  QWidget(parent), _config(config), ui(new Ui::RoamingChannelListView)
{
  Settings settings;

  ui->setupUi(this);

  ui->roamingChannelTableView->setModel(
        new RoamingChannelListWrapper(_config->roamingChannels(), ui->roamingChannelTableView));

  connect(ui->addRoamingChannel, SIGNAL(clicked()), this, SLOT(onAddChannel()));
  connect(ui->remRoamingChannel, SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(ui->roamingChannelTableView, SIGNAL(doubleClicked(unsigned)),
          this, SLOT(onEditChannel(unsigned)));

  connect(ui->roamingChannelListHint, SIGNAL(linkActivated(QString)),
          this, SLOT(onHideRoamingNote()));

  if (settings.hideRoamingNote())
    ui->roamingChannelListHint->setHidden(true);
}

RoamingChannelListView::~RoamingChannelListView() {
  delete ui;
}

void
RoamingChannelListView::onAddChannel() {
  RoamingChannelDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->roamingChannelTableView->hasSelection())
    row = ui->roamingChannelTableView->selection().second+1;
  _config->roamingChannels()->add(dialog.channel(), row);

}

void
RoamingChannelListView::onEditChannel(unsigned int idx) {
  RoamingChannelDialog dialog(_config, _config->roamingChannels()->channel(idx));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.channel();
}

void
RoamingChannelListView::onRemChannel() {
  if (! ui->roamingChannelTableView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete roaming channel"),
          tr("Cannot delete roaming channel: You have to select a channel first."));
    return;
  }

  // Get selection and ask for deletion
  QPair<int, int> rows = ui->roamingChannelTableView->selection();
  int rowcount = rows.second - rows.first + 1;
  if (rows.first==rows.second) {
    QString name = _config->roamingChannels()->channel(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete roaming zone?"), tr("Delete roaming zone %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete roaming zones?"), tr("Delete %1 roaming zones?").arg(rowcount)))
      return;
  }
  // collect all selected channels
  // need to collect them first as rows change when deleting
  QList<RoamingChannel *> lists; lists.reserve(rowcount);
  for (int row=rows.first; row<=rows.second; row++)
    lists.push_back(_config->roamingChannels()->channel(row));
  // remove
  foreach (RoamingChannel *channel, lists)
    _config->roamingChannels()->del(channel);
}

void
RoamingChannelListView::onHideRoamingNote() {
  Settings settings; settings.setHideRoamingNote(true);
  ui->roamingChannelListHint->setVisible(false);
}
