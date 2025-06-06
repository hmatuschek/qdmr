#ifndef OPENUV380_CODEPLUG_HH
#define OPENUV380_CODEPLUG_HH

#include "opengd77base_codeplug.hh"
#include "opengd77_extension.hh"


/** Represents, encodes and decodes the device specific codeplug for Open MD-UV380 firmware.
 *
 * This codeplug is almost identical to the original GD77 codeplug.
 *
 * @ingroup ogd77 */
class OpenUV380Codeplug: public OpenGD77BaseCodeplug
{
  Q_OBJECT

public:
  /** Constructs an empty codeplug for the Open MD-UV380. */
  explicit OpenUV380Codeplug(QObject *parent=nullptr);

public:
  void clearGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearDTMFSettings();
  bool encodeDTMFSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeDTMFSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearAPRSSettings();
  bool encodeAPRSSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeAPRSSettings(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkAPRSSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearContacts();
  bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearDTMFContacts();
  bool encodeDTMFContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createDTMFContacts(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearChannels();
  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearBootSettings();
  bool encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeBootSettings(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearVFOSettings();

  void clearZones();
  bool encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createZones(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack());

  void clearGroupLists();
  bool encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool createGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack());

public:
  /** Some Limits for this codeplug. */
  struct Limit: public Element::Limit {
    /** Number of channel banks. */
    static constexpr unsigned int channelBanks() { return 8; }
  };

protected:
  /** Internal used image indices. */
  struct ImageIndex {
    /// @cond DO_NOT_DOCUEMNT
    static constexpr unsigned int settings()           { return FLASH; }
    static constexpr unsigned int dtmfSettings()       { return FLASH; }
    static constexpr unsigned int aprsSettings()       { return FLASH; }
    static constexpr unsigned int dtmfContacts()       { return FLASH; }
    static constexpr unsigned int channelBank0()       { return FLASH; }
    static constexpr unsigned int bootSettings()       { return FLASH; }
    static constexpr unsigned int vfoA()               { return FLASH; }
    static constexpr unsigned int vfoB()               { return FLASH; }
    static constexpr unsigned int zoneBank()           { return FLASH; }
    static constexpr unsigned int additionalSettings() { return FLASH; }
    static constexpr unsigned int channelBank1()       { return FLASH; }
    static constexpr unsigned int contacts()           { return FLASH; }
    static constexpr unsigned int groupLists()         { return FLASH; }
    /// @endcond
  };

  /** Some offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUEMNT
    static constexpr unsigned int settings()           { return 0x00000080; }
    static constexpr unsigned int dtmfSettings()       { return 0x00001470; }
    static constexpr unsigned int aprsSettings()       { return 0x00001588; }
    static constexpr unsigned int dtmfContacts()       { return 0x00002f88; }
    static constexpr unsigned int channelBank0()       { return 0x00003780; } // Channels 1-128
    static constexpr unsigned int bootSettings()       { return 0x00007518; }
    static constexpr unsigned int vfoA()               { return 0x00007590; }
    static constexpr unsigned int vfoB()               { return 0x000075c8; }
    static constexpr unsigned int zoneBank()           { return 0x00008010; }
    static constexpr unsigned int additionalSettings() { return 0x00020000; }
    static constexpr unsigned int channelBank1()       { return 0x0009b1b0; } // Channels 129-1024
    static constexpr unsigned int contacts()           { return 0x000a7620; }
    static constexpr unsigned int groupLists()         { return 0x000ad620; }
    /// @endcond
  };
};

#endif // OPENUV380_CODEPLUG_HH
