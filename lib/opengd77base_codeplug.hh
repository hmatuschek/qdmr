#ifndef OPENGD77BASE_CODEPLUG_HH
#define OPENGD77BASE_CODEPLUG_HH

#include "channel.hh"
#include "codeplug.hh"
#include <QGeoCoordinate>


class OpenGD77BaseCodeplug : public Codeplug
{
  Q_OBJECT

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
      None, APRS, Text, Both
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
    virtual uint32_t rxFrequency() const;
    /** Sets the RX frequency of the channel. */
    virtual void setRXFrequency(uint32_t freq);
    /** Returns the TX frequency of the channel. */
    virtual uint32_t txFrequency() const;
    /** Sets the TX frequency of the channel. */
    virtual void setTXFrequency(uint32_t freq);

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

    /** Returns the alias transmitted on time slot 1. */
    virtual Alias aliasTimeSlot1() const;
    /** Sets the alias transmitted on time slot 1. */
    virtual void setAliasTimeSlot1(Alias alias);
    /** Returns the alias transmitted on time slot 2. */
    virtual Alias aliasTimeSlot2() const;
    /** Sets the alias transmitted on time slot 2. */
    virtual void setAliasTimeSlot2(Alias alias);

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
    virtual Channel *toChannelObj(Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Links a previously constructed channel to the rest of the configuration. */
    virtual bool linkChannelObj(Channel *c, Context &ctx, const ErrorStack &err=ErrorStack()) const;
    /** Initializes this codeplug channel from the given generic configuration. */
    virtual bool fromChannelObj(const Channel *c, Context &ctx, const ErrorStack &err=ErrorStack());

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
      static constexpr Bit simplex() { return {0x0026, 3}; }
      static constexpr Bit useFixedLocation() { return {0x0026, 3}; }
      static constexpr Bit disablePowerSave() { return {0x0026, 5}; }
      static constexpr Bit disableBeep() { return {0x0026, 6}; }
      static constexpr Bit overrideDMRID() { return {0x0026, 7}; }
      static constexpr unsigned int dmrId() { return 0x0027; }
      static constexpr unsigned int groupList() { return 0x002b; }
      static constexpr unsigned int colorCode() { return 0x002c; }
      static constexpr unsigned int aprsIndex() { return 0x002d; }
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

public:
  explicit OpenGD77BaseCodeplug(QObject *parent = nullptr);


};

#endif // OPENGD77BASE_CODEPLUG_HH
