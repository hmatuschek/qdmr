#include "encryptionextension.hh"
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of EncryptionKey
 * ********************************************************************************************* */
EncryptionKey::EncryptionKey(QObject *parent)
  : ConfigObject("key", parent)
{
  // pass...
}

void
EncryptionKey::clear() {
  _key.clear();
}

void
EncryptionKey::fromHex(const QString &hex) {
  QByteArray newKey = QByteArray::fromHex(hex.toLocal8Bit());
  if (_key == newKey)
    return;
  _key = newKey;
  emit modified(this);
}

QString
EncryptionKey::toHex() const {
  return _key.toHex();
}


/* ********************************************************************************************* *
 * Implementation of DMREncryptionKey
 * ********************************************************************************************* */
DMREncryptionKey::DMREncryptionKey(QObject *parent)
  : EncryptionKey(parent)
{
  // pass...
}

ConfigItem *
DMREncryptionKey::clone() const {
  DMREncryptionKey *key = new DMREncryptionKey();
  if (! key->copy(*this)) {
    key->deleteLater();
    return nullptr;
  }
  return key;
}

void
DMREncryptionKey::fromHex(const QString &hex) {
  if (4 != hex.size()) {
    logError() << "Cannot set DMR ecryption key to '" << hex << "': Not a 16bit key.";
    return;
  }
  EncryptionKey::fromHex(hex);
}


/* ********************************************************************************************* *
 * Implementation of AESEncryptionKey
 * ********************************************************************************************* */
AESEncryptionKey::AESEncryptionKey(QObject *parent)
  : EncryptionKey(parent)
{
  // pass...
}

ConfigItem *
AESEncryptionKey::clone() const {
  AESEncryptionKey *key = new AESEncryptionKey();
  if (! key->copy(*this)) {
    key->deleteLater();
    return nullptr;
  }
  return key;
}

void
AESEncryptionKey::fromHex(const QString &hex) {
  if (4 != hex.size()) {
    logError() << "Cannot set AES ecryption key to '" << hex << "': Not a 16bit key.";
    return;
  }
  EncryptionKey::fromHex(hex);
}



/* ********************************************************************************************* *
 * Implementation of EncryptionKeys
 * ********************************************************************************************* */
EncryptionKeys::EncryptionKeys(QObject *parent)
  : ConfigObjectList(EncryptionKey::staticMetaObject, parent)
{
  // pass...
}

int
EncryptionKeys::add(ConfigObject *obj, int row) {
  if ((nullptr == obj) || (! obj->is<EncryptionKey>())) {
    logError() << "Cannot add nullptr or non-encryption key objects to key list.";
    return -1;
  }
  return ConfigObjectList::add(obj, row);
}

ConfigItem *
EncryptionKeys::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)
  if (! node)
    return nullptr;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create encryption key: Expected object with one child.";
    return nullptr;
  }

  QString type = QString::fromStdString(node.begin()->first.as<std::string>());
  if ("dmr" == type) {
    return new DMREncryptionKey();
  } else if ("aes" == type) {
    return new AESEncryptionKey();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create encryption key: Unknown type '" << type << "'.";

  return nullptr;

}

/* ********************************************************************************************* *
 * Implementation of EncryptionExtension
 * ********************************************************************************************* */
EncryptionExtension::EncryptionExtension(QObject *parent)
  : ConfigExtension(parent), _keys(new EncryptionKeys(this))
{
  // pass...
}

EncryptionKeys *
EncryptionExtension::keys() const {
  return _keys;
}
