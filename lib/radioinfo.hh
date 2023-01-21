#ifndef RADIOINFO_HH
#define RADIOINFO_HH

#include <QString>
#include <QHash>
#include <QList>
#include "usbdevice.hh"


/** Provides some information about a radio model.
 *
 * This class is used to unify radio enumeration and detection.
 *
 * @since 0.9.0 */
class RadioInfo
{
public:
  /** Known radios. */
  enum Radio {
    // Open source firmware
    OpenGD77,
    OpenRTX,
    // Radioddity devices
    RD5R,
    GD77,
    // TyT devices
    MD390, MD380 = MD390, RT8 = MD390,
    UV390, UV380 = UV390, RT3S = UV390,
    MD2017, RT82 = MD2017,
    // Anytone devices
    D868UVE,
    D868UV = D868UVE, // Actually a different device. Implement!
    DMR6X2UV,
    D878UV,
    D878UVII,
    D578UV,
    // Baofeng/BTECH
    DM1701, RT84 = DM1701,
    DR1801UV
  };

public:
  /** Use static methods the access radio info or call @c Radio::defaultRadioInfo. */
  RadioInfo(Radio radio, const QString &name, const QString manufacturer,
            const USBDeviceInfo &interface,
            const QList<RadioInfo> &alias=QList<RadioInfo>());
  /** Use static methods the access radio info or call @c Radio::defaultRadioInfo. */
  RadioInfo(Radio radio, const QString &key, const QString &name, const QString manufacturer,
            const USBDeviceInfo &interface,
            const QList<RadioInfo> &alias=QList<RadioInfo>());

  /** Empty constructor. */
  RadioInfo();

  /** Returns @c true if the info is valid. */
  bool isValid() const;

  /** Returns the radio key (used to identify radios in the command line). */
  const QString &key() const;
  /** Returns the radio name. */
  const QString &name() const;
  /** Returns the manufacturer name. */
  const QString &manufacturer() const;
  /** Returns some information about the interface to the radio. */
  const USBDeviceInfo &interface() const;

  /** Returns @c true if the radio has aliases.
   * That is other radios that are identical. */
  bool hasAlias() const;
  /** Returns the list of alias radios. */
  const QList<RadioInfo> &alias() const;

  /** Returns the unique device ID (alias radios share ID). */
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
  static QList<RadioInfo> allRadios(const USBDeviceInfo &interface, bool flat=true);

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
  USBDeviceInfo _interface;

protected:
  /** Key->ID map. */
  static QHash<QString, Radio> _radiosByName;
  /** ID->Info map. */
  static QHash<unsigned, RadioInfo> _radiosById;
};


#endif // RADIOINFO_HH
