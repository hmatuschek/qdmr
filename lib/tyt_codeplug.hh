#ifndef RT3S_GPS_CODEPLUG_HH
#define RT3S_GPS_CODEPLUG_HH

#include <QDateTime>

#include "codeplug.hh"
#include "signaling.hh"

class Channel;
class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;
class GPSSystem;
class CodeplugContext;


/**
 * @ingroup tyt */
class TyTCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit TyTCodeplug(QObject *parent = nullptr);
  /** Destructor. */
  virtual ~TyTCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

public:
  virtual void clearTimestamp();
  virtual bool encodeTimestamp(Config *config, const Flags &flags);
  virtual bool decodeTimestamp(Config *config);

  virtual void clearGeneralSettings();
  virtual bool encodeGeneralSettings(Config *config, const Flags &flags);
  virtual bool decodeGeneralSettings(Config *config);

  virtual void clearBootSettings();
  virtual bool encodeBootSettings(Config *config, const Flags &flags);
  virtual bool decodeBootSettings(Config *config);

  virtual void clearContacts();
  virtual bool encodeContacts(Config *config, const Flags &flags);
  virtual bool createContacts(Config *config, CodeplugContext &ctx);

  virtual void clearGroupLists();
  virtual bool encodeGroupLists(Config *config, const Flags &flags);
  virtual bool createGroupLists(Config *config, CodeplugContext &ctx);
  virtual bool linkGroupLists(Config *config, CodeplugContext &ctx);

  virtual void clearChannels();
  virtual bool encodeChannels(Config *config, const Flags &flags);
  virtual bool createChannels(Config *config, CodeplugContext &ctx);
  virtual bool linkChannels(Config *config, CodeplugContext &ctx);

  virtual void clearZones();
  virtual bool encodeZones(Config *config, const Flags &flags);
  virtual bool createZones(Config *config, CodeplugContext &ctx);
  virtual bool linkZones(Config *config, CodeplugContext &ctx);

  virtual void clearScanLists();
  virtual bool encodeScanLists(Config *config, const Flags &flags);
  virtual bool createScanLists(Config *config, CodeplugContext &ctx);
  virtual bool linkScanLists(Config *config, CodeplugContext &ctx);

  virtual void clearPositioningSystems();
  virtual bool encodePositioningSystems(Config *config, const Flags &flags);
  virtual bool createPositioningSystems(Config *config, CodeplugContext &ctx);
  virtual bool linkPositioningSystems(Config *config, CodeplugContext &ctx);

  virtual void clearMenuSettings();
  virtual void clearButtonSettings();
  virtual void clearTextMessages();
  virtual void clearPrivacyKeys();
  virtual void clearEmergencySystems();
  virtual void clearVFOSettings();
};

#endif // UV390_CODEPLUG_HH
