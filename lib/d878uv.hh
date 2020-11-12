/** @defgroup d878uv Anytone AT-D878UV
 * Device specific classes for Anytone AT-D878UV.
 *
 * \image html d878uv.jpg "AT-D878UV" width=200px
 * \image latex d878uv.jpg "AT-D878UV" width=200px
 *
 * @ingroup dsc */
#ifndef __D878UV_HH__
#define __D878UV_HH__

#include "radio.hh"
#include "anytone_interface.hh"
#include "d878uv_codeplug.hh"


/** Implements an interface to Anytone AT-D878UV VHF/UHF 7W DMR (Tier I&II) radios.
 *
 * @ingroup d878uv */
class D878UV: public Radio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit D878UV(QObject *parent=nullptr);

  const QString &name() const;
  const Radio::Features &features() const;
  const CodePlug &codeplug() const;
  CodePlug &codeplug();

public slots:
  /** Starts the download of the codeplug and derives the generic configuration from it. */
	bool startDownload(Config *config, bool blocking=false);
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  bool startUpload(Config *config, bool blocking=false, bool update=true);

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
	void run();

protected:
  /** The device identifier. */
	QString _name;
  /** The interface to the radio. */
  AnytoneInterface *_dev;
  /** If @c true, the codeplug on the radio gets updated upon upload. If @c false, it gets
   * overridden. */
  bool _codeplugUpdate;
  /** The generic configuration. */
	Config *_config;
  /** A weak reference to the user-database. */
  UserDatabase *_userDB;
  /** The actual binary codeplug representation. */
  D878UVCodeplug _codeplug;
};

#endif // __D878UV_HH__
