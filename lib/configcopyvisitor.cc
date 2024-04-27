#include "configcopyvisitor.hh"
#include "configobject.hh"
#include "configreference.hh"
#include "channel.hh"
#include "radioid.hh"
#include "roamingzone.hh"
#include "logger.hh"

/* ********************************************************************************************* *
 * Implementation of ConfigCloneVisitor
 * ********************************************************************************************* */
ConfigCloneVisitor::ConfigCloneVisitor(QHash<ConfigObject *, ConfigObject *> &map)
  : Visitor(), _map(map)
{
  // pass...
}


bool
ConfigCloneVisitor::processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err) {
  if (prop.isEnumType() || (QString("bool") == prop.typeName())
      || (QString("int") == prop.typeName()) || (QString("uint") == prop.typeName())
      || (QString("double") == prop.typeName()) || (QString("QString") == prop.typeName())
      || (QString("Frequency") == prop.typeName()) || (QString("Interval") == prop.typeName())) {
    if ((! prop.isReadable()) && (!prop.isWritable())) {
      logDebug() << "Skip property " << prop.name()
                 << " of item " << item->metaObject()->className() << ": Not readable or writable.";
      return true;
    }

    if (! Visitor::processProperty(item, prop, err))
      return false;

    // Get clone
    ConfigItem *clone = qobject_cast<ConfigItem*>(_stack.back());
    if (nullptr == clone) {
      errMsg(err) << "Unexpected element on the stack. Found object of type " <<
                     _stack.back()->metaObject()->className() << ", expected ConfigItem.";
      return false;
    }
    // Find the property
    int pidx = clone->metaObject()->indexOfProperty(prop.name());
    if (0 > pidx) {
      errMsg(err) << "Cannot set property " << prop.name() << " on element on stack.";
      return false;
    }
    // Set it
    if (! clone->metaObject()->property(pidx).write(clone, prop.read(item))) {
      errMsg(err) << "Cannot set property " << prop.name() << " on element on stack.";
      return false;
    }
    return true;
  } else if (propIsInstance<ConfigObjectReference>(prop)) {
    // Get clone
    ConfigItem *clone = qobject_cast<ConfigItem*>(_stack.back());
    if (nullptr == clone) {
      errMsg(err) << "Unexpected element on the stack. Found object of type " <<
                     _stack.back()->metaObject()->className() << ", expected ConfigItem.";
      return false;
    }
    // Set reference to same object, will be replaced later.
    ConfigObjectReference *ref = prop.read(item).value<ConfigObjectReference *>();
    ConfigObjectReference *cloneRef = prop.read(clone).value<ConfigObjectReference*>();
    cloneRef->set(ref->as<ConfigObject>());
    return true;
  } else if (propIsInstance<ConfigItem>(prop)) {
    if (nullptr == prop.read(item).value<ConfigItem *>())
      return true;
    // If writeable, simply traverse config item
    if (prop.isWritable()) {
      // Clone item in property recursively
      if(! this->processItem(prop.read(item).value<ConfigItem *>(), err))
        return false;
      // Get cloned item from stack
      ConfigItem *newItem = dynamic_cast<ConfigItem *>(_stack.back()); _stack.pop_back();
      // Also, get item from stack, who owns the clones item
      ConfigItem *clone = dynamic_cast<ConfigItem *>(_stack.back());
      int pidx = clone->metaObject()->indexOfProperty(prop.name());
      // Find the property
      if (0 > pidx) {
        errMsg(err) << "Cannot set property " << prop.name() << " on element on stack.";
        return false;
      }
      // Set it
      if (! clone->metaObject()->property(pidx).write(clone, QVariant::fromValue(newItem))) {
        errMsg(err) << "Cannot set property " << prop.name() << " on element on stack.";
        return false;
      }
      return true;
    }

    // If property is not writeable, the parent element (top of stack), owns and creates the item
    // -> needs to be put on the stack
    ConfigItem *clone = dynamic_cast<ConfigItem *>(_stack.back());
    if (nullptr == clone) {
      errMsg(err) << "Unexpected element on the stack. Found object of type " <<
                     _stack.back()->metaObject()->className() << ", expected ConfigItem.";
      return false;
    }

    int pidx = clone->metaObject()->indexOfProperty(prop.name());
    if (0 > pidx) {
      errMsg(err) << "Cannot read property " << prop.name() << " on element on stack.";
      return false;
    }

    QVariant value = clone->metaObject()->property(pidx).read(clone);
    if (!value.isValid()) {
      errMsg(err) << "Cannot read property " << prop.name() << " on element on stack.";
      return false;
    }

    _stack.push_back(value.value<ConfigItem *>());
    if (! Visitor::processItem(prop.read(item).value<ConfigItem *>(), err)) {
      errMsg(err) << "Cannot process property " << prop.name() << ".";
      return false;
    }
    _stack.pop_back();
    return true;
  } else if (ConfigObjectList *lst = prop.read(item).value<ConfigObjectList *>()) {
    // Lists are always owned by the item. So dig up the list and put it on the stack
    ConfigItem *clone = dynamic_cast<ConfigItem *>(_stack.back());
    if (nullptr == clone) {
      errMsg(err) << "Unexpected element on the stack. Found object of type " <<
                     _stack.back()->metaObject()->className() << ", expected ConfigItem.";
      return false;
    }

    int pidx = clone->metaObject()->indexOfProperty(prop.name());
    if (0 > pidx) {
      errMsg(err) << "Cannot read property " << prop.name() << " on element on stack.";
      return false;
    }

    QVariant value = clone->metaObject()->property(pidx).read(clone);
    if (!value.isValid()) {
      errMsg(err) << "Cannot read property " << prop.name() << " on element on stack.";
      return false;
    }

    _stack.push_back(value.value<ConfigObjectList *>());
    if (! processList(lst, err)) {
      errMsg(err) << "Cannot process object list in property " << prop.name() << ".";
      return false;
    }
    _stack.pop_back();
    return true;
  } else if (ConfigObjectRefList *refs = prop.read(item).value<ConfigObjectRefList *>()) {
    // Lists are always owned by the item. So dig up the parent and get the list
    ConfigItem *clone = dynamic_cast<ConfigItem *>(_stack.back());
    if (nullptr == clone) {
      errMsg(err) << "Unexpected element on the stack. Found object of type " <<
                     _stack.back()->metaObject()->className() << ", expected ConfigItem.";
      return false;
    }

    int pidx = clone->metaObject()->indexOfProperty(prop.name());
    if (0 > pidx) {
      errMsg(err) << "Cannot read property " << prop.name() << " on element on stack.";
      return false;
    }

    QVariant value = clone->metaObject()->property(pidx).read(clone);
    if (!value.isValid()) {
      errMsg(err) << "Cannot read property " << prop.name() << " on element on stack.";
      return false;
    }

    _stack.push_back(value.value<ConfigObjectRefList *>());
    if (! processList(refs, err)) {
      errMsg(err) << "Cannot process reference list in property " << prop.name() << ".";
      return false;
    }
    _stack.pop_back();
    return true;
  } else {
    if (! this->processUnknownType(item, prop, err)) {
      errMsg(err) << "While processing property '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "' of unknown type.";
      return false;
    }
  }

  return true;
}


