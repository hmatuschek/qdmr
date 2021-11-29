#include "contact.hh"
#include "config.hh"
#include "utils.hh"
#include "logger.hh"
#include "opengd77_extension.hh"

/* ********************************************************************************************* *
 * Implementation of Contact
 * ********************************************************************************************* */
Contact::Contact(QObject *parent)
  : ConfigObject("cont", parent), _ring(false)
{
  // pass...
}

Contact::Contact(const QString &name, bool rxTone, QObject *parent)
  : ConfigObject(name, "cont", parent), _ring(rxTone)
{
  // pass...
}

void
Contact::clear() {
  ConfigObject::clear();
  _ring = false;
}

bool
Contact::ring() const {
  return _ring;
}
void
Contact::setRing(bool enable) {
  _ring = enable;
  emit modified(this);
}

ConfigItem *
Contact::allocateChild(QMetaProperty &prop, const YAML::Node &node,
                       const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no children yet
  return nullptr;
}

bool
Contact::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse contact: Expected object with one child.";
    return false;
  }

  YAML::Node cnt = node.begin()->second;
  return ConfigObject::parse(cnt, ctx, err);
}

bool
Contact::link(const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  return ConfigObject::link(node.begin()->second, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of DTMFContact
 * ********************************************************************************************* */
DTMFContact::DTMFContact(QObject *parent)
  : Contact(parent), _number()
{
  // pass...
}

DTMFContact::DTMFContact(const QString &name, const QString &number, bool rxTone, QObject *parent)
  : Contact(name, rxTone, parent), _number(number.simplified())
{
  // pass...
}

ConfigItem *
DTMFContact::clone() const {
  DTMFContact *c = new DTMFContact();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

void
DTMFContact::clear() {
  Contact::clear();
  _number.clear();
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
  emit modified(this);
  return true;
}

YAML::Node
DTMFContact::serialize(const Context &context) {
  YAML::Node node = Contact::serialize(context);
  if (node.IsNull())
    return node;

  node.SetStyle(YAML::EmitterStyle::Flow);
  YAML::Node type; type["dtmf"] = node;
  return type;
}


/* ********************************************************************************************* *
 * Implementation of DigitalContact
 * ********************************************************************************************* */
DigitalContact::DigitalContact(QObject *parent)
  : Contact(parent), _type(PrivateCall), _number(0), _openGD77(nullptr)
{
  // pass...
}

DigitalContact::DigitalContact(Type type, const QString &name, unsigned number, bool rxTone, QObject *parent)
  : Contact(name, rxTone, parent), _type(type), _number(number), _openGD77(nullptr)
{
  // pass...
}

ConfigItem *
DigitalContact::clone() const {
  DigitalContact *c = new DigitalContact();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

void
DigitalContact::clear() {
  Contact::clear();
  _type = PrivateCall;
  _number = 0;
  if (_openGD77)
    _openGD77->deleteLater();
  _openGD77 = nullptr;
}

DigitalContact::Type
DigitalContact::type() const {
  return _type;
}

void
DigitalContact::setType(DigitalContact::Type type) {
  _type = type;
}

unsigned
DigitalContact::number() const {
  return _number;
}

bool
DigitalContact::setNumber(unsigned number) {
  _number = number;
  emit modified(this);
  return true;
}

ConfigItem *
DigitalContact::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)

  if (0 == strcmp("openGD77", prop.name()))
    return new OpenGD77ContactExtension();
  return nullptr;
}

YAML::Node
DigitalContact::serialize(const Context &context) {
  YAML::Node node = Contact::serialize(context);
  if (node.IsNull())
    return node;

  node.SetStyle(YAML::EmitterStyle::Flow);
  YAML::Node type; type["dmr"] = node;
  return type;
}


OpenGD77ContactExtension *
DigitalContact::openGD77ContactExtension() const {
  return _openGD77;
}

void
DigitalContact::setOpenGD77ContactExtension(OpenGD77ContactExtension *ext) {
  if (_openGD77)
    _openGD77->deleteLater();
  _openGD77 = ext;
  if (_openGD77) {
    _openGD77->setParent(this);
    connect(_openGD77, &OpenGD77ContactExtension::modified,
            [this](ConfigItem*){ emit modified(this); });
  }
}

/* ********************************************************************************************* *
 * Implementation of ContactList
 * ********************************************************************************************* */
ContactList::ContactList(QObject *parent)
  : ConfigObjectList(Contact::staticMetaObject, parent)
{
  // pass...
}

int
ContactList::add(ConfigObject *obj, int row) {
  if ((nullptr == obj) || (! obj->is<Contact>()))
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
ContactList::findDigitalContact(unsigned number) const {
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

ConfigItem *
ContactList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)

  if (! node)
    return nullptr;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create contact: Expected object with one child.";
    return nullptr;
  }

  QString type = QString::fromStdString(node.begin()->first.as<std::string>());
  if ("dmr" == type) {
    return new DigitalContact();
  } else if ("dtmf" == type) {
    return new DTMFContact();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create contact: Unknown type '" << type << "'.";

  return nullptr;
}
