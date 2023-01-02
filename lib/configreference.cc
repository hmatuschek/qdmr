#include "configreference.hh"
#include "logger.hh"
#include "contact.hh"
#include "channel.hh"
#include "scanlist.hh"
#include "gpssystem.hh"
#include "radioid.hh"
#include "rxgrouplist.hh"
#include "roaming.hh"
#include "encryptionextension.hh"


/* ********************************************************************************************* *
 * Implementation of ConfigObjectReference
 * ********************************************************************************************* */
ConfigObjectReference::ConfigObjectReference(const QMetaObject &elementType, QObject *parent)
  : QObject(parent), _elementTypes(), _object(nullptr)
{
  _elementTypes.append(elementType.className());
}

bool
ConfigObjectReference::isNull() const {
  return nullptr == _object;
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
  bool typeCheck = false;
  foreach (const QString &cname, _elementTypes) {
    if (object->inherits(cname.toLatin1().constData())) {
      typeCheck = true;
      break;
    }
  }
  if (! typeCheck) {
    logError() << "Cannot reference element of type " << object->metaObject()->className()
               << ", expected instance of " << _elementTypes.join(", ");
    return false;
  }

  _object = object;
  if (_object)
    connect(_object, SIGNAL(destroyed(QObject*)), this, SLOT(onReferenceDeleted(QObject*)));

  emit modified();
  return true;
}

bool
ConfigObjectReference::copy(const ConfigObjectReference *ref) {
  clear();
  if (nullptr == ref)
    return true;
  return set(ref->_object);
}

int
ConfigObjectReference::compare(const ConfigObjectReference &other) const {
  if (isNull() && other.isNull())
    return 0;
  if (!isNull() && other.isNull())
    return 1;
  if (isNull() && !other.isNull())
    return -1;
  return this->_object->compare(*other._object);
}

bool
ConfigObjectReference::allow(const QMetaObject *elementType) {
  if (! _elementTypes.contains(elementType->className()))
    _elementTypes.append(elementType->className());
  return true;
}

const QStringList &
ConfigObjectReference::elementTypeNames() const {
  return _elementTypes;
}

void
ConfigObjectReference::onReferenceDeleted(QObject *obj) {
  // Check if destroyed obj is referenced one.
  if (_object != reinterpret_cast<ConfigObject*>(obj))
    return;
  // If it is
  _object = nullptr;
  emit modified();
}


/* ********************************************************************************************* *
 * Implementation of ContactReference
 * ********************************************************************************************* */
ContactReference::ContactReference(const QMetaObject &elementType, QObject *parent)
  : ConfigObjectReference(elementType, parent)
{
  // pass...
}

ContactReference::ContactReference(QObject *parent)
  : ConfigObjectReference(Contact::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DMRContactReference
 * ********************************************************************************************* */
DMRContactReference::DMRContactReference(QObject *parent)
  : ContactReference(DMRContact::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DMRContactRefList
 * ********************************************************************************************* */
DMRContactRefList::DMRContactRefList(QObject *parent)
  : ConfigObjectRefList(DMRContact::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ChannelReference
 * ********************************************************************************************* */
ChannelReference::ChannelReference(const QMetaObject &elementType, QObject *parent)
  : ConfigObjectReference(elementType, parent)
{
  // pass...
}

ChannelReference::ChannelReference(QObject *parent)
  : ConfigObjectReference(Channel::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DMRChannelReference
 * ********************************************************************************************* */
DMRChannelReference::DMRChannelReference(QObject *parent)
  : ChannelReference(DMRChannel::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of FMChannelReference
 * ********************************************************************************************* */
FMChannelReference::FMChannelReference(QObject *parent)
  : ChannelReference(FMChannel::staticMetaObject, parent)
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
 * Implementation of DMRChannelRefList
 * ********************************************************************************************* */
DMRChannelRefList::DMRChannelRefList(QObject *parent)
  : ChannelRefList(DMRChannel::staticMetaObject, parent)
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


/* ********************************************************************************************* *
 * Implementation of PositioningSystemReference
 * ********************************************************************************************* */
PositioningSystemReference::PositioningSystemReference(const QMetaObject &elementType, QObject *parent)
  : ConfigObjectReference(elementType, parent)
{
  // pass...
}

PositioningSystemReference::PositioningSystemReference(QObject *parent)
  : ConfigObjectReference(PositioningSystem::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of APRSSystemReference
 * ********************************************************************************************* */
APRSSystemReference::APRSSystemReference(QObject *parent)
  : PositioningSystemReference(APRSSystem::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of GPSSystemReference
 * ********************************************************************************************* */
GPSSystemReference::GPSSystemReference(QObject *parent)
  : PositioningSystemReference(APRSSystem::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of RadioIDReference
 * ********************************************************************************************* */
DMRRadioIDReference::DMRRadioIDReference(QObject *parent)
  : ConfigObjectReference(DMRRadioID::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of GroupListReference
 * ********************************************************************************************* */
GroupListReference::GroupListReference(QObject *parent)
  : ConfigObjectReference(RXGroupList::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of RoamingZoneReference
 * ********************************************************************************************* */
RoamingZoneReference::RoamingZoneReference(QObject *parent)
  : ConfigObjectReference(RoamingZone::staticMetaObject, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of EncryptionKeyReference
 * ********************************************************************************************* */
EncryptionKeyReference::EncryptionKeyReference(QObject *parent)
  : ConfigObjectReference(EncryptionKey::staticMetaObject, parent)
{
  // pass...
}
