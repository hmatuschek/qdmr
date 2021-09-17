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
class AnytoneCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Represents the base class for channel encodings in all AnyTone codeplugs.
   *
   * Memmory layout of encoded channel (64byte):
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
