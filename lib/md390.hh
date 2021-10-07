/** @defgroup md390 TYT MD-390(U/V)
 * Device specific classes for TyT MD-390, both the VHF and UHF version.
 *
 * \image html md390.jpg "MD-390" width=200px
 * \image latex md390.jpg "MD-390" width=200px
 *
 * The TYT MD-390 is a decent VHF/UHF FM and DMR handheld radio.
 * The radio is available with and without an GPS option. @c libdmrconf will support that
 * feature. Non-GPS variants of that radio will simply ignore any GPS system settings.
 *
 * These radios support up to 1000 channels organized in 250 zones. Each zone may hold up to 64
 * channels for each VFO (64 for VFO A and 64 for VFO B). There are also up to 250 scanlists
 * holding up to 31(?) channels each.
 *
 * The radio can hold up to 1000 contacts (DMR contacts) and 250 RX group lists as well as up to 50
 * pre-programmed messages. Depending on the firmware programmed on the radio, it may also hold a
 * callsign database of up to 100000 entries. This can be used to resolve amlost any DMR ID assigned
 * (at the time of this writing, there are about 140k IDs assigned) to name and callsign.
 *
 * @ingroup tyt */
#ifndef MD390_HH
#define MD390_HH

#include "tyt_radio.hh"
#include "md390_codeplug.hh"
//#include "uv390_callsigndb.hh"

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
   * @param parent The QObject parent. */
  MD390(TyTInterface *device=nullptr, QObject *parent=nullptr);

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
  /** The callsign DB object. */
  //UV390CallsignDB _callsigndb;
};

#endif // MD2017_HH
