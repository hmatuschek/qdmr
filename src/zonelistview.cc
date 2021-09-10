#include "zonelistview.hh"
#include "ui_zonelistview.h"
#include "config.hh"
#include "zonedialog.hh"
#include <QMessageBox>


ZoneListView::ZoneListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::ZoneListView), _config(config)
{
  ui->setupUi(this);

  ui->listView->setModel(new ZoneListWrapper(_config->zones(), ui->listView));
  connect(ui->addZone, SIGNAL(clicked()), this, SLOT(onAddZone()));
  connect(ui->remZone, SIGNAL(clicked()), this, SLOT(onRemZone()));
  connect(ui->listView, SIGNAL(doubleClicked(uint)), this, SLOT(onEditZone(uint)));
}

ZoneListView::~ZoneListView() {
  delete ui;
}

void
ZoneListView::onAddZone() {
  ZoneDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->zones()->add(dialog.zone(), row);
}

void
ZoneListView::onRemZone() {
  // Check if there is any zones seleced
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete zone"),
          tr("Cannot delete zone: You have to select a zone first."));
    return;
  }

  // Get selection and ask for deletion
  QPair<int,int> rows = ui->listView->selection();
  int rowcount = rows.second-rows.first+1;
  if (rows.first == rows.second) {
    QString name = _config->zones()->zone(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete zone?"), tr("Delete zone %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete zones?"), tr("Delete %1 zones?").arg(rowcount)))
      return;
  }

  // collect all selected zones
  // need to collect them first as rows change when deleting
  QList<Zone *> lists; lists.reserve(rowcount);
  for(int row=rows.first; row<=rows.second; row++)
    lists.push_back(_config->zones()->zone(row));
  // remove
  foreach (Zone *zone, lists)
    _config->zones()->del(zone);
}

void
ZoneListView::onEditZone(uint row) {
  ZoneDialog dialog(_config, _config->zones()->zone(row));
  if (QDialog::Accepted != dialog.exec())
    return;
  dialog.zone();
}
