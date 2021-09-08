#include "rxgrouplist.hh"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QInputDialog>
#include "contact.hh"
#include "config.hh"


/* ********************************************************************************************* *
 * Implementation of RXGroupList
 * ********************************************************************************************* */
RXGroupList::RXGroupList(const QString &name, QObject *parent)
  : ConfigObject("grp", parent), _name(name), _contacts()
{
  connect(&_contacts, SIGNAL(elementModified(int)), this, SLOT(onModified()));
  connect(&_contacts, SIGNAL(elementRemoved(int)), this, SLOT(onModified()));
  connect(&_contacts, SIGNAL(elementAdded(int)), this, SLOT(onModified()));
}

int
RXGroupList::count() const {
  return _contacts.count();
}

void
RXGroupList::clear() {
  _contacts.clear();
  emit modified(this);
}

const QString &
RXGroupList::name() const {
  return _name;
}

bool
RXGroupList::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name.simplified();
  return true;
}

DigitalContact *
RXGroupList::contact(int idx) const {
  if (idx >= _contacts.count())
    return nullptr;
  return _contacts.get(idx)->as<DigitalContact>();
}

int
RXGroupList::addContact(DigitalContact *contact, int idx) {
  return _contacts.add(contact, idx);
}

bool
RXGroupList::remContact(int idx) {
  return _contacts.del(_contacts.get(idx));
}

const DigitalContactRefList *
RXGroupList::contacts() const {
  return &_contacts;
}

DigitalContactRefList *
RXGroupList::contacts() {
  return &_contacts;
}

void
RXGroupList::onModified() {
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of RXGroupLists
 * ********************************************************************************************* */
RXGroupLists::RXGroupLists(QObject *parent)
  : ConfigObjectList(RXGroupList::staticMetaObject, parent)
{
  // pass...
}


RXGroupList *
RXGroupLists::list(int idx) const {
  if (ConfigObject *obj = get(idx))
    return obj->as<RXGroupList>();
  return nullptr;
}

int
RXGroupLists::add(ConfigObject *obj, int row) {
  if (obj && obj->is<RXGroupList>())
    return ConfigObjectList::add(obj, row);
  return -1;
}


