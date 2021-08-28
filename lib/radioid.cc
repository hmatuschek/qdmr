#include "radioid.hh"


/* ********************************************************************************************* *
 * Implementation of RadioID
 * ********************************************************************************************* */
RadioID::RadioID(uint32_t id, QObject *parent)
  : ConfigObject("id", parent), _id(id)
{
  // pass...
}

uint32_t
RadioID::id() const {
  return _id;
}

void
RadioID::setId(uint32_t id) {
  if (id == _id)
    return;
  _id = id;
  emit modified();
}


/* ********************************************************************************************* *
 * Implementation of RadioIDList
 * ********************************************************************************************* */
RadioIDList::RadioIDList(QObject *parent)
  : ConfigObjectList(parent)
{
  add(new RadioID(0));
}

void
RadioIDList::clear() {
  ConfigObjectList::clear();
  add(new RadioID(0));
}

RadioID *
RadioIDList::getId(int idx) const {
  if (ConfigObject *obj = get(idx))
    return obj->as<RadioID>();
  return nullptr;
}

RadioID *
RadioIDList::getDefaultId() const {
  return getId(0);
}

RadioID *
RadioIDList::find(uint32_t id) const {
  for (int i=0; i<count(); i++) {
    if (id == getId(i)->id())
      return getId(i);
  }
  return nullptr;
}

int
RadioIDList::add(ConfigObject *obj, int row) {
  if (obj && obj->is<RadioID>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

int
RadioIDList::addId(uint32_t id) {
  return add(new RadioID(id, this));
}

bool
RadioIDList::setDefault(uint idx) {
  RadioID *obj = getId(idx);
  if (nullptr == obj)
    return false;
  _items.removeAt(idx);
  _items.prepend(obj);
  emit modified();
  return true;
}

bool
RadioIDList::delId(uint32_t id) {
  return del(find(id));
}
