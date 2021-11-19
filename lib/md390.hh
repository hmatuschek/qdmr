/** @defgroup md390 TYT MD-390 (U/V)
 * Device specific classes for TyT MD-390, both the VHF and UHF version.
 *
 * \image html md390.jpg "MD-390" width=200px
 * \image latex md390.jpg "MD-390" width=200px
 *
 * The TYT MD-390 is a decent VHF or UHF FM and DMR handheld radio.
 * The radio is available with and without an GPS option. @c libdmrconf will support that
 * feature. Non-GPS variants of that radio will simply ignore any GPS system settings.
 *
 * These radios support up to 1000 channels organized in 250 zones. Each zone may hold up to 16
 * channels. There are also up to 250 scanlists
 * holding up to 31(?) channels each.
 *
 * The radio can hold up to 1000 contacts (DMR contacts) and 250 RX group lists as well as up to 50
 * pre-programmed messages.
 *
 * @ingroup tyt */
#ifndef MD390_HH
#define MD390_HH

#include "tyt_radio.hh"
#include "md390_codeplug.hh"

/** Implements an USB interface to the TYT MD-390 VHF/UHF 5W DMR (Tier I&II) radio.
 *
 * The TYT MD-390 radio use the TyT typical DFU-style communication protocol
 * to read and write codeplugs onto the radio (see @c TyTRadio).
 *
 * @ingroup md390 */
class MD390 : public TyTRadio
{
  Q_OBJECT

public:
  /** Constructor.
   * @param device Specifies the DFU device to use for communication with the device.
   * @param err Passes an error stack to put error messages on.
   * @param parent The QObject parent. */
  MD390(TyTInterface *device=nullptr, const ErrorStack &err=ErrorStack(), QObject *parent=nullptr);

  const QString &name() const;
  const Features &features() const;

  const Codeplug &codeplug() const;
  Codeplug &codeplug();

  const CallsignDB *callsignDB() const;
  CallsignDB *callsignDB();

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

private:
  /** Holds the name of the device. */
  QString _name;
  /** The codeplug object. */
  MD390Codeplug _codeplug;
  /** Dynamic features, includes frequency range of channels read from device. */
  Features _features;
};

#endif // MD2017_HH
