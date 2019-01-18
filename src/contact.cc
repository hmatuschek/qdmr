#include "contact.hh"
#include "config.hh"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QFormLayout>
#include <QDebug>


/* ********************************************************************************************* *
 * Implementation of Contact
 * ********************************************************************************************* */
Contact::Contact(const QString &name, bool rxTone, QObject *parent)
  : QObject(parent), _name(name), _rxTone(rxTone)
{
  // pass...
}

const QString &
Contact::name() const {
  return _name;
}
bool
Contact::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name;
  emit modified();
  return true;
}

bool
Contact::rxTone() const {
  return _rxTone;
}
void
Contact::setRXTone(bool enable) {
  _rxTone = enable;
  emit modified();
}


/* ********************************************************************************************* *
 * Implementation of DTMFContact
 * ********************************************************************************************* */
DTMFContact::DTMFContact(const QString &name, const QString &number, bool rxTone, QObject *parent)
  : Contact(name, rxTone, parent), _number(number.simplified())
{
  // pass...
}

const QString &
DTMFContact::number() const {
  return _number;
}

bool
DTMFContact::setNumber(const QString &number) {
  QRegExp re("[0-9a-dA-D\\*#]+");
  if (! re.exactMatch(number.simplified()))
    return false;
  _number = number.simplified();
  emit modified();
  return true;
}


/* ********************************************************************************************* *
 * Implementation of DigitalContact
 * ********************************************************************************************* */
DigitalContact::DigitalContact(Type type, const QString &name, uint number, bool rxTone, QObject *parent)
  : Contact(name, rxTone, parent), _type(type), _number(number)
{
  // pass...
}

DigitalContact::Type
DigitalContact::type() const {
  return _type;
}

void
DigitalContact::setType(DigitalContact::Type type) {
  _type = type;
}

uint
DigitalContact::number() const {
  return _number;
}

bool
DigitalContact::setNumber(uint number) {
  _number = number;
  emit modified();
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ContactList
 * ********************************************************************************************* */
ContactList::ContactList(QObject *parent)
  : QAbstractTableModel(parent), _contacts()
{
  connect(this, SIGNAL(modified()), this, SLOT(onContactEdited()));
}

int
ContactList::count() const {
  return _contacts.size();
}

void
ContactList::clear() {
  for (int i=0; i<count(); i++)
    _contacts[i]->deleteLater();
}

int
ContactList::indexOf(Contact *contact) const {
  if (! _contacts.contains(contact))
    return -1;
  return _contacts.indexOf(contact);
}

Contact *
ContactList::contact(int idx) const {
  if (idx >= _contacts.size())
    return nullptr;
  return _contacts[idx];
}

bool
ContactList::remContact(int idx) {
  if (idx >= _contacts.size())
    return false;
  Contact *contact = _contacts[idx];
  beginRemoveRows(QModelIndex(), idx, idx);
  _contacts.remove(idx);
  endRemoveRows();
  contact->deleteLater();
  emit modified();
  return true;
}

bool
ContactList::remContact(Contact *contact) {
  if (! _contacts.contains(contact))
    return false;
  int idx = _contacts.indexOf(contact);
  return remContact(idx);
}

int
ContactList::addContact(Contact *contact) {
  if (_contacts.contains(contact)) {
    return _contacts.indexOf(contact);
  }
  int idx = _contacts.size();
  contact->setParent(this);
  connect(contact, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDeleted(QObject*)));
  connect(contact, SIGNAL(modified()), this, SIGNAL(modified()));
  beginInsertRows(QModelIndex(), idx, idx);
  _contacts.append(contact);
  endInsertRows();
  emit modified();
  return idx;
}

bool
ContactList::moveUp(int row) {
  if ((row <= 0) || (row>=count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  std::swap(_contacts[row-1],_contacts[row]);
  endMoveRows();
  emit modified();
  return true;
}

bool
ContactList::moveDown(int row) {
  if ((row >= (count()-1)) || (0 > row))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  std::swap(_contacts[row+1],_contacts[row]);
  endMoveRows();
  emit modified();
  return true;
}

int
ContactList::rowCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return count();
}

int
ContactList::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return 4;
}

QVariant
ContactList::data(const QModelIndex &index, int role) const {
  if ((!index.isValid()) || (index.row()>=_contacts.size()))
    return QVariant();

  if (Qt::DisplayRole == role) {
    Contact *contact = _contacts.at(index.row());
    if (contact->is<DTMFContact>()) {
      DTMFContact *dtmf = contact->as<DTMFContact>();
      switch (index.column()) {
        case 0:
          return tr("DTMF (analog)");
        case 1:
          return dtmf->name();
        case 2:
          return dtmf->number();
        case 3:
          return (dtmf->rxTone() ? tr("On") : tr("Off"));
        default:
          return QVariant();
      }
    } else {
      DigitalContact *digi = contact->as<DigitalContact>();
      switch (index.column()) {
        case 0:
          switch (digi->type()) {
            case DigitalContact::PrivateCall: return tr("Private Call");
            case DigitalContact::GroupCall: return tr("Group Call");
            case DigitalContact::AllCall: return tr("All Call");
          }
        case 1:
          return digi->name();
        case 2:
          return digi->number();
        case 3:
          return (digi->rxTone() ? tr("On") : tr("Off"));
        default:
          return QVariant();
      }
    }
  }

  return QVariant();
}


QVariant
ContactList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole != role) || (Qt::Horizontal != orientation)) {
    return QVariant();
  }
  if (0 == section) {
    return tr("Type");
  } else if (1 == section) {
    return tr("Name");
  } else if (2 == section) {
    return tr("Number");
  } else if (3 == section) {
    return tr("RX Tone");
  }
  return QVariant();
}

void
ContactList::onContactDeleted(QObject *obj) {
  if (Contact *contact = reinterpret_cast<Contact *>(obj)) {
    remContact(contact);
  }
}

void
ContactList::onContactEdited() {
  if (0 == count())
    return;
  QModelIndex tl = index(0,0), br = index(count()-1, columnCount(QModelIndex()));
  emit dataChanged(tl, br);
}


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
      else if (DigitalContact::PrivateCall == digi->type())
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

  DigitalContact::Type t;
  if (1 == contactType->currentIndex())
    t = DigitalContact::PrivateCall;
  else if (2 == contactType->currentIndex())
    t = DigitalContact::GroupCall;
  else
    t = DigitalContact::AllCall;

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
