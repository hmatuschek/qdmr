#include "satellitedatabasedialog.hh"
#include "ui_satellitedatabasedialog.h"
#include "satellitedatabase.hh"
#include "satelliteselectiondialog.hh"
#include "transponderfrequencydelegate.hh"
#include "settings.hh"


SatelliteDatabaseDialog::SatelliteDatabaseDialog(SatelliteDatabase *db, QWidget *parent) :
  QDialog(parent), ui(new Ui::SatelliteDatabaseDialog), _database(db)
{
  ui->setupUi(this);

  connect(ui->addButton, SIGNAL(clicked(bool)), this, SLOT(onAddSatellite()));
  connect(ui->delButton, SIGNAL(clicked(bool)), this, SLOT(onDeleteSatellite()));

  ui->satellitesView->setModel(_database);

  ui->satellitesView->setItemDelegateForColumn(
        2, new TransponderFrequencyDelegate(false, Transponder::Mode::FM));
  ui->satellitesView->setItemDelegateForColumn(
        3, new TransponderFrequencyDelegate(true, Transponder::Mode::FM));
  ui->satellitesView->setItemDelegateForColumn(
        6, new TransponderFrequencyDelegate(false, Transponder::Mode::APRS));
  ui->satellitesView->setItemDelegateForColumn(
        7, new TransponderFrequencyDelegate(true, Transponder::Mode::APRS));
  ui->satellitesView->setItemDelegateForColumn(
        10, new TransponderFrequencyDelegate(false, Transponder::Mode::CW));

  this->restoreGeometry(Settings().headerState(objectName()));
  ui->satellitesView->horizontalHeader()->restoreState(
        Settings().headerState(ui->satellitesView->objectName()));
}


SatelliteDatabaseDialog::~SatelliteDatabaseDialog()
{
  Settings settings;
  settings.setHeaderState(this->objectName(), this->saveGeometry());
  settings.setHeaderState(ui->satellitesView->objectName(),
                          ui->satellitesView->horizontalHeader()->saveState());

  delete ui;
}


void
SatelliteDatabaseDialog::onAddSatellite() {
  SatelliteSelectionDialog dialog(&(_database->orbitalElements()));
  if (QDialog::Accepted != dialog.exec())
    return;
  _database->add(_database->orbitalElements().getById(dialog.satellite()));
}


void
SatelliteDatabaseDialog::onDeleteSatellite() {
  QModelIndexList selected = ui->satellitesView->selectionModel()->selectedRows();
  if (selected.isEmpty())
    return;

  _database->removeRow(selected.back().row());
}


