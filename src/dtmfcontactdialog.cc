#include "dtmfcontactdialog.hh"
#include "ui_dtmfcontactdialog.h"

#include <QRegExpValidator>
#include "contact.hh"
#include "settings.hh"

DTMFContactDialog::DTMFContactDialog(QWidget *parent) :
  QDialog(parent), _myContact(new DTMFContact(this)), _contact(nullptr),
  ui(new Ui::DTMFContactDialog)
{
  construct();
}

DTMFContactDialog::DTMFContactDialog(DTMFContact *contact, QWidget *parent) :
  QDialog(parent), _myContact(new DTMFContact(this)), _contact(contact),
  ui(new Ui::DTMFContactDialog)
{
  if (_contact)
    _myContact->copy(*_contact);

  construct();
}

DTMFContactDialog::~DTMFContactDialog() {
  delete ui;
}

void
DTMFContactDialog::construct() {
  ui->setupUi(this);
  Settings settings;

  ui->nameLineEdit->setText(_myContact->name());
  ui->numberLineEdit->setText(_myContact->number());
  ui->numberLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-dA-D\\*#]+")));
  ui->ringCheckBox->setChecked(_myContact->ring());

  ui->extensionView->setObject(_myContact);
  if (! settings.showCommercialFeatures())
    ui->tabWidget->tabBar()->hide();
}

DTMFContact *
DTMFContactDialog::contact() {
  _myContact->setName(ui->nameLineEdit->text().simplified());
  _myContact->setNumber(ui->numberLineEdit->text().simplified());
  _myContact->setRing(ui->ringCheckBox->isChecked());

  DTMFContact *contact = _myContact;
  if (nullptr == _contact) {
    _contact->copy(*_myContact);
    contact = _contact;
  } else {
    _myContact->setParent(nullptr);
  }

  return contact;
}

