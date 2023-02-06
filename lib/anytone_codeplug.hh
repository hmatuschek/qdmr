#ifndef ANYTONECODEPLUG_HH
#define ANYTONECODEPLUG_HH

#include "codeplug.hh"
#include "anytone_extension.hh"

/** Base class interface for all Anytone radio codeplugs.
 *
 * This class extends the generic @c CodePlug to provide an interface to the multi-step up and
 * download of the binary codeplug. In contrast to the majority of radios, the Anytone codeplugs
 * are heavily segmented and only valid sections are read from a written to the device.
 *
 * @ingroup anytone */
class AnytoneCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Represents the base class for channel encodings in all AnyTone codeplugs.
   *
   * Memory layout of encoded channel (0x40 bytes):
   * @verbinclude anytone_channel.txt
   */
  class ChannelElement: public Element
  {
  public:
    /** Defines all possible channel modes, see @c channelMode. */
    enum class Mode {
      Analog      = 0,               ///< Analog channel.
      Digital     = 1,               ///< Digital (DMR) channel.
      MixedAnalog = 2,               ///< Mixed, analog channel with digital RX.
      MixedDigital = 3               ///< Mixed, digital channel with analog RX.
    };

    /** Defines all possible power settings.*/
    enum Power {
      POWER_LOW    = 0,              ///< Low power, usually 1W.
      POWER_MIDDLE = 1,              ///< Medium power, usually 2.5W.
      POWER_HIGH   = 2,              ///< High power, usually 5W.
      POWER_TURBO  = 3               ///< Higher power, usually 7W on VHF and 6W on UHF.
    };

    /** Defines all possible repeater modes. */
    enum class RepeaterMode {
      Simplex  = 0,                  ///< Simplex mode, that is TX frequency = RX frequency. @c tx_offset is ignored.
      Positive = 1,                  ///< Repeater mode with positive @c tx_offset.
      Negative = 2                   ///< Repeater mode with negative @c tx_offset.
    };

    /** Possible analog signaling modes. */
    enum class SignalingMode {
      None = 0,                      ///< None.
      CTCSS = 1,                     ///< Use CTCSS tones
      DCS = 2                        ///< Use DCS codes.
    };

    /** Defines possible admit criteria. */
    enum class Admit {
      Always = 0,                  ///< For both channel types.
      Free = 1,                    ///< For digital channels.
      DifferentColorCode = 2,      ///< For digital channels.
      SameColorCode = 3,               ///< For digital channels.
      Tone = 1,                    ///< For analog channels
      Busy = 2                     ///< For analog channels.
    };

    /** Defines all possible optional signalling settings. */
    enum class OptSignaling {
      Off = 0,                    ///< None.
      DTMF = 1,                   ///< Use DTMF.
      TwoTone = 2,                ///< Use 2-tone.
      FiveTone = 3                ///< Use 5-tone.
    };

  protected:
    /** Hidden constructor. */
    ChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Resets the channel. */
    void clear();

    /** Returns the RX frequency in Hz. */
    virtual unsigned rxFrequency() const;
    /** Sets the RX frequency in Hz. */
    virtual void setRXFrequency(unsigned hz);

    /** Returns the TX frequency offset in Hz.
     * This method returns an unsigned value, the sign of the offset frequency is stored in
     * @c repeaterMode(). */
    virtual unsigned txOffset() const;
    /** Sets the TX frequency offset in Hz.
     * This method accepts unsigned values, the sign of the offset frequency is stored in
     * @c repeaterMode(). */
    virtual void setTXOffset(unsigned hz);
    /** Returns the TX frequency in Hz. */
    virtual unsigned txFrequency() const;
    /** Sets the TX frequency indirectly. That is, relative to the RX frequency which must be set
     * first. This method also updates the @c repeaterMode. */
    virtual void setTXFrequency(unsigned hz);

    /** Returns the channel mode (analog, digtital, etc). */
    virtual Mode mode() const;
    /** Sets the channel mode. */
    virtual void setMode(Mode mode);

    /** Returns the channel power. */
    virtual Channel::Power power() const;
    /** Sets the channel power. */
    virtual void setPower(Channel::Power power);

    /** Returns the band width of the channel. */
    virtual FMChannel::Bandwidth bandwidth() const;
    /** Sets the band width of the channel. */
    virtual void setBandwidth(FMChannel::Bandwidth bw);

    /** Returns the transmit offset direction. */
    virtual RepeaterMode repeaterMode() const;
    /** Sets the transmit offset direction. */
    virtual void setRepeaterMode(RepeaterMode mode);

    /** Returns the RX signaling mode */
    virtual SignalingMode rxSignalingMode() const;
    /** Sets the RX signaling mode */
    virtual void setRXSignalingMode(SignalingMode mode);
    /** Simplified access to RX signaling (tone). */
    virtual Signaling::Code rxTone() const;
    /** Sets the RX signaling (tone). */
    virtual void setRXTone(Signaling::Code code);

    /** Returns the TX signaling mode */
    virtual SignalingMode txSignalingMode() const;
    /** Sets the TX signaling mode */
    virtual void setTXSignalingMode(SignalingMode mode);
    /** Simplified access to TX signaling (tone). */
    virtual Signaling::Code txTone() const;
    /** Sets the RX signaling (tone). */
    virtual void setTXTone(Signaling::Code code);

    /** Returns @c true if the CTCSS phase reversal is enabled. */
    virtual bool ctcssPhaseReversal() const;
    /** Enables/disables CTCSS phase reversal. */
    virtual void enableCTCSSPhaseReversal(bool enable);

    /** Returns @c true if the RX only is enabled. */
    virtual bool rxOnly() const;
    /** Enables/disables RX only. */
    virtual void enableRXOnly(bool enable);
    /** Returns @c true if the call confirm is enabled. */
    virtual bool callConfirm() const;
    /** Enables/disables call confirm. */
    virtual void enableCallConfirm(bool enable);
    /** Returns @c true if the talkaround is enabled. */
    virtual bool talkaround() const;
    /** Enables/disables talkaround. */
    virtual void enableTalkaround(bool enable);

    /** Returns @c true if the TX CTCSS tone frequency is custom (non standard). */
    virtual bool txCTCSSIsCustom() const;
    /** Returns the TX CTCSS tone. */
    virtual Signaling::Code txCTCSS() const;
    /** Sets the TX CTCSS tone. */
    virtual void setTXCTCSS(Signaling::Code tone);
    /** Enables TX custom CTCSS frequency. */
    virtual void enableTXCustomCTCSS();
    /** Returns @c true if the RX CTCSS tone frequency is custom (non standard). */
    virtual bool rxCTCSSIsCustom() const;
    /** Returns the RX CTCSS tone. */
    virtual Signaling::Code rxCTCSS() const;
    /** Sets the RX CTCSS tone. */
    virtual void setRXCTCSS(Signaling::Code tone);
    /** Enables RX custom CTCSS frequency. */
    virtual void enableRXCustomCTCSS();

    /** Returns the TX DCS code. */
    virtual Signaling::Code txDCS() const;
    /** Sets the TX DCS code. */
    virtual void setTXDCS(Signaling::Code code);
    /** Returns the RX DCS code. */
    virtual Signaling::Code rxDCS() const;
    /** Sets the RX DCS code. */
    virtual void setRXDCS(Signaling::Code code);

    /** Returns the custom CTCSS frequency in Hz. */
    virtual double customCTCSSFrequency() const;
    /** Sets the custom CTCSS frequency in Hz. */
    virtual void setCustomCTCSSFrequency(double hz);

    /** Returns the 2-tone decode index (0-based). */
    virtual unsigned twoToneDecodeIndex() const;
    /** Sets the 2-tone decode index (0-based). */
    virtual void setTwoToneDecodeIndex(unsigned idx);

    /** Returns the transmit contact index (0-based). */
    virtual unsigned contactIndex() const;
    /** Sets the transmit contact index (0-based). */
    virtual void setContactIndex(unsigned idx);

    /** Returns the radio ID index (0-based). */
    virtual unsigned radioIDIndex() const;
    /** Sets the radio ID index (0-based). */
    virtual void setRadioIDIndex(unsigned idx);

    /** Returns @c true if the sequelch is silent and @c false if open. */
    virtual AnytoneFMChannelExtension::SquelchMode squelchMode() const;
    /** Enables/disables silent squelch. */
    virtual void setSquelchMode(AnytoneFMChannelExtension::SquelchMode mode);

    /** Returns the admit criterion. */
    virtual Admit admit() const;
    /** Sets the admit criterion. */
    virtual void setAdmit(Admit admit);

    /** Returns the optional signalling type. */
    virtual OptSignaling optionalSignaling() const;
    /** Sets the optional signaling type. */
    virtual void setOptionalSignaling(OptSignaling sig);

    /** Returns @c true, if a scan list index is set. */
    virtual bool hasScanListIndex() const;
    /** Returns the scan list index (0-based). */
    virtual unsigned scanListIndex() const;
    /** Sets the scan list index (0-based). */
    virtual void setScanListIndex(unsigned idx);
    /** Clears the scan list index. */
    virtual void clearScanListIndex();

    /** Returns @c true, if a group list index is set. */
    virtual bool hasGroupListIndex() const;
    /** Returns the scan list index (0-based). */
    virtual unsigned groupListIndex() const;
    /** Sets the group list index (0-based). */
    virtual void setGroupListIndex(unsigned idx);
    /** Clears the group list index. */
    virtual void clearGroupListIndex();

    /** Returns the two-tone ID index (0-based). */
    virtual unsigned twoToneIDIndex() const;
    /** Sets the two-tone ID index (0-based). */
    virtual void setTwoToneIDIndex(unsigned idx);
    /** Returns the five-tone ID index (0-based). */
    virtual unsigned fiveToneIDIndex() const;
    /** Sets the five-tone ID index (0-based). */
    virtual void setFiveToneIDIndex(unsigned idx);
    /** Returns the DTFM ID index (0-based). */
    virtual unsigned dtmfIDIndex() const;
    /** Sets the DTMF ID index (0-based). */
    virtual void setDTMFIDIndex(unsigned idx);

    /** Returns the color code. */
    virtual unsigned colorCode() const;
    /** Sets the color code. */
    virtual void setColorCode(unsigned code);

    /** Returns the time slot. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns @c true if SMS confirmation is enabled. */
    virtual bool smsConfirm() const;
    /** Enables/disables SMS confirmation. */
    virtual void enableSMSConfirm(bool enable);
    /** Returns @c true if simplex TDMA is enabled. */
    virtual bool simplexTDMA() const;
    /** Enables/disables simplex TDMA confirmation. */
    virtual void enableSimplexTDMA(bool enable);
    /** Returns @c true if adaptive TDMA is enabled. */
    virtual bool adaptiveTDMA() const;
    /** Enables/disables adaptive TDMA. */
    virtual void enableAdaptiveTDMA(bool enable);
    /** Returns @c true if RX APRS is enabled. */
    virtual bool rxAPRS() const;
    /** Enables/disables RX APRS. */
    virtual void enableRXAPRS(bool enable);
    /** Returns @c true if enhanced encryption is enabled. */
    virtual bool enhancedEncryption() const;
    /** Enables/disables enhanced encryption. */
    virtual void enableEnhancedEncryption(bool enable);
    /** Returns @c true if lone worker is enabled. */
    virtual bool loneWorker() const;
    /** Enables/disables lone worker. */
    virtual void enableLoneWorker(bool enable);

    /** Returns @c true if an encryption key is set. */
    virtual bool hasEncryptionKeyIndex() const;
    /** Returns the AES (enhanced) encryption key index (0-based). */
    virtual unsigned encryptionKeyIndex() const;
    /** Sets the AES (enahnced) encryption key index (0-based). */
    virtual void setEncryptionKeyIndex(unsigned idx);
    /** Clears the encryption key index. */
    virtual void clearEncryptionKeyIndex();

    /** Returns the channel name. */
    virtual QString name() const;
    /** Sets the channel name. */
    virtual void setName(const QString &name);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *toChannelObj(Context &ctx) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx);
  };

  /** Represents the base class for conacts in all AnyTone codeplugs.
   *
   * Memory layout of encoded contact (0x64 bytes):
   * @verbinclude anytone_contact.txt
   */
  class ContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ContactElement();

    /** Resets the contact element. */
    void clear();
    /** Returns @c true if the element is valid. */
    bool isValid() const;

    /** Returns the contact type. */
    virtual DMRContact::Type type() const;
    /** Sets the contact type. */
    virtual void setType(DMRContact::Type type);

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &name);

    /** Returns the contact number. */
    virtual unsigned number() const;
    /** Sets the contact number. */
    virtual void setNumber(unsigned number);

    /** Returns the alert type. */
    virtual AnytoneContactExtension::AlertType alertType() const;
    /** Sets the alert type. */
    virtual void setAlertType(AnytoneContactExtension::AlertType type);

    /** Assembles a @c DigitalContact from this contact. */
    virtual DMRContact *toContactObj(Context &ctx) const;
    /** Constructs this contact from the give @c DigitalContact. */
    virtual bool fromContactObj(const DMRContact *contact, Context &ctx);
  };

  /** Represents the base class for analog (DTMF) contacts in all AnyTone codeplugs.
   *
   * Encoding of the DTMF contact (0x30 bytes):
   * @verbinclude anytone_dtmfcontact.txt */
  class DTMFContactElement: public Element
  {
    enum Offsets {
      DIGITS = 0x0,
      DIGIT_COUNT = 0x7,
      NAME = DIGIT_COUNT + 1
    };
    static const unsigned NAME_LEN = 15;

  protected:
    /** Hidden constructor. */
    DTMFContactElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit DTMFContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~DTMFContactElement();

    /** Resets the contact element. */
    void clear();

    /** Returns the number of the contact. */
    virtual QString number() const;
    /** Sets the number of the contact. */
    virtual void setNumber(const QString &number);

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &name);

    /** Creates an DTMF contact from the entry. */
    virtual DTMFContact *toContact() const;
    /** Encodes an DTMF contact from the given one. */
    virtual bool fromContact(const DTMFContact *contact);
  };

  /** Represents the base class for group lists in all AnyTone codeplugs.
   *
   * Encoding of a group list (0x120 bytes):
   * @verbinclude anytone_grouplist.txt */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    /** Clears the group list. */
    void clear();
    /** Returns @c true if the group list is valid. */
    bool isValid() const;

    /** Returns the name of the group list. */
    virtual QString name() const;
    /** Sets the name of the group list. */
    virtual void setName(const QString &name);

    /** Returns @c true if the n-th member index is valid. */
    virtual bool hasMemberIndex(unsigned n) const;
    /** Returns the n-th member index. */
    virtual unsigned memberIndex(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(unsigned n, unsigned idx);
    /** Clears the n-th member index. */
    virtual void clearMemberIndex(unsigned n);

    /** Constructs a new @c RXGroupList from this group list.
     * None of the members are added yet. Call @c linkGroupList
     * to do that. */
    virtual RXGroupList *toGroupListObj() const;
    /** Populates the @c RXGroupList from this group list. The CodeplugContext
     * is used to map the member indices. */
    virtual bool linkGroupList(RXGroupList *lst, Context &ctx) const;
    /** Constructs this group list from the given @c RXGroupList. */
    virtual bool fromGroupListObj(const RXGroupList *lst, Context &ctx);
  };

  /** Represents the base class for scan lists in all AnyTone codeplugs.
   *
   * Memory layout of encoded scanlist (0x90 bytes):
   * @verbinclude anytone_scanlist.txt */
  class ScanListElement: public Element
  {
  public:
    /** Defines all possible priority channel selections. */
    enum class PriChannel {
      Off = 0,                   ///< Off.
      Primary = 1,               ///< Priority Channel Select 1.
      Secondary = 2,             ///< Priority Channel Select 2.
      Both = 3                   ///< Priority Channel Select 1 + Priority Channel Select 2.
    };

    /** Defines all possible reply channel selections. */
    enum class RevertChannel {
      Selected = 0,              ///< Selected channel.
      SelectedActive = 1,        ///< Selected + active channel.
      Primary = 2,               ///< Primary channel.
      Secondary = 3,             ///< Secondary channel.
      LastCalled = 4,            ///< Last Called.
      LastUsed = 5,              ///< Last Used.
      PrimaryActive = 6,         ///< Primary + active channel.
      SecondaryActive = 7        ///< Secondary + active channel.
    };

  protected:
    /** Hidden constructor. */
    ScanListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ScanListElement(uint8_t *ptr);

    /** Resets the scan list. */
    void clear();

    /** Returns the priority channel selection. */
    virtual PriChannel priorityChannels() const;
    /** Sets the priority channel selection. */
    virtual void setPriorityChannels(PriChannel sel);

    /** Returns @c true if the primary channel is set. */
    virtual bool hasPrimary() const;
    /** Returns @c true if the primary channel is set to the selected channel. */
    virtual bool primaryIsSelected() const;
    /** Returns the primary channel index. */
    virtual unsigned primary() const;
    /** Sets the primary channel index. */
    virtual void setPrimary(unsigned idx);
    /** Sets the primary channel to be selected channel. */
    virtual void setPrimarySelected();
    /** Clears the primary channel index. */
    virtual void clearPrimaryChannel();

    /** Returns @c true if the secondary channel is set. */
    virtual bool hasSecondary() const;
    /** Returns @c true if the secondary channel is set to the selected channel. */
    virtual bool secondaryIsSelected() const;
    /** Returns the secondary channel index. */
    virtual unsigned secondary() const;
    /** Sets the secondary channel index. */
    virtual void setSecondary(unsigned idx);
    /** Sets the secondary channel to be selected channel. */
    virtual void setSecondarySelected();
    /** Clears the secondary channel index. */
    virtual void clearSecondaryChannel();

    /** Returns the look back time A in seconds. */
    virtual unsigned lookBackTimeA() const;
    /** Sets the look back time A in seconds. */
    virtual void setLookBackTimeA(unsigned sec);
    /** Returns the look back time B in seconds. */
    virtual unsigned lookBackTimeB() const;
    /** Sets the look back time B in seconds. */
    virtual void setLookBackTimeB(unsigned sec);
    /** Returns the drop out delay in seconds. */
    virtual unsigned dropOutDelay() const;
    /** Sets the drop out delay in seconds. */
    virtual void setDropOutDelay(unsigned sec);
    /** Returns the dwell time in seconds. */
    virtual unsigned dwellTime() const;
    /** Sets the dwell time in seconds. */
    virtual void setDwellTime(unsigned sec);

    /** Returns the revert channel type. */
    virtual RevertChannel revertChannel() const;
    /** Sets the revert channel type. */
    virtual void setRevertChannel(RevertChannel type);

    /** Returns the name of the scan list. */
    virtual QString name() const;
    /** Sets the name of the scan list. */
    virtual void setName(const QString &name);

    /** Returns @c true if the n-th member index is set. */
    virtual bool hasMemberIndex(unsigned n) const;
    /** Returns the n-th member index. */
    virtual unsigned memberIndex(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(unsigned n, unsigned idx);
    /** Clears the n-th member index. */
    virtual void clearMemberIndex(unsigned n);

    /** Constructs a ScanList object from this definition. This only sets the properties of
     * the scan list. To associate all members with the scan list object, call @c linkScanListObj. */
    virtual ScanList *toScanListObj() const;
    /** Links all channels (members and primary channels) with the given scan-list object. */
    virtual bool linkScanListObj(ScanList *lst, Context &ctx) const;
    /** Constructs the binary representation from the give config. */
    virtual bool fromScanListObj(ScanList *lst, Context &ctx);
  };

  /** Represents the base class for radio IDs in all AnyTone codeplugs.
   *
   * Memory layout of encoded scanlist (0x20 bytes):
   * @verbinclude anytone_radioid.txt */
  class RadioIDElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RadioIDElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    RadioIDElement(uint8_t *ptr);

    /** Resets the radio ID. */
    void clear();

    /** Returns the number of the radio ID. */
    virtual unsigned number() const;
    /** Sets the number of the radio ID. */
    virtual void setNumber(unsigned number);

    /** Returns the name of the radio ID. */
    virtual QString name() const;
    /** Sets the name of the radio ID. */
    virtual void setName(const QString &name);

    /** Encodes the given RadioID. */
    virtual bool fromRadioID(DMRRadioID *id);
    /** Constructs a new radio id. */
    virtual DMRRadioID *toRadioID() const;
  };

  /** Represents the base class for the settings element in all AnyTone codeplugs.
   *
   * Memory layout of encoded general settings (0xd0 bytes):
   * @verbinclude anytone_generalsettings.txt
   */
  class GeneralSettingsElement: public Element
  {
  public:
    /** Possible automatic shutdown delays. */
    enum class AutoShutdown {
      Off = 0, After10min = 1, After30min  = 2, After60min  = 3, After120min = 4,
    };

    /** Encodes the auto-repeater offset sign. */
    enum class AutoRepDir {
      Off = 0,       ///< Disabled.
      Positive = 1,  ///< Positive frequency offset.
      Negative = 2   ///< Negative frequency offset.
    };

    /** What to show from the last caller. */
    enum class LastCallerDisplayMode {
      Off = 0, ID = 1, Call = 2, Both = 3
    };

    /** Possible display colors. */
    enum class Color {
      Black = 0, Red = 1
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);

    /** Resets the general settings. */
    void clear();

    /** Returns @c true if the key tone is enabled. */
    virtual bool keyTone() const;
    /** Enables/disables the key tone. */
    virtual void enableKeyTone(bool enable);
    /** Returns @c true if the radio displays frequecies instead of channels is enabled. */
    virtual bool displayFrequency() const;
    /** Enables/disables the frequency display. */
    virtual void enableDisplayFrequency(bool enable);
    /** Returns @c true if auto key-lock is enabled. */
    virtual bool autoKeyLock() const;
    /** Enables/disables auto key-lock. */
    virtual void enableAutoKeyLock(bool enable);
    /** Returns the auto-shutdown delay in minutes. */
    virtual unsigned autoShutdownDelay() const;
    /** Sets the auto-shutdown delay in minutes. */
    virtual void setAutoShutdownDelay(unsigned min);
    /** Returns the boot display mode. */
    virtual AnytoneBootSettingsExtension::BootDisplay bootDisplay() const;
    /** Sets the boot display mode. */
    virtual void setBootDisplay(AnytoneBootSettingsExtension::BootDisplay mode);
    /** Returns @c true if boot password is enabled. */
    virtual bool bootPassword() const;
    /** Enables/disables boot password. */
    virtual void enableBootPassword(bool enable);
    /** Squelch level of VFO A, (0=off). */
    virtual unsigned squelchLevelA() const;
    /** Returns the squelch level for VFO A, (0=off). */
    virtual void setSquelchLevelA(unsigned level);
    /** Squelch level of VFO B, (0=off). */
    virtual unsigned squelchLevelB() const;
    /** Returns the squelch level for VFO B, (0=off). */
    virtual void setSquelchLevelB(unsigned level);
    /** Returns the power-save mode. */
    virtual AnytoneSettingsExtension::PowerSave powerSave() const;
    /** Sets the power-save mode. */
    virtual void setPowerSave(AnytoneSettingsExtension::PowerSave mode);
    /** Returns the VOX level. */
    virtual unsigned voxLevel() const;
    /** Sets the VOX level. */
    virtual void setVOXLevel(unsigned level);
    /** Returns the VOX delay in ms. */
    virtual unsigned voxDelay() const;
    /** Sets the VOX delay in ms. */
    virtual void setVOXDelay(unsigned ms);
    /** Returns the VFO scan type. */
    virtual AnytoneSettingsExtension::VFOScanType vfoScanType() const;
    /** Sets the VFO scan type. */
    virtual void setVFOScanType(AnytoneSettingsExtension::VFOScanType type);
    /** Returns the mirophone gain. */
    virtual unsigned micGain() const;
    /** Sets the microphone gain. */
    virtual void setMICGain(unsigned gain);

    /** Returns the key function for a short press on the programmable function key 1. */
    virtual AnytoneKeySettingsExtension::KeyFunction progFuncKey1Short() const;
    /** Sets the key function for a short press on the programmable function key 1. */
    virtual void setProgFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a short press on the programmable function key 2. */
    virtual AnytoneKeySettingsExtension::KeyFunction progFuncKey2Short() const;
    /** Sets the key function for a short press on the programmable function key 2. */
    virtual void setProgFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a short press on the programmable function key 3. */
    virtual AnytoneKeySettingsExtension::KeyFunction progFuncKey3Short() const;
    /** Sets the key function for a short press on the programmable function key 3. */
    virtual void setProgFuncKey3Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a short press on the function key 1. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey1Short() const;
    /** Sets the key function for a short press on the function key 1. */
    virtual void setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a short press on the function key 2. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey2Short() const;
    /** Sets the key function for a short press on the function key 2. */
    virtual void setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns @c true if the VFO A is in VFO mode. */
    virtual bool vfoModeA() const;
    /** Enables/disables VFO mode for VFO A. */
    virtual void enableVFOModeA(bool enable);
    /** Returns @c true if the VFO B is in VFO mode. */
    virtual bool vfoModeB() const;
    /** Enables/disables VFO mode for VFO B. */
    virtual void enableVFOModeB(bool enable);
    /** Returns the memory zone for VFO A. */
    virtual unsigned memoryZoneA() const;
    /** Sets the memory zone for VFO A. */
    virtual void setMemoryZoneA(unsigned zone);

    /** Returns the memory zone for VFO B. */
    virtual unsigned memoryZoneB() const;
    /** Sets the memory zone for VFO B. */
    virtual void setMemoryZoneB(unsigned zone);
    /** Returns @c true if recording is enabled. */
    virtual bool recording() const;
    /** Enables/disables recording. */
    virtual void enableRecording(bool enable);
    /** Returns the display brightness. */
    virtual unsigned brightness() const;
    /** Sets the display brightness. */
    virtual void setBrightness(unsigned level);
    /** Returns @c true if the backlight is always on. */
    virtual bool backlightPermanent() const;
    /** Returns the backlight duration in seconds. */
    virtual unsigned backlightDuration() const;
    /** Sets the backlight duration in seconds. */
    virtual void setBacklightDuration(unsigned sec);
    /** Sets the backlight to permanent (always on). */
    virtual void enableBacklightPermanent();
    /** Returns @c true if GPS is enabled. */
    virtual bool gps() const;
    /** Enables/disables recording. */
    virtual void enableGPS(bool enable);
    /** Returns @c true if SMS alert is enabled. */
    virtual bool smsAlert() const;
    /** Enables/disables SMS alert. */
    virtual void enableSMSAlert(bool enable);
    /** Returns @c true if the active channel is VFO B. */
    virtual bool activeChannelB() const;
    /** Enables/disables VFO B as the active channel. */
    virtual void enableActiveChannelB(bool enable);
    /** Returns @c true if sub channel is enabled. */
    virtual bool subChannel() const;
    /** Enables/disables sub channel. */
    virtual void enableSubChannel(bool enable);
    /** Returns @c true if call alert is enabled. */
    virtual bool callAlert() const;
    /** Enables/disables call alert. */
    virtual void enableCallAlert(bool enable);

    /** Returns the GPS time zone. */
    virtual QTimeZone gpsTimeZone() const;
    /** Sets the GPS time zone. */
    virtual void setGPSTimeZone(const QTimeZone &zone);
    /** Returns @c true if the talk permit tone is enabled for digital channels. */
    virtual bool talkPermitDigital() const;
    /** Returns @c true if the talk permit tone is enabled for digital channels. */
    virtual bool talkPermitAnalog() const;
    /** Enables/disables the talk permit tone for digital channels. */
    virtual void enableTalkPermitDigital(bool enable);
    /** Enables/disables the talk permit tone for analog channels. */
    virtual void enableTalkPermitAnalog(bool enable);
    /** Returns @c true if the reset tone is enabled for digital calls. */
    virtual bool digitalResetTone() const;
    /** Enables/disables the reset tone for digital calls. */
    virtual void enableDigitalResetTone(bool enable);
    /** Returns the VOX source. */
    virtual AnytoneAudioSettingsExtension::VoxSource voxSource() const;
    /** Sets the VOX source. */
    virtual void setVOXSource(AnytoneAudioSettingsExtension::VoxSource source);
    /** Returns @c true if the idle channel tone is enabled. */
    virtual bool idleChannelTone() const;
    /** Enables/disables the idle channel tone. */
    virtual void enableIdleChannelTone(bool enable);
    /** Returns the menu exit time in seconds. */
    virtual unsigned menuExitTime() const;
    /** Sets the menu exit time in seconds. */
    virtual void setMenuExitTime(unsigned sec);
    /** Returns @c true if the startup tone is enabled. */
    virtual bool startupTone() const;
    /** Enables/disables the startup tone. */
    virtual void enableStartupTone(bool enable);
    /** Returns @c true if the call-end prompt is enabled. */
    virtual bool callEndPrompt() const;
    /** Enables/disables the call-end prompt. */
    virtual void enableCallEndPrompt(bool enable);
    /** Returns the maximum volume. */
    virtual unsigned maxVolume() const;
    /** Sets the maximum volume. */
    virtual void setMaxVolume(unsigned level);
    /** Returns @c true if get GPS position is enabled. */
    virtual bool getGPSPosition() const;
    /** Enables/disables get GPS position. */
    virtual void enableGetGPSPosition(bool enable);

    /** Returns the key function for a long press on the programmable function key 1. */
    virtual AnytoneKeySettingsExtension::KeyFunction progFuncKey1Long() const;
    /** Sets the key function for a long press on the programmable function key 1. */
    virtual void setProgFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a long press on the programmable function key 2. */
    virtual AnytoneKeySettingsExtension::KeyFunction progFuncKey2Long() const;
    /** Sets the key function for a long press on the programmable function key 2. */
    virtual void setProgFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a long press on the programmable function key 3. */
    virtual AnytoneKeySettingsExtension::KeyFunction progFuncKey3Long() const;
    /** Sets the key function for a long press on the programmable function key 3. */
    virtual void setProgFuncKey3Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a long press on the function key 1. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey1Long() const;
    /** Sets the key function for a long press on the function key 1. */
    virtual void setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the key function for a long press on the function key 2. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKey2Long() const;
    /** Sets the key function for a long press on the function key 2. */
    virtual void setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the long-press duration in ms. */
    virtual unsigned longPressDuration() const;
    /** Sets the long-press duration in ms. */
    virtual void setLongPressDuration(unsigned ms);
    /** Returns @c true if the volume change prompt is enabled. */
    virtual bool volumeChangePrompt() const;
    /** Enables/disables the volume change prompt. */
    virtual void enableVolumeChangePrompt(bool enable);
    /** Returns the auto repeater offset direction for VFO A. */
    virtual AutoRepDir autoRepeaterDirectionA() const;
    /** Sets the auto-repeater offset direction for VFO A. */
    virtual void setAutoRepeaterDirectionA(AutoRepDir dir);
    /** Returns the last-caller display mode. */
    virtual LastCallerDisplayMode lastCallerDisplayMode() const;
    /** Sets the last-caller display mode. */
    virtual void setLastCallerDisplayMode(LastCallerDisplayMode mode);

    /** Returns @c true if the clock is shown. */
    virtual bool displayClock() const;
    /** Enables/disables clock display. */
    virtual void enableDisplayClock(bool enable);
    /** Returns the maximum headphone volume. */
    virtual unsigned maxHeadPhoneVolume() const;
    /** Sets the maximum headphone volume. */
    virtual void setMaxHeadPhoneVolume(unsigned max);
    /** Returns @c true if the audio is "enhanced". */
    virtual bool enhanceAudio() const;
    /** Enables/disables "enhanced" audio. */
    virtual void enableEnhancedAudio(bool enable);
    /** Returns the minimum VFO scan frequency for the UHF band in Hz. */
    virtual unsigned minVFOScanFrequencyUHF() const;
    /** Sets the minimum VFO scan frequency for the UHF band in Hz. */
    virtual void setMinVFOScanFrequencyUHF(unsigned hz);
    /** Returns the maximum VFO scan frequency for the UHF band in Hz. */
    virtual unsigned maxVFOScanFrequencyUHF() const;
    /** Sets the maximum VFO scan frequency for the UHF band in Hz. */
    virtual void setMaxVFOScanFrequencyUHF(unsigned hz);

    /** Returns the minimum VFO scan frequency for the VHF band in Hz. */
    virtual unsigned minVFOScanFrequencyVHF() const;
    /** Sets the minimum VFO scan frequency for the VHF band in Hz. */
    virtual void setMinVFOScanFrequencyVHF(unsigned hz);
    /** Returns the maximum VFO scan frequency for the VHF band in Hz. */
    virtual unsigned maxVFOScanFrequencyVHF() const;
    /** Sets the maximum VFO scan frequency for the VHF band in Hz. */
    virtual void setMaxVFOScanFrequencyVHF(unsigned hz);
    /** Returns @c true if the auto-repeater offset frequency for UHF is set. */
    virtual bool hasAutoRepeaterOffsetFrequencyIndexUHF() const;
    /** Returns the auto-repeater offset frequency index for UHF. */
    virtual unsigned autoRepeaterOffsetFrequencyIndexUHF() const;
    /** Sets the auto-repeater offset frequency index for UHF. */
    virtual void setAutoRepeaterOffsetFrequenyIndexUHF(unsigned idx);
    /** Clears the auto-repeater offset frequency index for UHF. */
    virtual void clearAutoRepeaterOffsetFrequencyIndexUHF();
    /** Returns @c true if the auto-repeater offset frequency for VHF is set. */
    virtual bool hasAutoRepeaterOffsetFrequencyIndexVHF() const;
    /** Returns the auto-repeater offset frequency index for UHF. */
    virtual unsigned autoRepeaterOffsetFrequencyIndexVHF() const;
    /** Sets the auto-repeater offset frequency index for VHF. */
    virtual void setAutoRepeaterOffsetFrequenyIndexVHF(unsigned idx);
    /** Clears the auto-repeater offset frequency index for VHF. */
    virtual void clearAutoRepeaterOffsetFrequencyIndexVHF();

    /** Returns the call-tone melody. */
    virtual void callToneMelody(Melody &melody) const;
    /** Sets the call-tone melody. */
    virtual void setCallToneMelody(const Melody &melody);
    /** Returns the idle-tone melody. */
    virtual void idleToneMelody(Melody &melody) const;
    /** Sets the idle-tone melody. */
    virtual void setIdleToneMelody(const Melody &melody);
    /** Returns the reset-tone melody. */
    virtual void resetToneMelody(Melody &melody) const;
    /** Sets the reset-tone melody. */
    virtual void setResetToneMelody(const Melody &melody);
    /** Returns the recording delay in ms. */
    virtual unsigned recordingDelay() const;
    /** Sets the recording delay in ms. */
    virtual void setRecodringDelay(unsigned ms);
    /** Returns @c true if the call is displayed instead of the name. */
    virtual bool displayCall() const;
    /** Enables/disables call display. */
    virtual void enableDisplayCall(bool enable);

    /** Encodes the general settings. */
    virtual bool fromConfig(const Flags &flags, Context &ctx);
    /** Updates the abstract config from general settings. */
    virtual bool updateConfig(Context &ctx);
  };

  /** Represents the base class for zone channel list for all AnyTone codeplugs.
   * Zone channel lists assign a default channel to each zone for VFO A and B.
   *
   * Memory layout of ecoded zone channel lists (size 0x400 bytes):
   * @verbinclude anytone_zonechannellist.txt */
  class ZoneChannelListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneChannelListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ZoneChannelListElement(uint8_t *ptr);

    /** Resets the zone channel list. */
    void clear();

    /** Returns @c true if the channel index for VFO A is set for the n-th zone. */
    virtual bool hasChannelA(unsigned n) const;
    /** Returns the channel index (0-based) for VFO A for the n-th zone. */
    virtual unsigned channelIndexA(unsigned n) const;
    /** Sets the channel index (0-based) for VFO A for the n-th zone. */
    virtual void setChannelIndexA(unsigned n, unsigned idx);
    /** Clears the channel index (0-based) for VFO A for the n-th zone. */
    virtual void clearChannelIndexA(unsigned n);

    /** Returns @c true if the channel index for VFO B is set for the n-th zone. */
    virtual bool hasChannelB(unsigned n) const;
    /** Returns the channel index (0-based) for VFO B for the n-th zone. */
    virtual unsigned channelIndexB(unsigned n) const;
    /** Sets the channel index (0-based) for VFO B for the n-th zone. */
    virtual void setChannelIndexB(unsigned n, unsigned idx);
    /** Clears the channel index (0-based) for VFO B for the n-th zone. */
    virtual void clearChannelIndexB(unsigned n);
  };

  /** Represents the base class of the boot settings for all AnyTone codeplug.
   *
   * Memory layout of encoded boot settings (size 0x0030):
   * @verbinclude anytone_bootsettings.txt */
  class BootSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    BootSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    BootSettingsElement(uint8_t *ptr);

    /** Resets the boot settings. */
    void clear();

    /** Returns the first intro line. */
    virtual QString introLine1() const;
    /** Sets the first intro line. */
    virtual void setIntroLine1(const QString &txt);
    /** Returns the second intro line. */
    virtual QString introLine2() const;
    /** Sets the second intro line. */
    virtual void setIntroLine2(const QString &txt);

    /** Returns the password. */
    virtual QString password() const;
    /** Sets the password. */
    virtual void setPassword(const QString &txt);

    /** Updates the general settings from the given abstract configuration. */
    virtual bool fromConfig(const Flags &flags, Context &ctx);
    /** Updates the abstract configuration from this general settings. */
    virtual bool updateConfig(Context &ctx);
  };

  /** Represents the base class of DMR APRS settings for all AnyTone codeplugs.
   *
   * Memory encoding of the DMR APRS settings (size 0x0030 bytes):
   * @verbinclude anytone_dmraprssettings.txt */
  class DMRAPRSSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DMRAPRSSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit DMRAPRSSettingsElement(uint8_t *ptr);

    /** Resets the APRS settings. */
    void clear();

    /** Returns the Manual TX interval in seconds. */
    virtual unsigned manualInterval() const;
    /** Sets the manual TX interval in seconds. */
    virtual void setManualInterval(unsigned sec);

    /** Returns @c true if the automatic APRS is enabled. */
    virtual bool automatic() const;
    /** Returns the automatic transmit interval in seconds. */
    virtual unsigned automaticInterval() const;
    /** Sets the automatic transmit interval in seconds. */
    virtual void setAutomaticInterval(unsigned sec);
    /** Disables the automatic APRS. To enable it, set an interval. */
    virtual void disableAutomatic();

    /** Returns @c true if the fixed location beacon is enabled. */
    virtual bool fixedLocation() const;
    /** Returns the location of the fixed position. */
    virtual QGeoCoordinate location() const;
    /** Sets the location of the fixed position. */
    virtual void setLocation(const QGeoCoordinate &pos);
    /** Enables/disables fixed location beacon. */
    virtual void enableFixedLocation(bool enable);

    /** Returns the transmit power. */
    virtual Channel::Power power() const;
    /** Sets the transmit power. */
    virtual void setPower(Channel::Power power);

    /** Returns @c true if the n-th channel is set. */
    virtual bool hasChannel(unsigned n) const;
    /** Returns @c true if the n-th channel is VFO A. */
    virtual bool channelIsVFOA(unsigned n) const;
    /** Returns @c true if the n-th channel is VFO B. */
    virtual bool channelIsVFOB(unsigned n) const;
    /** Returns @c true if the n-th channel is selected channel. */
    virtual bool channelIsSelected(unsigned n) const;
    /** Returns the index of the n-th channel. */
    virtual unsigned channelIndex(unsigned n) const;
    /** Sets the n-th channel index. */
    virtual void setChannelIndex(unsigned n, unsigned idx);
    /** Sets the n-th channel to VFO A. */
    virtual void setChannelVFOA(unsigned n);
    /** Sets the n-th channel to VFO B. */
    virtual void setChannelVFOB(unsigned n);
    /** Sets the n-th channel to selected channel. */
    virtual void setChannelSelected(unsigned n);
    /** Resets the n-th channel. */
    virtual void clearChannel(unsigned n);

    /** Returns the destination DMR ID to send the APRS information to. */
    virtual unsigned destination() const;
    /** Sets the destination DMR ID to send the APRS information to. */
    virtual void setDestination(unsigned id);

    /** Returns the call type. */
    virtual DMRContact::Type callType() const;
    /** Sets the call type. */
    virtual void setCallType(DMRContact::Type type);

    /** Returns @c true if the timeslot of the channel is overridden. */
    virtual bool timeSlotOverride() const;
    /** Returns the timeslot (only valid if @c timeSlotOverride returns @c true). */
    virtual DMRChannel::TimeSlot timeslot() const;
    /** Sets the timeslot. */
    virtual void overrideTimeSlot(DMRChannel::TimeSlot ts);
    /** Disables TS override. */
    virtual void disableTimeSlotOverride();

    /** Updates the GPS settings from the given config. */
    virtual bool fromConfig(const Flags &flags, Context &ctx);
    /** Creates GPS system from this GPS settings. */
    virtual bool createGPSSystem(uint8_t i, Context &ctx);
    /** Links GPS system from this GPS settings. */
    virtual bool linkGPSSystem(uint8_t i, Context &ctx);
  };

  /** Represents the base class of prefabricated message linked list for all AnyTone codeplugs.
   * This element is some weird linked list that determines some order for the prefabricated
   * SMS messages.
   *
   * Memory encoding of the message list (size 0x0010 bytes):
   * @verbinclude anytone_messagelist.txt */
  class MessageListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageListElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit MessageListElement(uint8_t *ptr);

    /** Clears the message list item. */
    void clear();

    /** Returns @c true if there is a next message (EOL otherwise). */
    virtual bool hasNext() const;
    /** Returns the index of the next message in list. */
    virtual unsigned next() const;
    /** Sets the index of the next message in list. */
    virtual void setNext(unsigned idx);
    /** Clears the next message index. */
    virtual void clearNext();

    /** Returns @c true if there is a message index. */
    virtual bool hasIndex() const;
    /** Returns the index of the message. */
    virtual unsigned index() const;
    /** Sets the index of the message. */
    virtual void setIndex(unsigned idx);
    /** Clears the index of the message. */
    virtual void clearIndex();
  };

  /** Represents base class of a preset message for all AnyTone codeplugs.
   *
   * Memory encoding of the message (0x100 bytes):
   * @verbinclude anytone_message.txt */
  class MessageElement: public Element
  {
  protected:
    /** Hidden constructor. */
    MessageElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    MessageElement(uint8_t *ptr);

    /** Resets the message. */
    void clear();

    /** Returns the message text. */
    virtual QString message() const;
    /** Sets the message text. */
    virtual void setMessage(const QString &msg);
  };

  /** Represents base class of a analog quick call entry for all AnyTone codeplugs.
   *
   * Memory encoding of the message (0x0002 bytes):
   * @verbinclude anytone_analogquickcall.txt */
  class AnalogQuickCallElement: public Element
  {
  public:
    /** Analog quick-call types. */
    enum class Type {
      None     = 0,                ///< None, quick-call disabled.
      DTMF     = 1,                ///< DTMF call.
      TwoTone  = 2,                ///< 2-tone call.
      FiveTone = 3                 ///< 5-tone call
    };

  protected:
    /** Hidden constructor. */
    AnalogQuickCallElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit AnalogQuickCallElement(uint8_t *ptr);

    /** Resets the quick call entry. */
    void clear();

    /** Returns the call type. */
    virtual Type type() const;
    /** Sets the type of the quick call. */
    virtual void setType(Type type);

    /** Returns @c true if an analog contact index is set. */
    virtual bool hasContactIndex() const;
    /** Returns the analog contact index. */
    virtual unsigned contactIndex() const;
    /** Sets the analog contact index. */
    virtual void setContactIndex(unsigned idx);
    /** Clears the contact index. */
    virtual void clearContactIndex();
  };

  /** Represents the base class of a hot-key setting entry for all AnyTone codeplugs.
   *
   * Memory encoding of a hot-key setting (size 0x0030 bytes):
   * @verbinclude anytone_hotkey.txt */
  class HotKeyElement: public Element
  {
  public:
    /** Hot-key types. */
    enum class Type {
      Call = 0,             ///< Perform a call.
      Menu = 1              ///< Show a menu item.
    };

    /** Possible menu items. */
    enum class MenuItem {
      SMS = 1,          ///< Show SMS menu.
      NewSMS = 2,       ///< Create new SMS.
      HotText = 3,      ///< Send a hot-text.
      Inbox = 4,        ///< Show SMS inbox.
      Outbox = 5,       ///< Show SMS outbox.
      Contacts = 6,     ///< Show contact list.
      ManualDial = 7,   ///< Show manual dial.
      CallLog = 8       ///< Show call log.
    };

    /** Possible call types. */
    enum class CallType {
      Analog = 0,       ///< Perform an analog call.
      Digital = 1       ///< Perform a digital call.
    };

    /** Possible digital call sub-types. */
    enum class DigiCallType {
      Off = 0xff,          ///< Call disabled.
      GroupCall = 0,       ///< Perform a group call.
      PrivateCall= 1,      ///< Perform private call.
      AllCall = 2,         ///< Perform all call.
      HotText = 3,         ///< Send a text message.
      CallTip = 4,         ///< Send a call tip (?).
      StatusMessage = 5    ///< Send a state message.
    };

  protected:
    /** Hidden constructor. */
    HotKeyElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit HotKeyElement(uint8_t *ptr);

    /** Resets the hot-key entry. */
    void clear();

    /** Returns the type of the hot-key entry. */
    virtual Type type() const;
    /** Sets the type of the hot-key entry. */
    virtual void setType(Type type);

    /** If @c type returns @c Type::Menu, returns the menu item. */
    virtual MenuItem menuItem() const;
    /** Sets the menu item. For this setting to have an effect, the @c type must be set to
     * @c Type::Menu. */
    virtual void setMenuItem(MenuItem item);

    /** If @c type returns @c Type::Call, returns the type of the call. */
    virtual CallType callType() const;
    /** Sets the call type. For this settings to have an effect, the type must be set to
     * @c Type::Call. */
    virtual void setCallType(CallType type);

    /** If @c type returns @c Type::Call and @c callType @c CalLType::Digital, returns the digital
     * call type. */
    virtual DigiCallType digiCallType() const;
    /** Sets the digital call type. For this setting to have an effect, the @c type must be
     * @c Type::Call and @c callType must be @c CallType::Digital. */
    virtual void setDigiCallType(DigiCallType type);

    /** Returns @c true if the contact index is set. */
    virtual bool hasContactIndex() const;
    /** If @c type is @c Type::Call, returns the contact index. This is either an index of an
     * analog quick call if @c callType is CallType::Analog or a contact index if @c callType is
     * @c CallType::Digital. If set to 0xffffffff the index is invalid. */
    virtual unsigned contactIndex() const;
    /** Sets the contact index. This can either be an index of an analog quick-call or a contact
     * index. */
    virtual void setContactIndex(unsigned idx);
    /** Clears the contact index. */
    virtual void clearContactIndex();

    /** Returns @c true if a message index is set. */
    virtual bool hasMessageIndex() const;
    /** Returns the message index. This can either be an index of an SMS or an index of a status
     * message. */
    virtual unsigned messageIndex() const;
    /** Sets the message index. */
    virtual void setMessageIndex(unsigned idx);
    /** Clears the message index. */
    virtual void clearMessageIndex();
  };

  /** Represents the base class of alarm setting entry for all AnyTone codeplugs.
   *
   * Memory encoding of an alarm setting (size 0x0020 bytes):
   * @verbinclude anytone_alarmsetting.txt */
  class AlarmSettingElement: public Element
  {
  public:
    /** Represents the base class of an analog alarm setting for all AnyTone codeplugs.
     *
     * Memory representation of an analog alarm setting (size 0x000a bytes):
     * @verbinclude anytone_analogalarm.txt */
    class AnalogAlarm: public Element
    {
    public:
      /** Possible analog alarm types. */
      enum class Action {
        None = 0,            ///< No alarm at all.
        Background = 1,      ///< Transmit and background.
        TXAlarm = 2,         ///< Transmit and alarm
        Both = 3,            ///< Both?
      };

      /** Possible alarm signalling types. */
      enum class ENIType {
        None = 0,            ///< No alarm code signalling.
        DTMF = 1,            ///< Send alarm code as DTMF.
        FiveTone = 2         ///< Send alarm code as 5-tone.
      };

    protected:
      /** Hidden constructor. */
      AnalogAlarm(uint8_t *ptr, unsigned size);

    public:
      /** Constructor. */
      AnalogAlarm(uint8_t *ptr);

      /** Resets the alarm. */
      void clear();

      /** Returns the alarm action. */
      virtual Action action() const;
      /** Sets the alarm action. */
      virtual void setAction(Action action);

      /** Returns the encoding type. */
      virtual ENIType encodingType() const;
      /** Sets the encoding type. */
      virtual void setEncodingType(ENIType type);

      /** Returns the emergency ID index. */
      virtual unsigned emergencyIndex() const;
      /** Sets the emergency ID index. */
      virtual void setEmergencyIndex(unsigned idx);

      /** Returns the alarm duration in seconds. */
      virtual unsigned duration() const;
      /** Sets the alarm duration in seconds. */
      virtual void setDuration(unsigned sec);
      /** Returns the TX duration in seconds. */
      virtual unsigned txDuration() const;
      /** Sets the TX duration in seconds. */
      virtual void setTXDuration(unsigned sec);
      /** Returns the RX duration in seconds. */
      virtual unsigned rxDuration() const;
      /** Sets the RX duration in seconds. */
      virtual void setRXDuration(unsigned sec);

      /** Returns @c true if the alarm channel is the selected channel. */
      virtual bool channelIsSelected() const;
      /** Returns the channel index. */
      virtual unsigned channelIndex() const;
      /** Sets the channel index. */
      virtual void setChannelIndex(unsigned idx);
      /** Sets the alarm channel to the selected channel. */
      virtual void setChannelSelected();

      /** Returns @c true if the alarm is repeated continuously. */
      virtual bool repeatContinuously() const;
      /** Returns the number of alarm repetitions. */
      virtual unsigned repetitions() const;
      /** Sets the number of alarm repetitions. */
      virtual void setRepetitions(unsigned num);
      /** Sets the alarm to be repeated continuously. */
      virtual void setRepatContinuously();
    };

    /** Represents the base class of an digital alarm setting for all AnyTone codeplugs.
     *
     * Memory representation of a digital alarm setting (size 0x000c bytes):
     * @verbinclude anytone_digitalalarm.txt */
    class DigitalAlarm: public Element
    {
    public:
      /** Possible alarm types. */
      enum class Action {
        None = 0,           ///< No alarm at all.
        Background = 1,     ///< Transmit and background.
        NonLocal = 2,       ///< Transmit and non-local alarm.
        Local = 3,          ///< Transmit and local alarm.
      };

    protected:
      /** Hidden constructor. */
      DigitalAlarm(uint8_t *ptr, unsigned size);

    public:
      /** Constructor. */
      explicit DigitalAlarm(uint8_t *ptr);

      /** Resets the digital alarm settings. */
      void clear();

      /** Returns the alarm action. */
      virtual Action action() const;
      /** Sets the alarm action. */
      virtual void setAction(Action action);

      /** Returns the alarm duration in seconds. */
      virtual unsigned duration() const;
      /** Sets the alarm duration in seconds. */
      virtual void setDuration(unsigned sec);
      /** Returns the TX duration in seconds. */
      virtual unsigned txDuration() const;
      /** Sets the TX duration in seconds. */
      virtual void setTXDuration(unsigned sec);
      /** Returns the RX duration in seconds. */
      virtual unsigned rxDuration() const;
      /** Sets the RX duration in seconds. */
      virtual void setRXDuration(unsigned sec);

      /** Returns @c true if the alarm channel is the selected channel. */
      virtual bool channelIsSelected() const;
      /** Returns the channel index. */
      virtual unsigned channelIndex() const;
      /** Sets the channel index. */
      virtual void setChannelIndex(unsigned idx);
      /** Sets the alarm channel to the selected channel. */
      virtual void setChannelSelected();

      /** Returns @c true if the alarm is repeated continuously. */
      virtual bool repeatContinuously() const;
      /** Returns the number of alarm repetitions. */
      virtual unsigned repetitions() const;
      /** Sets the number of alarm repetitions. */
      virtual void setRepetitions(unsigned num);
      /** Sets the alarm to be repeated continuously. */
      virtual void setRepatContinuously();

      /** Returns voice broadcast duration in minutes. */
      virtual unsigned voiceBroadcastDuration() const;
      /** Sets voice broadcast duration in minutes. */
      virtual void setVoiceBroadcastDuration(unsigned min);
      /** Returns area broadcast duration in minutes. */
      virtual unsigned areaBroadcastDuration() const;
      /** Sets area broadcast duration in minutes. */
      virtual void setAreaBroadcastDuration(unsigned min);

      /** Returns @c true if the VOX gets enabled. */
      virtual bool vox() const;
      /** Enables/disables the VOX for alarms. */
      virtual void enableVOX(bool enable);
      /** Returns @c true if alarms gets received enabled. */
      virtual bool rxAlarm() const;
      /** Enables/disables the reception of alarms. */
      virtual void enableRXAlarm(bool enable);
    };

  protected:
    /** Hidden constructor. */
    AlarmSettingElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    AlarmSettingElement(uint8_t *ptr);

    /** Clears the alarm settings. */
    void clear();

    /** Returns a pointer to the analog alarm settings. */
    virtual uint8_t *analog() const;
    /** Returns a pointer to the digital alarm settings. */
    virtual uint8_t *digital() const;
  };

  /** Represents the base class of digital alarm setting extension for all AnyTone codeplugs.
   *
   * Memory encoding of a digital alarm setting extension (size 0x0030 bytes):
   * @verbinclude anytone_digitalalarmextension.txt */
  class DigitalAlarmExtensionElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DigitalAlarmExtensionElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    DigitalAlarmExtensionElement(uint8_t *ptr);

    /** Clears the settings. */
    void clear();

    /** Returns the call type. */
    virtual DMRContact::Type callType() const;
    /** Sets the call type. */
    virtual void setCallType(DMRContact::Type type);

    /** Returns the destination DMR number. */
    virtual unsigned destination() const;
    /** Sets the destination DMR number. */
    virtual void setDestination(unsigned number);
  };

  /** Represents the base-class for 5Tone IDs for all AnyTone codeplugs.
   *
   * Memory encoding of the ID (size 0x0020 bytes):
   * @verbinclude anytone_5toneid.txt */
  class FiveToneIDElement: public Element
  {
  public:
    /** Possible 5-tone encoding standards. */
    enum class Standard {
      ZVEI1 = 0, ZVEI2, ZVEI3, PZVEI, DZVEI, PDZVEI, CCIR1, CCIR2, PCCIR, EEA, EuroSignal, NATEL,
      MODAT, CCITT, EIA
    };

  protected:
    /** Hidden constructor. */
    FiveToneIDElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    FiveToneIDElement(uint8_t *ptr);

    /** Clears the ID. */
    void clear();

    /** Returns the 5Tone encoding standard. */
    virtual Standard standard() const;
    /** Sets the encoding standard. */
    virtual void setStandard(Standard std);

    /** Returns the tone duration in ms. */
    virtual unsigned toneDuration() const;
    /** Sets the tone duration in ms. */
    virtual void setToneDuration(unsigned ms);

    /** Returns the ID. */
    virtual QString id() const;
    /** Sets the ID. */
    virtual void setID(const QString &id);

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);
  };

  /** Represents the base-class for 5Tone function for all AnyTone codeplugs.
   *
   * Memory encoding of the function (size 0x0020 bytes):
   * @verbinclude anytone_5tonefunction.txt */
  class FiveToneFunctionElement: public Element
  {
  public:
    /** Possible function being performed on 5-tone decoding. */
    enum class Function {
      OpenSquelch=0, CallAll, EmergencyAlarm, RemoteKill, RemoteStun, RemoteWakeup,
      GroupCall
    };

    /** Possible responses to 5-tone decoding. */
    enum class Response {
      None=0, Tone, ToneRespond
    };

  protected:
    /** Hidden constructor. */
    FiveToneFunctionElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit FiveToneFunctionElement(uint8_t *ptr);

    /** Clears the function settings. */
    void clear();

    /** Returns the function. */
    virtual Function function() const;
    /** Sets the function. */
    virtual void setFunction(Function function);
    /** Returns the response. */
    virtual Response response() const;
    /** Sets the response. */
    virtual void setResponse(Response response);

    /** Returns the ID. */
    virtual QString id() const;
    /** Sets the ID. */
    virtual void setID(const QString &id);

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);
  };

  /** Represents the base-class for 5Tone settings for all AnyTone codeplugs.
   *
   * Memory encoding of the settings (size 0x0080 bytes):
   * @verbinclude anytone_5tonesettings.txt */
  class FiveToneSettingsElement: public Element
  {
  public:
    /** Possible responses to decoded 5-tone codes. */
    enum class Response {
      None = 0, Tone, ToneRespond
    };
    /** Possible 5-tone encoding standards. */
    typedef enum FiveToneIDElement::Standard Standard;

  protected:
    /** Hidden constructor. */
    FiveToneSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    FiveToneSettingsElement(uint8_t *ptr);

    /** Resets the 5tone settings. */
    void clear();

    /** Returns the decoding response. */
    virtual Response decodingResponse() const;
    /** Sets the decoding response. */
    virtual void setDecodingResponse(Response response);

    /** Returns the decoding standard. */
    virtual Standard decodingStandard() const;
    /** Sets the decoding standard. */
    virtual void setDecodingStandard(Standard standard);

    /** Returns the decoding tone duration in ms. */
    virtual unsigned decodingToneDuration() const;
    /** Sets the decoding tone duration in ms. */
    virtual void setDecodingToneDuration(unsigned ms);

    /** Returns the 5tone radio ID. */
    virtual QString id() const;
    /** Sets the 5tone radio ID. */
    virtual void setID(const QString &id);

    /** Returns the post-encode delay in ms. */
    virtual unsigned postEncodeDelay() const;
    /** Sets the post-encode delay in ms. */
    virtual void setPostEncodeDelay(unsigned ms);

    /** Returns @c true if the PTT ID is set. */
    virtual bool hasPTTID() const;
    /** Returns the PTT ID. */
    virtual unsigned pttID() const;
    /** Sets the PTT ID [5,75]. */
    virtual void setPTTID(unsigned id);
    /** Clears the PTT ID. */
    virtual void clearPTTID();

    /** Returns the auto-reset time in seconds. */
    virtual unsigned autoResetTime() const;
    /** Sets the auto-reset time in seconds. */
    virtual void setAutoResetTime(unsigned s);

    /** Returns the first delay in ms. */
    virtual unsigned firstDelay() const;
    /** Sets the first delay in ms. */
    virtual void setFirstDelay(unsigned ms);

    /** Returns @c true if the sidetone is enabled. */
    virtual bool sidetoneEnabled() const;
    /** Enables/disables side tone. */
    virtual void enableSidetone(bool enable);
    /** Returns the stop code [0,15]. */
    virtual unsigned stopCode() const;
    /** Sets the stop code. */
    virtual void setStopCode(unsigned code);
    /** Returns the stop time in ms. */
    virtual unsigned stopTime() const;
    /** Sets the stop time in ms. */
    virtual void setStopTime(unsigned ms);
    /** Returns the decode time in ms. */
    virtual unsigned decodeTime() const;
    /** Sets the decode time in ms. */
    virtual void setDecodeTime(unsigned ms);
    /** Returns the delay after stop in ms. */
    virtual unsigned delayAfterStop() const;
    /** Sets the delay after stop in ms. */
    virtual void setDelayAfterStop(unsigned ms);
    /** Returns the pre time in ms. */
    virtual unsigned preTime() const;
    /** Sets the pre time in ms. */
    virtual void setPreTime(unsigned ms);

    /** Returns the BOT standard. */
    virtual Standard botStandard() const;
    /** Sets the BOT standard. */
    virtual void setBOTStandard(Standard standard);
    /** Returns the BOT tone duration in ms. */
    virtual unsigned botToneDuration() const;
    /** Sets the BOT tone duration in ms. */
    virtual void setBOTToneDuration(unsigned ms);
    /** Returns the 5tone BOT ID. */
    virtual QString botID() const;
    /** Sets the 5tone BOT ID. */
    virtual void setBOTID(const QString &id);

    /** Returns the EOT standard. */
    virtual Standard eotStandard() const;
    /** Sets the EOT standard. */
    virtual void setEOTStandard(Standard standard);
    /** Returns the EOT tone duration in ms. */
    virtual unsigned eotToneDuration() const;
    /** Sets the EOT tone duration in ms. */
    virtual void setEOTToneDuration(unsigned ms);
    /** Returns the 5tone EOT ID. */
    virtual QString eotID() const;
    /** Sets the 5tone EOT ID. */
    virtual void setEOTID(const QString &id);
  };

  /** Represents the base-class for a 2-tone ID for all AnyTone codeplugs.
   *
   * Memory encoding of the ID (size 0x0020 bytes):
   * @verbinclude anytone_2toneid.txt */
  class TwoToneIDElement: public Element
  {
  protected:
    /** Hidden constructor. */
    TwoToneIDElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    TwoToneIDElement(uint8_t *ptr);

    /** Resets the ID. */
    void clear();

    /** Returns the first tone of the sequence. */
    virtual double firstTone() const;
    /** Sets the first tone of the sequence. */
    virtual void setFirstTone(double f);

    /** Returns the second tone of the sequence. */
    virtual double secondTone() const;
    /** Sets the second tone of the sequence. */
    virtual void setSecondTone(double f);

    /** Returns the name of the function. */
    virtual QString name() const;
    /** Sets the name of the function. */
    virtual void setName(const QString &name);
  };

  /** Represents the base-class for a 2-tone function for all AnyTone codeplugs.
   *
   * Memory encoding of the function (size 0x0020 bytes):
   * @verbinclude anytone_2tonefunction.txt */
  class TwoToneFunctionElement: public Element
  {
  public:
    /** Possible responses to a decode. */
    enum class Response {
      None = 0, Tone, ToneRespond
    };

  protected:
    /** Hidden constructor. */
    TwoToneFunctionElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    TwoToneFunctionElement(uint8_t *ptr);

    /** Resets the function. */
    void clear();

    /** Returns the first tone of the sequence. */
    virtual double firstTone() const;
    /** Sets the first tone of the sequence. */
    virtual void setFirstTone(double f);

    /** Returns the second tone of the sequence. */
    virtual double secondTone() const;
    /** Sets the second tone of the sequence. */
    virtual void setSecondTone(double f);

    /** Returns the response. */
    virtual Response response() const;
    /** Sets the response. */
    virtual void setResponse(Response resp);

    /** Returns the name of the function. */
    virtual QString name() const;
    /** Sets the name of the function. */
    virtual void setName(const QString &name);
  };

  /** Represents the base class of 2-tone settings for all AnyTone codeplugs.
   *
   * Memory encoding of the settings (size 0x0010 bytes):
   * @verbinclude anytone_2tonesettings.txt */
  class TwoToneSettingsElement : public Element
  {
  protected:
    /** Hidden constructor. */
    TwoToneSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    TwoToneSettingsElement(uint8_t *ptr);

    /** Resets the settings. */
    void clear();

    /** Returns the first tone duration in ms. */
    virtual unsigned firstToneDuration() const;
    /** Sets the first tone duration in ms. */
    virtual void setFirstToneDuration(unsigned ms);

    /** Returns the second tone duration in ms. */
    virtual unsigned secondToneDuration() const;
    /** Sets the second tone duration in ms. */
    virtual void setSecondToneDuration(unsigned ms);

    /** Returns the long tone duration in ms. */
    virtual unsigned longToneDuration() const;
    /** Sets the long tone duration in ms. */
    virtual void setLongToneDuration(unsigned ms);

    /** Returns the gap duration in ms. */
    virtual unsigned gapDuration() const;
    /** Sets the gap duration in ms. */
    virtual void setGapDuration(unsigned ms);

    /** Returns the auto-reset time in seconds. */
    virtual unsigned autoResetTime() const;
    /** Sets the auto-reset time in seconds. */
    virtual void setAutoResetTime(unsigned sec);

    /** Returns @c true if the sidetone is enabled. */
    virtual bool sidetone() const;
    /** Enables/disables the sidetone. */
    virtual void enableSidetone(bool enable);
  };

  /** Represents the base class of DTMF settings for all AnyTone codeplugs.
   *
   * Memory representation of the settings (size 0x0050):
   * @verbinclude anytone_dtmfsettings.txt */
  class DTMFSettingsElement: public Element
  {
  public:
    /** Possible responses to a DTMF decode. */
    enum Response {
      None=0, Tone, ToneRespond
    };

  protected:
    /** Hidden constructor. */
    DTMFSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit DTMFSettingsElement(uint8_t *ptr);

    /** Resets the settings. */
    void clear();

    /** Returns the interval/repeat symbol [0,15]. */
    virtual unsigned intervalSymbol() const;
    /** Sets the interval/repeat symbol [0,15]. */
    virtual void setIntervalSymbol(unsigned symb);

    /** Returns the group code [0,15]. */
    virtual unsigned groupCode() const;
    /** Sets the group code [0,15]. */
    virtual void setGroupCode(unsigned symb);

    /** Returns the response to a DMTF decode. */
    virtual Response response() const;
    /** Sets the response to a DTMF decode. */
    virtual void setResponse(Response resp);

    /** Returns the pre time in ms. */
    virtual unsigned preTime() const;
    /** Sets the pre time in ms. */
    virtual void setPreTime(unsigned ms);

    /** Returns the first digit duration in ms. */
    virtual unsigned firstDigitDuration() const;
    /** Sets the first digit duration in ms. */
    virtual void setFirstDigitDuration(unsigned ms);

    /** Returns the auto reset time in seconds. */
    virtual unsigned autoResetTime() const;
    /** Sets the auto reset time in seconds. */
    virtual void setAutoResetTime(unsigned sec);

    /** Returns the radio ID. */
    virtual QString id() const;
    /** Sets the radio ID. */
    virtual void setID(const QString &id);

    /** Returns the post encoding delay in ms. */
    virtual unsigned postEncodingDelay() const;
    /** Sets the post encoding delay in ms. */
    virtual void setPostEncodingDelay(unsigned ms);

    /** Returns the PTT ID pause in seconds. */
    virtual unsigned pttIDPause() const;
    /** Sets the PTT ID pause in seconds. */
    virtual void setPTTIDPause(unsigned sec);

    /** Returns @c true if the PTT ID is enabled. */
    virtual bool pttIDEnabled() const;
    /** Enables/disables the PTT ID. */
    virtual void enablePTTID(bool enable);

    /** Returns the D-code pause in seconds. */
    virtual unsigned dCodePause() const;
    /** Sets the D-code pause in seconds. */
    virtual void setDCodePause(unsigned sec);

    /** Returns @c true if the sidetone is enabled. */
    virtual bool sidetone() const;
    /** Enables/disables the sidetone. */
    virtual void enableSidetone(bool enable);

    /** Returns the BOT ID. */
    virtual QString botID() const;
    /** Sets the BOT ID. */
    virtual void setBOTID(const QString &id);

    /** Returns the EOT ID. */
    virtual QString eotID() const;
    /** Sets the EOT ID. */
    virtual void setEOTID(const QString &id);

    /** Returns the remote kill ID. */
    virtual QString remoteKillID() const;
    /** Sets the remote kill ID. */
    virtual void setRemoteKillID(const QString &id);

    /** Returns the remote stun ID. */
    virtual QString remoteStunID() const;
    /** Sets the remote stun ID. */
    virtual void setRemoteStunID(const QString &id);
  };

  /** Represents the base class for entries to the contact indices in all AnyTone codeplugs.
   *
   * Memory representation of the entry (size 0x0008):
   * @verbinclude anytone_contactmapentry.txt */
  class ContactMapElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactMapElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    ContactMapElement(uint8_t *ptr);

    /** Clears the entry. */
    void clear();
    /** Returns @c true if the contact map is valid. */
    bool isValid() const;

    /** Returns @c true if the entry is a group call. */
    virtual bool isGroup() const;
    /** Returns the id. */
    virtual unsigned id() const;
    /** Encodes ID and group call flag. */
    virtual void setID(unsigned id, bool group=false);
    /** Returns the index. */
    virtual unsigned index() const;
    /** Sets the index. */
    virtual void setIndex(unsigned idx);

  public:
    /** Returns the size of this entry. */
    static unsigned size();
  };

protected:
  /** Hidden constructor. */
  AnytoneCodeplug(const QString &label, QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~AnytoneCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  bool encode(Config *config, const Flags &flags, const ErrorStack &err);
  bool decode(Config *config, const ErrorStack &err);

protected:
  virtual bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  /** Allocates the bitmaps. This is also performed during a clear. */
  virtual bool allocateBitmaps() = 0;
  /** Sets all bitmaps for the given config. */
  virtual void setBitmaps(Config *config) = 0;

  /** Allocate all code-plug elements that must be written back to the device to maintain a working
   * codeplug. These elements might be updated during encoding. */
  virtual void allocateUpdated() = 0;
  /** Allocate all code-plug elements that must be downloaded for decoding. All code-plug elements
   * within the radio that are not represented within the common Config are omitted. */
  virtual void allocateForDecoding() = 0;
  /** Allocate all code-plug elements that are defined through the common Config. */
  virtual void allocateForEncoding() = 0;

  /** Encodes the given config (via context) to the binary codeplug. */
  virtual bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Decodes the downloaded codeplug. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

protected:
  /** Holds the image label. */
  QString _label;

  // Allow access to protected allocation methods.
  friend class AnytoneRadio;
};

#endif // ANYTONECODEPLUG_HH
