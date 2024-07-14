#include "encryptionextension.hh"
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of EncryptionKey
 * ********************************************************************************************* */
EncryptionKey::EncryptionKey(QObject *parent)
  : ConfigObject(parent)
{
  // pass...
}

void
EncryptionKey::clear() {
  _key.clear();
}

bool
EncryptionKey::fromHex(const QString &hex, const ErrorStack &err) {
  return setKey(QByteArray::fromHex(hex.toLocal8Bit()), err);
}

QString
EncryptionKey::toHex() const {
  return _key.toHex();
}

const QByteArray &
EncryptionKey::key() const {
  return _key;
}

bool
EncryptionKey::setKey(const QByteArray &key, const ErrorStack &err) {
  if (key.isEmpty()) {
    errMsg(err) << "Cannot set empty encryption key.";
    return false;
  }

  if (_key == key)
    return true;

  _key = key;
  emit modified(this);

  return true;
}


/* ********************************************************************************************* *
 * Implementation of BasicEncryptionKey
 * ********************************************************************************************* */
BasicEncryptionKey::BasicEncryptionKey(QObject *parent)
  : EncryptionKey(parent)
{
  // pass...
}

ConfigItem *
BasicEncryptionKey::clone() const {
  BasicEncryptionKey *key = new BasicEncryptionKey();
  if (! key->copy(*this)) {
    key->deleteLater();
    return nullptr;
  }
  return key;
}

YAML::Node
BasicEncryptionKey::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = EncryptionKey::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["basic"] = node;
  return type;
}

bool
BasicEncryptionKey::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse basic encryption key: Expected object with one child.";
    return false;
  }

  YAML::Node key = node.begin()->second;
  return EncryptionKey::parse(key, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of EnhancedEncryptionKey
 * ********************************************************************************************* */
EnhancedEncryptionKey::EnhancedEncryptionKey(QObject *parent)
  : EncryptionKey(parent)
{
  // pass...
}

ConfigItem *
EnhancedEncryptionKey::clone() const {
  EnhancedEncryptionKey *key = new EnhancedEncryptionKey();
  if (! key->copy(*this)) {
    key->deleteLater();
    return nullptr;
  }
  return key;
}

bool
EnhancedEncryptionKey::fromHex(const QString &hex, const ErrorStack &err) {
  if (10 != hex.size()) {
    errMsg(err) << "Cannot set RC4 (enhanced) ecryption key to '" << hex << "': Not a 40bit key.";
    return false;
  }
  return EncryptionKey::fromHex(hex);
}

bool
EnhancedEncryptionKey::setKey(const QByteArray &key, const ErrorStack &err) {
  if (5 != key.size()) {
    errMsg(err) << "Cannot set RC4 (enhanced) ecryption key: Not a 40bit key.";
    return false;
  }

  return EncryptionKey::setKey(key, err);
}

YAML::Node
EnhancedEncryptionKey::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = EncryptionKey::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["rc4"] = node;
  return type;
}

bool
EnhancedEncryptionKey::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse basic encryption key: Expected object with one child.";
    return false;
  }

  YAML::Node key = node.begin()->second;
  return EncryptionKey::parse(key, ctx, err);
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

bool
AESEncryptionKey::setKey(const QByteArray &key, const ErrorStack &err) {
  if (16 > key.size()) {
    errMsg(err) << "Cannot set AES ecryption key to '" << key.toHex() << "': Key smaller than 128bit.";
    return false;
  }
  return EncryptionKey::setKey(key, err);
}

YAML::Node
AESEncryptionKey::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = EncryptionKey::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["aes"] = node;
  return type;
}

bool
AESEncryptionKey::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! node)
    return false;

  if ((! node.IsMap()) || (1 != node.size())) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot parse enhanced encryption key: Expected object with one child.";
    return false;
  }

  YAML::Node key = node.begin()->second;
  return EncryptionKey::parse(key, ctx, err);
}


/* ********************************************************************************************* *
 * Implementation of EncryptionKeys
 * ********************************************************************************************* */
EncryptionKeys::EncryptionKeys(QObject *parent)
  : ConfigObjectList({BasicEncryptionKey::staticMetaObject, EnhancedEncryptionKey::staticMetaObject, AESEncryptionKey::staticMetaObject}, parent)
{
  // pass...
}

int
EncryptionKeys::add(ConfigObject *obj, int row, bool unique) {
  if ((nullptr == obj) || (! obj->is<EncryptionKey>())) {
    logError() << "Cannot add nullptr or non-encryption key objects to key list.";
    return -1;
  }
  return ConfigObjectList::add(obj, row, unique);
}

EncryptionKey *
EncryptionKeys::key(int index) const {
  if (index >= count())
    return nullptr;
  return get(index)->as<EncryptionKey>();
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
  if ("basic" == type) {
    return new BasicEncryptionKey();
  } else if ("rc4" == type) {
    return new EnhancedEncryptionKey();
  } else if ("aes" == type) {
    return new AESEncryptionKey();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create encryption key: Unknown type '" << type << "'.";

  return nullptr;

}
