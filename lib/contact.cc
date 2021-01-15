#include "contact.hh"
#include "config.hh"
#include "utils.hh"


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

bool
DTMFContact::isValid() const {
  return validDTMFNumber(_number);
}

const QString &
DTMFContact::number() const {
  return _number;
}

bool
DTMFContact::setNumber(const QString &number) {
  if (! validDTMFNumber(number))
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

int
ContactList::digitalCount() const {
  int c=0;
  for (int i=0; i<_contacts.size(); i++)
    if (_contacts.at(i)->is<DigitalContact>())
      c++;
  return c;
}

int
ContactList::dtmfCount() const {
  int c=0;
  for (int i=0; i<_contacts.size(); i++)
    if (_contacts.at(i)->is<DTMFContact>())
      c++;
  return c;
}

void
ContactList::clear() {
  for (int i=0; i<count(); i++)
    _contacts[i]->deleteLater();
  _contacts.clear();
}

int
ContactList::indexOf(Contact *contact) const {
  if (! _contacts.contains(contact))
    return -1;
  return _contacts.indexOf(contact);
}

int
ContactList::indexOfDigital(DigitalContact *contact) const {
  int idx = 0;
  for (int i=0; i<_contacts.size(); i++) {
    if (_contacts.at(i) == contact)
      return idx;
    else if (_contacts.at(i)->is<DigitalContact>())
      idx++;
  }
  return -1;
}

int
ContactList::indexOfDTMF(DTMFContact *contact) const {
  int idx = 0;
  for (int i=0; i<_contacts.size(); i++) {
    if (_contacts.at(i) == contact)
      return idx;
    else if (_contacts.at(i)->is<DTMFContact>())
      idx++;
  }
  return -1;
}

Contact *
ContactList::contact(int idx) const {
  if (idx >= _contacts.size())
    return nullptr;
  return _contacts[idx];
}

DigitalContact *
ContactList::digitalContact(int idx) const {
  for (int i=0; i<_contacts.size(); i++) {
    if (_contacts.at(i)->is<DigitalContact>()) {
      if (0 == idx)
        return _contacts.at(i)->as<DigitalContact>();
      else
        idx--;
    }
  }
  return nullptr;
}

DigitalContact *
ContactList::findDigitalContact(uint number) const {
  for (int i=0; i<_contacts.size(); i++) {
    if (! _contacts.at(i)->is<DigitalContact>())
      continue;
    if (_contacts.at(i)->as<DigitalContact>()->number() == number)
      return _contacts.at(i)->as<DigitalContact>();
  }
  return nullptr;
}

DTMFContact *
ContactList::dtmfContact(int idx) const {
  for (int i=0; i<_contacts.size(); i++) {
    if (_contacts.at(i)->is<DTMFContact>()) {
      if (0 == idx)
        return _contacts.at(i)->as<DTMFContact>();
      else
        idx--;
    }
  }
  return nullptr;
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
ContactList::addContact(Contact *contact, int row) {
  if (_contacts.contains(contact)) {
    return _contacts.indexOf(contact);
  }
  if ((row<0) || (row>_contacts.size()))
    row = _contacts.size();
  contact->setParent(this);
  connect(contact, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDeleted(QObject*)));
  connect(contact, SIGNAL(modified()), this, SIGNAL(modified()));
  beginInsertRows(QModelIndex(), row, row);
  _contacts.insert(row, contact);
  endInsertRows();
  emit modified();
  return row;
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
          return tr("DTMF");
        case 1:
          return dtmf->name();
        case 2:
          return dtmf->number();
        case 3:
          return (dtmf->rxTone() ? tr("On") : tr("Off"));
        default:
          return QVariant();
      }
    } else if (contact->is<DigitalContact>()) {
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


