#ifndef OPENGD77BASE_CODEPLUG_HH
#define OPENGD77BASE_CODEPLUG_HH

#include "channel.hh"
#include "codeplug.hh"
#include "gpssystem.hh"
#include "contact.hh"
#include "zone.hh"
#include "satellitedatabase.hh"

#include <QGeoCoordinate>


class OpenGD77BaseCodeplug : public Codeplug
{
  Q_OBJECT

public:
  /** Possible image types. */
  enum ImageType { EEPROM = 0, FLASH = 1 };

public:
  /** Encodes an angle used to store locations. */
  static uint32_t encodeAngle(double degee);
  /** Decodes an angle used to store locations. */
  static double decodeAngle(uint32_t code);

  /** Encodes a selective call (tx/rx tone). */
  static uint16_t encodeSelectiveCall(const SelectiveCall &call);
  /** Decodes a selective call (tx/rx tone). */
  static SelectiveCall decodeSelectiveCall(uint16_t code);

public:
  /** Implements the base for all OpenGD77 channel encodings.
   */
  class ChannelElement: public Codeplug::Element
  {
  public:
    /** Possible channel types. */
    enum Mode {
      MODE_ANALOG = 0,   ///< Analog channel, aka FM.
      MODE_DIGITAL = 1   ///< Digital channel, aka DMR.
    };

    /** Alias to transmit. */
    enum class Alias {
      None = 0, APRS = 1, Text = 2, Both = 3
    };

    /** Possible squelch modes. */
    enum class SquelchMode {
      Global, Open, Normal, Closed
    };

  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    explicit ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();

    /** The size of the channel. */
    static constexpr unsigned int size() { return 0x0038; }

    /** Resets the channel. */
    virtual void clear();

    /** Returns the name of the channel. */
    virtual QString name() const;
    /** Sets the name of the channel. */
    virtual void setName(const QString &n);

    /** Returns the RX frequency of the channel. */
    virtual Frequency rxFrequency() const;
    /** Sets the RX frequency of the channel. */
    virtual void setRXFrequency(const Frequency &freq);
    /** Returns the TX frequency of the channel. */
    virtual Frequency txFrequency() const;
    /** Sets the TX frequency of the channel. */
    virtual void setTXFrequency(const Frequency &freq);

    /** Returns the channel mode. */
    virtual Mode mode() const;
    /** Sets the channel mode. */
    virtual void setMode(Mode mode);

    /** Returns @c true if the power-setting is global. */
    virtual bool globalPower() const;
    /** Returns the power setting of the channel. */
    virtual Channel::Power power() const;
    /** Sets the power setting of the channel. */
    virtual void setPower(Channel::Power pwr);
    /** Clears the power setting. The global power setting is used. */
    virtual void clearPower();

    /** Returns @c true, if a fixed position is set for the channel. */
    virtual bool hasFixedPosition() const;
    /** Returns the fixed position. */
    virtual QGeoCoordinate fixedPosition() const;
    /** Sets the fixed position for this channel. */
    virtual void setFixedPosition(const QGeoCoordinate &coordinate);
    /** Resets the fixed position. */
    virtual void clearFixedPosition();

    /** Returns the RX subtone. */
    virtual SelectiveCall rxTone() const;
    /** Sets the RX subtone. */
    virtual void setRXTone(const SelectiveCall &code);
    /** Returns the TX subtone. */
    virtual SelectiveCall txTone() const;
    /** Sets the TX subtone. */
    virtual void setTXTone(const SelectiveCall &code);

    /** Returns @c true if the channel is set to simplex. */
    virtual bool isSimplex() const;
    /** Sets the channel to simplex. */
    virtual void enableSimplex(bool enable);

    /** Returns @c true, if the power-save feature is enabled. */
    virtual bool powerSave() const;
    /** Enables/disables power-save. */
    virtual void enablePowerSave(bool enable);

    /** Returns @c true, if the "beep" is enabled. */
    virtual bool beep() const;
    /** Enables/disables "the beep". */
    virtual void enableBeep(bool enable);

    /** Returns @c true if the global DMR ID is overridden. */
    virtual bool hasDMRId() const;
    /** Retruns the DMR ID for this channel. */
    virtual unsigned int dmrId() const;
    /** Sets the DMR ID for this channel. */
    virtual void setDMRId(unsigned int id);
    /** Resets the DMR ID for this channel to the global one. */
    virtual void clearDMRId();

    /** Returns @c true if a group list is set. */
    virtual bool hasGroupList() const;
    /** Returns the group-list index. */
    virtual unsigned groupListIndex() const;
    /** Sets the group-list index. */
    virtual void setGroupListIndex(unsigned index);
    /** Clears the group list index. */
    virtual void clearGroupListIndex();

    /** Returns the color code. */
    virtual unsigned colorCode() const;
    /** Sets thecolor code. */
    virtual void setColorCode(unsigned cc);

    /** Returns @c true, if the APRS system index is set. */
    virtual bool hasAPRSIndex() const;
    /** Returns the APRS system index. */
    virtual unsigned int aprsIndex() const;
    /** Sets the APRS system index. */
    virtual void setAPRSIndex(unsigned int index);
    /** Resets the APRS system index. */
    virtual void clearAPRSIndex();

