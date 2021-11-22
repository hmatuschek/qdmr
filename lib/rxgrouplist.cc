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
RXGroupList::RXGroupList(QObject *parent)
  : ConfigObject("grp", parent), _contacts()
{
  connect(&_contacts, SIGNAL(elementModified(int)), this, SLOT(onModified()));
  connect(&_contacts, SIGNAL(elementRemoved(int)), this, SLOT(onModified()));
  connect(&_contacts, SIGNAL(elementAdded(int)), this, SLOT(onModified()));
}

RXGroupList::RXGroupList(const QString &name, QObject *parent)
  : ConfigObject(name, "grp", parent), _contacts()
{
  connect(&_contacts, SIGNAL(elementModified(int)), this, SLOT(onModified()));
  connect(&_contacts, SIGNAL(elementRemoved(int)), this, SLOT(onModified()));
  connect(&_contacts, SIGNAL(elementAdded(int)), this, SLOT(onModified()));
}

RXGroupList &
RXGroupList::operator =(const RXGroupList &other) {
  copy(other);
  return *this;
}

ConfigItem *
RXGroupList::clone() const {
  RXGroupList *lst = new RXGroupList();
  if (! lst->copy(*this)) {
    lst->deleteLater();
    return nullptr;
  }
  return lst;
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

YAML::Node
RXGroupList::serialize(const Context &context) {
  YAML::Node node = ConfigObject::serialize(context);
  node.SetStyle(YAML::EmitterStyle::Flow);
  return node;
}

ConfigItem *
RXGroupList::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                           const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no children yet.
  return nullptr;
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
  if (ConfigItem *obj = get(idx))
    return obj->as<RXGroupList>();
  return nullptr;
}

int
RXGroupLists::add(ConfigObject *obj, int row) {
  if (obj && obj->is<RXGroupList>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

ConfigItem *
RXGroupLists::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)

  if (! node)
    return nullptr;

  if (! node.IsMap()) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create group list: Expected object.";
    return nullptr;
  }

  return new RXGroupList();
}

