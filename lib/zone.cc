#include "zone.hh"
#include "channel.hh"
#include "config.hh"
#include "anytone_extension.hh"
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
  : ConfigObject(parent), _A(), _B(), _anytone(nullptr)
{
  connect(&_A, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_A, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementAdded(int)), this, SIGNAL(modified()));
  connect(&_B, SIGNAL(elementRemoved(int)), this, SIGNAL(modified()));
}

Zone::Zone(const QString &name, QObject *parent)
  : ConfigObject(name, parent), _A(), _B(), _anytone(nullptr)
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
ChannelRefList *
Zone::A() {
  return &_A;
}

const ChannelRefList *
Zone::B() const {
  return &_B;
}
ChannelRefList *
Zone::B() {
  return &_B;
}

bool
Zone::contains(Channel *obj) const {
  return _A.has(obj) || _B.has(obj);
}

AnytoneZoneExtension *
Zone::anytoneExtension() const {
  return _anytone;
}
void
Zone::setAnytoneExtension(AnytoneZoneExtension *ext) {
  if (_anytone == ext)
    return;
  if (_anytone) {
    disconnect(_anytone, SIGNAL(modified(ConfigItem*)), this, SIGNAL(modified(ConfigItem*)));
    _anytone->deleteLater();
  }
  _anytone = ext;
  if (_anytone) {
    _anytone->setParent(this);
    connect(_anytone, SIGNAL(modified(ConfigItem*)), this, SIGNAL(modified(ConfigItem*)));
  }
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