    /** Returns @c true, if the TX contact is set. */
    virtual bool hasTXContact() const;
    /** Returns the TX contact index. */
    virtual unsigned int txContactIndex() const;
    /** Sets the TX contact index. */
    virtual void setTXContactIndex(unsigned int index);
    /** Clears the TX contact index. */
    virtual void clearTXContact();

    /** Returns the alias transmitted on time slot 1. */
    virtual OpenGD77ChannelExtension::TalkerAlias aliasTimeSlot1() const;
    /** Sets the alias transmitted on time slot 1. */
    virtual void setAliasTimeSlot1(OpenGD77ChannelExtension::TalkerAlias alias);
    /** Returns the alias transmitted on time slot 2. */
    virtual OpenGD77ChannelExtension::TalkerAlias aliasTimeSlot2() const;
    /** Sets the alias transmitted on time slot 2. */
    virtual void setAliasTimeSlot2(OpenGD77ChannelExtension::TalkerAlias alias);

    /** Returns the time slot of the channel. */
    virtual DMRChannel::TimeSlot timeSlot() const;
    /** Sets the time slot of the channel. */
    virtual void setTimeSlot(DMRChannel::TimeSlot ts);

    /** Returns the bandwidth. */
    virtual FMChannel::Bandwidth bandwidth() const;
    /** Sets the bandwidth. */
    virtual void setBandwidth(FMChannel::Bandwidth bw);

    /** Returns @c true if RX only is enabled. */
    virtual bool rxOnly() const;
    /** Enables/disables RX only. */
    virtual void enableRXOnly(bool enable);

    /** Returns @c true if channel is skipped in a scan. */
    virtual bool skipScan() const;
    /** Enables/disables skipping in scan. */
    virtual void enableSkipScan(bool enable);
    /** Returns @c true if channel is skipped in zone scan. */
    virtual bool skipZoneScan() const;
    /** Enables/disables skipping in zone scan. */
    virtual void enableSkipZoneScan(bool enable);

    /** Returns @c true if VOX is enabled. */
    virtual bool vox() const;
    /** Enables/disables VOX. */
    virtual void enableVOX(bool enable);

    /** Returns the squelch mode*/
    virtual SquelchMode squelchMode() const;
    /** Retunrs the squelch level. */
    virtual unsigned int squelchLevel() const;
    /** Set the squelch level and mode. Level is ignored, if mode is not normal.*/
    virtual void setSquelch(SquelchMode mode, unsigned int level);

