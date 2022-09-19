#include "contactlistview.hh"
#include "ui_contactlistview.h"
#include "dmrcontactdialog.hh"
#include "m17contactdialog.hh"
#include "dtmfcontactdialog.hh"
#include "application.hh"
#include "settings.hh"
#include <QMessageBox>
#include <QHeaderView>
#include <QMenu>

ContactListView::ContactListView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::ContactListView), _config(config)
{
  ui->setupUi(this);

  connect(ui->listView->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadHeaderState()));
  connect(ui->listView->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeHeaderState()));

  ui->listView->setModel(new ContactListWrapper(_config->contacts(), ui->listView));

  QMenu *menu = new QMenu();
  menu->addAction(ui->actionAdd_DMR_Contact);
  menu->addAction(ui->actionAdd_M17_Contact);
  menu->addAction(ui->actionAdd_DTMF_Contact);
  ui->addContact->setMenu(menu);

  connect(ui->actionAdd_DMR_Contact, SIGNAL(triggered(bool)), this, SLOT(onAddDMRContact()));
  connect(ui->actionAdd_M17_Contact, SIGNAL(triggered(bool)), this, SLOT(onAddM17Contact()));
  connect(ui->actionAdd_DTMF_Contact, SIGNAL(triggered(bool)), this, SLOT(onAddDTMFContact()));
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
ContactListView::onAddM17Contact() {
  M17ContactDialog dialog(_config);
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

  if (DMRContact *dmr = contact->as<DMRContact>()) {
    DMRContactDialog dialog(dmr, app->user(), app->talkgroup(), _config);
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.contact();
  } else if (M17Contact *m17 = contact->as<M17Contact>()) {
    M17ContactDialog dialog(_config, m17);
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
