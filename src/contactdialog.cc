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
#include "talkgroupdatabase.hh"
#include <QDebug>


/* ********************************************************************************************* *
 * Implementation of ContactDialog
 * ********************************************************************************************* */
ContactDialog::ContactDialog(UserDatabase *users, TalkGroupDatabase *tgs, QWidget *parent)
  : QDialog(parent), _contact(nullptr), _user_completer(nullptr)
{
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

ContactDialog::ContactDialog(Contact *contact, QWidget *parent)
  : QDialog(parent), _contact(contact), _user_completer(nullptr), _tg_completer(nullptr)
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
    contactRxTone->setChecked(_contact->ring());
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
    contactName->setCompleter(_user_completer);
  }

  connect(contactType, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void
ContactDialog::onTypeChanged(int idx) {
  if (0 == idx) {
    contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    contactNumber->setEnabled(true);
    contactName->setCompleter(_user_completer);
  } else if (1 == idx) {
    contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    contactNumber->setEnabled(true);
    contactName->setCompleter(_tg_completer);
  } else if (2 == idx) {
    contactNumber->setText("16777215");
    contactNumber->setEnabled(false);
    contactName->setCompleter(nullptr);
  } else if (3 == idx) {
    contactNumber->setValidator(new QRegExpValidator(QRegExp("[0-9a-dA-D\\*#]+")));
    contactNumber->setEnabled(true);
    contactName->setCompleter(nullptr);
  }
}

void
ContactDialog::onCompleterActivated(const QModelIndex &idx) {

  if (0 == contactType->currentIndex()) { // Private call
    if (nullptr == _user_completer)
      return;
    UserDatabase *db = qobject_cast<UserDatabase *>(_user_completer->model());
    if (nullptr == db)
      return;
    QAbstractProxyModel *model = qobject_cast<QAbstractProxyModel *>(_user_completer->completionModel());
    if (nullptr == model)
      return;
    QModelIndex srcidx = model->mapToSource(idx);
    contactNumber->setText(QString::number(db->user(srcidx.row()).id));
  } else if (1 == contactType->currentIndex()) { // Group call
    if (nullptr == _tg_completer)
      return;
    TalkGroupDatabase *db = qobject_cast<TalkGroupDatabase *>(_tg_completer->model());
    if (nullptr == db)
      return;
    QAbstractProxyModel *model = qobject_cast<QAbstractProxyModel *>(_tg_completer->completionModel());
    if (nullptr == model)
      return;
    QModelIndex srcidx = model->mapToSource(idx);
    contactNumber->setText(QString::number(db->talkgroup(srcidx.row()).id));
  }
}

Contact *
ContactDialog::contact()
{
  if (_contact) {
    _contact->setName(contactName->text());
    _contact->setRing(contactRxTone->isChecked());
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