    /** Constructs a generic @c Channel object from the codeplug channel. */
    virtual Channel *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool link(Channel *c, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool encode(const Channel *c, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for this element. */
    struct Limit {
      /** The maximum length of the name. */
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Some internal offsets within the channel element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int rxFrequency() { return 0x0010; }
      static constexpr unsigned int txFrequency() { return 0x0014; }
      static constexpr unsigned int mode() { return 0x0018; }
      static constexpr unsigned int power() { return 0x0019; }
      static constexpr unsigned int latitude0() { return 0x001a; }
      static constexpr unsigned int latitude1() { return 0x001c; }
      static constexpr unsigned int latitude2() { return 0x001d; }
      static constexpr unsigned int longitude0() { return 0x001e; }
      static constexpr unsigned int longitude1() { return 0x001f; }
      static constexpr unsigned int rxTone() { return 0x0020; }
      static constexpr unsigned int txTone() { return 0x0022; }
      static constexpr unsigned int longitude2() { return 0x0024; }
      static constexpr Bit simplex() { return {0x0026, 2}; }
      static constexpr Bit useFixedLocation() { return {0x0026, 3}; }
      static constexpr Bit disablePowerSave() { return {0x0026, 5}; }
      static constexpr Bit disableBeep() { return {0x0026, 6}; }
      static constexpr Bit overrideDMRID() { return {0x0026, 7}; }
      static constexpr unsigned int dmrId() { return 0x0027; }
      static constexpr unsigned int groupList() { return 0x002b; }
      static constexpr unsigned int colorCode() { return 0x002c; }
      static constexpr unsigned int aprsIndex() { return 0x002d; }
      static constexpr unsigned int txContact() { return 0x002e; }
      static constexpr Bit aliasTimeSlot2() { return { 0x030, 2}; }
      static constexpr Bit aliasTimeSlot1() { return { 0x030, 0}; }
      static constexpr Bit timeSlot() { return {0x0031, 6}; }
      static constexpr Bit bandwidth() { return {0x0033, 1}; }
      static constexpr Bit rxOnly() { return {0x0033, 2}; }
      static constexpr Bit skipScan() { return {0x0033, 4}; }
      static constexpr Bit skipZoneScan() { return {0x0033, 5}; }
      static constexpr Bit vox() { return {0x0033, 6}; }
      static constexpr unsigned int squelch() { return 0x0037; }
    };
  };


  /** Implements the base for channel banks in Radioddity codeplugs.
   *
   * Memory layout of a channel bank:
   * @verbinclude radioddity_channelbank.txt */
  class ChannelBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ChannelBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ChannelBankElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelBankElement();

    /** The size of the channel bank. */
    static constexpr unsigned int size() { return 0x1c10; }

    /** Clears the bank. */
    void clear();

    /** Returns @c true if the channel is enabled. */
    virtual bool isEnabled(unsigned idx) const ;
    /** Enable/disable a channel in the bank. */
    virtual void enable(unsigned idx, bool enabled);
    /** Returns a pointer to the channel at the given index. */
    virtual uint8_t *get(unsigned idx) const;
    /** Retunrs the n-th channel. */
    ChannelElement channel(unsigned int n);

  public:
    /** Some limits for the channel bank. */
    struct Limit {
      /** The maximum number of channels. */
      static constexpr unsigned int channelCount() { return 128; }
    };

  protected:
    /** Some internal offset within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int bitmask() { return 0x0000; }
      static constexpr unsigned int channels() { return 0x0010; }
      /// @endcond
    };
  };


  /** VFO Channel representation within the binary codeplug.
   *
   * Each channel requires 0x38b:
   * @verbinclude radioddity_vfochannel.txt */
  class VFOChannelElement: public ChannelElement
  {
  public:
    /** Possible offset frequency modes. */
    enum class OffsetMode {
      Off = 0,       ///< Disables transmit frequency offset.
      Positive = 1,  ///< Transmit offset frequency is positive (TX above RX).
      Negative = 2   ///< Transmit offset frequency is negative (TX below RX).
    };

    /** Possible tuning step sizes. */
    enum class StepSize {
      SS2_5kHz = 0,  ///< 2.5kHz
      SS5kHz = 1,    ///< 5kHz
      SS6_25kHz = 2, ///< 6.25kHz
      SS10kHz = 3,   ///< 10kHz
      SS12_5kHz = 4, ///< 12.5kHz
      SS20kHz = 5,   ///< 20kHz
      SS30kHz = 6,   ///< 30kHz
      SS50kHz = 7    ///< 50kHz
    };

  protected:
    /** Hidden constructor. */
    VFOChannelElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit VFOChannelElement(uint8_t *ptr);

    void clear();

    /** The VFO channel has no name. */
    QString name() const;
    /** The VFO channel has no name. */
    void setName(const QString &name);

    /** Returns the tuning step-size in kHz. */
    virtual double stepSize() const;
    /** Sets the tuning step-size in kHz. */
    virtual void setStepSize(double kHz);

    /** Returns the transmit frequency offset mode. */
    virtual OffsetMode offsetMode() const;
    /** Returns the transmit frequency offset. */
    virtual double txOffset() const;
    /** Sets the transmit frequency offset in MHz. */
    virtual void setTXOffset(double f);
    /** Sets the transmit frequency offset mode. */
    virtual void setOffsetMode(OffsetMode mode);

  protected:
    /// @cond DO_NOT_DOCUMENT
    struct Offset: public ChannelElement::Offset {
      static constexpr Bit stepSize()                         { return {0x0036, 4} ; }
      static constexpr Bit offsetMode()                       { return {0x0036, 2} ; }
      static constexpr unsigned int txOffset()                { return 0x0034; }
    };
    /// @endcond
  };


  /** Encodes the settings element for all OpenGD77 codeplugs. */
  class GeneralSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GeneralSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GeneralSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0090; }

    void clear();

    /** Returns the UHF minimum frequency. */
    virtual Frequency uhfMinFrequency() const;
    /** Sets the UHF minimum frequency. */
    virtual void setUHFMinFrequency(const Frequency &f);
    /** Returns the UHF maximum frequency. */
    virtual Frequency uhfMaxFrequency() const;
    /** Sets the UHF maximum frequency. */
    virtual void setUHFMaxFrequency(const Frequency &f);

    /** Returns the VHF minimum frequency. */
    virtual Frequency vhfMinFrequency() const;
    /** Sets the VHF minimum frequency. */
    virtual void setVHFMinFrequency(const Frequency &f);
    /** Returns the VHF maximum frequency. */
    virtual Frequency vhfMaxFrequency() const;
    /** Sets the VHF maximum frequency. */
    virtual void setVHFMaxFrequency(const Frequency &f);

    /** Returns the radio callsign. */
    virtual QString call() const;
    /** Sets the radio callsign. */
    virtual void setCall(const QString &call);

    /** Returns the DMR ID. */
    virtual unsigned int radioId() const;
    /** Sets the DMR ID. */
    virtual void setRadioId(unsigned int id);

    /** Encodes the settings. */
    virtual bool encode(const Context &ctx, const ErrorStack &err = ErrorStack());
    /** Decodes the settings. */
    virtual bool decode(const Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      /** The maximum call length. */
      static constexpr unsigned int callLength() { return 8; }
    };

  protected:
    /** Some internal offset within the element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int uhfMinFrequency() { return 0x0000; }
      static constexpr unsigned int uhfMaxFrequency() { return 0x0002; }
      static constexpr unsigned int vhfMinFrequency() { return 0x0004; }
      static constexpr unsigned int vhfMaxFrequency() { return 0x0006; }
      static constexpr unsigned int call()            { return 0x0060; }
      static constexpr unsigned int dmrId()           { return 0x0068; }
      /// @endcond
    };
  };


  /** APRS system for OpenGD77 devices. */
  class APRSSettingsElement: public Element
  {
  public:
    enum class BaudRate {
      Baud300 = 1, Baud1200 = 0
    };