bool
ConfigCloneVisitor::processItem(ConfigItem *item, const ErrorStack &err) {
  // Call default constructor.
  QObject *obj = item->metaObject()->newInstance();
  if (nullptr == obj) {
    errMsg(err) << "Cannot construct new instance for item of type "
                << item->metaObject()->className() << ".";
    return false;
  }
  // Put new item on stack
  _stack.push_back(obj);

  // If item is an object -> store in map
  if (item->is<ConfigObject>())
    _map[item->as<ConfigObject>()] = dynamic_cast<ConfigObject*>(obj);

  // then traverse item
  return Visitor::processItem(item, err);
}

bool
ConfigCloneVisitor::processList(AbstractConfigObjectList *list, const ErrorStack &err) {
  if (ConfigObjectRefList *refs = dynamic_cast<ConfigObjectRefList*>(list)) {
    ConfigObjectRefList *clone = dynamic_cast<ConfigObjectRefList*>(_stack.back());
    if (nullptr == clone) {
      errMsg(err) << "Cannot clone reference list, no ref list found on the stack. Got "
                  << _stack.back()->metaObject()->className() << ".";
      return false;
    }
    // Just copy references, they will be replaced later
    for (int i=0; i<refs->count(); i++) {
      clone->add(refs->get(i));
    }
    return true;
  }

  if (ConfigObjectList *olist = dynamic_cast<ConfigObjectList*>(list)) {
    // First, get object list from stack
    ConfigObjectList *clone = dynamic_cast<ConfigObjectList*>(_stack.back());
    if (nullptr == clone) {
      errMsg(err) << "Cannot clone list, no list found on the stack. Got "
                  << _stack.back()->metaObject()->className() << ".";
      return false;
    }

    // just traverse list
    if (! Visitor::processList(olist, err))
      return false;

    // Take generated items from stack
    for (int i=0; i<olist->count(); i++) {
      clone->add(dynamic_cast<ConfigObject *>(_stack.back()),0); _stack.pop_back();
    }

    return true;
  }

  errMsg(err) << "Unexpected list type " << list->metaObject()->className() << ".";
  return false;
}

