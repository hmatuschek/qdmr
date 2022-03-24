#ifndef COMMERCIALEXTENSION_HH
#define COMMERCIALEXTENSION_HH

#include "configobject.hh"
#include "configreference.hh"
#include "encryptionextension.hh"

/** Implements the generic extension for the codeplug to represent some commercial features of DMR.
 * @ingroup conf */
class CommercialExtension: public ConfigExtension
{
  Q_OBJECT

  /** Read only property returning holding the list of encryption keys. */
  Q_PROPERTY(EncryptionKeys* encryptionKeys READ encryptionKeys)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit CommercialExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the list of encryption keys. */
  EncryptionKeys *encryptionKeys() const;

protected:
  /** Owns the instance of the encryption key list. */
  EncryptionKeys *_encryptionKeys;
};


/** Implements the generic extension for all channels configuring commercial features of DMR.
 * @ingroup conf */
class CommercialChannelExtension: public ConfigExtension
{
  Q_OBJECT

  /** Holds a reference to the associated encryption key. */
  Q_PROPERTY(EncryptionKeyReference* encryptionKey READ encryptionKeyRef WRITE setEncryptionKeyRef)

public:
  /** Empty constructor. */
  Q_INVOKABLE explicit CommercialChannelExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the reference to the encryption key. */
  EncryptionKeyReference *encryptionKeyRef();
  /** Sets the reference to the encryption key. */
  void setEncryptionKeyRef(EncryptionKeyReference *ref);

  /** Returns the referenced encryption key. */
  EncryptionKey *encryptionKey();
  /** References the given encryption key. */
  void setEncryptionKey(EncryptionKey *key);

protected:
  /** The actual reference to the the encryption key. */
  EncryptionKeyReference _encryptionKey;
};


#endif // COMMERCIALEXTENSION_HH