    enum class PositionPrecision {
      Max = 0,
      Mask1_8sec = 1,
      Mask3_6sec = 2,
      Mask18sec = 3,
      Mask36sec = 4,
      Mask3min = 5,
      Mask6min = 6,
      Mask30min = 7
    };

  public:
    /** Constructor from pointer. */
    explicit APRSSettingsElement(uint8_t *ptr);

  protected:
    /** Hidden constructor. */
    APRSSettingsElement(uint8_t *ptr, size_t size);

  public:
    /** The size of the channel bank. */
    static constexpr unsigned int size() { return 0x40; }

    /** Clears the bank. */
    void clear();

    /** Returns @c true, if the system is valid. */
    virtual bool isValid() const;

    /** Returns the name of the system. */
    virtual QString name() const;
    /** Sets the name of the system. */
    virtual void setName(const QString &name);

    /** Returns the source SSID. */
    virtual unsigned int sourceSSID() const;
    /** Sets the source SSID. */
    virtual void setSourceSSID(unsigned int ssid);

    /** Returns @c true, if a fixed position is send. */
    virtual bool hasFixedPosition() const;
    /** Returns the fixed position. */
    virtual QGeoCoordinate fixedPosition() const;
    /** Sets the fixed position. */
    virtual void setFixedPosition(const QGeoCoordinate &coor);
    /** Resets the fixed position. */
    virtual void clearFixedPosition();

    /** Retunrs the posiiton reporting precision. */
    virtual PositionPrecision positionPrecision() const;
    /** Sets the position reporting precision in degrees. */
    virtual void setPositionPrecision(PositionPrecision prec);

    /** Returns @c true, if the first via node is set. */
    virtual bool hasVia1() const;
    /** Returns the first via node call. */
    virtual QString via1Call() const;
    /** Returns the first via node ssid. */
    virtual unsigned int via1SSID() const;
    /** Sets the first via node. */
    virtual void setVia1(const QString &call, unsigned int ssid);
    /** Clears the first via node. */
    virtual void clearVia1();

    /** Returns @c true, if the second via node is set. */
    virtual bool hasVia2() const;
    /** Returns the second via node call. */
    virtual QString via2Call() const;
    /** Returns the second via node ssid. */
    virtual unsigned int via2SSID() const;
    /** Sets the second via node. */
    virtual void setVia2(const QString &call, unsigned int ssid);
    /** Clears the second via node. */
    virtual void clearVia2();

    /** Retunrs the icon. */
    virtual APRSSystem::Icon icon() const;
    /** Sets the icon. */
    virtual void setIcon(APRSSystem::Icon icon);

    /** Retunrs the comment text. */
    virtual QString comment() const;
    /** Sets the comment text. */
    virtual void setComment(const QString &text);

    /** Retunrs the baud-rate. */
    virtual BaudRate baudRate() const;
    /** Sets the baud rate. */
    virtual void setBaudRate(BaudRate rate);

    /** Encodes the APRS settings. */
    virtual bool encode(const APRSSystem *system, const Context &ctx, const ErrorStack &err=ErrorStack());
    /** Decodes some APRS settings. */
    virtual APRSSystem *decode(const Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the ARPS settings. */
    virtual bool link(APRSSystem *system, const Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. */
    struct Limit: public Element::Limit {
      static constexpr unsigned int nameLength() { return 8; }
      static constexpr unsigned int commentLength() { return 23; }
    };

  protected:
    /// @cond DO_NOT_DOCUMENT
    struct Offset: public Element::Offset
    {
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int sourceSSID() { return 0x0008; }
      static constexpr unsigned int latitude() { return 0x0009; }
      static constexpr unsigned int longitude() { return 0x000c; }
      static constexpr unsigned int via1Call() { return 0x000f; }
      static constexpr unsigned int via1SSID() { return 0x0015; }
      static constexpr unsigned int via2Call() { return 0x0016; }
      static constexpr unsigned int via2SSID() { return 0x001c; }
      static constexpr unsigned int iconTable() { return 0x001d; }
      static constexpr unsigned int iconIndex() { return 0x001e; }
      static constexpr unsigned int comment() { return 0x001f; }
      static constexpr Bit positionPrecision() { return { 0x003d, 4}; }
      static constexpr Bit useFixedPosition() { return { 0x003d, 1}; }
      static constexpr Bit baudRate() { return { 0x003d, 0}; }
    };
    /// @endcond
  };


  /** APRS System bank. */
  class APRSSettingsBankElement: public Element
  {
  public:
    /** Constructor from pointer. */
    explicit APRSSettingsBankElement(uint8_t *ptr);

  protected:
    /** Hidden constructor. */
    APRSSettingsBankElement(uint8_t *ptr, size_t size);

  public:
    /** The size of the channel bank. */
    static constexpr unsigned int size() { return 0x40; }

    /** Clears the bank. */
    void clear();

    /** Retunrs the n-th APRS system. */
    APRSSettingsElement system(unsigned int idx) const;

    /** Encodes all FM APRS systems. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Decodes all FM APRS systems. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links all FM APRS systems. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the bank. */
    struct Limit: public Element::Limit {
      /** The total number of APRS systems. */
      static constexpr unsigned int systems() { return 8; }
    };

  public:
    /// @cond DO_NOT_DOCUMENT
    struct Offset: public Element::Offset {
      static constexpr unsigned int systems() { return 0x0000; }
      static constexpr unsigned int betweenSystems() { return APRSSettingsElement::size(); }
    };
    /// @endcond
  };


  /** DTMF contact element.
   * Just a name and DTMF number. */
  class DTMFContactElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFContactElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    explicit DTMFContactElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0020; }

