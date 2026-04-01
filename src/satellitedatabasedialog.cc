#include "satellitedatabasedialog.hh"
#include "ui_satellitedatabasedialog.h"
#include "satellitedatabase.hh"
#include "satelliteselectiondialog.hh"
#include "transponderfrequencydelegate.hh"
#include "settings.hh"
#include "selectivecallbox.hh"
#include "satellitetransponderdialog.hh"



SatelliteDatabaseDialog::SatelliteDatabaseDialog(SatelliteDatabase *db, QWidget *parent) :
  QDialog(parent), ui(new Ui::SatelliteDatabaseDialog), _database(db)
{
  ui->setupUi(this);
  setWindowIcon(QIcon::fromTheme("edit-satellites"));

  connect(ui->addButton, &QPushButton::clicked,
          this, &SatelliteDatabaseDialog::onAddSatellite);
  connect(ui->delButton, &QPushButton::clicked,
          this, &SatelliteDatabaseDialog::onDeleteSatellite);

  ui->satellitesView->setModel(_database);
  connect(ui->satellitesView, &QTableView::doubleClicked,
          this, &SatelliteDatabaseDialog::onEditTransponder);

  // FM downlink
  //ui->satellitesView->setItemDelegateForColumn(2, new TransponderFrequencyDelegate(false, Transponder::Mode::FM));
  // FM uplink
  //ui->satellitesView->setItemDelegateForColumn(3, new TransponderFrequencyDelegate(true, Transponder::Mode::FM));
  // FM downlink sub tone
  //ui->satellitesView->setItemDelegateForColumn(4, new SelectiveCallDelegate());
  // FM uplink sub tone
  //ui->satellitesView->setItemDelegateForColumn(5, new SelectiveCallDelegate());
  // APRS downlink
  //ui->satellitesView->setItemDelegateForColumn(6, new TransponderFrequencyDelegate(false, Transponder::Mode::APRS));
  // APRS uplink
  //ui->satellitesView->setItemDelegateForColumn(7, new TransponderFrequencyDelegate(true, Transponder::Mode::APRS));
  // APRS downlink sub tone
  //ui->satellitesView->setItemDelegateForColumn(8, new SelectiveCallDelegate());
  // APRS uplink sub tone
  //ui->satellitesView->setItemDelegateForColumn(9, new SelectiveCallDelegate());
  // Beacon
  //ui->satellitesView->setItemDelegateForColumn(10, new TransponderFrequencyDelegate(false, Transponder::Mode::CW));

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
SatelliteDatabaseDialog::onEditTransponder(const QModelIndex &idx) {
  if ((! idx.isValid()) || (idx.row() >= (int)_database->count()))
    return;

  SatelliteTransponderDialog dialog(_database->getAt(idx.row()), _database->transponders());
  if (QDialog::Accepted != dialog.exec())
    return;
  _database->setAt(dialog.satellite(), idx.row());
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


