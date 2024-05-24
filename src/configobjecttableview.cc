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
  connect(ui->itemTop, SIGNAL(clicked(bool)), this, SLOT(onMoveItemTop()));
  connect(ui->itemTenUp, SIGNAL(clicked(bool)), this, SLOT(onMoveItemTenUp()));
  connect(ui->itemUp, SIGNAL(clicked(bool)), this, SLOT(onMoveItemUp()));
  connect(ui->itemDown, SIGNAL(clicked(bool)), this, SLOT(onMoveItemDown()));
  connect(ui->itemTenDown, SIGNAL(clicked(bool)), this, SLOT(onMoveItemTenDown()));
  connect(ui->itemBottom, SIGNAL(clicked(bool)), this, SLOT(onMoveItemBottom()));
  connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(onDoubleClicked(QModelIndex)));

  ui->tableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

  ui->tableView->setDragEnabled(true);
  ui->tableView->viewport()->setAcceptDrops(true);
  ui->tableView->setDropIndicatorShown(true);
  ui->tableView->setDefaultDropAction(Qt::MoveAction);
  ui->tableView->setDragDropMode(QAbstractItemView::InternalMove);
  ui->tableView->setDragDropOverwriteMode(false);

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
  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalud or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::max(0, rows.first-1));
}

void
ConfigObjectTableView::onMoveItemTenUp() {
  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalud or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  int dest = std::max(0, rows.first-9);
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), dest);

}

void
ConfigObjectTableView::onMoveItemTop() {
  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalud or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), 0);
}


void
ConfigObjectTableView::onMoveItemDown() {
  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalud or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::min(model()->rowCount(QModelIndex()), rows.second+2));
}


void
ConfigObjectTableView::onMoveItemTenDown() {
  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalud or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::min(rows.second+10, model()->rowCount(QModelIndex())));
}

void
ConfigObjectTableView::onMoveItemBottom() {
  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalud or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), model()->rowCount(QModelIndex()));
}

void
ConfigObjectTableView::onDoubleClicked(QModelIndex idx) {
  if ((0 > idx.row()) || (idx.row() >= model()->rowCount(QModelIndex())))
    return;
  emit doubleClicked(idx.row());
}