    void clear();
    bool isValid() const;

    /** Returns the name. */
    virtual QString name() const;
    /** Sets the name. */
    virtual void setName(const QString &name);

    /** Returns the DTMF number. */
    virtual QString number() const;
    /** Sets the DTMF number. */
    virtual void setNumber(const QString &number);

    /** Encodes a number. */
    virtual bool encode(const DTMFContact *contact, const Context &ctx, const ErrorStack &err=ErrorStack());
    /** Decodes a number. */
    virtual DTMFContact *decode(const Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits. Ü*/
    struct Limit: public Element::Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength() { return 16; }
      /** The maximum length of the number. */
      static constexpr unsigned int numberLength() { return 16; }
    };

  protected:
    /// @cond DO_NOT_DOCUMENT
    struct Offset: public Element::Offset {
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int number() { return 0x0010; }
    };
    /// @endcond
  };


  /** DTMF contact bank. */
  class DTMFContactBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    DTMFContactBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    DTMFContactBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return Limit::contacts()*DTMFContactElement::size(); }

    void clear();

    /** Returns the n-th DTMF contact. */
    DTMFContactElement contact(unsigned int n) const;

    /** Encodes all DTMF contacts. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Decodes all DTMF contacts. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the bank. */
    struct Limit: public Element::Limit {
      /** The total number of contacts. */
      static constexpr unsigned int contacts() { return 64; }
    };

