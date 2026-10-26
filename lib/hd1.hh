/** @defgroup hd1 Ailunce HD1
 * Device specific classes for the Ailunce HD1 (also sold as Retevis Ailunce HD1).
 * @ingroup dsc */
#ifndef HD1_HH
#define HD1_HH

#include "radio.hh"
#include "hd1_codeplug.hh"

class HD1Interface;

/** Implements an interface to the Ailunce HD1.
 *
 * A download reads the regions of @c HD1Codeplug::Region::reads() in that order. An upload
 * first downloads the codeplug (so bytes qdmr does not model are kept), ends that session,
 * encodes the configuration into it and writes it back in a second session.
 *
 * @ingroup hd1 */
class HD1 : public Radio
{
  Q_OBJECT

public:
  /** Takes ownership of the interface. */
  explicit HD1(HD1Interface *device=nullptr, QObject *parent=nullptr);
  ~HD1();

  /** Radio info for the HD1. */
  static RadioInfo defaultRadioInfo();

  const QString &name() const override;
  const RadioLimits &limits() const override;
  const Codeplug &codeplug() const override;
  Codeplug &codeplug() override;

  bool startDownload(const TransferFlags &flags, const ErrorStack &err=ErrorStack()) override;
  bool startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err=ErrorStack()) override;
  bool startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err=ErrorStack()) override;
  bool startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err=ErrorStack()) override;

protected:
  void run() override;

private:
  /** Reads all regions into the codeplug; @c progressScale is the share of the progress bar. */
  bool download(int progressScale, const ErrorStack &err=ErrorStack());
  /** Download, encode, write. */
  bool upload(const ErrorStack &err=ErrorStack());

protected:
  /** The interface to the radio. */
  HD1Interface *_dev;
  /** The name of the radio. */
  QString _name;
  /** The codeplug. */
  HD1Codeplug _codeplug;
  /** Flags for encoding the codeplug of an upload. */
  Codeplug::Flags _codeplugFlags;
  /** The configuration to upload. */
  Config *_config;
  /** The limits, created on first use. */
  static RadioLimits *_limits;
};

#endif // HD1_HH
