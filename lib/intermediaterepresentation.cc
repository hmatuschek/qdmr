#include "intermediaterepresentation.hh"
#include "configobject.hh"
#include "zone.hh"
#include "encryptionextension.hh"



/* ********************************************************************************************* *
 * Implementation of ZoneSplitVisitor
 * ********************************************************************************************* */
ZoneSplitVisitor::ZoneSplitVisitor()
  : Visitor()
{
  // pass...
}

bool
ZoneSplitVisitor::processItem(ConfigItem *item, const ErrorStack &err) {
  // Skip non-zones
  if (! item->is<Zone>())
    return Visitor::processItem(item, err);
  Zone *zone = item->as<Zone>();

  // skip zones with empty B list
  if (0 == zone->B()->count())
    return Visitor::processItem(item, err);

  // create new zone with B list as A list, clear B list of "old" zone
  Zone *newZone = new Zone();
  while (zone->B()->count()) {
    newZone->A()->add(zone->B()->get(0));
    zone->B()->take(zone->B()->get(0));
  }

  // set names
  newZone->setName(QString("%1 B").arg(zone->name()));
  zone->setName(QString("%1 A").arg(zone->name()));

  // add zone to list of zones
  ConfigObjectList *lst = qobject_cast<ConfigObjectList*>(item->parent());
  int idx = lst->indexOf(zone);
  lst->add(newZone, idx+1);

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ZoneMergeVisitor
 * ********************************************************************************************* */
ZoneMergeVisitor::ZoneMergeVisitor()
  : Visitor(), _lastZone(nullptr), _mergedZones()
{
  // pass...
}

bool
ZoneMergeVisitor::processList(AbstractConfigObjectList *list, const ErrorStack &err) {
  if (nullptr == qobject_cast<ZoneList *>(list))
    return Visitor::processList(list, err);

  if (2 > list->count())
    return Visitor::processList(list, err);

  _mergedZones.clear();
  _lastZone = nullptr;

  if (! processItem(list->get(0), err))
    return false;
  _lastZone = list->get(0)->as<Zone>();

  for (int i=1; i<list->count(); i++) {
    if (! processItem(list->get(i), err))
      return false;
    _lastZone = list->get(i)->as<Zone>();
  }

  foreach(Zone *zone, _mergedZones) {
    list->del(zone);
  }
  _mergedZones.clear();

  return true;
}

bool
ZoneMergeVisitor::processItem(ConfigItem *item, const ErrorStack &err) {
  if (! item->is<Zone>())
    return Visitor::processItem(item, err);
  Zone *currentZone = item->as<Zone>();

  if ((! _lastZone) || (!_lastZone->name().endsWith(" A")) || (0 != _lastZone->B()->count()))
    return Visitor::processItem(item, err);

  if ((!currentZone->name().endsWith(" B")) || (0 != currentZone->B()->count()))
    return Visitor::processItem(item, err);

  while (currentZone->A()->count()) {
    ConfigObject *ch = currentZone->A()->get(0);
    _lastZone->B()->add(ch);
    currentZone->A()->del(ch);
  }

  _lastZone->setName(_lastZone->name().chopped(2));
  _mergedZones.append(currentZone);

  return Visitor::processItem(item);
}



/* ********************************************************************************************* *
 * Implementation of AbstractObjectFilterVisitor
 * ********************************************************************************************* */
AbstractObjectFilterVisitor::AbstractObjectFilterVisitor()
  : Visitor()
{
  // pass...
}

bool
AbstractObjectFilterVisitor::processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err) {
  if (! propIsInstance<ConfigItem>(prop))
    return Visitor::processProperty(item, prop, err);

  if (prop.read(item).isNull())
    return Visitor::processProperty(item, prop, err);

  if (! prop.isWritable())
    return Visitor::processProperty(item, prop, err);

  ConfigItem *propItem = prop.read(item).value<ConfigItem *>();
  if (toRemove(propItem)) {
    if (prop.write(item, QVariant(prop.metaType())))
      return true;
    errMsg(err) << "Cannot delete instance of " << item->metaObject()->className()
                << " from property " << prop.name() << ".";
    return false;
  }

  return Visitor::processProperty(item, prop, err);
}

bool
AbstractObjectFilterVisitor::processList(AbstractConfigObjectList *list, const ErrorStack &err) {
  if (qobject_cast<ConfigObjectRefList *>(list))
    return Visitor::processList(list, err);

  ConfigObjectList *objList = qobject_cast<ConfigObjectList *>(list);
  if (nullptr == objList)
    return Visitor::processList(list, err);;

  QList<ConfigObject *> filtered;
  for (int i=0; i<objList->count(); i++) {
    if (toRemove(objList->get(i)))
      filtered.append(objList->get(i));
  }

  foreach (ConfigObject *item, filtered)
    objList->del(item);

  return Visitor::processList(list, err);
}



/* ********************************************************************************************* *
 * Implementation of ObjectFilterVisitor
 * ********************************************************************************************* */
ObjectFilterVisitor::ObjectFilterVisitor(const std::initializer_list<QMetaObject> &types)
  : AbstractObjectFilterVisitor(), _filter(types)
{
  // pass...
}

bool
ObjectFilterVisitor::toRemove(ConfigItem *item) {
  foreach (const QMetaObject &meta, _filter)
    if (item->inherits(meta.className()))
      return true;
  return false;
}



/* ********************************************************************************************* *
 * Implementation of EncryptionKeyFilterVisitor::Filter
 * ********************************************************************************************* */
EncryptionKeyFilterVisitor::Filter::Filter(const QMetaObject &meta_type, unsigned int bits)
  : type(meta_type), minBits(bits), maxBits(bits)
{
  // pass...
}

EncryptionKeyFilterVisitor::Filter::Filter(const QMetaObject &meta_type, unsigned int min_bits, unsigned int max_bits)
  : type(meta_type), minBits(min_bits), maxBits(max_bits)
{
  // pass...
}

bool
EncryptionKeyFilterVisitor::Filter::accepts(const EncryptionKey *key) const {
  return key->inherits(type.className()) && (key->key().size()*8>=minBits)
      && (key->key().size()*8<=maxBits);
}


/* ********************************************************************************************* *
 * Implementation of EncryptionKeyFilterVisitor
 * ********************************************************************************************* */
EncryptionKeyFilterVisitor::EncryptionKeyFilterVisitor(const std::initializer_list<Filter> &filter)
  : AbstractObjectFilterVisitor(), _filter(filter)
{
  // pass...
}

bool
EncryptionKeyFilterVisitor::toRemove(ConfigItem *item) {
  if (! item->is<EncryptionKey>())
    return false;

  auto key = item->as<EncryptionKey>();
  foreach (const Filter &filter, _filter) {
    if (filter.accepts(key))
      return false;
  }
  return true;
}
