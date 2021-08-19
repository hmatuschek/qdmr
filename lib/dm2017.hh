/** @defgroup md2017 TYT MD-2017, Retevis RT82
 * Device specific classes for TYT MD-2017 and Retevis RT82.
 *
 * \image html md2017.jpg "MD-2017" width=200px
 * \image latex uv2017.jpg "MD-2017" width=200px
 *
 * <img src="sm2017.jpg" width="200px" align="left"/>
 * The TYT MD-2017 and the identical Retevis RT-82 are decent VHF/UHF FM and DMR handheld radios.
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
#ifndef DM2017_HH
#define DM2017_HH

#include "tyt_radio.hh"
#include "dm2017_codeplug.hh"
#include "dm2017_callsigndb.hh"

/** Implements an USB interface to the TYT MD-2017 & Retevis RT82 VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * The TYT MD-2017 and Retevis RT82 radios use a DFU-style communication protocol to read and write
 * codeplugs onto the radio (see @c DFUDevice). This class implements the communication details
 * using DFU protocol.
 *
 * @ingroup md2017 */
class MD2017 : public TyTRadio
{
  Q_OBJECT

public:
  MD2017(DFUDevice *device=nullptr, QObject *parent=nullptr);
  virtual ~MD2017();

  const QString &name() const;
  const Features &features() const;

  const CodePlug &codeplug() const;
  CodePlug &codeplug();

  const CallsignDB *callsignDB() const;
  CallsignDB *callsignDB();

private:
  QString _name;
  MD2017Codeplug _codeplug;
  MD2017CallsignDB _callsigndb;
};

#endif // DM2017_HH
