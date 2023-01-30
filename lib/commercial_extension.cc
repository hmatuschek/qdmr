#include "commercial_extension.hh"
#include "encryptionextension.hh"


/* ********************************************************************************************* *
 * Implementation of CommercialExtension
 * ********************************************************************************************* */
CommercialExtension::CommercialExtension(QObject *parent)
  : ConfigExtension(parent), _encryptionKeys(new EncryptionKeys(this))
{
  // pass...
}

ConfigItem *
CommercialExtension::clone() const {
  CommercialExtension *ext = new CommercialExtension();
  if (! ext->copy(*this)) {
    ext->deleteLater();
    return nullptr;
  }
  return ext;
}

EncryptionKeys *
CommercialExtension::encryptionKeys() const {
  return _encryptionKeys;
}



/* ********************************************************************************************* *
 * Implementation of CommercialChannelExtension
 * ********************************************************************************************* */
CommercialChannelExtension::CommercialChannelExtension(QObject *parent)
  : ConfigExtension(parent), _encryptionKey()
{
  // pass...
}

ConfigItem *
CommercialChannelExtension::clone() const {
  CommercialChannelExtension *ex = new CommercialChannelExtension();
  if (! ex->copy(*this)) {
    ex->deleteLater();
    return nullptr;
  }
  return ex;
}

EncryptionKeyReference *
CommercialChannelExtension::encryptionKeyRef() {
  return &_encryptionKey;
}

void
CommercialChannelExtension::setEncryptionKeyRef(EncryptionKeyReference *ref) {
  if (nullptr == ref)
    setEncryptionKey(nullptr);
  else
    setEncryptionKey(ref->as<EncryptionKey>());
}

EncryptionKey *
CommercialChannelExtension::encryptionKey() {
  return _encryptionKey.as<EncryptionKey>();
}

void
CommercialChannelExtension::setEncryptionKey(EncryptionKey *key) {
  if (_encryptionKey.as<EncryptionKey>() == key)
    return;
  _encryptionKey.set(key);
  emit modified(this);
}