ConfigItem *
ConfigCloneVisitor::takeResult(const ErrorStack &err) {
  if (1 != _stack.size()) {
    errMsg(err) << "Cannot take result, result stack does not contain one element, got "
                << _stack.size() << ".";
    return nullptr;
  }
  ConfigItem *item = dynamic_cast<ConfigItem*>(_stack.back());
  if (nullptr == item) {
    errMsg(err) << "Cannot take result, last element on stack is not a ConfigItem, got "
                << _stack.back()->metaObject()->className() << ".";
    return nullptr;
  }
  _stack.pop_back();
  return item;
}



/* ********************************************************************************************* *
 * Implementation of FixReferencesVisitor
 * ********************************************************************************************* */
FixReferencesVisistor::FixReferencesVisistor(QHash<ConfigObject *, ConfigObject *> &map, bool keepUnknown)
  : Visitor(), _map(map), _keepUnknown(keepUnknown)
{
  // Populate with default singleton instances.
  map[SelectedChannel::get()] = SelectedChannel::get();
  map[DefaultRadioID::get()]  = DefaultRadioID::get();
  map[DefaultRoamingZone::get()] = DefaultRoamingZone::get();
}

bool
FixReferencesVisistor::processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err) {
  // First, traverse normally
  if (! Visitor::processProperty(item, prop, err))
    return false;

  if (ConfigObjectReference *ref = prop.read(item).value<ConfigObjectReference *>()) {
    if (ref->isNull())
      return true;
    ConfigObject *obj = ref->as<ConfigObject>();
    if ((! _keepUnknown) && (! _map.contains(obj))) {
      errMsg(err) << "Cannot fix refrence to object '" << obj->name()
                  << "' of type " << obj->metaObject()->className()
                  << ": Not mapped/cloned yet.";
      return false;
    }
    ref->set(_map[obj]);
    return true;
  }

  return true;
}

bool
FixReferencesVisistor::processList(AbstractConfigObjectList *list, const ErrorStack &err) {
  // First traverse list
  if (! Visitor::processList(list, err))
    return false;
  if (ConfigObjectRefList *rlist = dynamic_cast<ConfigObjectRefList*>(list)) {
    // Resolve all references.
    for (int i=0; i<rlist->count(); i++) {
      if ((! _keepUnknown) && (! _map.contains(rlist->get(i)))) {
        errMsg(err) << "Cannot fix refrence to object '" << rlist->get(i)->name()
                    << "' of type " << rlist->get(i)->metaObject()->className()
                    << ": Not mapped/cloned yet.";
        return false;
      }
      rlist->replace(_map[rlist->get(i)], i);
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigCopy
 * ********************************************************************************************* */
ConfigItem *
ConfigCopy::copy(ConfigItem *original, const ErrorStack &err) {
  QHash<ConfigObject*, ConfigObject*> map;
  ConfigCloneVisitor cloner(map);
  if (! cloner.processItem(original, err)) {
    errMsg(err) << "Cannot clone item of type " << original->metaObject()->className() << ".";
    return nullptr;
  }
  ConfigItem *clone = cloner.takeResult();
  FixReferencesVisistor fixer(map);
  if (! fixer.processItem(clone, err)) {
    errMsg(err) << "Cannot fix references in item of type "
                << clone->metaObject()->className() << ".";
    delete clone;
    return nullptr;
  }
  return clone;
}
