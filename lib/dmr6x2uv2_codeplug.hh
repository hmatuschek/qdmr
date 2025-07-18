#ifndef DMR6X2UV2CODEPLUG_HH
#define DMR6X2UV2CODEPLUG_HH

#include "dmr6x2uv_codeplug.hh"
#include "ranges.hh"


/** Represents the device specific binary codeplug for BTECH DMR-6X2UV PRO radios.
 *
 * This codeplug implementation is compatible with firmware revision 1.21.
 *
 * For details, see https://dmr-tools.github.io/codeplugs
 *
 * @ingroup dmr6x2uv */
class DMR6X2UV2Codeplug : public DMR6X2UVCodeplug
{
  Q_OBJECT

protected:
public:
  /** General settings element for the DMR-6X2UV PRO.
   *
   * Extends the @c DMR6X2UVCodeplug::GeneralSettingsElement by the device specific settings for
   * the BTECH DMR-6X2UV PRO. */
  class GeneralSettingsElement: public DMR6X2UVCodeplug::GeneralSettingsElement
  {
  protected:
    /** Device specific encoding of the key functions. */
    struct KeyFunction {
    public:
      /** Encodes key function. */
      static uint8_t encode(AnytoneKeySettingsExtension::KeyFunction tone);
      /** Decodes key function. */
      static AnytoneKeySettingsExtension::KeyFunction decode(uint8_t code);

    protected:
      /** Device specific key functions. */
      typedef enum {
        Off = 0x00, Voltage = 0x01, Power = 0x02, Repeater = 0x03, Reverse = 0x04,
        Encryption = 0x05, Call = 0x06, VOX = 0x07, ToggleVFO = 0x08, SubPTT = 0x09,
        Scan = 0x0a, WFM = 0x0b, Alarm = 0x0c, RecordSwitch = 0x0d, Record = 0x0e, SMS = 0x0f,
        Dial = 0x10, GPSInformation = 0x11, Monitor = 0x12, ToggleMainChannel = 0x13, HotKey1 = 0x14,
        HotKey2 = 0x15, HotKey3 = 0x16, HotKey4 = 0x17, HotKey5 = 0x18, HotKey6 = 0x19,
        WorkAlone = 0x1a, SkipChannel = 0x1b, DMRMonitor = 0x1c, SubChannel = 0x1d,
        PriorityZone = 0x1e, VFOScan = 0x1f, MICSoundQuality = 0x20, LastCallReply = 0x21,
        ChannelType = 0x22, SimplexRepeater = 0x23, Ranging = 0x24, ChannelRanging = 0x25,
        MaxVolume = 0x26, Slot = 0x27, Squelch = 0x28, Roaming = 0x29, Zone = 0x2a, RoamingSet = 0x2b,
        Mute = 0x02c, CtcssDcsSet=0x2d, APRSType = 0x2e, APRSSet = 0x2f, TBSTSend = 0x30,
        BluetoothToggle = 0x31, GPSToggle = 0x32, ChannelName = 0x33, APRSSend = 0x34,
        APRSInfo = 0x35, GPSRoaming = 0x36, CTCSSScan = 0x37, DIMShut = 0x38, SatellitePredict = 0x39
      } KeyFunctionCode;
    };

  protected:
    /** Hidden Constructor. */
    GeneralSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x00e0; }

