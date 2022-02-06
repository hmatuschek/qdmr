/** @defgroup dsc Supported devices
 * This module collects all classes are device specific.
 *
 * That is, implementing device specific configurations, aka codeplugs as well as the specific
 * communication whith these radios.
 */

#ifndef RADIO_HH
#define RADIO_HH

#include <QThread>
#include "radioinfo.hh"
#include "radiointerface.hh"
#include "codeplug.hh"
#include "callsigndb.hh"
#include "errorstack.hh"

class Config;
class UserDatabase;


/** Simple container class to collect codeplug verification issues.
 * As all radios are programmed from a common configuration, some radios may not support all
 * features within the confiuration. Before uploading the device specific configuration, the
 * generic configuration gets verified against the device features. This may lead to several
 * issues/warning. This class represents such a issue/warning message.
 *
 * @ingroup rif */
class VerifyIssue {
public:
  /** Issue type. */
	typedef enum {
    NONE = 0,     ///< All ok.
    NOTIFICATION, ///< Inform user about changes made to the config to fit radio.
    WARNING,      ///< Verification warning, some configured fature is just ignored for the particular radio.
    ERROR         ///< Verification error, a consistent device specific configutation cannot be derived from the generic config.
	} Type;

public:
  /** Constructor from @c type and @c message. */
	inline VerifyIssue(Type type, const QString &message)
	    : _type(type), _message(message) { }

  /** Returns the verification issue type. */
	inline Type type() const { return _type; }
  /** Returns the verification issue message. */
	inline const QString & message() const { return _message; }

protected:
  /** The issue type. */
	Type _type;
  /** The issue message. */
	QString _message;
};

/** Flags to control the verification process.
 * @ingroup rif*/
struct VerifyFlags {
  /** If set, warnings will not interrupt the upload. */
  bool ignoreWarnings;
  /** If set, frequency limit voilations will be handled as warnings. */
  bool ignoreFrequencyLimits;

  /** Default constructor. */
  inline VerifyFlags()
    : ignoreWarnings(false), ignoreFrequencyLimits(false) {
    // pass...
  }
  /** Full constructor. */
  inline VerifyFlags(bool ignWarn, bool ignFreqLimits)
    : ignoreWarnings(ignWarn), ignoreFrequencyLimits(ignFreqLimits) {
    // pass...
  }
};


/** Base class for all Radio objects.
 *
 * The radio objects represents a connected radio. This class controlles the communication
 * with the device as well as the conversion betwenn device specific code-plugs and generic
 * configurations.
 *
 * @ingroup rif
 */
class Radio : public QThread
{
	Q_OBJECT

public:
  /** Represents a radio feature list, a generic configuration is verified against. */
  struct Features {
    /** Represents a frequency range [min, max]. */
    struct FrequencyRange {
      double min; ///< Lower frequency limit.
      double max; ///< Upper frequency limit.
      /** Constructs a frequency range from limits. */
      FrequencyRange(double lower, double upper);
      /** Constructs a frequency range from limits. */
      FrequencyRange(double limits[2]);
      /** Returns @c true if @c f is inside this limit. */
      bool contains(double f) const;
    };

    /** A list of frequency limits. */
    struct FrequencyLimits {
      /** The actual list of frequency limits. */
      QVector <FrequencyRange> ranges;
      /** Constructs a list of frequency limits. */
      FrequencyLimits(const QVector<FrequencyRange> &frequency_ranges);
      /** Check if the given frequency is within one of the frequency limits. */
      bool contains(double f) const;
    };

    /** If @c true, shows a beta warning at upload. */
    bool betaWarning;

    /** If @c true, the device supports DMR. */
		bool hasDigital;
    /** If @c true, the device supports FM. */
		bool hasAnalog;

    /** The frequency limits of the radio. */
    FrequencyLimits frequencyLimits;

    /** Maximum number of radio IDs. */
    int maxRadioIDs;
    /** If @c true, the radio requires a default radio ID. */
    bool needsDefaultRadioID;

    /** Maximum length of boot messages. */
		int maxIntroLineLength;

    /** Maximum number of channels. */
		int maxChannels;
    /** Maximum length of channel names. */
		int maxChannelNameLength;
    /** If @c true, the radio allows channels without default contact. */
    bool allowChannelNoDefaultContact;

    /** Maximum number of zones. */
		int maxZones;
    /** Maximum length of zone names. */
		int maxZoneNameLength;
    /** Maximum number of channels per zone. */
		int maxChannelsInZone;
    /** If @c true, the radio has a zone list for each VFO. */
    bool hasABZone;

    /** If true, the radio support scanlists. */
    bool hasScanlists;
    /** Maximum number of scanlists. */
		int maxScanlists;
    /** Maximum length of scanlist names. */
		int maxScanlistNameLength;
    /** Maximum number of channels per scanlist. */
		int maxChannelsInScanlist;
    /** Does scanlist needs a priority channel?. */
    bool scanListNeedsPriority;

