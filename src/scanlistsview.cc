#include "scanlistsview.hh"
#include "ui_scanlistsview.h"

#include "scanlistdialog.hh"

#include <QMessageBox>



ScanListsView::ScanListsView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::ScanListsView), _config(config)
{
  ui->setupUi(this);

  ui->listView->setModel(new ScanListsWrapper(_config->scanlists(), ui->listView));
  connect(ui->addScanList, SIGNAL(clicked()), this, SLOT(onAddScanList()));
  connect(ui->remScanList, SIGNAL(clicked()), this, SLOT(onRemScanList()));
  connect(ui->listView, SIGNAL(doubleClicked(uint)), this, SLOT(onEditScanList(uint)));
}

ScanListsView::~ScanListsView() {
  delete ui;
}

void
ScanListsView::onAddScanList() {
  ScanListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->scanlists()->add(dialog.scanlist(), row);
}

void
ScanListsView::onRemScanList() {
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete scanlist"),
          tr("Cannot delete scanlist: You have to select a scanlist first."));
    return;
  }

  // Get selection and ask for deletion
  QPair<int,int> rows = ui->listView->selection();
  int rowcount = rows.second-rows.first+1;
  if (rows.first == rows.second) {
    QString name = _config->scanlists()->scanlist(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete scan list?"), tr("Delete scan list %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete scan lists?"), tr("Delete %1 scan lists?").arg(rowcount)))
      return;
  }

  // collect all selected scan lists
  // need to collect them first as rows change when deleting
  QList<ScanList *> lists; lists.reserve(rowcount);
  for (int row=rows.first; row<=rows.second; row++)
    lists.push_back(_config->scanlists()->scanlist(row));
  // remove
  foreach (ScanList *list, lists)
    _config->scanlists()->del(list);
}

void
ScanListsView::onEditScanList(uint row) {
  ScanListDialog dialog(_config, _config->scanlists()->scanlist(row));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.scanlist();
}


