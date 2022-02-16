#ifndef COMMERCIALEXTENSION_HH
#define COMMERCIALEXTENSION_HH

#include "configobject.hh"
#include "configreference.hh"

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
