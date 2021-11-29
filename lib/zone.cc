#include "zone.hh"
#include "channel.hh"
#include "config.hh"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QInputDialog>


/* ********************************************************************************************* *
 * Implementation of Zone
 * ********************************************************************************************* */
Zone::Zone(QObject *parent)
  : ConfigObject("zone", parent), _A(), _B()
{
  connect(&_A, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_A, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
}

Zone::Zone(const QString &name, QObject *parent)
  : ConfigObject(name, "zone", parent), _A(), _B()
{
  connect(&_A, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_A, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
}

Zone &
Zone::operator =(const Zone &other) {
  copy(other);
  return *this;
}

ConfigItem *
Zone::clone() const {
  Zone *z = new Zone();
  if (! z->copy(*this)) {
    z->deleteLater();
    return nullptr;
  }
  return z;
}

void
Zone::clear() {
  _name.clear();
  _A.clear();
  _B.clear();
}

const ChannelRefList *
Zone::A() const {
  return &_A;
}
ChannelRefList *Zone::A() {
  return &_A;
}

const ChannelRefList *Zone::B() const {
  return &_B;
}
ChannelRefList *Zone::B() {
  return &_B;
}

ConfigItem *
Zone::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx, const ErrorStack &err) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err)
  // There are no children yet.
  return nullptr;
}


/* ********************************************************************************************* *
 * Implementation of ZoneList
 * ********************************************************************************************* */
ZoneList::ZoneList(QObject *parent)
  : ConfigObjectList(Zone::staticMetaObject, parent)
{
  // pass...
}

Zone *
ZoneList::zone(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<Zone>();
  return nullptr;
}

int
ZoneList::add(ConfigObject *obj, int row) {
  if (obj && obj->is<Zone>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

ConfigItem *
ZoneList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)
  if (! node)
    return nullptr;

  if (! node.IsMap()) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create zone: Expected object.";
    return nullptr;
  }

  return new Zone();
}
