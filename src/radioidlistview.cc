#include "radioidlistview.hh"
#include "ui_radioidlistview.h"
#include "config.hh"
#include "configobjecttableview.hh"
#include "configitemwrapper.hh"
#include "settings.hh"
#include <QMessageBox>
#include <QHeaderView>


RadioIDListView::RadioIDListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::RadioIDListView), _config(config)
{
  ui->setupUi(this);

  connect(ui->listView->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadHeaderState()));
  connect(ui->listView->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeHeaderState()));

  ui->listView->setModel(new RadioIdListWrapper(_config->radioIDs(), ui->listView));
  ui->defaultID->setModel(new RadioIdListWrapper(_config->radioIDs(), ui->defaultID));
  ui->defaultID->setModelColumn(1);

  connect(_config, SIGNAL(modified(ConfigObject*)), this, SLOT(onConfigModified()));
  connect(ui->addID, SIGNAL(clicked(bool)), this, SLOT(onAddID()));
  connect(ui->delID, SIGNAL(clicked(bool)), this, SLOT(onDeleteID()));
  connect(ui->listView, SIGNAL(doubleClicked(unsigned)), this, SLOT(onEditID(unsigned)));
  connect(ui->defaultID, SIGNAL(currentIndexChanged(int)), this, SLOT(onDefaultIDSelected(int)));
}

RadioIDListView::~RadioIDListView() {
  delete ui;
}

void
RadioIDListView::onConfigModified() {
  if (_config->radioIDs()->defaultId()) {
    ui->defaultID->setCurrentIndex(_config->radioIDs()->indexOf(_config->radioIDs()->defaultId()));
  } else {
    ui->defaultID->setCurrentIndex(-1);
  }
}

void
RadioIDListView::onAddID() {
  /*RadioIdDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row = -1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second;
  _config->scanlists()->add(dialog.scanlist(), row);*/
}

void
RadioIDListView::onDeleteID() {
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete radio IDs"),
          tr("Cannot delete radio IDs: You have to select a radio ID first."));
    return;
  }
  // Get selection and ask for deletion
  QPair<int,int> rows = ui->listView->selection();
  int numrows = rows.second-rows.first+1;
  if (rows.first == rows.second) {
    QString name = _config->radioIDs()->getId(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete radio ID?"), tr("Delete radio ID %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete scan lists?"), tr("Delete %1 scan lists?").arg(numrows)))
      return;
  }
  // collect all selected scan lists
  // need to collect them first as rows change when deleting
  QList<RadioID *> ids; ids.reserve(numrows);
  for(int i=rows.first; i<=rows.second; i++)
    ids.push_back(_config->radioIDs()->getId(i));
  // remove
  foreach (RadioID *id, ids)
    _config->radioIDs()->del(id);
}

void
RadioIDListView::onEditID(unsigned row) {
}

void
RadioIDListView::onDefaultIDSelected(int idx) {
  if ((idx < 0) || (idx >= _config->radioIDs()->count()))
    return;
  _config->radioIDs()->setDefaultId(idx);
}

void
RadioIDListView::loadHeaderState() {
  ui->listView->header()->restoreState(Settings().radioIdListHeaderState());
}
void
RadioIDListView::storeHeaderState() {
  Settings().setRadioIdListHeaderState(ui->listView->header()->saveState());
}



