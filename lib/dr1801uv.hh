/** @defgroup dr1801uv BTECH DR-1801 UV
 *
 * This module collects all classes implementing the codeplug and communication protocol
 * for the BTECH DR-1801UV. This device is also known as BF-1801 A6 and is a completely different
 * device than the well known DM-1801. The former uses the Auctu A6 radio-on-a-chip.
 *
 * @ingroup auctus */
#ifndef DR1801UV_HH
#define DR1801UV_HH

#include "radio.hh"
#include "dr1801uv_interface.hh"
#include "dr1801uv_codeplug.hh"


/** Implements the BTECH DR-1801UV (BF-1801 A6).
 *
 * @ingroup dr1801uv */
class DR1801UV : public Radio
{
  Q_OBJECT

public:
  /** Constructs a new instance representig a DR-1801UV. */
  explicit DR1801UV(DR1801UVInterface *device=nullptr, QObject *parent=nullptr);


public:
  /** Retunrs the default radio info. */
  static RadioInfo defaultRadioInfo();

  const QString &name() const;

  const RadioLimits &limits() const;

  const Codeplug &codeplug() const;
  Codeplug &codeplug();

  bool startDownload(bool blocking, const ErrorStack &err);
  bool startUpload(Config *config, bool blocking, const Codeplug::Flags &flags, const ErrorStack &err);
  bool startUploadCallsignDB(UserDatabase *db, bool blocking, const CallsignDB::Selection &selection, const ErrorStack &err);

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
  void run();

private:
  virtual bool download();
  virtual bool upload();

protected:
  /** Owns the interface to the device. */
  DR1801UVInterface *_device;
  /** Holds the device name, once it got identified. */
  QString _name;
  /** The binary codeplug. */
  DR1801UVCodeplug _codeplug;

private:
  /** Holds the singleton instance of the radio limits. */
  static RadioLimits *_limits;
};

#endif // DR1801UV_HH
