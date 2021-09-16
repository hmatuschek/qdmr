#include "positioningsystemlistview.hh"
#include "ui_positioningsystemlistview.h"
#include "config.hh"

#include "aprssystemdialog.hh"
#include "gpssystemdialog.hh"
#include "settings.hh"

#include <QMessageBox>
#include <QHeaderView>

PositioningSystemListView::PositioningSystemListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::PositioningSystemListView), _config(config)
{
  Settings settings;

  ui->setupUi(this);

  connect(ui->listView->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadPositioningSectionState()));
  connect(ui->listView->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storePositioningSectionState()));

  ui->listView->setModel(new PositioningSystemListWrapper(_config->posSystems(), ui->listView));

  if (settings.hideGSPNote())
    ui->gpsNote->setVisible(false);

  connect(ui->addGPS, SIGNAL(clicked()), this, SLOT(onAddGPS()));
  connect(ui->addAPRS, SIGNAL(clicked()), this, SLOT(onAddAPRS()));
  connect(ui->remGPS, SIGNAL(clicked()), this, SLOT(onRemGPS()));
  connect(ui->listView, SIGNAL(doubleClicked(uint)), this, SLOT(onEditGPS(uint)));
  connect(ui->gpsNote, SIGNAL(linkActivated(QString)), this, SLOT(onHideGPSNote()));


}

PositioningSystemListView::~PositioningSystemListView() {
  delete ui;
}


void
PositioningSystemListView::onAddGPS() {
  GPSSystemDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->posSystems()->add(dialog.gpsSystem(), row);
}

void
PositioningSystemListView::onAddAPRS() {
  APRSSystemDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->posSystems()->add(dialog.aprsSystem(), row);
}

void
PositioningSystemListView::onRemGPS() {
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete GPS system"),
          tr("Cannot delete GPS system: You have to select a GPS system first."));
    return;
  }
  // Get selection and ask for deletion
  QPair<int,int> rows = ui->listView->selection();
  int rowcount = rows.second-rows.first+1;
  if (rows.first == rows.second) {
    QString name = _config->posSystems()->system(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete positioning system?"), tr("Delete positioning system %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete positioning system?"), tr("Delete %1 positioning systems?").arg(rowcount)))
      return;
  }

  // collect all selected systems
  // need to collect them first as rows change when deleting systems
  QList<PositioningSystem *> systems; systems.reserve(rowcount);
  for(int row=rows.first; row<=rows.second; row++)
    systems.push_back(_config->posSystems()->system(row));
  // remove systems
  foreach (PositioningSystem *system, systems)
    _config->posSystems()->del(system);
}

void
PositioningSystemListView::onEditGPS(uint row) {
  PositioningSystem *sys = _config->posSystems()->system(row);

  if (sys->is<GPSSystem>()) {
    GPSSystemDialog dialog(_config, sys->as<GPSSystem>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.gpsSystem();
  } else if (sys->is<APRSSystem>()) {
    APRSSystemDialog dialog(_config, sys->as<APRSSystem>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.aprsSystem();
  }
}

void
PositioningSystemListView::onHideGPSNote() {
  Settings setting; setting.setHideGPSNote(true);
  ui->gpsNote->setVisible(false);
}

void
PositioningSystemListView::loadPositioningSectionState() {
  Settings settings;
  ui->listView->header()->restoreState(settings.positioningHeaderState());
}
void
PositioningSystemListView::storePositioningSectionState() {
  Settings settings;
  settings.setPositioningHeaderState(ui->listView->header()->saveState());
}

