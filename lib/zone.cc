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

ConfigItem *
Zone::allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx) {
  Q_UNUSED(prop); Q_UNUSED(node); Q_UNUSED(ctx)
  // There are no children yet.
  return nullptr;
}

Zone &
Zone::operator =(const Zone &other) {
  clear();
  _name = other._name;
  for (int i=0; i<other._A.count(); i++)
    _A.add(other._A.get(i));
  for (int i=0; i<other._B.count(); i++)
    _B.add(other._B.get(i));
  return *this;
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
ZoneList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx) {
  Q_UNUSED(ctx)
  if (! node)
    return nullptr;

  if (! node.IsMap()) {
    errMsg() << node.Mark().line << ":" << node.Mark().column
             << ": Cannot create zone: Expected object.";
    return nullptr;
  }

  return new Zone();
}
