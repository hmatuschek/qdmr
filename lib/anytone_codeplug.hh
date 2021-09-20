#ifndef ANYTONECODEPLUG_HH
#define ANYTONECODEPLUG_HH

#include "codeplug.hh"

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
   * Memmory layout of encoded channel (0x40 bytes):
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

    /** Defines all possible admit criteria. */
    enum class Admit {
      Always = 0,                 ///< Admit TX always.
      Colorcode = 1,              ///< Admit TX on matching color-code.
      Free = 2,                   ///< Admit TX on channel free.
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
    ChannelElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** Resets the channel. */
    void clear();

    /** Returns the RX frequency in Hz. */
    virtual uint rxFrequency() const;
    /** Sets the RX frequency in Hz. */
    virtual void setRXFrequency(uint hz);

    /** Returns the TX frequency offset in Hz.
     * This method returns an unsigned value, the sign of the offset frequency is stored in
     * @c repeaterMode(). */
    virtual uint txOffset() const;
    /** Sets the TX frequency offset in Hz.
     * This method accepts unsigned values, the sign of the offset frequency is stored in
     * @c repeaterMode(). */
    virtual void setTXOffset(uint hz);
    /** Returns the TX frequency in Hz. */
    virtual uint txFrequency() const;
    /** Sets the TX frequency indirectly. That is, relative to the RX frequency which must be set
     * first. This method also updates the @c repeaterMode. */
    virtual void setTXFrequency(uint hz);

    /** Returns the channel mode (analog, digtital, etc). */
    virtual Mode mode() const;
    /** Sets the channel mode. */
    virtual void setMode(Mode mode);

    /** Returns the channel power. */
    virtual Channel::Power power() const;
    /** Sets the channel power. */
    virtual void setPower(Channel::Power power);

    /** Returns the band width of the channel. */
    virtual AnalogChannel::Bandwidth bandwidth() const;
    /** Sets the band width of the channel. */
    virtual void setBandwidth(AnalogChannel::Bandwidth bw);

    /** Retuns the transmit offset direction. */
    virtual RepeaterMode repeaterMode() const;
    /** Sets the transmit offset direction. */
    virtual void setRepeaterMode(RepeaterMode mode);

    /** Returns the RX signaling mode */
    virtual SignalingMode rxSignalingMode() const;
    /** Sets the RX signaling mode */
    virtual void setRXSignalingMode(SignalingMode mode);
    /** Returns the TX signaling mode */
    virtual SignalingMode txSignalingMode() const;
    /** Sets the TX signaling mode */
    virtual void setTXSignalingMode(SignalingMode mode);

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

    /** Returns the TX CTCSS tone. */
    virtual Signaling::Code txCTCSS() const;
    /** Sets the TX CTCSS tone. */
    virtual void setTXCTCSS(Signaling::Code tone);
    /** Returns the RX CTCSS tone. */
    virtual Signaling::Code rxCTCSS() const;
    /** Sets the RX CTCSS tone. */
    virtual void setRXCTCSS(Signaling::Code tone);
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
    virtual uint twoToneDecodeIndex() const;
    /** Sets the 2-tone decode index (0-based). */
    virtual void setTwoToneDecodeIndex(uint idx);

    /** Retunrs the transmit contact index (0-based). */
    virtual uint contactIndex() const;
    /** Sets the transmit contact index (0-based). */
    virtual void setContactIndex(uint idx);

    /** Retunrs the radio ID index (0-based). */
    virtual uint radioIDIndex() const;
    /** Sets the radio ID index (0-based). */
    virtual void setRadioIDIndex(uint idx);

    /** Returns @c true if the sequelch is silent and @c false if open. */
    virtual bool silentSquelch() const;
    /** Enables/disables silent squelch. */
    virtual void enableSilentSquelch(bool enable);

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
    virtual uint scanListIndex() const;
    /** Sets the scan list index (0-based). */
    virtual void setScanListIndex(uint idx);
    /** Clears the scan list index. */
    virtual void clearScanListIndex();

    /** Returns @c true, if a group list index is set. */
    virtual bool hasGroupListIndex() const;
    /** Returns the scan list index (0-based). */
    virtual uint groupListIndex() const;
    /** Sets the group list index (0-based). */
    virtual void setGroupListIndex(uint idx);
    /** Clears the group list index. */
    virtual void clearGroupListIndex();

    /** Returns the two-tone ID index (0-based). */
    virtual uint twoToneIDIndex() const;
    /** Sets the two-tone ID index (0-based). */
    virtual void setTwoToneIDIndex(uint idx);
    /** Returns the five-tone ID index (0-based). */
    virtual uint fiveToneIDIndex() const;
    /** Sets the five-tone ID index (0-based). */
    virtual void setFiveToneIDIndex(uint idx);
    /** Returns the DTFM ID index (0-based). */
    virtual uint dtmfIDIndex() const;
    /** Sets the DTMF ID index (0-based). */
    virtual void setDTMFIDIndex(uint idx);

    /** Returns the color code. */
    virtual uint colorCode() const;
    /** Sets the color code. */
    virtual void setColorCode(uint code);

    /** Returns the time slot. */
    virtual DigitalChannel::TimeSlot timeSlot() const;
    /** Sets the time slot. */
    virtual void setTimeSlot(DigitalChannel::TimeSlot ts);

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
    virtual uint encryptionKeyIndex() const;
    /** Sets the AES (enahnced) encryption key index (0-based). */
    virtual void setEncryptionKeyIndex(uint idx);
    /** Clears the encryption key index. */
    virtual void clearEncryptionKeyIndex();

    /** Returns the channel name. */
    virtual QString name() const;
    /** Sets the channel name. */
    virtual void setName(const QString &name);

    /** Returns @c true if ranging is enabled. */
    virtual bool ranging() const;
    /** Enables/disables ranging. */
    virtual void enableRanging(bool enable);
    /** Returns @c true if through mode is enabled. */
    virtual bool throughMode() const;
    /** Enables/disables though mode. */
    virtual void enableThroughMode(bool enable);
    /** Returns @c true if data ACK is enabled. */
    virtual bool dataACK() const;
    /** Enables/disables data ACK. */
    virtual void enableDataACK(bool enable);

    /** Returns @c true if TX APRS is enabled. */
    virtual bool txAPRS() const;
    /** Enables/disables TX APRS. */
    virtual void enableTXAPRS(bool enable);
    /** Returns the DMR APRS system index. */
    virtual uint dmrAPRSSystemIndex() const;
    /** Sets the DMR APRS system index. */
    virtual void setDMRAPRSSystemIndex(uint idx);

    /** Returns the DMR encryption key index (+1), 0=Off. */
    virtual uint dmrEncryptionKeyIndex() const;
    /** Sets the DMR encryption key index (+1), 0=Off. */
    virtual void setDMREncryptionKeyIndex(uint idx);
    /** Returns @c true if multiple key encryption is enabled. */
    virtual bool multipleKeyEncryption() const;
    /** Enables/disables multiple key encryption. */
    virtual void enableMultipleKeyEncryption(bool enable);
    /** Returns @c true if random key is enabled. */
    virtual bool randomKey() const;
    /** Enables/disables random key. */
    virtual void enableRandomKey(bool enable);
    /** Returns @c true if SMS is enabled. */
    virtual bool sms() const;
    /** Enables/disables SMS. */
    virtual void enableSMS(bool enable);
  };

  /** Represents the base class for conacts in all AnyTone codeplugs.
   *
   * Memmory layout of encoded contact (0x64 bytes):
   * @verbinclude anytone_contact.txt
   */
  class ContactElement: public Element
  {
  public:
    /** Possible ring-tone types. */
    enum class AlertType {
      None = 0,                   ///< Alert disabled.
      Ring = 1,                   ///< Ring tone.
      Online = 2                  ///< WTF?
    };

  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, uint size);

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
    virtual DigitalContact::Type type() const;
    /** Sets the contact type. */
    virtual void setType(DigitalContact::Type type);

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString &name);

    /** Returns the contact number. */
    virtual uint number() const;
    /** Sets the contact number. */
    virtual void setNumber(uint number);

    /** Returns the alert type. */
    virtual AlertType alertType() const;
    /** Sets the alert type. */
    virtual void setAlertType(AlertType type);
  };

  /** Represents the base class for analog (DTMF) contacts in all AnyTone codeplugs.
   *
   * Encoding of the DTMF contact (0x30 bytes):
   * @verbinclude anytone_dtmfcontact.txt */
  class DTMFContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFContactElement(uint8_t *ptr, uint size);

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
  };

  /** Represents the base class for group lists in all AnyTone codeplugs.
   *
   * Encoding of a group list (0x120 bytes):
   * @verbinclude anytone_grouplist.txt */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, uint size);

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
    virtual bool hasMemberIndex(uint n) const;
    /** Returns the n-th member index. */
    virtual uint memberIndex(uint n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(uint n, uint idx);
    /** Clears the n-th member index. */
    virtual void clearMemberIndex(uint n);
  };

  /** Represents the base class for scan lists in all AnyTone codeplugs.
   *
   * Memmory layout of encoded scanlist (0x90 bytes):
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
    ScanListElement(uint8_t *ptr, uint size);

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
    virtual uint primary() const;
    /** Sets the primary channel index. */
    virtual void setPrimary(uint idx);
    /** Sets the primary channel to be selected channel. */
    virtual void setPrimarySelected();
    /** Clears the primary channel index. */
    virtual void clearPrimaryChannel();

    /** Returns @c true if the secondary channel is set. */
    virtual bool hasSecondary() const;
    /** Returns @c true if the secondary channel is set to the selected channel. */
    virtual bool secondaryIsSelected() const;
    /** Returns the secondary channel index. */
    virtual uint secondary() const;
    /** Sets the secondary channel index. */
    virtual void setSecondary(uint idx);
    /** Sets the secondary channel to be selected channel. */
    virtual void setSecondarySelected();
    /** Clears the secondary channel index. */
    virtual void clearSecondaryChannel();

    /** Returns the look back time A in seconds. */
    virtual uint lookBackTimeA() const;
    /** Sets the look back time A in seconds. */
    virtual void setLookBackTimeA(uint sec);
    /** Returns the look back time B in seconds. */
    virtual uint lookBackTimeB() const;
    /** Sets the look back time B in seconds. */
    virtual void setLookBackTimeB(uint sec);
    /** Returns the drop out delay in seconds. */
    virtual uint dropOutDelay() const;
    /** Sets the drop out delay in seconds. */
    virtual void setDropOutDelay(uint sec);
    /** Returns the dwell time in seconds. */
    virtual uint dwellTime() const;
    /** Sets the dwell time in seconds. */
    virtual void setDwellTime(uint sec);

    /** Returns the revert channel type. */
    virtual RevertChannel revertChannel() const;
    /** Sets the revert channel type. */
    virtual void setRevertChannel(RevertChannel type);

    /** Retunrs the name of the scan list. */
    virtual QString name() const;
    /** Sets the name of the scan list. */
    virtual void setName(const QString &name);

    /** Returns @c true if the n-th member index is set. */
    virtual bool hasMemberIndex(uint n) const;
    /** Returns the n-th member index. */
    virtual uint memberIndex(uint n) const;
    /** Sets the n-th member index. */
    virtual void setMemberIndex(uint n, uint idx);
    /** Clears the n-th member index. */
    virtual void clearMemberIndex(uint n);
  };

  /** Represents the base class for radio IDs in all AnyTone codeplugs.
   *
   * Memmory layout of encoded scanlist (0x20 bytes):
   * @verbinclude anytone_radioid.txt */
  class RadioIDElement: public Element
  {
  protected:
    /** Hidden constructor. */
    RadioIDElement(uint8_t *ptr, uint size);

  public:
    /** Constructor. */
    RadioIDElement(uint8_t *ptr);

    /** Resets the radio ID. */
    void clear();

    /** Returns the number of the radio ID. */
    virtual uint number() const;
    /** Sets the number of the radio ID. */
    virtual void setNumber(uint number);

    /** Returns the name of the radio ID. */
    virtual QString name() const;
    /** Sets the name of the radio ID. */
    virtual void setName(const QString &name);
  };

  /** Represents the base class for the settings element in all AnyTone codeplugs.
   *
   * Memmory layout of encoded general settings (0xd0 bytes):
   * @verbinclude anytone_generalsettings.txt
   */
  class GeneralSettingsElement: public Element
  {
  public:
    /** Possible automatic shutdown delays. */
    enum class AutoShutdown {
      Off = 0, After10min = 1, After30min  = 2, After60min  = 3, After120min = 4,
    };

    /** What to display during boot. */
    enum class BootDisplay {
      Default = 0, CustomText = 1, CustomImage = 2
    };

    /** Possible power save modes. */
    enum class PowerSave {
      Off = 0, Save50 = 1, Save66 = 2
    };

    /** Encodes the possible VFO scan types. */
    enum class VFOScanType {
      TO = 0, CO = 1, SE = 2
    };

    /** All possible key functions. */
    enum class KeyFunction {
      Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
      DigitalEncryption = 0x05, Call = 0x06, VOX = 0x07, VFOChannel = 0x08, SubPTT = 0x09,
      Scan = 0x0a, FM = 0x0b, Alarm = 0x0c, RecordSwitch = 0x0d, Record = 0x0e, SMS = 0x0f,
      Dial = 0x10, GPSInformation = 0x11, Monitor = 0x12, MainChannelSwitch = 0x13, HotKey1 = 0x14,
      HotKey2 = 0x15, HotKey3 = 0x16, HotKey4 = 0x17, HotKey5 = 0x18, HotKey6 = 0x19,
      WorkAlone = 0x1a, NuisanceDelete = 0x1b, DigitalMonitor = 0x1c, SubChannelSwitch = 0x1d,
      PriorityZone = 0x1e, VFOScan = 0x1f, MICSoundQuality = 0x20, LastCallReply = 0x21,
      ChannelTypeSwitch = 0x22, Ranging = 0x23, Roaming = 0x24, ChannelRanging = 0x25,
      MaxVolume = 0x26, SlotSwitch = 0x27, APRSTypeSwitch = 0x28, ZoneSelect = 0x29,
      TimedRoamingSet = 0x2a, APRSSet = 0x2b, MuteTimeing = 0x2c, CtcssDcsSet = 0x2d,
      TBSTSend = 0x2e, Bluetooth = 0x2f, GPS = 0x30, ChannelName = 0x31, CDTScan = 0x32
    };

    /** Source for the VOX. */
    enum class VoxSource {
      Internal = 0, External = 1, Both = 2
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

    /** Represents a configurable ring tone melody. */
    struct Melody {
      /** Represents a note of the melody. */
      struct Note {
        uint frequency; ///< Tone frequency in Hz.
        uint duration;  ///< Tone duration in ms.
      };

      /** Holds the 5 notes of the melody. */
      Note notes[5];

      /** Empty constructor. */
      Melody();
      /** Copy constructor. */
      Melody(const Melody &ohter);
      /** Assignment operator. */
      Melody &operator =(const Melody &other);
    };

    /** Possible display colors. */
    enum class Color {
      Black = 0, Red = 1
    };

  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, uint size);

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
    virtual uint autoShutdownDelay() const;
    /** Sets the auto-shutdown delay in minutes. */
    virtual void setAutoShutdownDelay(uint min);
    /** Returns the boot display mode. */
    virtual BootDisplay bootDisplay() const;
    /** Sets the boot display mode. */
    virtual void setBootDisplay(BootDisplay mode);
    /** Returns @c true if boot password is enabled. */
    virtual bool bootPassword() const;
    /** Enables/disables boot password. */
    virtual void enableBootPassword(bool enable);
    /** Squelch level of VFO A, (0=off). */
    virtual uint squelchLevelA() const;
    /** Returns the squelch level for VFO A, (0=off). */
    virtual void setSquelchLevelA(uint level);
    /** Squelch level of VFO B, (0=off). */
    virtual uint squelchLevelB() const;
    /** Returns the squelch level for VFO B, (0=off). */
    virtual void setSquelchLevelB(uint level);
    /** Returns the power-save mode. */
    virtual PowerSave powerSave() const;
    /** Sets the power-save mode. */
    virtual void setPowerSave(PowerSave mode);
    /** Returns the VOX level. */
    virtual uint voxLevel() const;
    /** Sets the VOX level. */
    virtual void setVOXLevel(uint level);
    /** Returns the VOX delay in ms. */
    virtual uint voxDelay() const;
    /** Sets the VOX delay in ms. */
    virtual void setVOXDelay(uint ms);
    /** Retuns the VFO scan type. */
    virtual VFOScanType vfoScanType() const;
    /** Sets the VFO scan type. */
    virtual void setVFOScanType(VFOScanType type);
    /** Returns the mirophone gain. */
    virtual uint micGain() const;
    /** Sets the microphone gain. */
    virtual void setMICGain(uint gain);

    /** Retruns the key function for a short press on the programmable function key 1. */
    virtual KeyFunction progFuncKey1Short() const;
    /** Sets the key function for a short press on the programmable function key 1. */
    virtual void setProgFuncKey1Short(KeyFunction func);
    /** Retruns the key function for a short press on the programmable function key 2. */
    virtual KeyFunction progFuncKey2Short() const;
    /** Sets the key function for a short press on the programmable function key 2. */
    virtual void setProgFuncKey2Short(KeyFunction func);
    /** Retruns the key function for a short press on the programmable function key 3. */
    virtual KeyFunction progFuncKey3Short() const;
    /** Sets the key function for a short press on the programmable function key 3. */
    virtual void setProgFuncKey3Short(KeyFunction func);
    /** Retruns the key function for a short press on the function key 1. */
    virtual KeyFunction funcKey1Short() const;
    /** Sets the key function for a short press on the function key 1. */
    virtual void setFuncKey1Short(KeyFunction func);
    /** Retruns the key function for a short press on the function key 2. */
    virtual KeyFunction funcKey2Short() const;
    /** Sets the key function for a short press on the function key 2. */
    virtual void setFuncKey2Short(KeyFunction func);
    /** Returns @c true if the VFO A is in VFO mode. */
    virtual bool vfoModeA() const;
    /** Enables/disables VFO mode for VFO A. */
    virtual void enableVFOModeA(bool enable);
    /** Returns @c true if the VFO B is in VFO mode. */
    virtual bool vfoModeB() const;
    /** Enables/disables VFO mode for VFO B. */
    virtual void enableVFOModeB(bool enable);
    /** Returns the memory zone for VFO A. */
    virtual uint memoryZoneA() const;
    /** Sets the memory zone for VFO A. */
    virtual void setMemoryZoneA(uint zone);

    /** Returns the memory zone for VFO B. */
    virtual uint memoryZoneB() const;
    /** Sets the memory zone for VFO B. */
    virtual void setMemoryZoneB(uint zone);
    /** Returns @c true if recording is enabled. */
    virtual bool recording() const;
    /** Enables/disables recording. */
    virtual void enableRecording(bool enable);
    /** Retruns the display brightness. */
    virtual uint brightness() const;
    /** Sets the display brightness. */
    virtual void setBrightness(uint level);
    /** Returns @c true if the backlight is always on. */
    virtual bool backlightPermanent() const;
    /** Retunrs the backlight duration in seconds. */
    virtual uint backlightDuration() const;
    /** Sets the backlight duration in seconds. */
    virtual void setBacklightDuration(uint sec);
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
    /** Retruns @c true if the talk permit tone is enabled for digital channels. */
    virtual bool talkPermitDigital() const;
    /** Retruns @c true if the talk permit tone is enabled for digital channels. */
    virtual bool talkPermitAnalog() const;
    /** Enables/disables the talk permit tone for digital channels. */
    virtual void enableTalkPermitDigital(bool enable);
    /** Enables/disables the talk permit tone for analog channels. */
    virtual void enableTalkPermitAnalog(bool enable);
    /** Retruns @c true if the reset tone is enabled for digital calls. */
    virtual bool digitalResetTone() const;
    /** Enables/disables the reset tone for digital calls. */
    virtual void enableDigitalResetTone(bool enable);
    /** Returns the VOX source. */
    virtual VoxSource voxSource() const;
    /** Sets the VOX source. */
    virtual void setVOXSource(VoxSource source);
    /** Returns @c true if the idle channel tone is enabled. */
    virtual bool idleChannelTone() const;
    /** Enables/disables the idle channel tone. */
    virtual void enableIdleChannelTone(bool enable);
    /** Returns the menu exit time in seconds. */
    virtual uint menuExitTime() const;
    /** Sets the menu exit time in seconds. */
    virtual void setMenuExitTime(uint sec);
    /** Returns @c true if the startup tone is enabled. */
    virtual bool startupTone() const;
    /** Enables/disables the startup tone. */
    virtual void enableStartupTone(bool enable);
    /** Returns @c true if the call-end prompt is enabled. */
    virtual bool callEndPrompt() const;
    /** Enables/disables the call-end prompt. */
    virtual void enableCallEndPrompt(bool enable);
    /** Returns the maximum volume. */
    virtual uint maxVolume() const;
    /** Sets the maximum volume. */
    virtual void setMaxVolume(uint level);
    /** Returns @c true if get GPS position is enabled. */
    virtual bool getGPSPosition() const;
    /** Enables/disables get GPS position. */
    virtual void enableGetGPSPosition(bool enable);

    /** Retruns the key function for a long press on the programmable function key 1. */
    virtual KeyFunction progFuncKey1Long() const;
    /** Sets the key function for a long press on the programmable function key 1. */
    virtual void setProgFuncKey1Long(KeyFunction func);
    /** Retruns the key function for a long press on the programmable function key 2. */
    virtual KeyFunction progFuncKey2Long() const;
    /** Sets the key function for a long press on the programmable function key 2. */
    virtual void setProgFuncKey2Long(KeyFunction func);
    /** Retruns the key function for a long press on the programmable function key 3. */
    virtual KeyFunction progFuncKey3Long() const;
    /** Sets the key function for a long press on the programmable function key 3. */
    virtual void setProgFuncKey3Long(KeyFunction func);
    /** Retruns the key function for a long press on the function key 1. */
    virtual KeyFunction funcKey1Long() const;
    /** Sets the key function for a long press on the function key 1. */
    virtual void setFuncKey1Long(KeyFunction func);
    /** Retruns the key function for a long press on the function key 2. */
    virtual KeyFunction funcKey2Long() const;
    /** Sets the key function for a long press on the function key 2. */
    virtual void setFuncKey2Long(KeyFunction func);
    /** Returns the long-press duration in ms. */
    virtual uint longPressDuration() const;
    /** Sets the long-press duration in ms. */
    virtual void setLongPressDuration(uint ms);
    /** Returns @c true if the volume change prompt is enabled. */
    virtual bool volumeChangePrompt() const;
    /** Enables/disables the volume change prompt. */
    virtual void enableVolumeChangePrompt(bool enable);
    /** Retruns the auto repeater offset direction for VFO A. */
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
    /** Retuns the maximum headphone volume. */
    virtual uint maxHeadphoneVolume() const;
    /** Sets the maximum headphone volume. */
    virtual void setMaxHeadPhoneVolume(uint max);
    /** Returns @c true if the audio is "enhanced". */
    virtual bool enhanceAudio() const;
    /** Enables/disables "enhanced" audio. */
    virtual void enableEnhancedAudio(bool enable);
    /** Retruns the minimum VFO scan frequency for the UHF band in Hz. */
    virtual uint minVFOScanFrequencyUHF() const;
    /** Sets the minimum VFO scan frequency for the UHF band in Hz. */
    virtual void setMinVFOScanFrequencyUHF(uint hz);
    /** Retruns the maximum VFO scan frequency for the UHF band in Hz. */
    virtual uint maxVFOScanFrequencyUHF() const;
    /** Sets the maximum VFO scan frequency for the UHF band in Hz. */
    virtual void setMaxVFOScanFrequencyUHF(uint hz);

    /** Retruns the minimum VFO scan frequency for the VHF band in Hz. */
    virtual uint minVFOScanFrequencyVHF() const;
    /** Sets the minimum VFO scan frequency for the VHF band in Hz. */
    virtual void setMinVFOScanFrequencyVHF(uint hz);
    /** Retruns the maximum VFO scan frequency for the VHF band in Hz. */
    virtual uint maxVFOScanFrequencyVHF() const;
    /** Sets the maximum VFO scan frequency for the VHF band in Hz. */
    virtual void setMaxVFOScanFrequencyVHF(uint hz);
    /** Returns @c true if the auto-repeater offset frequency for UHF is set. */
    virtual bool hasAutoRepeaterOffsetFrequencyIndexUHF() const;
    /** Returns the auto-repeater offset frequency index for UHF. */
    virtual uint autoRepeaterOffsetFrequencyIndexUHF() const;
    /** Sets the auto-repeater offset frequency index for UHF. */
    virtual void setAutoRepeaterOffsetFrequenyIndexUHF(uint idx);
    /** Clears the auto-repeater offset frequency index for UHF. */
    virtual void clearAutoRepeaterOffsetFrequencyIndexUHF();
    /** Returns @c true if the auto-repeater offset frequency for VHF is set. */
    virtual bool hasAutoRepeaterOffsetFrequencyIndexVHF() const;
    /** Returns the auto-repeater offset frequency index for UHF. */
    virtual uint autoRepeaterOffsetFrequencyIndexVHF() const;
    /** Sets the auto-repeater offset frequency index for VHF. */
    virtual void setAutoRepeaterOffsetFrequenyIndexVHF(uint idx);
    /** Clears the auto-repeater offset frequency index for VHF. */
    virtual void clearAutoRepeaterOffsetFrequencyIndexVHF();

    /** Retuns the call-tone melody. */
    virtual Melody callToneMelody() const;
    /** Sets the call-tone melody. */
    virtual void setCallToneMelody(const Melody &melody);
    /** Retuns the idle-tone melody. */
    virtual Melody idleToneMelody() const;
    /** Sets the idle-tone melody. */
    virtual void setIdleToneMelody(const Melody &melody);
    /** Retuns the reset-tone melody. */
    virtual Melody resetToneMelody() const;
    /** Sets the reset-tone melody. */
    virtual void setResetToneMelody(const Melody &melody);
    /** Returns the recording delay in ms. */
    virtual uint recordingDelay() const;
    /** Sets the recording delay in ms. */
    virtual void setRecodringDelay(uint ms);
    /** Returns @c true if the call is displayed instead of the name. */
    virtual bool displayCall() const;
    /** Enables/disables call display. */
    virtual void enableDisplayCall(bool enable);

    /** Returns the display color for callsigns. */
    virtual Color callDisplayColor() const;
    /** Sets the display color for callsigns. */
    virtual void setCallDisplayColor(Color color);
    /** Returns the GPS update preriod in seconds. */
    virtual uint gpsUpdatePeriod() const;
    /** Sets the GPS update period in seconds. */
    virtual void setGPSUpdatePeriod(uint sec);
    /** Returns @c true if the zone and contact are shown. */
    virtual bool showZoneAndContact() const;
    /** Enables/disables @c true the zone and contact display. */
    virtual void enableShowZoneAndContact(bool enable);
    /** Returns @c true if the key-tone level is adjustable. */
    virtual bool keyToneLevelAdjustable() const;
    /** Returns the key-tone level (0=adjustable). */
    virtual uint keyToneLevel() const;
    /** Sets the key-tone level. */
    virtual void setKeyToneLevel(uint level);
    /** Sets the key-tone level adjustable. */
    virtual void setKeyToneLevelAdjustable();
    /** Returns @c true if the GPS units are imperical. */
    virtual bool gpsUnitsImperial() const;
    /** Enables/disables imperical GPS units. */
    virtual void enableGPSUnitsImperial(bool enable);
    /** Returns @c true if the knob is locked. */
    virtual bool knobLock() const;
    /** Enables/disables the knob lock. */
    virtual void enableKnobLock(bool enable);
    /** Returns @c true if the keypad is locked. */
    virtual bool keypadLock() const;
    /** Enables/disables the keypad lock. */
    virtual void enableKeypadLock(bool enable);
    /** Returns @c true if the sidekeys are locked. */
    virtual bool sidekeysLock() const;
    /** Enables/disables the sidekeys lock. */
    virtual void enableSidekeysLock(bool enable);
    /** Returns @c true if the "professional" key is locked. */
    virtual bool professionalKeyLock() const;
    /** Enables/disables the "professional" key lock. */
    virtual void enableProfessionalKeyLock(bool enable);
    /** Returns @c true if the last heard is shown while pressing PTT. */
    virtual bool showLastHeard() const;
    /** Enables/disables showing last heard. */
    virtual void enableShowLastHeard(bool enable);
    /** Returns the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual uint autoRepeaterMinFrequencyVHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMinFrequencyVHF(uint Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual uint autoRepeaterMaxFrequencyVHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in VHF band. */
    virtual void setAutoRepeaterMaxFrequencyVHF(uint Hz);

    /** Returns the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual uint autoRepeaterMinFrequencyUHF() const;
    /** Sets the minimum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMinFrequencyUHF(uint Hz);
    /** Returns the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual uint autoRepeaterMaxFrequencyUHF() const;
    /** Sets the maximum frequency in Hz for the auto-repeater range in UHF band. */
    virtual void setAutoRepeaterMaxFrequencyUHF(uint Hz);
    /** Returns the auto-repeater direction for VFO B. */
    virtual AutoRepDir autoRepeaterDirectionB() const;
    /** Sets the auto-repeater direction for VFO B. */
    virtual void setAutoRepeaterDirectionB(AutoRepDir dir);
    /** Retuns @c true if the default boot channel is enabled. */
    virtual bool defaultChannel() const;
    /** Enables/disables default boot channel. */
    virtual void enableDefaultChannel(bool enable);
    /** Returns the default zone index (0-based) for VFO A. */
    virtual uint defaultZoneIndexA() const;
    /** Sets the default zone (0-based) for VFO A. */
    virtual void setDefaultZoneIndexA(uint idx);
    /** Returns the default zone index (0-based) for VFO B. */
    virtual uint defaultZoneIndexB() const;
    /** Sets the default zone (0-based) for VFO B. */
    virtual void setDefaultZoneIndexB(uint idx);
    /** Returns @c true if the default channel for VFO A is VFO. */
    virtual bool defaultChannelAIsVFO() const;
    /** Returns the default channel index for VFO A.
     * Must be within default zone. If 0xff, default channel is VFO. */
    virtual uint defaultChannelAIndex() const;
    /** Sets the default channel index for VFO A. */
    virtual void setDefaultChannelAIndex(uint idx);
    /** Sets the default channel for VFO A to be VFO. */
    virtual void setDefaultChannelAToVFO();
    /** Returns @c true if the default channel for VFO B is VFO. */
    virtual bool defaultChannelBIsVFO() const;
    /** Returns the default channel index for VFO B.
     * Must be within default zone. If 0xff, default channel is VFO. */
    virtual uint defaultChannelBIndex() const;
    /** Sets the default channel index for VFO B. */
    virtual void setDefaultChannelBIndex(uint idx);
    /** Sets the default channel for VFO B to be VFO. */
    virtual void setDefaultChannelBToVFO();
    /** Returns @c true if the last caller is kept when changeing channel. */
    virtual bool keepLastCaller() const;
    /** Enables/disables keeping the last caller when changeing the channel. */
    virtual void enableKeepLastCaller(bool enable);
  };

protected:
  /** Hidden constructor. */
  explicit AnytoneCodeplug(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~AnytoneCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear() = 0;

  virtual bool index(Config *config, Context &ctx) const;

  /** Sets all bitmaps for the given config. */
  virtual void setBitmaps(Config *config) = 0;
  /** Allocate all code-plug elements that must be downloaded for decoding. All code-plug elements
   * within the radio that are not represented within the common Config are omitted. */
  virtual void allocateForDecoding() = 0;
  /** Allocate all code-plug elements that must be written back to the device to maintain a working
   * codeplug. These elements might be updated during encoding. */
  virtual void allocateUpdated() = 0;
  /** Allocate all code-plug elements that are defined through the common Config. */
  virtual void allocateForEncoding() = 0;
};

#endif // ANYTONECODEPLUG_HH
