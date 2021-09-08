#include "configreference.hh"
#include "logger.hh"
#include "contact.hh"
#include "channel.hh"
#include "scanlist.hh"


/* ********************************************************************************************* *
 * Implementation of ConfigObjectReference
 * ********************************************************************************************* */
ConfigObjectReference::ConfigObjectReference(const QMetaObject &elementType, QObject *parent)
  : QObject(parent), _elementType(elementType), _object(nullptr)
{
  // pass...
}

void
ConfigObjectReference::clear() {
  if (_object) {
    disconnect(_object, SIGNAL(destroyed(QObject*)), this, SLOT(onReferenceDeleted(QObject*)));
    emit modified();
  }
  _object = nullptr;
}

bool
ConfigObjectReference::set(ConfigObject *object) {
  if (_object)
    disconnect(_object, SIGNAL(destroyed(QObject*)), this, SLOT(onReferenceDeleted(QObject*)));

  if (nullptr == object) {
    _object = nullptr;
    return true;
  }

  // Check type
  if (! object->inherits(_elementType.className())) {
    logError() << "Cannot reference element of type " << object->metaObject()->className()
               << ", expected instance of " << _elementType.className();
    return false;
  }

  _object = object;
  if (_object)
    connect(_object, SIGNAL(destroyed(QObject*)), this, SLOT(onReferenceDeleted(QObject*)));

  emit modified();
  return true;
}

void
ConfigObjectReference::onReferenceDeleted(QObject *obj) {
  _object = nullptr;
  emit modified();
}


/* ********************************************************************************************* *
 * Implementation of DigitalContactRefList
 * ********************************************************************************************* */
DigitalContactRefList::DigitalContactRefList(QObject *parent)
  : ConfigObjectRefList(DigitalContact::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ChannelReference
 * ********************************************************************************************* */
ChannelReference::ChannelReference(QObject *parent)
  : ConfigObjectReference(Channel::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ChannelRefList
 * ********************************************************************************************* */
ChannelRefList::ChannelRefList(const QMetaObject &elementType, QObject *parent)
  : ConfigObjectRefList(elementType, parent)
{
  // pass...
}

ChannelRefList::ChannelRefList(QObject *parent)
  : ConfigObjectRefList(Channel::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DigitalChannelRefList
 * ********************************************************************************************* */
DigitalChannelRefList::DigitalChannelRefList(QObject *parent)
  : ChannelRefList(DigitalChannel::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ScanListReference
 * ********************************************************************************************* */
ScanListReference::ScanListReference(QObject *parent)
  : ConfigObjectReference(ScanList::staticMetaObject, parent)
{
  // pass...
}


