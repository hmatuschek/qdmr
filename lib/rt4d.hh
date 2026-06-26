/** @defgroup rt4d Radtel RT-4D
 *  @ingroup dsc */
#ifndef LIBDMRCONF_RT4D_HH
#define LIBDMRCONF_RT4D_HH

#include "radio.hh"
#include "radioinfo.hh"
#include "rt4d_codeplug.hh"
#include "rt4d_limits.hh"

class RT4DInterface;


/** Implements the Radtel RT-4D.
 * @ingroup rt4d */
class RT4D: public Radio
{
  Q_OBJECT;

public:
  /** Default constructor. */
  explicit RT4D(RT4DInterface *device=nullptr, QObject *parent=nullptr);
  /** Destructor. Closes and frees the device interface. */
  ~RT4D();

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

  const QString &name() const override;

  const RadioLimits &limits() const override;

  const Codeplug &codeplug() const override;
  Codeplug &codeplug() override;

  bool startDownload(const TransferFlags &flags, const ErrorStack &err=ErrorStack()) override;
  bool startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err) override;
  bool startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err) override;
  bool startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err) override;

private:
  virtual bool download(const ErrorStack &err=ErrorStack());
  virtual bool upload(const ErrorStack &err=ErrorStack());
  virtual bool uploadCallsigns(const ErrorStack &err=ErrorStack());

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
  void run() override;

protected:
  /** Holds the radio name. */
  QString _radioName;
  RT4DInterface *_dev;
  /** Some limits for the codeplug.*/
  RT4DLimits _limits;
  /** Holds the flags to control assembly and upload of code-plugs. */
  Codeplug::Flags _codeplugFlags;
  RT4DCodeplug _codeplug;
  /** The generic configuration. */
  Config *_config;
};

#endif //LIBDMRCONF_RT4D_HH
