#include "visitor.hh"
#include "config.hh"
#include "configobject.hh"
#include "configreference.hh"
#include "logger.hh"

Visitor::Visitor()
{
  // Pass...
}

Visitor::~Visitor() {
  // pass...
}

bool
Visitor::process(Config *config, const ErrorStack &err) {
  return this->processItem(config, err);
}

bool
Visitor::processItem(ConfigItem *item, const ErrorStack &err) {
  // Serialize all properties
  const QMetaObject *meta = item->metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    QMetaProperty prop = meta->property(p);
    if (! prop.isValid()) {
      logWarn() << "Found invalid property at index " << p << " in an instance of '"
                << meta->className() << "'. Skip.";
      continue;
    }

    if (! this->processProperty(item, prop, err)) {
      errMsg(err) << "While processing property '" << prop.name() << "' of '"
                  << meta->className() << "'.";
      return false;
    }
  }
  return true;
}

bool
Visitor::processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err) {
  if (prop.isEnumType()) {
    if (! this->processEnum(item, prop, err)) {
      errMsg(err) << "While processing enum '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (QString("bool") == prop.typeName()) {
    if (! this->processBool(item, prop, err)) {
      errMsg(err) << "While processing boolean '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (QString("int") == prop.typeName()) {
    if (! this->processInt(item, prop, err)) {
      errMsg(err) << "While processing integer '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (QString("uint") == prop.typeName()) {
    if (! this->processUInt(item, prop, err)) {
      errMsg(err) << "While processing unsigned integer '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (QString("double") == prop.typeName()) {
    if (! this->processDouble(item, prop, err)) {
      errMsg(err) << "While processing double '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (QString("QString") == prop.typeName()) {
    if (! this->processBool(item, prop, err)) {
      errMsg(err) << "While processing string '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (ConfigObjectReference *ref = prop.read(item).value<ConfigObjectReference *>()) {
    if (! this->processReference(ref, err)) {
      errMsg(err) << "While processing reference '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (ConfigObjectRefList *refs = prop.read(item).value<ConfigObjectRefList *>()) {
    if (! this->processList(refs, err)) {
      errMsg(err) << "While processing reference list '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (propIsInstance<ConfigItem>(prop)) {
    if (! this->processItem(prop.read(item).value<ConfigItem *>(), err)) {
      errMsg(err) << "While processing item '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
  } else if (ConfigObjectList *lst = prop.read(item).value<ConfigObjectList *>()) {
    if (! this->processList(lst, err)) {
      errMsg(err) << "While processing reference list '" << prop.name() << "' of '"
                  << item->metaObject()->className() << "'.";
      return false;
    }
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
Visitor::processBool(ConfigItem *parent, const QMetaProperty &prop, const ErrorStack &err) {
  Q_UNUSED(parent); Q_UNUSED(prop); Q_UNUSED(err)
  // Does nothing, return true;
  return true;
}

bool
Visitor::processEnum(ConfigItem *parent, const QMetaProperty &prop, const ErrorStack &err) {
  Q_UNUSED(parent); Q_UNUSED(prop); Q_UNUSED(err)
  // Does nothing, return true;
  return true;
}

bool
Visitor::processInt(ConfigItem *parent, const QMetaProperty &prop, const ErrorStack &err) {
  Q_UNUSED(parent); Q_UNUSED(prop); Q_UNUSED(err)
  // Does nothing, return true;
  return true;
}

bool
Visitor::processUInt(ConfigItem *parent, const QMetaProperty &prop, const ErrorStack &err) {
  Q_UNUSED(parent); Q_UNUSED(prop); Q_UNUSED(err)
  // Does nothing, return true;
  return true;
}

bool
Visitor::processDouble(ConfigItem *parent, const QMetaProperty &prop, const ErrorStack &err) {
  Q_UNUSED(parent); Q_UNUSED(prop); Q_UNUSED(err)
  // Does nothing, return true;
  return true;
}

bool
Visitor::processString(ConfigItem *parent, const QMetaProperty &prop, const ErrorStack &err) {
  Q_UNUSED(parent); Q_UNUSED(prop); Q_UNUSED(err)
  // Does nothing, return true;
  return true;
}

bool
Visitor::processUnknownType(ConfigItem *parent, const QMetaProperty &prop, const ErrorStack &err) {
  errMsg(err) << "Cannot handle property '" << prop.name() << "' of '"
              << parent->metaObject()->className() << "': Unknown type '"
              << prop.typeName()  << "'.";
  return false;
}

bool
Visitor::processReference(ConfigObjectReference* ref, const ErrorStack &err) {
  Q_UNUSED(ref); Q_UNUSED(err)
  // Does nothing, returns true;
  return true;
}

bool
Visitor::processList(AbstractConfigObjectList *list, const ErrorStack &err) {
  if (ConfigObjectList *objList = qobject_cast<ConfigObjectList *>(list)) {
    for (int i=0; i<objList->count(); i++) {
      if (! this->processItem(objList->get(i), err)) {
        errMsg(err) << "While processing object list.";
        return false;
      }
    }
  }
  return true;
}

