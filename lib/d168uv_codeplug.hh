#ifndef D168UV_CODEPLUG_HH
#define D168UV_CODEPLUG_HH

#include <QDateTime>

#include "d878uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv_codeplug.hh"

class Channel;
class DMRContact;
class Zone;
class RXGroupList;
class ScanList;
class DMRAPRSSystem;


/** Represents the device specific binary codeplug for Anytone AT-D878UVII radios.
 *
 * This class only implements the difference to the AT-D878UV codeplug. In fact there is only a
 * difference in the address of the contact ID<->Index map.
 *
 * @ingroup d878uv2 */
class D168UVCodeplug : public D878UVCodeplug
{
  Q_OBJECT

public:
  /** Impleemnts a channel for the AnyTone AT-D168UV. */
  class ChannelElement: public D878UVCodeplug::ChannelElement
  {
  public:
    /** Constructor. */
    ChannelElement(uint8_t *ptr);

    /** Returns @c true if the CRC check for DMR channels is disabled. */
    virtual bool dmrCRCDisabled() const;
    /** Disables DMR CRC check. */
    virtual void disableDMRCRC(bool disable);

    /** Constructs a Channel object from this element. */
    Channel *toChannelObj(Context &ctx) const override;
    /** Encodes the given channel object. */
    bool fromChannelObj(const Channel *c, Context &ctx) override;

  protected:
    /** Some internal offsets. */
    struct Offset: D878UVCodeplug::ChannelElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr Bit disableDMRCRC() { return {34, 3}; }
      /// @endcond
    };
  };


  /** Represents the general config of the radio within the D168UV binary codeplug.
   * This covers the CPS version 1.07. */
  class GeneralSettingsElement: public D878UVCodeplug::GeneralSettingsElement
  {
  protected:
  protected:
    /** Device specific key functions. */
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
        ChannelType = 0x22, Ranging = 0x23, Roaming = 0x24, ChannelRanging = 0x25,
        MaxVolume = 0x26, Slot = 0x27, APRSType = 0x28, Zone = 0x29, RoamingSet = 0x2a,
        APRSSet = 0x2b, Mute=0x2c, CtcssDcsSet=0x2d, TBSTSend = 0x2e, Bluetooth = 0x2f,
        GPS = 0x30, ChannelName = 0x31, CDTScan = 0x32, APRSSend = 0x33, DIMShut = 0x34,
        GPSRoaming = 0x35, WXAlarm = 0x36, Squelch = 0x37, ChannelSettings = 0x38,
        SatPredict = 0x39
      } KeyFunctionCode;
    };

    /** Device specific time zones. */
    struct TimeZone {
    public:
      /** Encodes time zone. */
      static uint8_t encode(const QTimeZone& zone);
      /** Decodes time zone. */
      static QTimeZone decode(uint8_t code);

    protected:
      /** Vector of possible time-zones. */
      static QVector<QTimeZone> _timeZones;
    };

  public:
    /** Constructor. */
    explicit GeneralSettingsElement(uint8_t *ptr);

    QTimeZone gpsTimeZone() const override;
    void setGPSTimeZone(const QTimeZone &zone) override;

    // These function keys are not present
    AnytoneKeySettingsExtension::KeyFunction funcKey1Short() const override;
    void setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKey2Short() const override;
    void setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKeyCShort() const override;
    AnytoneKeySettingsExtension::KeyFunction funcKey1Long() const override;
    void setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKey2Long() const override;
    void setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func) override;
    void setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKeyCLong() const override;
    void setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) override;

    AnytoneKeySettingsExtension::KeyFunction funcKeyALong() const override;
    void setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func) override;
    AnytoneKeySettingsExtension::KeyFunction funcKeyBLong() const override;
    void setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func) override;

    /** Returns the function for the very-long press of side key 1. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKeyAVeryLong() const;
    /** Sets the function for the very-long press of side key 1. */
    virtual void setFuncKeyAVeryLong(AnytoneKeySettingsExtension::KeyFunction func);
    /** Returns the function for the very-long press of side key 2. */
    virtual AnytoneKeySettingsExtension::KeyFunction funcKeyBVeryLong() const;
    /** Sets the function for the very-long press of side key 2. */
    virtual void setFuncKeyBVeryLong(AnytoneKeySettingsExtension::KeyFunction func);

    bool fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err) override;
    bool updateConfig(Context &ctx, const ErrorStack &err) override;

  protected:
    struct Offset: D878UVCodeplug::GeneralSettingsElement::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int progFuncKeyALong()     { return 0x0012; }
      static constexpr unsigned int progFuncKeyBLong()     { return 0x0013; }
      static constexpr unsigned int progFuncKeyAVeryLong() { return 0x0041; }
      static constexpr unsigned int progFuncKeyBVeryLong() { return 0x0042; }
      /// @endcond
    };
  };

  /** Represents the extended settings within the D168UV codeplug.
   * This covers the CPS version 1.07. */
  class ExtendedSettingsElement: public D878UVCodeplug::ExtendedSettingsElement
  {
  protected:
    /** Encoding of possible talker-alias types. That is the text being sent as talker alias.*/
    enum class TalkerAliasType {
      RadioName = 0, CustomText = 1
    };

  public:
    /** Constructor. */
    ExtendedSettingsElement(uint8_t *ptr);

    // Size changed
    static constexpr unsigned int size() { return 0x100; }

    bool fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) override;

  protected:
    struct Offset: D878UVCodeplug::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int talkerAliasType() { return 0x0001; }
      /// @endcond
    };
  };

  /** Represents a set of up to 8 fixed location names. The locations are stored in the
   * APRSSettingsElement. */
  class FixedLocationNamesElement: public Element
  {
  public:
    /** Constructor. */
    explicit FixedLocationNamesElement(uint8_t *ptr);

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x0090; }

    void clear() override;

    /** Returns the n-th name. */
    virtual QString name(unsigned int n) const;
    /** Sets the n-th name. */
    virtual void setName(unsigned int n, const QString &name);

  public:
    /** Some limits. */
    struct Limit: public D878UVCodeplug::Limit {
      /// Number of names.
      static constexpr unsigned int nameCount() { return 8; }
      static constexpr unsigned int nameLength() { return 16; }
    };

  protected:
    /** Some internal offsets. */
    struct Offset: D878UVCodeplug::Offset {
      /// @cond DO_NOT_DOCUMENT
      static constexpr unsigned int names() { return 0x0003; }
      static constexpr unsigned int betweenNames() { return Limit::nameLength() + 1; }
      /// @endcond
    };
  };

protected:
  /** Hidden constructor. */
  explicit D168UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D168UVCodeplug(QObject *parent = nullptr);

protected:
  void allocateUpdated() override;

  bool encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) override;
  bool createChannels(Context &ctx, const ErrorStack &err) override;
  bool linkChannels(Context &ctx, const ErrorStack &err) override;

  bool encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err=ErrorStack()) override;
  bool decodeGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack()) override;
  bool linkGeneralSettings(Context &ctx, const ErrorStack &err=ErrorStack()) override;

protected:
  /** Internal used offsets within the codeplug. */
  struct Offset: public D878UVCodeplug::Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int fixedLocationNames() { return 0x02504800; }
    static constexpr unsigned int unknown_25c2000()    { return 0x025c2000; }
    static constexpr unsigned int unknown_25c2c80()    { return 0x025c2c80; }
    /// @endcond
  };
};

#endif // D168UV_CODEPLUG_HH
