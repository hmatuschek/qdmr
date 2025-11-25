#include "configobjectlistview.hh"
#include "ui_configobjectlistview.h"
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


ConfigObjectListView::ConfigObjectListView(QWidget *parent)
  : QWidget(parent), ui(new Ui::ConfigObjectListView)
{
  ui->setupUi(this);

  connect(ui->itemTop, SIGNAL(clicked(bool)), this, SLOT(onMoveItemTop()));
  connect(ui->itemTenUp, SIGNAL(clicked(bool)), this, SLOT(onMoveItemTenUp()));
  connect(ui->itemUp, SIGNAL(clicked(bool)), this, SLOT(onMoveItemUp()));
  connect(ui->itemDown, SIGNAL(clicked(bool)), this, SLOT(onMoveItemDown()));
  connect(ui->itemTenDown, SIGNAL(clicked(bool)), this, SLOT(onMoveItemTenDown()));
  connect(ui->itemBottom, SIGNAL(clicked(bool)), this, SLOT(onMoveItemBottom()));
  connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));

  ui->listView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);

  ui->listView->setDragEnabled(true);
  ui->listView->viewport()->setAcceptDrops(true);
  ui->listView->setDropIndicatorShown(true);
  ui->listView->setDragDropMode(QAbstractItemView::InternalMove);

  SearchPopup::attach(ui->listView);
}

ConfigObjectListView::~ConfigObjectListView() {
  delete ui;
}

GenericListWrapper *
ConfigObjectListView::model() const {
  return qobject_cast<GenericListWrapper *>(ui->listView->model());
}

void
ConfigObjectListView::setModel(GenericListWrapper *model) {
  ui->listView->setModel(model);
}

bool
ConfigObjectListView::hasSelection() const {
  return ui->listView->selectionModel()->hasSelection();
}

QPair<int,int>
ConfigObjectListView::selection() const {
  return getSelectionRowRange(ui->listView->selectionModel()->selection().indexes());
}

void
ConfigObjectListView::onMoveItemUp() {
  // Check if there is a selection
  if (! ui->listView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::max(0, rows.first-1));
}

void
ConfigObjectListView::onMoveItemTenUp() {
  // Check if there is a selection
  if (! ui->listView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  int dest = std::max(0, rows.first-9);
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), dest);

}

void
ConfigObjectListView::onMoveItemTop() {
  // Check if there is a selection
  if (! ui->listView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), 0);
}


void
ConfigObjectListView::onMoveItemDown() {
  // Check if there is a selection
  if (! ui->listView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::min(model()->rowCount(QModelIndex()), rows.second+2));
}


void
ConfigObjectListView::onMoveItemTenDown() {
  // Check if there is a selection
  if (! ui->listView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::min(rows.second+10, model()->rowCount(QModelIndex())));
}

void
ConfigObjectListView::onMoveItemBottom() {
  // Check if there is a selection
  if (! ui->listView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return;
  }

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), model()->rowCount(QModelIndex()));
}


void
ConfigObjectListView::onDoubleClicked(QModelIndex idx) {
  if ((0 > idx.row()) || (idx.row() >= model()->rowCount(QModelIndex())))
    return;
  emit doubleClicked(idx.row());
}
