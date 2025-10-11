#ifndef ENCRYPTIONEXTENSION_HH
#define ENCRYPTIONEXTENSION_HH

#include "configobject.hh"


/** Base class of all encryption keys.
 * @ingroup conf */
class EncryptionKey: public ConfigObject
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "key")

  /** The key representation as a hex string. */
  Q_PROPERTY(QString key READ toHex WRITE fromHex)

protected:
  /** Hidden constructor. */
  explicit EncryptionKey(QObject *parent=nullptr);

public:
  void clear();

  /** Creates a key from the given hex-string. */
  virtual bool fromHex(const QString &hex, const ErrorStack &err=ErrorStack());
  /** Converts a key to a hex string. */
  virtual QString toHex() const;

  /** Returns the binary key. */
  const QByteArray &key() const;
  /** Sets the binary key. */
  virtual bool setKey(const QByteArray &key, const ErrorStack &err=ErrorStack());

protected:
  /** Holds the key data.
   * The size depends on the key type. */
  QByteArray _key;
};


/** Represents a DMR (basic) encryption key.
 *
 * This is a variable sized key used for the DMR basic encryption method.
 *
 * @ingroup conf */
class BasicEncryptionKey: public EncryptionKey
{
  Q_OBJECT
  Q_CLASSINFO("description", "A basic DMR encryption key.")
  Q_CLASSINFO("longDescription",
              "This is a variable sized pre-shared key that can be used to encrypt/decrypt traffic "
              "on DMR channels. Encryption is forbidden in HAM radio context!")

public:
  /** Empty constructor. */
  Q_INVOKABLE explicit BasicEncryptionKey(QObject *parent=nullptr);

  ConfigItem *clone() const;

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());
  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());
};


/** Represents an (enhanced) ARC4 encryption key.
 *
 * This is a 40bit key used for the DMR enhanced encryption method.
 *
 * @ingroup conf */
class ARC4EncryptionKey: public EncryptionKey
{
  Q_OBJECT
  Q_CLASSINFO("description", "An enhanced DMR encryption key.")
  Q_CLASSINFO("longDescription",
              "This is a 40bit pre-shared RC4 key, that can be used to encrypt/decrypt traffic on "
              "DMR channels. Encryption is forbidden in HAM radio context!")

public:
  /** Empty constructor. */
  Q_INVOKABLE explicit ARC4EncryptionKey(QObject *parent=nullptr);

  ConfigItem *clone() const;
  bool fromHex(const QString &hex, const ErrorStack &err=ErrorStack());

  bool setKey(const QByteArray &key, const ErrorStack &err);

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());
  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());
};


/** Represents a variable size AES (enhanced) encryption key.
 *
 * This is a 128-256bit key used for the DMR enhanced encryption method.
 *
 * @ingroup conf */
class AESEncryptionKey: public EncryptionKey
{
  Q_OBJECT

  Q_CLASSINFO("description", "An AES (advanced) DMR encryption key.")
  Q_CLASSINFO("longDescription",
              "This is a variable sized (usually 128-256bit) pre-shared key that can be used to "
              "encrypt/decrypt traffic on DMR channels. Encryption is forbidden in HAM radio "
              "context!")

public:
  /** Empty constructor. */
  Q_INVOKABLE explicit AESEncryptionKey(QObject *parent=nullptr);

  ConfigItem *clone() const;

  bool setKey(const QByteArray &key, const ErrorStack &err);

public:
  YAML::Node serialize(const Context &context, const ErrorStack &err=ErrorStack());
  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());
};

Q_DECLARE_OPAQUE_POINTER(AESEncryptionKey*)


/** The list of encryption keys.
 *
 * This list holds all encryption keys defined within the codeplug.
 *
 * @warning Please note that this is a commercial feature and forbidden for HAM radio use.
 *
 * @ingroup conf */
class EncryptionKeys: public ConfigObjectList
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit EncryptionKeys(QObject *parent=nullptr);

  int add(ConfigObject *obj, int row=-1, bool unique=true);

  /** Returns the key the given index. */
  EncryptionKey *key(int index) const;

  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};



#endif // ENCRYPTIONEXTENSION_HH
