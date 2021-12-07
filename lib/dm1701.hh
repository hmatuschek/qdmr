/** @defgroup dm1701 Baofeng DM-1701, Retevis RT84
 * Device specific classes for Baofeng DM-1701 and Retevis RT84.
 *
 * \image html dm1701.png "DM-1701" width=200px
 * \image latex dm1701.png "DM-1701" width=200px
 *
 * The Baofeng DM-1701 and the identical Retevis RT84.
 *
 * Features:
 *   - VHF/UHF, 136-174 MHz and 400-480 MHz
 *   - Pout = 1, 3 and 5W
 *   - 3000 channels
 *   - 10000 contacts
 *   - 250 RX group lists with ?? contacts each
 *   - 250 zones, 64 channels each (A & B)
 *   - 250 scan lists, 31 channels each
 *   - ...
 * @ingroup tyt */
#ifndef DM1701_HH
#define DM1701_HH

#include "radio.hh"
#include "tyt_radio.hh"
#include "tyt_codeplug.hh"
#include "tyt_callsigndb.hh"
#include "dm1701_codeplug.hh"
#include "uv390_callsigndb.hh"


/** Implements an USB interface to the  Baofeng DM-1701 and Retevis RT84 VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * The  Baofeng DM-1701 and Retevis RT84 radios use a DFU-style communication protocol to read and write
 * codeplugs onto the radio (see @c DFUDevice). This class implements the communication details
 * using DFU protocol.
 *
 * @ingroup dm1701 */
class DM1701: public TyTRadio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit DM1701(TyTInterface *device=nullptr, QObject *parent=nullptr);

  const QString &name() const;
  const Radio::Features &features() const;
  const Codeplug &codeplug() const;
  Codeplug &codeplug();

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

protected:
  /** The device identifier. */
	QString _name;
  /** The actual binary codeplug representation. */
  DM1701Codeplug _codeplug;
};

#endif // DM1701_HH
