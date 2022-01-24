#include "dmrcontactdialog.hh"
#include "ui_dmrcontactdialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QFormLayout>
#include <QCompleter>
#include "contact.hh"
#include "userdatabase.hh"
#include "talkgroupdatabase.hh"
#include "settings.hh"


DMRContactDialog::DMRContactDialog(UserDatabase *users, TalkGroupDatabase *tgs, QWidget *parent)
  : QDialog(parent), _myContact(new DigitalContact(this)), _contact(nullptr),
    _user_completer(nullptr), _tg_completer(nullptr),
    ui(new Ui::DMRContactDialog)
{
  setWindowTitle(tr("Create DMR Contact"));

  _user_completer = new QCompleter(users, this);
  _user_completer->setCompletionColumn(0);
  _user_completer->setCaseSensitivity(Qt::CaseInsensitive);

  _tg_completer = new QCompleter(tgs, this);
  _tg_completer->setCompletionColumn(0);
  _tg_completer->setCaseSensitivity(Qt::CaseInsensitive);

  connect(_user_completer, SIGNAL(activated(QModelIndex)),
          this, SLOT(onCompleterActivated(QModelIndex)));

  connect(_tg_completer, SIGNAL(activated(QModelIndex)),
          this, SLOT(onCompleterActivated(QModelIndex)));

  construct();
}

DMRContactDialog::DMRContactDialog(DigitalContact *contact, UserDatabase *users,
                                   TalkGroupDatabase *tgs, QWidget *parent)
  : QDialog(parent), _myContact(new DigitalContact(this)), _contact(contact),
    _user_completer(nullptr), _tg_completer(nullptr),
    ui(new Ui::DMRContactDialog)
{
  setWindowTitle(tr("Edit DMR Contact"));
  _user_completer = new QCompleter(users, this);
  _user_completer->setCompletionColumn(0);
  _user_completer->setCaseSensitivity(Qt::CaseInsensitive);

  _tg_completer = new QCompleter(tgs, this);
  _tg_completer->setCompletionColumn(0);
  _tg_completer->setCaseSensitivity(Qt::CaseInsensitive);

  if (_contact)
    _myContact->copy(*_contact);

  connect(_user_completer, SIGNAL(activated(QModelIndex)),
          this, SLOT(onCompleterActivated(QModelIndex)));

  connect(_tg_completer, SIGNAL(activated(QModelIndex)),
          this, SLOT(onCompleterActivated(QModelIndex)));

  construct();
}

DMRContactDialog::~DMRContactDialog()
{
  delete ui;
}

void
DMRContactDialog::construct() {
  ui->setupUi(this);
  Settings settings;

  ui->typeComboBox->addItem(tr("Private Call"), unsigned(DigitalContact::PrivateCall));
  ui->typeComboBox->addItem(tr("Group Call"), unsigned(DigitalContact::GroupCall));
  ui->typeComboBox->addItem(tr("All Call"), unsigned(DigitalContact::AllCall));
  if (DigitalContact::PrivateCall == _myContact->type()) {
    ui->typeComboBox->setCurrentIndex(0);
    ui->nameLineEdit->setCompleter(_user_completer);
  } else if (DigitalContact::GroupCall == _myContact->type()) {
    ui->typeComboBox->setCurrentIndex(1);
    ui->nameLineEdit->setCompleter(_tg_completer);
  } else {
    ui->typeComboBox->setCurrentIndex(2);
    ui->nameLineEdit->setCompleter(nullptr);
    ui->nameLineEdit->setEnabled(false);
  }
  ui->nameLineEdit->setText(_myContact->name());
  ui->numberLineEdit->setText(QString::number(_myContact->number()));
  ui->numberLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
  ui->ringCheckBox->setChecked(_myContact->ring());

  ui->extensionView->setObjectName("dmrContactExtension");
  ui->extensionView->setObject(_myContact);
  if (! settings.showCommercialFeatures())
    ui->tabWidget->tabBar()->hide();

  connect(ui->typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));
  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void
DMRContactDialog::onTypeChanged(int idx) {
  if (0 == idx) {
    ui->numberLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    ui->numberLineEdit->setEnabled(true);
    ui->nameLineEdit->setCompleter(_user_completer);
  } else if (1 == idx) {
    ui->numberLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    ui->numberLineEdit->setEnabled(true);
    ui->nameLineEdit->setCompleter(_tg_completer);
  } else if (2 == idx) {
    ui->numberLineEdit->setText("16777215");
    ui->numberLineEdit->setEnabled(false);
    ui->nameLineEdit->setCompleter(nullptr);
  }
}

void
DMRContactDialog::onCompleterActivated(const QModelIndex &idx) {
  if (0 == ui->typeComboBox->currentIndex()) { // Private call
    if (nullptr == _user_completer)
      return;
    UserDatabase *db = qobject_cast<UserDatabase *>(_user_completer->model());
    if (nullptr == db)
      return;
    QAbstractProxyModel *model = qobject_cast<QAbstractProxyModel *>(_user_completer->completionModel());
    if (nullptr == model)
      return;
    QModelIndex srcidx = model->mapToSource(idx);
    ui->numberLineEdit->setText(QString::number(db->user(srcidx.row()).id));
  } else if (1 == ui->typeComboBox->currentIndex()) { // Group call
    if (nullptr == _tg_completer)
      return;
    TalkGroupDatabase *db = qobject_cast<TalkGroupDatabase *>(_tg_completer->model());
    if (nullptr == db)
      return;
    QAbstractProxyModel *model = qobject_cast<QAbstractProxyModel *>(_tg_completer->completionModel());
    if (nullptr == model)
      return;
    QModelIndex srcidx = model->mapToSource(idx);
    ui->numberLineEdit->setText(QString::number(db->talkgroup(srcidx.row()).id));
  }
}

DigitalContact *
DMRContactDialog::contact()
{
  _myContact->setType(DigitalContact::Type(ui->typeComboBox->currentData().toUInt()));
  _myContact->setName(ui->nameLineEdit->text().simplified());
  _myContact->setNumber(ui->numberLineEdit->text().toUInt());
  _myContact->setRing(ui->ringCheckBox->isChecked());

  DigitalContact *contact = _myContact;
  if (_contact) {
    _contact->copy(*_myContact);
    contact = _contact;
  } else {
    _myContact->setParent(nullptr);
  }

  return contact;
}

