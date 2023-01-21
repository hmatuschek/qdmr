#ifndef DR1801UV_HH
#define DR1801UV_HH

#include "radio.hh"
#include "dr1801uv_interface.hh"
#include "dr1801uv_codeplug.hh"


class DR1801UV : public Radio
{
  Q_OBJECT

public:
  explicit DR1801UV(DR1801UVInterface *device=nullptr, QObject *parent=nullptr);


public:
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
  DR1801UVInterface *_device;
  QString _name;
  DR1801UVCodeplug _codeplug;

private:
  /** Holds the singleton instance of the radio limits. */
  static RadioLimits *_limits;
};

#endif // DR1801UV_HH
