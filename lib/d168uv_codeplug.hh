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
    GeneralSettingsElement(uint8_t *ptr);

    QTimeZone gpsTimeZone() const override;
    void setGPSTimeZone(const QTimeZone &zone) override;
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