  public:
    /// @cond DO_NOT_DOCUMENT
    struct Offset: public Element::Offset {
      static constexpr unsigned int contacts() { return 0x0000; }
      static constexpr unsigned int betweenContacts() { return DTMFContactElement::size(); }
    };
    /// @endcond
  };


  /** Implements the base class of boot settings for all OpenGD77 codeplugs. */
  class BootSettingsElement: public Element
  {
  protected:
    /** Hidden constructor. */
    BootSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit BootSettingsElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~BootSettingsElement();

    /** Resets the settings. */
    void clear();

    /** Returns @c true if the text is shown on boot, other wise an image is shown. */
    virtual bool bootText() const;
    /** Enables/disables boot text. */
    virtual void enableBootText(bool enable);

    /** Returns @c true if the boot password is enabled. */
    virtual bool bootPasswordEnabled() const;
    /** Returns the boot password (6 digit). */
    virtual unsigned bootPassword() const;
    /** Sets the boot password (6 digit). */
    virtual void setBootPassword(unsigned passwd);
    /** Clear boot password. */
    virtual void clearBootPassword();

    /** Returns the first line. */
    virtual QString line1() const;
    /** Sets the first line. */
    virtual void setLine1(const QString &text);
    /** Returns the Second line. */
    virtual QString line2() const;
    /** Sets the second line. */
    virtual void setLine2(const QString &text);

    /** Encodes boot text settings from configuration. */
    virtual bool encode(const Context &ctx, const ErrorStack &err = ErrorStack());
    /** Updates the configuration with the boot text settings. */
    virtual bool decode(Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits for the settings. */
    struct Limit: public Element::Limit {
      /** The total number of contacts. */
      static constexpr unsigned int lineLength() { return 16; }
    };

  public:
    /// @cond DO_NOT_DOCUMENT
    struct Offset: public Element::Offset {
      static constexpr unsigned int bootText() { return 0x0000; }
      static constexpr unsigned int bootPasswdEnable() { return 0x0001; }
      static constexpr unsigned int bootPasswd() { return 0x000c; }
      static constexpr unsigned int line1() { return 0x0028; }
      static constexpr unsigned int line2() { return 0x0038; }
    };
    /// @endcond
  };


  /** Represents a zone within OpenGD77 codeplugs. */
  class ZoneElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ZoneElement(uint8_t *ptr);
    virtual ~ZoneElement();

    /** The size of the zone element. */
    static constexpr unsigned int size() { return 0x00b0; }

    /** Resets the zone. */
    void clear();
    /** Returns @c true if the zone is valid. */
    bool isValid() const;

    /** Returns the name of the zone. */
    virtual QString name() const;
    /** Sets the name of the zone. */
    virtual void setName(const QString &name);

    /** Returns @c true if a member is stored at the given index. */
    virtual bool hasMember(unsigned n) const;
    /** Returns the n-th member index. */
    virtual unsigned member(unsigned n) const;
    /** Sets the n-th member index. */
    virtual void setMember(unsigned n, unsigned idx);
    /** Clears the n-th member index. */
    virtual void clearMember(unsigned n);

    /** Resets this codeplug zone representation from the given generic @c Zone object. */
    virtual bool encode(const Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Constructs a generic @c Zone object from this codeplug zone. */
    virtual Zone *decode(const Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links a previously constructed @c Zone object to the rest of the configuration. That is
     * linking to the referred channels. */
    virtual bool link(Zone *zone, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  public:
    /** Some limits for zone elements. */
    struct Limit: public Element::Limit {
      /** The maximum length of the zone name. */
      static constexpr unsigned int nameLength() { return 16; }
      /** The maximum number of members. */
      static constexpr unsigned int memberCount() { return 80; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()  { return 0x0000; }
      static constexpr unsigned int channels() { return 0x0010; }
      static constexpr unsigned int betweenChannels() { return 0x0002; }
      /// @endcond
    };
  };


  /** Implements the base class for all zone banks of OpenGD77 codeplugs. */
  class ZoneBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ZoneBankElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ZoneBankElement(uint8_t *ptr);
    /** Destructor. */
    ~ZoneBankElement();

    /** The size of the zone element. */
    static constexpr unsigned int size() {
      return 0x0020 + Limit::zoneCount()*ZoneElement::size();
    }

    /** Resets the bank. */
    void clear();

    /** Returns @c true if the zone is enabled. */
    virtual bool isEnabled(unsigned idx) const ;
    /** Enable/disable a zone in the bank. */
    virtual void enable(unsigned idx, bool enabled);

    /** Retunrs the n-th zone. */
    ZoneElement zone(unsigned int n);

    /** Encodes all zones. */
    virtual bool encode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Decodes all zones. */
    virtual bool decode(Context &ctx, const ErrorStack &err=ErrorStack());
    /** Links all zones. */
    virtual bool link(Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the zone bank. */
    struct Limit: public Element::Limit {
      /** The maximum number of zones in this bank. */
      static constexpr unsigned int zoneCount() { return 68; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int bitmap() { return 0x0000; }
      static constexpr unsigned int zones()  { return 0x0020; }
      static constexpr unsigned int betweenZones() { return ZoneElement::size(); }
      /// @endcond
    };
  };


  class OrbitalElement: public Element
  {
  protected:
    /** Hidden constructor. */
    OrbitalElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    OrbitalElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x0064; }

    void clear();

    /** Sets the name of the element. */
    virtual void setName(const QString &name);

    /** Sets the epoch. */
    virtual void setEpoch(const ::OrbitalElement::Epoch &epoch);
    /** Sets the first derivative of mean motion. */
    virtual void setMeanMotion(double mm);
    /** Sets the first derivative of mean motion. */
    virtual void setMeanMotionDerivative(double dmm);
    /** Sets the inclination. */
    virtual void setInclination(double incl);
    /** Right ascension of the ascending node. */
    virtual void setAscension(double asc);
    /** Sets eccentricity. */
    virtual void setEccentricity(double ecc);
    /** Sets argument of perigee. */
    virtual void setPerigee(double arg);
    /** Set the mean anomaly. */
    virtual void setMeanAnomaly(double ma);
    /** Sets the revolution number at epoch. */
    virtual void setRevolutionNumber(unsigned int num);

    /** Sets the downlink frequency. */
    void setFMDownlink(const Frequency &f);
    /** Sets the uplink frequency. */
    void setFMUplink(const Frequency &f);
    /** Sets the CTCSS tone. */
    void setCTCSS(const SelectiveCall &call);
    /** Sets the APRS downlink frequency. */
    void setAPRSDownlink(const Frequency &f);
    /** Sets the APRS uplink frequency. */
    void setAPRSUplink(const Frequency &f);

    /** Sets the beacon frequency. */
    void setBeacon(const Frequency &f);

    /** Sets the APRS path. */
    void setAPRSPath(const QString &path);

    /** Encodes a satellite. */
    virtual bool encode(const Satellite &sat, const ErrorStack &err = ErrorStack());

  protected:
    /** Writes a fixed point value as a BCD number. Using 0-9 as digits, ah as decimal dot and bh
     * as blank.
     * @param offset Specifies, where to write the fixed point value.
     * @param value The value to write.
     * @param sign If @c true, a sign is written.
     * @param dec The number of digits in the integer part.
     * @param frac The number of digits in the fractional part.
     */
    void writeFixedPoint(const Offset::Bit &offset, double value, bool sign, unsigned int dec, unsigned int frac);
    /** Writes a fixed point value as a BCD number. Using 0-9 as digits and bh as blank. In contrast
     * to @c writeFixedPoint, this function expects no integer part.
     * @param offset Specifies, where to write the fixed point value.
     * @param value The value to write.
     * @param sign If @c true, a sign is written.
     * @param frac The number of digits in the fractional part.
     */
    void writeFractional(const Offset::Bit &offset, double value, bool sign, unsigned int frac);
    /** Write a fixed digit integer value. */
    void writeInteger(const Offset::Bit &offset, int value, bool sign, unsigned dec);
    /** Writes a single digit at the given offset. */
    void writeDigit(const Offset::Bit &offset, uint8_t digit);

  public:
    /** Some limits for the zone bank. */
    struct Limit: public Element::Limit {
      /** The maximum name length. */
      static constexpr unsigned int nameLength() { return 8; }
      /** Maximum length of the APRS path. */
      static constexpr unsigned int pathLength() { return 24; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset: public Element::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name()         { return 0x0000; }
      static constexpr Bit epochYear()             { return {0x0008, 4}; }
      static constexpr Bit epochJulienDay()        { return {0x0009, 4}; }
      static constexpr Bit meanMotionDerivative()  { return {0x000f, 4}; }
      static constexpr Bit inclination()           { return {0x0014, 4}; }
      static constexpr Bit ascension()             { return {0x0018, 4}; }
      static constexpr Bit eccentricity()          { return {0x001c, 4}; }
      static constexpr Bit perigee()               { return {0x001f, 0}; }
      static constexpr Bit meanAnomaly()           { return {0x0023, 0}; }
      static constexpr Bit meanMotion()            { return {0x0027, 0}; }
      static constexpr Bit revolutionNumber()      { return {0x002d, 4}; }
      static constexpr unsigned int fmDownlink()   { return 0x0030; }
      static constexpr unsigned int fmUplink()     { return 0x0034; }
      static constexpr unsigned int ctcss()        { return 0x0038; }
      static constexpr unsigned int aprsDownlink() { return 0x003c; }
      static constexpr unsigned int aprsUplink()   { return 0x0040; }
      static constexpr unsigned int beacon()       { return 0x0044; }
      static constexpr unsigned int aprsPath()     { return 0x004c; }
      /// @endcond
    };
  };


  /** Implements a bank of several satellites for all OpenGD77 codeplugs. */
  class OrbitalBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    OrbitalBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    OrbitalBankElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() { return 0x09cc; }

    /** Returns the i-th orbital element. */
    OrbitalElement element(unsigned int i) const;

    /** Encodes the given satellites. */
    virtual bool encode(Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits for the element. */
    struct Limit: public Element::Limit {
      /** Maximum number of orbital elements. */
      static constexpr unsigned int elements() { return 25; }
    };

  protected:
    /** Some internal offset. */
    struct Offset: public Element::Offset
    {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int elements() { return 0x0008; }
      static constexpr unsigned int betweenElements() { return OrbitalElement::size(); }
      /// @endcond
    };
  };


  /** Implements digital contacts in OpenGD77 codeplugs. */
  class ContactElement: public Element
  {
  public:
    enum class TimeSlotOverride {
      None, TS1, TS2
    };

  protected:
    /** Hidden constructor. */
    ContactElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ContactElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ContactElement();

    /** The size of the contact element. */
    static constexpr unsigned int size() { return 0x0018; }

    /** Resets the contact. */
    void clear();

    /** Returns @c true if the contact is valid. */
    bool isValid() const;

    /** Returns the name of the contact. */
    virtual QString name() const;
    /** Sets the name of the contact. */
    virtual void setName(const QString name);

    /** Returns the DMR number of the contact. */
    virtual unsigned number() const;
    /** Sets the DMR number of the contact. */
    virtual void setNumber(unsigned id);

    /** Returns the call type. */
    virtual DMRContact::Type type() const;
    /** Sets the call type. */
    virtual void setType(DMRContact::Type type);

    /** Returns the time slot override of the contact. */
    virtual TimeSlotOverride timeSlotOverride() const;
    /** Sets the time slot override. */
    virtual void setTimeSlotOverride(TimeSlotOverride ts);

    /** Constructs a @c DigitalContact instance from this codeplug contact. */
    virtual DMRContact *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Resets this codeplug contact from the given @c DigitalContact. */
    virtual bool encode(const DMRContact *obj, Context &ctx, const ErrorStack &err=ErrorStack());

  public:
    /** Some limits for the contact. */
    struct Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int number() { return 0x0010; }
      static constexpr unsigned int type() { return 0x0014; }
      static constexpr unsigned int timeSlotOverride() { return 0x0017; }
      /// @endcond
    };
  };


  /** Encodes the contact bank. */
  class ContactBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    ContactBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    ContactBankElement(uint8_t *ptr);

    /** The size of the element. */
    static constexpr unsigned int size() {
      return Limit::contactCount() * ContactElement::size();
    }

    /** Resets the contact. */
    void clear();

    /** Returns the i-th contact element. */
    ContactElement contact(unsigned int idx) const;

    /** Encodes all DMR contacts. */
    virtual bool encode(Context &ctx, const ErrorStack &err = ErrorStack());
    /** Decodes all DMR contacts. */
    virtual bool decode(Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits for the element. */
    struct Limit {
      /** Maximum number of contacts. */
      static constexpr unsigned int contactCount() { return 1024; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int contacts() { return 0x0000; }
      static constexpr unsigned int betweenContacts() { return ContactElement::size(); }
      // @endcond
    };
  };


  /** Encodes a group list for all OpenGD77 codeplugs. */
  class GroupListElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0050; }

    /** Clears the group list. */
    void clear();

    /** Returns the name of the group list. */
    virtual QString name() const;
    /** Sets the name of the group list. */
    virtual void setName(const QString &name);

    /** Returns @c true, if the i-th contact is set. */
    virtual bool hasContactIndex(unsigned int i) const;
    /** Returns the i-th contact index. */
    virtual unsigned int contactIndex(unsigned int i) const;
    /** Sets the i-th contact index. */
    virtual void setContactIndex(unsigned int i, unsigned int contactIdx);
    /** Clears the i-th contact index. */
    virtual void clearContactIndex(unsigned int i);

    /** Encodes group list element. */
    virtual bool encode(RXGroupList *lst, Context &ctx, const ErrorStack &err=ErrorStack());
    /** Decodes group list element. */
    virtual RXGroupList *decode(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links the group list element. */
    virtual bool link(RXGroupList *lst, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  public:
    /** Some limits for the element. */
    struct Limit {
      /** Maximum name length. */
      static constexpr unsigned int nameLength() { return 15; }
      /** Maximum number of contacts. */
      static constexpr unsigned int contactCount() { return 32; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int name() { return 0x0000; }
      static constexpr unsigned int contacts() { return 0x0010; }
      static constexpr unsigned int betweenContacts() { return 0x0002; }
      // @endcond
    };
  };


  /** Encodes a group list bank for all OpenGD77 codeplugs. */
  class GroupListBankElement: public Element
  {
  protected:
    /** Hidden constructor. */
    GroupListBankElement(uint8_t *ptr, size_t size);

  public:
    /** Constructor. */
    GroupListBankElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x1840; }

    /** Clears the group list bank. */
    void clear();

    /** Returns @c true, if the i-th group list is encoded. */
    virtual bool hasGroupList(unsigned int i) const;
    /** Returns the number of contacts in the given group list. */
    virtual unsigned int groupListContactCount(unsigned int i) const;
    /** Sets the number of contacts in the given group list. */
    virtual void setGroupListContactCount(unsigned int i, unsigned int count);
    /** Returns the i-th group list. */
    virtual GroupListElement groupList(unsigned int i) const;
    /** Clears the i-th group list. */
    virtual void clearGroupList(unsigned int i);

    /** Encodes all group lists. */
    virtual bool encode(Context &ctx, const ErrorStack &err = ErrorStack());
    /** Decodes all group lists. */
    virtual bool decode(Context &ctx, const ErrorStack &err = ErrorStack());
    /** Links all group lists. */
    virtual bool link(Context &ctx, const ErrorStack &err = ErrorStack());

  public:
    /** Some limits for the element. */
    struct Limit {
      /** Maximum number of group lists. */
      static constexpr unsigned int groupListCount() { return 76; }
    };

  protected:
    /** Some internal offsets within the element. */
    struct Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int length() { return 0x0000; }
      static constexpr unsigned int groupLists() { return 0x0080; }
      static constexpr unsigned int betweenGroupLists() { return GroupListElement::size(); }
      // @endcond
    };
  };


public:
  explicit OpenGD77BaseCodeplug(QObject *parent = nullptr);

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  bool index(Config *config, Context &ctx, const ErrorStack &err=ErrorStack()) const;

  bool decode(Config *config, const ErrorStack &err=ErrorStack());
  bool postprocess(Config *config, const ErrorStack &err=ErrorStack()) const;

  Config *preprocess(Config *config, const ErrorStack &err=ErrorStack()) const;
  bool encode(Config *config, const Flags &flags = Flags(), const ErrorStack &err=ErrorStack());

public:
  /** Decodes the binary codeplug and stores its content in the given generic configuration using
   * the given context. */
  virtual bool decodeElements(Context &ctx, const ErrorStack &err=ErrorStack());
  /** Encodes the given generic configuration as a binary codeplug using the given context. */
  virtual bool encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());

  /** Clears the general settings in the codeplug. */
  virtual void clearGeneralSettings() = 0;
  /** Updates the general settings from the given configuration. */
  virtual bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Updates the given configuration from the general settings. */
  virtual bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears the DTMF settings. */
  virtual void clearDTMFSettings() = 0;
  /** Encodes DTMF settings. */
  virtual bool encodeDTMFSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Decodes the DTMF settings. */
  virtual bool decodeDTMFSettings(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears the APRS settings. */
  virtual void clearAPRSSettings() = 0;
  /** Encodes APRS settings. */
  virtual bool encodeAPRSSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Decodes the APRS settings. */
  virtual bool decodeAPRSSettings(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links the APRS settings. */
  virtual bool linkAPRSSettings(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all DTMF contacts in the codeplug. */
  virtual void clearDTMFContacts() = 0;
  /** Encodes all DTMF contacts. */
  virtual bool encodeDTMFContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds all DTMF contacts to the configuration. */
  virtual bool createDTMFContacts(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clear all channels. */
  virtual void clearChannels() = 0;
  /** Encode all channels. */
  virtual bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds all defined channels to the configuration. */
  virtual bool createChannels(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all channels. */
  virtual bool linkChannels(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clear boot settings. */
  virtual void clearBootSettings() = 0;
  /** Encodes boot settings. */
  virtual bool encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Decodes the boot settings. */
  virtual bool decodeBootSettings(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears the VFO settings. */
  virtual void clearVFOSettings() = 0;

  /** Clears all zones. */
  virtual void clearZones() = 0;
  /** Encodes zones. */
  virtual bool encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds zones to the configuration. */
  virtual bool createZones(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all zones within the configuration. */
  virtual bool linkZones(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all contacts in the codeplug. */
  virtual void clearContacts() = 0;
  /** Encodes all digital contacts in the configuration into the codeplug. */
  virtual bool encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Adds a digital contact to the configuration for each one in the codeplug. */
  virtual bool createContacts(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;

  /** Clears all group lists. */
  virtual void clearGroupLists() = 0;
  /** Encodes all group lists. */
  virtual bool encodeGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Creates all group lists. */
  virtual bool createGroupLists(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
  /** Links all group lists. */
  virtual bool linkGroupLists(Context &ctx, const ErrorStack &err=ErrorStack()) = 0;
};

#endif // OPENGD77BASE_CODEPLUG_HH
