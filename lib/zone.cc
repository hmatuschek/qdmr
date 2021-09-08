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
Zone::Zone(const QString &name, QObject *parent)
  : ConfigObject("zone", parent), _name(name),
    _A(), _B()
{
  connect(&_A, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_A, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
}

const QString &
Zone::name() const {
  return _name;
}
bool
Zone::setName(const QString &name) {
  if (name.simplified().isEmpty())
    return false;
  _name = name;
  emit modified();
  return true;
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
  if (ConfigObject *obj = get(idx))
    return obj->as<Zone>();
  return nullptr;
}

int
ZoneList::add(ConfigObject *obj, int row) {
  if (obj && obj->is<Zone>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

