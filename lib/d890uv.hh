/** @defgroup d890uv Anytone AT-D890UV
 * Device specific classes for Anytone AT-D890UV.
 *
 * @ingroup anytone */
#ifndef __D890UV_HH__
#define __D890UV_HH__

#include "anytone_radio.hh"
#include "anytone_interface.hh"

/** Implements an interface to the Anytone AT-D890UV VHF/UHF DMR radio.
 *
 * This is the first radio using the second generation of AnyTone codeplugs, see
 * @c AnytoneCodeplugGen2.
 *
 * @ingroup d890uv */
class D890UV: public AnytoneRadio
{
  Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit D890UV(AnytoneInterface *device=nullptr, QObject *parent=nullptr);

  const RadioLimits &limits() const;

  /** Refuses to upload a codeplug generated from scratch (i.e., @c --init-codeplug), which would
   * not preserve the regions of the codeplug that are not modelled. */
  bool startUpload(Config *config, const Codeplug::Flags &flags = Codeplug::Flags(), const ErrorStack &err=ErrorStack());

  /** Not implemented yet, always fails. */
  bool startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection=CallsignDB::Flags(), const ErrorStack &err=ErrorStack());
  /** Not implemented yet, always fails. */
  bool startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err=ErrorStack());

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

private:
  /** The limits for the radio. */
  RadioLimits *_limits;
};

#endif // __D890UV_HH__
