#include "dtmfcontactdialog.hh"
#include "ui_dtmfcontactdialog.h"

#include <QRegularExpressionValidator>
#include "contact.hh"
#include "settings.hh"

DTMFContactDialog::DTMFContactDialog(Config *context, QWidget *parent) :
  QDialog(parent), _myContact(new DTMFContact(this)), _contact(nullptr), _config(context),
  ui(new Ui::DTMFContactDialog)
{
  setWindowTitle(tr("Create DTMF Contact"));
  construct();
}

DTMFContactDialog::DTMFContactDialog(DTMFContact *contact, Config *context, QWidget *parent) :
  QDialog(parent), _myContact(new DTMFContact(this)), _contact(contact), _config(context),
  ui(new Ui::DTMFContactDialog)
{
  setWindowTitle(tr("Edit DMR Contact"));
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
  ui->numberLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9a-dA-D\\*#]+")));
  ui->ringCheckBox->setChecked(_myContact->ring());

  ui->extensionView->setObjectName("dtmfContactExtension");
  ui->extensionView->setObject(_myContact, _config);
  if (! settings.showExtensions())
    ui->tabWidget->tabBar()->hide();
}

DTMFContact *
DTMFContactDialog::contact() {
  _myContact->setName(ui->nameLineEdit->text().simplified());
  _myContact->setNumber(ui->numberLineEdit->text().simplified());
  _myContact->setRing(ui->ringCheckBox->isChecked());

  DTMFContact *contact = _myContact;
  if (_contact) {
    _contact->copy(*_myContact);
    contact = _contact;
  } else {
    _myContact->setParent(nullptr);
  }

  return contact;
}

