#include "contact.hh"
#include "config.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of Contact
 * ********************************************************************************************* */
Contact::Contact(const QString &name, bool rxTone, QObject *parent)
  : ConfigObject("cont", parent), _name(name), _rxTone(rxTone)
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
  : ConfigObjectList(parent)
{
  // pass...
}

int
ContactList::add(ConfigObject *obj, int row) {
  if ((nullptr == obj) || (obj->is<Contact>()))
    return -1;
  return ConfigObjectList::add(obj, row);
}

int
ContactList::digitalCount() const {
  int c=0;
  for (int i=0; i<_items.size(); i++)
    if (_items.at(i)->is<DigitalContact>())
      c++;
  return c;
}

int
ContactList::dtmfCount() const {
  int c=0;
  for (int i=0; i<_items.size(); i++)
    if (_items.at(i)->is<DTMFContact>())
      c++;
  return c;
}


int
ContactList::indexOfDigital(DigitalContact *contact) const {
  int idx = 0;
  for (int i=0; i<_items.size(); i++) {
    if (_items.at(i) == contact)
      return idx;
    else if (_items.at(i)->is<DigitalContact>())
      idx++;
  }
  return -1;
}

int
ContactList::indexOfDTMF(DTMFContact *contact) const {
  int idx = 0;
  for (int i=0; i<_items.size(); i++) {
    if (_items.at(i) == contact)
      return idx;
    else if (_items.at(i)->is<DTMFContact>())
      idx++;
  }
  return -1;
}

Contact *
ContactList::contact(int idx) const {
  if ((0>idx) || (idx >= count()))
    return nullptr;
  return _items[idx]->as<Contact>();
}

DigitalContact *
ContactList::digitalContact(int idx) const {
  for (int i=0; i<_items.size(); i++) {
    if (_items.at(i)->is<DigitalContact>()) {
      if (0 == idx)
        return _items.at(i)->as<DigitalContact>();
      else
        idx--;
    }
  }
  return nullptr;
}

DigitalContact *
ContactList::findDigitalContact(uint number) const {
  for (int i=0; i<_items.size(); i++) {
    if (! _items.at(i)->is<DigitalContact>())
      continue;
    if (_items.at(i)->as<DigitalContact>()->number() == number)
      return _items.at(i)->as<DigitalContact>();
  }
  return nullptr;
}

DTMFContact *
ContactList::dtmfContact(int idx) const {
  for (int i=0; i<_items.size(); i++) {
    if (_items.at(i)->is<DTMFContact>()) {
      if (0 == idx)
        return _items.at(i)->as<DTMFContact>();
      else
        idx--;
    }
  }
  return nullptr;
}
