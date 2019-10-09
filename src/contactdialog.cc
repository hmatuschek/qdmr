#include "contactdialog.hh"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QFormLayout>


/* ********************************************************************************************* *
 * Implementation of ContactDialog
 * ********************************************************************************************* */
ContactDialog::ContactDialog(QWidget *parent)
  : QDialog(parent), _contact(nullptr)
{
  construct();
}

ContactDialog::ContactDialog(Contact *contact, QWidget *parent)
  : QDialog(parent), _contact(contact)
{
  construct();
}

void
ContactDialog::construct() {
  setupUi(this);
  contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
  contactType->setItemData(0, uint(DigitalContact::PrivateCall));
  contactType->setItemData(1, uint(DigitalContact::GroupCall));
  contactType->setItemData(2, uint(DigitalContact::AllCall));

  if (_contact) {
    if (_contact->is<DTMFContact>()) {
      DTMFContact *dtmf = _contact->as<DTMFContact>();
      contactType->setEnabled(false);
      contactNumber->setText(dtmf->number());
    } else {
      DigitalContact *digi = _contact->as<DigitalContact>();
      if (DigitalContact::PrivateCall == digi->type())
        contactType->setCurrentIndex(0);
      else if (DigitalContact::GroupCall == digi->type())
        contactType->setCurrentIndex(1);
      else
        contactType->setCurrentIndex(2);
      contactNumber->setText(QString::number(digi->number()));
    }
    contactName->setText(_contact->name());
    contactRxTone->setChecked(_contact->rxTone());
  }

  connect(contactType, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void
ContactDialog::onTypeChanged() {
  // pass...
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

  DigitalContact::Type t = DigitalContact::Type(contactType->currentData().toUInt());
  return new DigitalContact(t, contactName->text(), contactNumber->text().toUInt(), contactRxTone->isChecked());
}


/* ********************************************************************************************* *
 * Implementation of DigitalContactBox
 * ********************************************************************************************* */
DigitalContactBox::DigitalContactBox(ContactList *contacts, QWidget *parent)
  : QComboBox(parent)
{
  for (int i=0; i<contacts->rowCount(QModelIndex()); i++) {
    if (contacts->contact(i)->is<DigitalContact>()) {
      DigitalContact *digi = contacts->contact(i)->as<DigitalContact>();
      addItem(digi->name(), QVariant::fromValue(digi));
    }
  }
}
