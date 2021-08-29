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
#include "config.hh"


/* ********************************************************************************************* *
 * Implementation of RXGroupList
 * ********************************************************************************************* */
RXGroupList::RXGroupList(const QString &name, QObject *parent)
  : ConfigObject("grp", parent), _name(name), _contacts()
{
  // pass...
}

int
RXGroupList::count() const {
  return _contacts.size();
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
  if (idx >= _contacts.size())
    return nullptr;
  return _contacts.at(idx);
}

int
RXGroupList::addContact(DigitalContact *contact) {
  if (_contacts.contains(contact))
    return -1;
  int idx = _contacts.size();
  connect(contact, SIGNAL(modified(ConfigObject*)), this, SIGNAL(modified(ConfigObject*)));
  connect(contact, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDeleted(QObject*)));
  _contacts.append(contact);
  emit modified(this);
  return idx;
}

bool
RXGroupList::remContact(int idx) {
  if (idx >= _contacts.size())
    return false;
  _contacts.remove(idx);
  emit modified(this);
  return true;
}

bool
RXGroupList::remContact(DigitalContact *contact) {
  if (! _contacts.contains(contact))
    return false;
  int idx = _contacts.indexOf(contact);
  return remContact(idx);
}

void
RXGroupList::onContactDeleted(QObject *obj) {
  if (DigitalContact *contact = reinterpret_cast<DigitalContact *>(obj))
    remContact(contact);
}

bool
RXGroupList::serialize(YAML::Node &node, const Context &context) {
  if (! ConfigObject::serialize(node, context))
    return false;
  node["name"] = _name.toStdString();
  YAML::Node contacts = YAML::Node(YAML::NodeType::Sequence);
  contacts.SetStyle(YAML::EmitterStyle::Flow);
  foreach (DigitalContact *contact, _contacts) {
    if (! context.contains(contact))
      return false;
    contacts.push_back(context.getId(contact).toStdString());
  }
  node["contacts"] = contacts;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RXGroupLists
 * ********************************************************************************************* */
RXGroupLists::RXGroupLists(QObject *parent)
  : ConfigObjectList(parent)
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


