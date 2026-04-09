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

protected:
  /** Hidden constructor. */
  explicit D168UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Empty constructor. */
  explicit D168UVCodeplug(QObject *parent = nullptr);

protected:
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

    /// @endcond
  };
};

#endif // D168UV_CODEPLUG_HH
