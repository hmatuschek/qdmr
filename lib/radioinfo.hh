#ifndef RADIOINFO_HH
#define RADIOINFO_HH

#include <QString>
#include <QHash>
#include <QList>

/** Generic information about a possible radio interface. */
class InterfaceInfo
{
public:
  /** Possible interface types. */
  enum class Class {
    None,       ///< Class for invalid interface info.
    Serial,     ///< Serial port interface class.
    DFU,        ///< DFU interface class.
    HID         ///< HID (human-interface device) interface class.
  };

public:
  /** Empty constructor. */
  InterfaceInfo();
  /** Constructor from class, VID and PID. */
  InterfaceInfo(Class cls, uint16_t vid, uint16_t pid);
  /** Destructor. */
  virtual ~InterfaceInfo();

  /** Copy constructor. */
  InterfaceInfo(const InterfaceInfo &other);
  /** Assignment. */
  InterfaceInfo &operator =(const InterfaceInfo &other);

  /** Comparison. */
  bool operator ==(const InterfaceInfo &other) const;
  /** Comparison. */
  bool operator !=(const InterfaceInfo &other) const;

  /** Returns @c true if the interface info is valid. */
  bool isValid() const;

  /** Returns the interface class. */
  Class interfaceClass() const;

  /** Returns the vendor ID or 0 if not set. */
  uint16_t vendorId() const;
  /** Returns the product ID or 0 if not set. */
  uint16_t productId() const;

  /** Returns a brief human readable description of the interface. */
  QString description() const;

  /** Returns a more extensive human readable description of the interface. */
  QString longDescription() const;

protected:
  /** The class of the interface. */
  Class _class;
  /** The USB vid. */
  uint16_t _vid;
  /** The USB pid. */
  uint16_t _pid;
};


/** Provides some information about a radio model.
 *
 * This class is used to unify radio enumeration and detection. It also will contains the detailed
 * features of the radio as used by the @c Radio::verify method.
 *
 * @since 0.9.0 This class will replace the Radio::Features class in future. */
class RadioInfo
{
public:
  /** Known radios. */
  enum Radio {
    // Open soruce firmware
    OpenGD77,
    // Radioddity devices
    RD5R,
    GD77,
    // TyT devices
    MD390, RT8 = MD390,
    UV390, UV380 = UV390, RT3S = UV390,
    MD2017, RT82 = MD2017,
    // Anytone devices
    D868UVE,
    D868UV = D868UVE, // Actually a different device. Implement!
    DMR6X2 = D868UVE, // Actually a D868UV, implement!
    D878UV,
    D878UVII,
    D578UV,
    // Baofeng/BTECH
    DM1701, RT84 = DM1701
  };

public:
  /** Use static methods the access radio info or call @c Radio::defaultRadioInfo. */
  RadioInfo(Radio radio, const QString &name, const QString manufacturer,
            const QList<RadioInfo> &alias=QList<RadioInfo>(), const InterfaceInfo &interface=InterfaceInfo());
  /** Use static methods the access radio info or call @c Radio::defaultRadioInfo. */
  RadioInfo(Radio radio, const QString &key, const QString &name, const QString manufacturer,
            const QList<RadioInfo> &alias=QList<RadioInfo>(), const InterfaceInfo &interface=InterfaceInfo());

  /** Empty constructor. */
  RadioInfo();

  /** Retunrs @c true if the info is valid. */
  bool isValid() const;

  /** Returns the radio key (used to identify radios in the command line). */
  const QString &key() const;
  /** Returns the radio name. */
  const QString &name() const;
  /** Retunrs the manufacturer name. */
  const QString &manufactuer() const;
  /** Returns some information about the iterface to the radio. */
  const InterfaceInfo &interface() const;

  /** Returns @c true if the radio has aliases.
   * That is other radios that are identical. */
  bool hasAlias() const;
  /** Returns the list of alias radios. */
  const QList<RadioInfo> &alias() const;

  /** Retuns the unique device ID (alias radios share ID). */
  Radio id() const;

public:
  /** Returns @c true if the given key is known. */
  static bool hasRadioKey(const QString &key);
  /** Returns the radio info by key. */
  static RadioInfo byKey(const QString &key);
  /** Returns the radio info by id. */
  static RadioInfo byID(Radio radio);

  /** Returns the list of all known radios. */
  static QList<RadioInfo> allRadios(bool flat=true);
  /** Returns a list of all known radios for the specified interface. */
  static QList<RadioInfo> allRadios(const InterfaceInfo &interface, bool flat=true);

protected:
  /** Holds the radio id. */
  Radio _radio;
  /** Holds the key of the radio. */
  QString _key;
  /** Holds the name of the radio. */
  QString _name;
  /** Holds the name of the manufacturer. */
  QString _manufacturer;
  /** Holds possible identical radios from other manufactuers. */
  QList<RadioInfo> _alias;
  /** Holds some information about the interface to the radio. */
  InterfaceInfo _interface;

protected:
  /** Key->ID map. */
  static QHash<QString, Radio> _radiosByName;
  /** ID->Info map. */
  static QHash<unsigned, RadioInfo> _radiosById;
};


#endif // RADIOINFO_HH
