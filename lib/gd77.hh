/** @defgroup gd77 Radioddity GD-77
 * Device specific classes for Radioddity GD-77 and GD-77S.
 *
 * \image html gd77.jpg "GD-77" width=200px
 * \image latex gd77.jpg "GD-77" width=200px
 *
 * @ingroup radioddity */

#ifndef GD77_HH
#define GD77_HH

#include "radioddity_radio.hh"
#include "radioddity_interface.hh"
#include "gd77_codeplug.hh"
#include "gd77_callsigndb.hh"


/** Implements an USB interface to the Radioddity GD-77(S) VHF/UHF 5W DMR (Tier I&II) radios.
 *
 * @ingroup gd77 */
class GD77 : public RadioddityRadio
{
	Q_OBJECT

public:
	/** Do not construct this class directly, rather use @c Radio::detect. */
  explicit GD77(RadioddityInterface *device=nullptr, QObject *parent=nullptr);

	const QString &name() const;
  const Radio::Features &features() const;
  const Codeplug &codeplug() const;
  Codeplug &codeplug();

public slots:
  /** Encodes the given user-database and uploades it to the device. */
  bool startUploadCallsignDB(UserDatabase *db, bool blocking=false,
                             const CallsignDB::Selection &selection=CallsignDB::Selection());

protected:
  /** Implements the actual callsign DB upload process. */
  bool uploadCallsigns();

protected:
  /** The device identifier. */
	QString _name;
  /** The codeplug. */
  GD77Codeplug _codeplug;
  /** The acutal binary callsign DB representation. */
  GD77CallsignDB _callsigns;
};

#endif // GD77_HH
