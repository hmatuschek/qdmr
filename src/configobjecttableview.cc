#include "configobjecttableview.hh"
#include "ui_configobjecttableview.h"
#include "searchpopup.hh"
#include <QMessageBox>


inline QPair<int, int>
getSelectionRowRange(const QModelIndexList &indices) {
  int rmin=-1, rmax=-1;
  foreach (QModelIndex idx, indices) {
    if ((-1==rmin) || (rmin>idx.row()))
      rmin = idx.row();
    if ((-1==rmax) || (rmax<idx.row()))
      rmax = idx.row();
  }
  return QPair<int,int>(rmin, rmax);
}


ConfigObjectTableView::ConfigObjectTableView(QWidget *parent)
  : QWidget(parent), ui(new Ui::ConfigObjectTableView)
{
  ui->setupUi(this);
  connect(ui->itemUp, SIGNAL(clicked(bool)), this, SLOT(onMoveItemUp()));
  connect(ui->itemDown, SIGNAL(clicked(bool)), this, SLOT(onMoveItemDown()));
  connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(onDoubleClicked(QModelIndex)));
  ui->tableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  SearchPopup::attach(ui->tableView);
}

ConfigObjectTableView::~ConfigObjectTableView() {
  delete ui;
}

GenericTableWrapper *
ConfigObjectTableView::model() const {
  return qobject_cast<GenericTableWrapper *>(ui->tableView->model());
}

void
ConfigObjectTableView::setModel(GenericTableWrapper *model) {
  ui->tableView->setModel(model);
}

bool
ConfigObjectTableView::hasSelection() const {
  return ui->tableView->selectionModel()->hasSelection();
}

QPair<int,int>
ConfigObjectTableView::selection() const {
  return getSelectionRowRange(ui->tableView->selectionModel()->selection().indexes());
}

QHeaderView *
ConfigObjectTableView::header() const {
  return ui->tableView->horizontalHeader();
}

void
ConfigObjectTableView::onMoveItemUp() {
  if (! model())
    return;

  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveUp(rows.first, rows.second);
}


void
ConfigObjectTableView::onMoveItemDown() {
  if (! model())
    return;

  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveDown(rows.first, rows.second);
}

void
ConfigObjectTableView::onDoubleClicked(QModelIndex idx) {
  if ((0 > idx.row()) || (idx.row() >= model()->rowCount(QModelIndex())))
    return;
  emit doubleClicked(idx.row());
}
