#include "m17contactdialog.hh"
#include "ui_m17contactdialog.h"
#include "contact.hh"
#include "settings.hh"
#include <QRegExpValidator>



M17ContactDialog::M17ContactDialog(Config *config, QWidget *parent) :
  QDialog(parent), ui(new Ui::M17ContactDialog), _config(config), _contact(nullptr),
  _myContact(new M17Contact(this))
{    
  construct();
}

M17ContactDialog::M17ContactDialog(Config *config, M17Contact *contact, QWidget *parent) :
  QDialog(parent), ui(new Ui::M17ContactDialog), _config(config), _contact(contact),
  _myContact(new M17Contact(this))
{
  if (_contact)
    _myContact->copy(*_contact);

  construct();
}

M17ContactDialog::~M17ContactDialog()
{
  delete ui;
}


void
M17ContactDialog::construct() {
  ui->setupUi(this);
  Settings settings;

  ui->call->setValidator(new QRegExpValidator(QRegExp("[A-Z0-9\\./\\-]{0,9}")));

  if (_contact) {
    setWindowTitle(tr("Edit M17 Contact"));
    ui->name->setText(_contact->name());
    ui->call->setText(_contact->call());
    ui->broadcast->setChecked(_contact->isBroadcast());
    ui->call->setEnabled(! _contact->isBroadcast());
  } else {
    setWindowTitle(tr("Create M17 Contact"));
  }

  ui->extensionView->setObjectName("m17ContactExtension");
  ui->extensionView->setObject(_myContact, _config);
  if (! settings.showExtensions())
    ui->tabWidget->tabBar()->hide();

  connect(ui->broadcast, SIGNAL(toggled(bool)), this, SLOT(onBroadcastToggled(bool)));
  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

M17Contact *
M17ContactDialog::contact() const {

  _myContact->setName(ui->name->text().simplified());
  _myContact->setCall(ui->call->text());
  _myContact->setBroadcast(ui->broadcast->isChecked());
  _myContact->setRing(ui->ring->isChecked());

  M17Contact *contact = _myContact;
  if (_contact) {
    _contact->copy(*_myContact);
    contact = _contact;
  } else {
    _myContact->setParent(nullptr);
  }

  return contact;

  return _contact;
}

void
M17ContactDialog::onBroadcastToggled(bool enable) {
  ui->call->setEnabled(! enable);
}
