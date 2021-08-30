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

  connect(ui->itemUp, SIGNAL(clicked(bool)), this, SLOT(onMoveItemUp()));
  connect(ui->itemDown, SIGNAL(clicked(bool)), this, SLOT(onMoveItemDown()));
  ui->listView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
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

  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveUp(rows.first, rows.second);
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

  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->listView->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveDown(rows.first, rows.second);
}

void
ConfigObjectListView::onDoubleClicked(QModelIndex idx) {
  if ((0 > idx.row()) || (idx.row() >= model()->rowCount(QModelIndex())))
    return;
  emit doubleClicked(idx.row());
}
