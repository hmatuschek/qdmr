#include "grouplistsview.hh"
#include "ui_grouplistsview.h"
#include "rxgrouplistdialog.hh"
#include "config.hh"

#include <QHeaderView>
#include <QMessageBox>


GroupListsView::GroupListsView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::GroupListsView), _config(config)
{
  ui->setupUi(this);

  ui->listView->setModel(new GroupListsWrapper(_config->rxGroupLists(), ui->listView));

  connect(ui->addRXGroup, SIGNAL(clicked()), this, SLOT(onAddRxGroup()));
  connect(ui->remRXGroup, SIGNAL(clicked()), this, SLOT(onRemRxGroup()));
  connect(ui->listView, SIGNAL(doubleClicked(unsigned)), this, SLOT(onEditRxGroup(unsigned)));

}

GroupListsView::~GroupListsView() {
  delete ui;
}


void
GroupListsView::onAddRxGroup() {
  RXGroupListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->rxGroupLists()->add(dialog.groupList(), row);
}

void
GroupListsView::onRemRxGroup() {
  // Check if there is any groups selected
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete RX group list"),
          tr("Cannot delete RX group lists: You have to select a group list first."));
    return;
  }
  // Get selection and ask for deletion
  QPair<int,int> rows = ui->listView->selection();
  int rowcount = rows.second-rows.first+1;
  if (rows.first == rows.second) {
    QString name = _config->rxGroupLists()->list(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete RX group list?"), tr("Delete RX group list %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete RX group list?"), tr("Delete %1 RX group lists?").arg(rowcount)))
      return;
  }
  // collect all selected group lists
  // need to collect them first as rows change when deleting
  QList<RXGroupList *> lists; lists.reserve(rowcount);
  for (int row=rows.first; row<=rows.second; row++)
    lists.push_back(_config->rxGroupLists()->list(row));
  // remove list
  foreach (RXGroupList *list, lists)
    _config->rxGroupLists()->del(list);
}

void
GroupListsView::onEditRxGroup(unsigned row) {
  RXGroupListDialog dialog(_config, _config->rxGroupLists()->list(row));
  if (QDialog::Accepted != dialog.exec())
    return;
  dialog.groupList();
}

