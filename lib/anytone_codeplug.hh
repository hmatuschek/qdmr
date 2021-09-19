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
