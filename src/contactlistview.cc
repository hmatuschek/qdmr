#include "contactlistview.hh"
#include "ui_contactlistview.h"
#include "dmrcontactdialog.hh"
#include "dtmfcontactdialog.hh"
#include "application.hh"
#include "settings.hh"
#include <QMessageBox>
#include <QHeaderView>


ContactListView::ContactListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::ContactListView), _config(config)
{
  ui->setupUi(this);

  connect(ui->listView->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadHeaderState()));
  connect(ui->listView->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeHeaderState()));

  ui->listView->setModel(new ContactListWrapper(_config->contacts(), ui->listView));

  connect(ui->addDMRContact, SIGNAL(clicked()), this, SLOT(onAddDMRContact()));
  connect(ui->addDTMFContact, SIGNAL(clicked()), this, SLOT(onAddDTMFContact()));
  connect(ui->remContact, SIGNAL(clicked()), this, SLOT(onRemContact()));
  connect(ui->listView, SIGNAL(doubleClicked(unsigned)), this, SLOT(onEditContact(unsigned)));
}

ContactListView::~ContactListView() {
  delete ui;
}

void
ContactListView::onAddDMRContact() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  DMRContactDialog dialog(app->user(), app->talkgroup(), _config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->contacts()->add(dialog.contact(), row);
}

void
ContactListView::onAddDTMFContact() {
  DTMFContactDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (ui->listView->hasSelection())
    row = ui->listView->selection().second+1;
  _config->contacts()->add(dialog.contact(), row);
}

void
ContactListView::onRemContact() {
  // Check if there is any contacts selected
  if (! ui->listView->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete contact"),
          tr("Cannot delete contact: You have to select a contact first."));
    return;
  }

  // Get selection and ask for deletion
  QPair<int,int> rows = ui->listView->selection();
  int numrows = rows.second-rows.first+1;
  if (rows.first == rows.second) {
    QString name = _config->contacts()->contact(rows.first)->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete contact?"), tr("Delete contact %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete contacts?"), tr("Delete %1 contacts?").arg(numrows)))
      return;
  }

  // collect all selected contacts
  // need to collect them first as rows change when deleting contacts
  QList<Contact *> contacts; contacts.reserve(numrows);
  for (int i=rows.first; i<=rows.second; i++)
    contacts.push_back(_config->contacts()->contact(i));
  // remove contacts
  foreach (Contact *contact, contacts)
    _config->contacts()->del(contact);
}

void
ContactListView::onEditContact(unsigned row) {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  Contact *contact = _config->contacts()->contact(row);

  if (DMRContact *digi = contact->as<DMRContact>()) {
    DMRContactDialog dialog(digi, app->user(), app->talkgroup(), _config);
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.contact();
  } else if (DTMFContact *dtmf = contact->as<DTMFContact>()) {
    DTMFContactDialog dialog(dtmf, _config);
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.contact();
  }
}

void
ContactListView::loadHeaderState() {
  Settings settings;
  ui->listView->header()->restoreState(settings.headerState("contactList"));
}
void
ContactListView::storeHeaderState() {
  Settings settings;
  settings.setHeaderState("contactList", ui->listView->header()->saveState());
}
