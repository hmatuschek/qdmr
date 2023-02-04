#include "contact.hh"
#include "config.hh"
#include "utils.hh"
#include "logger.hh"
#include "opengd77_extension.hh"
#include <QRegularExpression>


/* ********************************************************************************************* *
 * Implementation of Contact
 * ********************************************************************************************* */
Contact::Contact(QObject *parent)
  : ConfigObject(parent), _ring(false)
{
  // pass...
}

Contact::Contact(const QString &name, bool rxTone, QObject *parent)
  : ConfigObject(name, parent), _ring(rxTone)
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
 * Implementation of AnalogContact
 * ********************************************************************************************* */
AnalogContact::AnalogContact(QObject *parent)
  : Contact(parent)
{
  // pass...
}

AnalogContact::AnalogContact(const QString &name, bool ring, QObject *parent)
  : Contact(name, ring, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DTMFContact
 * ********************************************************************************************* */
DTMFContact::DTMFContact(QObject *parent)
  : AnalogContact(parent), _number()
{
  // pass...
}

DTMFContact::DTMFContact(const QString &name, const QString &number, bool rxTone, QObject *parent)
  : AnalogContact(name, rxTone, parent), _number(number.simplified())
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
DTMFContact::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = AnalogContact::serialize(context, err);
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
  : Contact(parent)
{
  // pass...
}

DigitalContact::DigitalContact(const QString &name, bool ring, QObject *parent)
  : Contact(name, ring, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DMRContact
 * ********************************************************************************************* */
DMRContact::DMRContact(QObject *parent)
  : DigitalContact(parent), _type(PrivateCall), _number(0), _anytone(nullptr), _openGD77(nullptr)
{
  // pass...
}

DMRContact::DMRContact(Type type, const QString &name, unsigned number, bool rxTone, QObject *parent)
  : DigitalContact(name, rxTone, parent), _type(type), _number(number), _anytone(nullptr), _openGD77(nullptr)
{
  // pass...
}

ConfigItem *
DMRContact::clone() const {
  DMRContact *c = new DMRContact();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

void
DMRContact::clear() {
  DigitalContact::clear();
  _type = PrivateCall;
  _number = 0;
  if (_openGD77)
    _openGD77->deleteLater();
  _openGD77 = nullptr;
  if (_anytone)
    _anytone->deleteLater();
  _anytone = nullptr;
}

DMRContact::Type
DMRContact::type() const {
  return _type;
}

void
DMRContact::setType(DMRContact::Type type) {
  _type = type;
}

unsigned
DMRContact::number() const {
  return _number;
}

bool
DMRContact::setNumber(unsigned number) {
  _number = number;
  emit modified(this);
  return true;
}

YAML::Node
DMRContact::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = DigitalContact::serialize(context, err);
  if (node.IsNull())
    return node;

  node.SetStyle(YAML::EmitterStyle::Flow);
  YAML::Node type; type["dmr"] = node;
  return type;
}


OpenGD77ContactExtension *
DMRContact::openGD77ContactExtension() const {
  return _openGD77;
}

void
DMRContact::setOpenGD77ContactExtension(OpenGD77ContactExtension *ext) {
  if (_openGD77)
    _openGD77->deleteLater();
  _openGD77 = ext;
  if (_openGD77) {
    _openGD77->setParent(this);
    connect(_openGD77, &OpenGD77ContactExtension::modified,
            [this](ConfigItem*){ emit modified(this); });
  }
}

AnytoneContactExtension *
DMRContact::anytoneExtension() const {
  return _anytone;
}

void
DMRContact::setAnytoneExtension(AnytoneContactExtension *ext) {
  if (_anytone)
    _anytone->deleteLater();
  _anytone = ext;
  if (_anytone) {
    _anytone->setParent(this);
    connect(_anytone, &OpenGD77ContactExtension::modified,
            [this](ConfigItem*){ emit modified(this); });
  }
}


/* ********************************************************************************************* *
 * Implementation of M17Contact
 * ********************************************************************************************* */
M17Contact::M17Contact(QObject *parent)
  : DigitalContact(parent), _isBroadcast(false), _call()
{
  // pass...
}

M17Contact::M17Contact(const QString &name, bool ring, const QString &call, QObject *parent)
  : DigitalContact(name, ring, parent), _isBroadcast(false), _call()
{
  _call = normalizeCall(call);
}

ConfigItem *
M17Contact::clone() const {
  M17Contact *c = new M17Contact();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

void
M17Contact::clear() {
  DigitalContact::clear();
  _isBroadcast = false;
  _call.clear();
}

const QString &
M17Contact::call() const {
  return _call;
}

void
M17Contact::setCall(const QString &call) {
  _call = normalizeCall(call);
}

bool
M17Contact::isBroadcast() const {
  return _isBroadcast;
}

void
M17Contact::setBroadcast(bool enable) {
  _isBroadcast = enable;
}

YAML::Node
M17Contact::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = DigitalContact::serialize(context, err);
  if (node.IsNull())
    return node;

  node.SetStyle(YAML::EmitterStyle::Flow);
  YAML::Node type; type["m17"] = node;
  return type;
}

QString
M17Contact::normalizeCall(const QString call) {
  QString tmp = call.toUpper();
  tmp.remove(QRegularExpression("[^A-Z0-9/\\.\\-]"));
  return tmp.mid(0, 9);
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

Contact *
ContactList::contact(int idx) const {
  if ((0>idx) || (idx >= count()))
    return nullptr;
  return _items[idx]->as<Contact>();
}

DMRContact *
ContactList::findDMRContact(unsigned number) const {
  for (int i=0; i<_items.size(); i++) {
    if (! _items.at(i)->is<DMRContact>())
      continue;
    if (_items.at(i)->as<DMRContact>()->number() == number)
      return _items.at(i)->as<DMRContact>();
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
    return new DMRContact();
  } else if ("m17" == type) {
    return new M17Contact();
  } else if ("dtmf" == type) {
    return new DTMFContact();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create contact: Unknown type '" << type << "'.";

  return nullptr;
}
