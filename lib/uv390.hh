/** @defgroup uv390 TYT MD-UV390, Retevis RT3S
 * Device specific classes for TyT MD-UV390 and Retevis RT3S.
 *
 * \image html uv390.jpg "MD-UV390" width=200px
 * \image latex uv390.jpg "MD-UV390" width=200px
 *
 * The TYT MD-UV390 and the identical Retevis RT3S are decent VHF/UHF FM and DMR handheld radios.
 * Both radios are available with and without an GPS option. @c libdmrconf will support that
 * feature. Non-GPS variants of that radio will simply ignore any GPS system settings.
 *
 * These radios support up to 3000 channels organized in 250 zones. Each zone may hold up to 64
 * channels for each VFO (64 for VFO A and 64 for VFO B). There are also up to 250 scanlists
 * holding up to 31(?) channels each.
 *
 * The radio can hold up to 3000 contacts (DMR contacts) and 250 RX group lists as well as up to 50
 * pre-programmed messages. Depending on the firmware programmed on the radio, it may also hold a
 * callsign database of up to 100000 entries. This can be used to resolve amlost any DMR ID assigned
 * (at the time of this writing, there are about 140k IDs assigned) to name and callsign.
 *
 * @ingroup tyt */
#ifndef UV390_HH
#define UV390_HH

#include "tyt_radio.hh"
#include "uv390_codeplug.hh"
#include "uv390_callsigndb.hh"

class RadioLimits;

/** Implements an USB interface to the TYT MD-UV390 & Retevis RT3S VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * The TYT MD-UV390 and Retevis RT3S radios use the TyT typical DFU-style communication protocol
 * to read and write codeplugs onto the radio (see @c TyTRadio).
 *
 * @ingroup uv390 */
class UV390 : public TyTRadio
{
  Q_OBJECT

public:
  /** Constructor.
   * @param device Specifies the DFU device to use for communication with the device.
   * @param parent The QObject parent. */
  UV390(TyTInterface *device=nullptr, QObject *parent=nullptr);
  /** Desturctor. */
  virtual ~UV390();

  const QString &name() const;
  const RadioLimits &limits() const;

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
  UV390Codeplug _codeplug;
  /** The callsign DB object. */
  UV390CallsignDB _callsigndb;

private:
  /** Holds the singleton instance of the radio limits. */
  static RadioLimits *_limits;
};

#endif // MD2017_HH
