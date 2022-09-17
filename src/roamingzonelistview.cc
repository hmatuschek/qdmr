#include "roamingzonelistview.hh"
#include "ui_roamingzonelistview.h"

#include "settings.hh"
#include "config.hh"
#include "roamingzonedialog.hh"
#include "contactselectiondialog.hh"

#include <QMessageBox>


RoamingZoneListView::RoamingZoneListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::RoamingZoneListView), _config(config)
{
  Settings settings;
  ui->setupUi(this);

  ui->listView->setModel(new RoamingListWrapper(_config->roaming(), ui->listView));

  connect(ui->addRoamingZone, SIGNAL(clicked()), this, SLOT(onAddRoamingZone()));
  connect(ui->genRoamingZone, SIGNAL(clicked(bool)), this, SLOT(onGenRoamingZone()));
  connect(ui->remRoamingZone, SIGNAL(clicked()), this, SLOT(onRemRoamingZone()));
  connect(ui->listView, SIGNAL(doubleClicked(unsigned)),
          this, SLOT(onEditRoamingZone(unsigned)));
  connect(ui->roamingNote, SIGNAL(linkActivated(QString)), this, SLOT(onHideRoamingNote()));

  if (settings.hideRoamingNote())
    ui->roamingNote->setHidden(true);

}

RoamingZoneListView::~RoamingZoneListView() {
  delete ui;
}

void
RoamingZoneListView::onAddRoamingZone() {
  RoamingZoneDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->roaming()->add(dialog.zone(), row);
}

void
RoamingZoneListView::onGenRoamingZone() {
  MultiGroupCallSelectionDialog contSel(_config->contacts(), false);
  contSel.setWindowTitle(tr("Generate roaming zone"));
  contSel.setLabel(tr("Create a roaming zone by collecting all channels with these group calls."));

  if (QDialog::Accepted != contSel.exec())
    return;

  QList<DMRContact *> contacts = contSel.contacts();
  RoamingZone *zone = new RoamingZone("Name");
  for (int i=0; i<_config->channelList()->count(); i++) {
    DMRChannel *dch = _config->channelList()->channel(i)->as<DMRChannel>();
    if (nullptr == dch)
      continue;
    if (contacts.contains(dch->txContactObj()))
      zone->addChannel(dch);
  }

  RoamingZoneDialog dialog(_config, zone);
  if (QDialog::Accepted != dialog.exec()) {
    zone->deleteLater();
    return;
  }

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->roaming()->add(dialog.zone(), row);
}

void
RoamingZoneListView::onRemRoamingZone() {
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete roaming zone"),
          tr("Cannot delete roaming zone: You have to select a zone first."));
    return;
  }

  // Get selection and ask for deletion
  QPair<int, int> rows = ui->listView->selection();
  int rowcount = rows.second - rows.first + 1;
  if (rows.first==rows.second) {
    QString name = _config->roaming()->zone(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete roaming zone?"), tr("Delete roaming zone %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete roaming zones?"), tr("Delete %1 roaming zones?").arg(rowcount)))
      return;
  }
  // collect all selected zones
  // need to collect them first as rows change when deleting
  QList<RoamingZone *> lists; lists.reserve(rowcount);
  for (int row=rows.first; row<=rows.second; row++)
    lists.push_back(_config->roaming()->zone(row));
  // remove
  foreach (RoamingZone *zone, lists)
    _config->roaming()->del(zone);
}

void
RoamingZoneListView::onEditRoamingZone(unsigned row) {
  RoamingZoneDialog dialog(_config, _config->roaming()->zone(row));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.zone();
}

void
RoamingZoneListView::onHideRoamingNote() {
  Settings setting; setting.setHideRoamingNote(true);
  ui->roamingNote->setVisible(false);
}