    /** Maximum number of contacts. */
		int maxContacts;
    /** Maximum length of contact name. */
		int maxContactNameLength;
    /** Maximum number of RX group lists. */
		int maxGrouplists;
    /** Maximum length of grouplist name. */
		int maxGrouplistNameLength;
    /** Maximum number contacts per RX group list. */
		int maxContactsInGrouplist;

    /** If @c true, the device supports GPS. */
    bool hasGPS;
    /** Maximum number of different GPS systems. */
    int maxGPSSystems;

    /** If @c true, the device supports APRS (analog) position reporting. */
    bool hasAPRS;
    /** Maximum number of different APRS systems. */
    int maxAPRSSystems;

    /** If @c true, the device supports roaming. */
    bool hasRoaming;
    /** Specifies the maximum number of roaming channels. */
    int maxRoamingChannels;
    /** Specifies the maximum number of roaming zones. */
    int maxRoamingZones;
    /** Specifies the maximum number of channels per roaming zone. */
    int maxChannelsInRoamingZone;

    /** If true, the radio supports a callsign DB. */
    bool hasCallsignDB;
    /** If true, the callsign DB of the radio is implemented. */
    bool callsignDBImplemented;
    /** Maximum number of entries in callsign DB. */
    unsigned maxCallsignsInDB;
  };

  /** Possible states of the radio object. */
	typedef enum {
    StatusIdle,            ///< Idle, nothing to do.
    StatusDownload,        ///< Downloading codeplug.
    StatusUpload,          ///< Uploading codeplug.
    StatusUploadCallsigns, ///< Uploading codeplug.
    StatusError            ///< An error occured.
  } Status;

public:
  /** Default constructor. */
	explicit Radio(QObject *parent = nullptr);

  virtual ~Radio();

  /** Returns the name of the radio (e.g., device identifier). */
	virtual const QString &name() const = 0;

  /** Returns the features for the particular radio. */
	virtual const Features &features() const = 0;

  /** Returns the codeplug instance. */
  virtual const Codeplug &codeplug() const = 0;
  /** Returns the codeplug instance. */
  virtual Codeplug &codeplug() = 0;

  /** Returns the call-sign DB instance. */
  virtual const CallsignDB *callsignDB() const;
  /** Returns the call-sign DB instance. */
  virtual CallsignDB *callsignDB();

  /** Verifies the configuration against the radio features.
   * On exit, @c issues will contain the issues found and the maximum severity is returned. */
  virtual VerifyIssue::Type verifyConfig(Config *config, QList<VerifyIssue> &issues,
                                         const VerifyFlags &flags=VerifyFlags());

  /** Returns the current status. */
  Status status() const;

  /** Returns the error stack, passed to @c startDownload, @c startUpload or
   * @c startUploadCallsignDB. It contains the error messages from the upload/download process. */
  const ErrorStack &errorStack() const;

public:
  /** Tries to detect the radio connected to the specified interface or constructs the specified
   * radio using the @c RadioInfo passed by @c force. */
  static Radio *detect(const USBDeviceDescriptor &descr, const RadioInfo &force=RadioInfo(),
                       const ErrorStack &err=ErrorStack());

public slots:
  /** Starts the download of the codeplug.
   * Once the download finished, the codeplug can be accessed and decoded using
   * the @c codeplug() method. */
  virtual bool startDownload(bool blocking=false, const ErrorStack &err=ErrorStack()) = 0;
  /** Derives the device-specific codeplug from the generic configuration and uploads that
   * codeplug to the radio. */
  virtual bool startUpload(
      Config *config, bool blocking=false,
      const Codeplug::Flags &flags = Codeplug::Flags(), const ErrorStack &err=ErrorStack()) = 0;
  /** Assembles the callsign DB from the given one and uploads it to the device. */
  virtual bool startUploadCallsignDB(
      UserDatabase *db, bool blocking=false,
      const CallsignDB::Selection &selection=CallsignDB::Selection(),
      const ErrorStack &err=ErrorStack()) = 0;

signals:
  /** Gets emitted once the codeplug download has been started. */
	void downloadStarted();
  /** Gets emitted on download progress (e.g., for progress bars). */
	void downloadProgress(int percent);
  /** Gets emitted once the codeplug download has been finished. */
  void downloadFinished(Radio *radio, Codeplug *codeplug);
  /** Gets emitted if there was an error during the codeplug download. */
	void downloadError(Radio *radio);

  /** Gets emitted once the codeplug upload has been started. */
	void uploadStarted();
  /** Gets emitted on upload progress (e.g., for progress bars). */
	void uploadProgress(int percent);
  /** Gets emitted if there was an error during the upload. */
	void uploadError(Radio *radio);
  /** Gets emitted once the codeplug upload has been completed successfully. */
	void uploadComplete(Radio *radio);

protected:
  /** The current state/task. */
  Status _task;
  /** The error stack. */
  ErrorStack _errorStack;
};

#endif // RADIO_HH
