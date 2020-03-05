#include "contactdialog.hh"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QFormLayout>
#include <QCompleter>
#include "userdatabase.hh"
#include <QDebug>


/* ********************************************************************************************* *
 * Implementation of ContactDialog
 * ********************************************************************************************* */
ContactDialog::ContactDialog(UserDatabase *users, QWidget *parent)
  : QDialog(parent), _contact(nullptr), _completer(nullptr)
{
  _completer = new QCompleter(users, this);
  _completer->setCompletionColumn(0);
  _completer->setCaseSensitivity(Qt::CaseInsensitive);

  connect(_completer, SIGNAL(activated(QModelIndex)),
          this, SLOT(onCompleterActivated(QModelIndex)));

  construct();
}

ContactDialog::ContactDialog(UserDatabase *users, Contact *contact, QWidget *parent)
  : QDialog(parent), _contact(contact), _completer(nullptr)
{
  construct();
}

void
ContactDialog::construct() {
  setupUi(this);

  if (_contact) {
    if (_contact->is<DTMFContact>()) {
      DTMFContact *dtmf = _contact->as<DTMFContact>();
      contactType->addItem(tr("DTMF"));
      contactType->setCurrentIndex(0);
      contactType->setEnabled(false);
      contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9a-dA-D\\*#]+")));
      contactNumber->setText(dtmf->number());
    } else {
      DigitalContact *digi = _contact->as<DigitalContact>();
      contactType->addItem(tr("Private Call"));
      contactType->addItem(tr("Group Call"));
      contactType->addItem(tr("All Call"));
      contactType->setItemData(0, uint(DigitalContact::PrivateCall));
      contactType->setItemData(1, uint(DigitalContact::GroupCall));
      contactType->setItemData(2, uint(DigitalContact::AllCall));
      contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
      if (DigitalContact::PrivateCall == digi->type())
        contactType->setCurrentIndex(0);
      else if (DigitalContact::GroupCall == digi->type())
        contactType->setCurrentIndex(1);
      else
        contactType->setCurrentIndex(2);
      contactNumber->setText(QString::number(digi->number()));
    }
    contactName->setText(_contact->name());
    contactName->setCompleter(nullptr);
    contactRxTone->setChecked(_contact->rxTone());
  } else {
    contactType->addItem(tr("Private Call"));
    contactType->addItem(tr("Group Call"));
    contactType->addItem(tr("All Call"));
    contactType->addItem(tr("DTMF"));
    contactType->setItemData(0, uint(DigitalContact::PrivateCall));
    contactType->setItemData(1, uint(DigitalContact::GroupCall));
    contactType->setItemData(2, uint(DigitalContact::AllCall));
    contactType->setCurrentIndex(0);
    contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    contactName->setCompleter(_completer);
  }

  connect(contactType, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void
ContactDialog::onTypeChanged(int idx) {
  if (3 == idx) {
    contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9a-dA-D\\*#]+")));
    contactNumber->setEnabled(true);
  } else if (2 == idx) {
    contactNumber->setText("16777215");
    contactNumber->setEnabled(false);
  } else {
    contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    contactNumber->setEnabled(true);
  }
}

void
ContactDialog::onCompleterActivated(const QModelIndex &idx) {
  if (nullptr == _completer)
    return;
  UserDatabase *db = qobject_cast<UserDatabase *>(_completer->model());
  if (nullptr == db)
    return;
  QAbstractProxyModel *model = qobject_cast<QAbstractProxyModel *>(_completer->completionModel());
  if (nullptr == model)
    return;
  QModelIndex srcidx = model->mapToSource(idx);
  contactNumber->setText(QString::number(db->user(srcidx.row()).id));
}

Contact *
ContactDialog::contact()
{
  if (_contact) {
    _contact->setName(contactName->text());
    _contact->setRXTone(contactRxTone->isChecked());
    if (_contact->is<DTMFContact>()) {
      _contact->as<DTMFContact>()->setNumber(contactNumber->text());
    } else {
      _contact->as<DigitalContact>()->setNumber(contactNumber->text().toUInt());
      _contact->as<DigitalContact>()->setType(
            DigitalContact::Type(contactType->currentData().toUInt()));
    }
    return _contact;
  }

  if (3 == contactType->currentIndex()) {
    // If DTMF contact
    return new DTMFContact(contactName->text(), contactNumber->text(), contactRxTone->isChecked());
  }

  DigitalContact::Type t = DigitalContact::Type(contactType->currentData().toUInt());
  return new DigitalContact(t, contactName->text(), contactNumber->text().toUInt(), contactRxTone->isChecked());
}


/* ********************************************************************************************* *
 * Implementation of DigitalContactBox
 * ********************************************************************************************* */
DigitalContactBox::DigitalContactBox(ContactList *contacts, QWidget *parent)
  : QComboBox(parent)
{
  for (int i=0; i<contacts->digitalCount(); i++) {
    DigitalContact *digi = contacts->digitalContact(i);
    addItem(digi->name(), QVariant::fromValue(digi));
  }
}
