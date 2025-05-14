#include "satelliteselectiondialog.hh"
#include "ui_satelliteselectiondialog.h"
#include "orbitalelementsdatabase.hh"
#include "logger.hh"
#include "settings.hh"
#include "searchpopup.hh"


SatelliteSelectionDialog::SatelliteSelectionDialog(OrbitalElementsDatabase *db, QWidget *parent)
  : QDialog(parent), ui(new Ui::SatelliteSelectionDialog), _database(db)
{
  ui->setupUi(this);
  ui->satellites->setModel(_database);
  ui->satellites->hideColumn(2);

  SearchPopup::attach(ui->satellites);
  this->restoreGeometry(Settings().headerState(objectName()));
}

SatelliteSelectionDialog::~SatelliteSelectionDialog() {
  Settings().setHeaderState(this->objectName(), this->saveGeometry());
  delete ui;
}


unsigned int
SatelliteSelectionDialog::satellite() const {
  QModelIndexList selection = ui->satellites->selectionModel()->selectedRows();
  if (0 == selection.count())
      return 0;
  return _database->getAt(selection.back().row()).id();
}
