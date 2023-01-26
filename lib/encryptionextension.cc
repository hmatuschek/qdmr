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

bool
EncryptionKey::fromHex(const QString &hex, const ErrorStack &err) {
  Q_UNUSED(err);
  QByteArray newKey = QByteArray::fromHex(hex.toLocal8Bit());
  if (_key == newKey)
    return true;
  _key = newKey;
  emit modified(this);
  return true;
}

QString
EncryptionKey::toHex() const {
  return _key.toHex();
}

const QByteArray &
EncryptionKey::key() const {
  return _key;
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

bool
DMREncryptionKey::fromHex(const QString &hex, const ErrorStack &err) {
  if (4 != hex.size()) {
    errMsg(err) << "Cannot set DMR ecryption key to '" << hex << "': Not a 16bit key.";
    return false;
  }
  return EncryptionKey::fromHex(hex);
}

YAML::Node
DMREncryptionKey::serialize(const Context &context, const ErrorStack &err) {
  YAML::Node node = EncryptionKey::serialize(context, err);
  if (node.IsNull())
    return node;

  YAML::Node type;
  type["basic"] = node;
  return type;
}

bool
DMREncryptionKey::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
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
AESEncryptionKey::fromHex(const QString &hex, const ErrorStack &err) {
  if (32 != hex.size()) {
    errMsg(err) << "Cannot set AES ecryption key to '" << hex << "': Not a 16bit key.";
    return false;
  }
  return EncryptionKey::fromHex(hex);
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
  : ConfigObjectList({DMREncryptionKey::staticMetaObject, AESEncryptionKey::staticMetaObject}, parent)
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
  if ("basic" == type) {
    return new DMREncryptionKey();
  } else if ("aes" == type) {
    return new AESEncryptionKey();
  }

  errMsg(err) << node.Mark().line << ":" << node.Mark().column
              << ": Cannot create encryption key: Unknown type '" << type << "'.";

  return nullptr;

}