    AnytoneKeySettingsExtension::KeyFunction funcKeyAShort() const;
    void setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBShort() const;
    void setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCShort() const;
    void setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey1Short() const;
    void setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Short() const;
    void setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyALong() const;
    void setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyBLong() const;
    void setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKeyCLong() const;
    void setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey1Long() const;
    void setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func);
    AnytoneKeySettingsExtension::KeyFunction funcKey2Long() const;
    void setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func);

    bool fromConfig(const Flags &flags, Context &ctx);
    bool updateConfig(Context &ctx);

  protected:
    /** Some internal used offsets within the element. */
    struct Offset: public D868UVCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      /// @endcond
    };

  };

  /** Implements some settings extension for the BTECH DMR-6X2UV PRO. */
  class ExtendedSettingsElement: public DMR6X2UVCodeplug::ExtendedSettingsElement
  {
  protected:
    /** Hidden Constructor. */
    ExtendedSettingsElement(uint8_t *ptr, unsigned size);

  public:
    /** Constructor. */
    explicit ExtendedSettingsElement(uint8_t *ptr);

    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0030; }

    /** Returns @c true, if bluetooth is enabled. */
    virtual bool bluetoothEnabled() const;
    /** Enables/disables bluetooth. */
    virtual void enableBluetooth(bool enable);

    /** Returns @c true, if the internal mic is enabled additionally to the bluetooth input. */
    virtual bool internalMicEnabled() const;
    /** Enables/disables the internal mic additionally to the bluetooth input. */
    virtual void enableInternalMic(bool enable);

    /** Returns @c true, if the internal speaker is enabled additionally to the bluetooth output. */
    virtual bool internalSpeakerEnabled() const;
    /** Enables/disables the internal speaker additionally to the bluetooth output. */
    virtual void enableInternalSpeaker(bool enable);

    /** Returns the bluetooth mic gain. Valid values are 0,...,4. */
    virtual unsigned int bluetoothMicGain() const;
    /** Sets the bluetooth mic gain. Valid values are 0,...,4. */
    virtual void setBluetoothMicGain(unsigned int gain);

    /** Returns the bluetooth speaker gain. Valid values are 0,...,4. */
    virtual unsigned int bluetoothSpeakerGain() const;
    /** Sets the bluetooth speaker gain. Valid values are 0,...,4. */
    virtual void setBluetoothSpeakerGain(unsigned int gain);

    /** Returns the hold duration. */
    Interval bluetoothHoldDuration() const;
    /** Sets the hold duration. */
    void setBluetoothHoldDuration(const Interval &dur);

    /** Returns the hold delay. */
    Interval bluetoothHoldDelay() const;
    /** Sets the hold duration. */
    void setBluetoothHoldDelay(const Interval &dur);

    /** Returns @c true, if PTT latches. */
    bool bluetoothPTTLatchEnabled() const;
    /** Enable/disable bluetooth PTT latch. */
    void enableBluetoothPTTLatch(bool enable);

    /** Returns the bluetooth PTT sleep timeout. */
    Interval bluetoothPTTSleepTimeout() const;
    /** Sets the bluetooth PTT sleep timeout. */
    void setBluetoothPTTSleepTimeout(const Interval &dur);

    bool fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
    bool updateConfig(Context &ctx, const ErrorStack &err=ErrorStack());
    bool linkConfig(Context &ctx, const ErrorStack &err=ErrorStack());

  protected:
    /** Some internal offset within the codeplug element. */
    struct Offset {
      /// @cond DO_NOT_DOCUEMNT
      static constexpr unsigned int bluetoothEnable()            { return 0x0016; }
      static constexpr unsigned int internalMicEnable()          { return 0x0017; }
      static constexpr unsigned int internalSpeakerEnable()      { return 0x0018; }
      static constexpr unsigned int bluetoothMicGain()           { return 0x0019; }
      static constexpr unsigned int bluetoothSpeakerGain()       { return 0x001a; }
      static constexpr unsigned int bluetoothHoldDuration()      { return 0x001b; }
      static constexpr unsigned int bluetoothHoldDelay()         { return 0x001c; }
      static constexpr unsigned int bluetoothPTTLatch()          { return 0x001d; }
      static constexpr unsigned int bluetoothPTTSleepTimeout()   { return 0x001e; }
      /// @endcond
    };
  };

public:
  /** Hidden constructor. */
  explicit DMR6X2UV2Codeplug(const QString &label, QObject *parent=nullptr);

public:
  /** Empty constructor. */
  explicit DMR6X2UV2Codeplug(QObject *parent=nullptr);

protected:
  void allocateGeneralSettings();
  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack());
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());
  bool linkGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // DMR6X2UV2CODEPLUG_HH
