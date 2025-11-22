#include "configobjecttableview.hh"
#include "ui_configobjecttableview.h"
#include "searchpopup.hh"
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "settings.hh"


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
  : QWidget(parent), _model(nullptr), ui(new Ui::ConfigObjectTableView)
{
  ui->setupUi(this);

  ui->filterToggleButton->setDefaultAction(ui->actionToggleFilterSort);

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

  toggleSortFilter(ui->actionToggleFilterSort->isChecked());
  connect(this, &QObject::objectNameChanged, [this](const QString &objName) {
    this->ui->actionToggleFilterSort->setChecked(Settings().sortFilterEnabled(objName));
  });

  ui->tableView->setDropIndicatorShown(true);
  ui->tableView->setDefaultDropAction(Qt::MoveAction);
  ui->tableView->setDragDropMode(QAbstractItemView::InternalMove);
  ui->tableView->setDragDropOverwriteMode(false);

  ui->tableView->addAction(ui->actionToggleFilterSort);
  connect(ui->actionToggleFilterSort, &QAction::toggled,
          this, &ConfigObjectTableView::toggleSortFilter);

  connect(ui->clearButton, &QPushButton::clicked, ui->filterEdit, &QLineEdit::clear);

  ui->filterEdit->addAction(ui->actionCloseSortFilter);
  connect(ui->actionCloseSortFilter, &QAction::triggered, [this]() {
    this->ui->actionToggleFilterSort->setChecked(false);
  });

  SearchPopup::attach(ui->tableView);
}

ConfigObjectTableView::~ConfigObjectTableView() {
  delete ui;
}

GenericTableWrapper *
ConfigObjectTableView::model() const {
  return _model;
}

void
ConfigObjectTableView::setModel(GenericTableWrapper *model) {
  _model = model;
  _model->setParent(this);
  // If sorting is enabled -> set source model of proxy
  if (ui->actionToggleFilterSort->isChecked()) {
    proxy()->setSourceModel(_model);
  } else {
    // if not, set model directly
    auto selectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(_model);
    if (selectionModel)
      selectionModel->deleteLater();
  }
}


QSortFilterProxyModel *
ConfigObjectTableView::proxy() const {
  if (nullptr == ui->tableView->model())
    return nullptr;
  return qobject_cast<QSortFilterProxyModel*>(ui->tableView->model());
}

bool
ConfigObjectTableView::isFilteredOrSorted() const {
  if (nullptr == ui->tableView->model())
    return false;
  return nullptr != qobject_cast<QSortFilterProxyModel*>(ui->tableView->model());
}

bool
ConfigObjectTableView::hasSelection() const {
  return ui->tableView->selectionModel()->hasSelection();
}

QPair<int,int>
ConfigObjectTableView::selection() const {
  if (isFilteredOrSorted())
    return getSelectionRowRange(
          proxy()->mapSelectionToSource(
            ui->tableView->selectionModel()->selection()).indexes());
  return getSelectionRowRange(ui->tableView->selectionModel()->selection().indexes());
}

QHeaderView *
ConfigObjectTableView::header() const {
  return ui->tableView->horizontalHeader();
}

void
ConfigObjectTableView::onMoveItemUp() {
  // check if we can move items around safely
  if (! canMove()) return;

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::max(0, rows.first-1));
}

void
ConfigObjectTableView::onMoveItemTenUp() {
  // check if we can move items around safely
  if (! canMove()) return;

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  int dest = std::max(0, rows.first-9);
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), dest);

}

void
ConfigObjectTableView::onMoveItemTop() {
  // check if we can move items around safely
  if (! canMove()) return;

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());
  // If selection range is invalid or I cannot move at all: done.
  if ((0>=rows.first) || (0>rows.second))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), 0);
}


void
ConfigObjectTableView::onMoveItemDown() {
  // check if we can move items around safely
  if (! canMove()) return;

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());

  // If selection range is invalid or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::min(model()->rowCount(QModelIndex()), rows.second+2));
}


void
ConfigObjectTableView::onMoveItemTenDown() {
  // check if we can move items around safely
  if (! canMove()) return;

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());

  // If selection range is invalid or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), std::min(rows.second+10, model()->rowCount(QModelIndex())));
}

void
ConfigObjectTableView::onMoveItemBottom() {
  // check if we can move items around safely
  if (! canMove()) return;

  // Get selection range assuming only continuous selection mode
  QPair<int, int> rows = getSelectionRowRange(
        ui->tableView->selectionModel()->selection().indexes());

  // If selection range is invalid or I cannot move at all: done.
  if ((0>rows.first) || (0>rows.second) ||
      ((rows.second+1)>=model()->rowCount(QModelIndex())))
    return;

  // Then move rows
  model()->moveRows(QModelIndex(), rows.first, (rows.second-rows.first+1),
                    QModelIndex(), model()->rowCount(QModelIndex()));
}

void
ConfigObjectTableView::onDoubleClicked(QModelIndex idx) {
  // Map index if sort/filter is enabled
  if (isFilteredOrSorted())
    idx = proxy()->mapToSource(idx);

  if ((0 > idx.row()) || (idx.row() >= model()->rowCount(QModelIndex())))
    return;

  emit doubleClicked(idx.row());
}

bool
ConfigObjectTableView::canMove() const {
  // Check if there is a selection
  if (! ui->tableView->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items: You have to select at least one item first."));
    return false;
  }

  if (isFilteredOrSorted()) {
    QMessageBox::information(
          nullptr, tr("Cannot move items."),
          tr("Cannot move items as long as there is some filter or sorting applied."));
    return false;
  }

  return true;
}

void
ConfigObjectTableView::toggleSortFilter(bool enableSortFilter) {
  Settings().enableSortFilter(objectName(), enableSortFilter);

  ui->filterWidget->setVisible(enableSortFilter);
  ui->tableView->setSortingEnabled(enableSortFilter);
  ui->moveWidget->setVisible(! enableSortFilter);

  auto selectionModel = ui->tableView->selectionModel();

  if (enableSortFilter) {
    // Setup proxy model
    ui->tableView->setModel(new QSortFilterProxyModel());
    proxy()->setFilterKeyColumn(-1);
    proxy()->setFilterCaseSensitivity(Qt::CaseInsensitive);
    if (_model) proxy()->setSourceModel(_model);
    // Connect filter edit
    connect(ui->filterEdit, &QLineEdit::textChanged,
            proxy(), &QSortFilterProxyModel::setFilterFixedString);
    ui->filterEdit->setFocus();
  } else {
    if (_model)
      ui->tableView->setModel(_model);
    ui->filterEdit->clear();
  }

  if (selectionModel)
    selectionModel->deleteLater();

  ui->tableView->setDragEnabled(! enableSortFilter);
  ui->tableView->viewport()->setAcceptDrops(! enableSortFilter);
}
