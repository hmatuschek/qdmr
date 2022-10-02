#ifndef D868UVFILEREADER_HH
#define D868UVFILEREADER_HH

#include "d868uv_codeplug.hh"
#include "anytone_filereader.hh"

/** Reads manufacturer CPS files.
 * @warning This is mostly incomplete. */
class D868UVFileReader: public AnytoneFileReader
{
public:
  /** Representsa a channel encoded in the manufacturer CPS file. */
  class ChannelElement: public AnytoneFileReader::Element
  {
  public:
    /** Constructor. */
    ChannelElement(const uint8_t *ptr);
    /** Returns the index of the channel.*/
    uint16_t index() const;
    /** Returns the channel mode (Analog, Digital, Mixed...). */
    D868UVCodeplug::channel_t::Mode mode() const;
    /** Return the RX frequency. */
    double rxFrequency() const;
    /** Return the TX frequency. */
    double txFrequency() const;
    /** Returns the power setting. */
    Channel::Power power() const;
    /** Returns the bandwidth. */
    AnalogChannel::Bandwidth bandwidth() const;
    /** Returns @c true if the channel is RX only. */
    bool rxOnly() const;
    /** Returns the RX CTCSS/DCS settings. */
    Signaling::Code rxSignaling() const;
    /** Returns the TX CTCSS/DCS settings. */
    Signaling::Code txSignaling() const;
    /** Returns the admit cirterion for the channel (if digital). */
    DigitalChannel::Admit admitDigital() const;
    /** Returns the admit cirterion for the channel (if analog). */
    AnalogChannel::Admit admitAnalog() const;
    /** Returns the color code of the channel (if digital). */
    uint8_t colorCode() const;
    /** Returns the time slot of the channel (if digital). */
    DigitalChannel::TimeSlot timeSlot() const;
    /** Returns the name of the channel. */
    QString name() const;

    size_t size() const;

  protected:
    /** The length of the name. */
    size_t _nameLength;
  };

  /** Decodes a radio ID from the manufacturer CPS file. */
  class RadioIDElement: public AnytoneFileReader::Element
  {
  public:
    /** Constructor for radio ID element. */
    RadioIDElement(const uint8_t *ptr);

    /** Returns the index of the ID. */
    uint8_t index() const;
    /** Returns the ID. */
    uint32_t id() const;
    /** Returns the name of the radio ID. */
    QString name() const;

    size_t size() const;

  protected:
    /** Holds the length of the name. */
    size_t _nameLength;
  };

  /** Represents a zone within the manufacturer CPS file. */
  class ZoneElement: public AnytoneFileReader::Element
  {
  public:
    /** Constructor. */
    ZoneElement(const uint8_t *ptr);

    /** Returns the index of the zone. */
    uint8_t index() const;
    /** Returns the number of channels in A. */
    uint16_t channelA() const;
    /** Returns the number of channels in B. */
    uint16_t channelB() const;
    /** Returns the total number of channels in zone. */
    uint8_t numChannels() const;
    /** Returns the channel index for the i-th entry. */
    uint16_t channel(uint8_t index) const;
    /** Returns the name of the zone. */
    QString name() const;

    size_t size() const;

  protected:
    /** Holds the number of channels. */
    uint8_t _numChannels;
    /** Holds the name length. */
    size_t _nameLength;
  };

  /** Represents a scan list in the manufacturer CPS file. */
  class ScanListElement: public AnytoneFileReader::Element
  {
  public:
    /** Constructor. */
    ScanListElement(const uint8_t *ptr);

    /** Returns the index of the scan list. */
    uint8_t index() const;
    /** Returns the number of channels in the scan list. */
    uint8_t numChannels() const;
    /** Returns the priority channel selection. */
    AnytoneCodeplug::ScanListElement::PriChannel prioChannelSelect() const;
    /** Returns the priority channel 1 index. */
    uint16_t prioChannel1() const;
    /** Returns the priority channel 2 index. */
    uint16_t prioChannel2() const;
    /** Returns the i-th channel index. */
    uint16_t channel(uint8_t index) const;
    /** Returns the name of the scan list. */
    QString name() const;

    size_t size() const;

  protected:
    /** Holds the number of channels. */
    uint8_t _numChannels;
    /** Holds the name length. */
    size_t _nameLength;
  };

  /** Represents an analog contact within the manufacturer CPS file. */
  class AnalogContactElement: public AnytoneFileReader::Element
  {
  public:
    /** Constructor. */
    AnalogContactElement(const uint8_t *ptr);

    /** Returns the index of the analog contact. */
    uint8_t index() const;
    /** Returns the number. */
    QString number() const;
    /** Returns the name. */
    QString name() const;

    size_t size() const;

  protected:
    /** Returns the length of the number. */
    uint8_t _numberLength;
    /** Returns the length of the name. */
    size_t _nameLength;
  };


public:
  /** Constructs a file reader for the given manufacturer CPS file (data, size). */
  D868UVFileReader(Config *config, const uint8_t *data, size_t size, QString &message);

  bool linkHeader();
  bool readHeader();

  bool readChannels();
  /** Reads a channel. */
  virtual bool readChannel();
  bool linkChannels();
  /** Links a channel. */
  virtual bool linkChannel();

  bool readRadioIDs();
  /** Reads a radio ID. */
  virtual bool readRadioID();
  bool linkRadioIDs();

  bool readZones();
  /** Reads a zone. */
  virtual bool readZone();
  bool linkZones();

  bool readScanLists();
  /** Reads a scan list. */
  virtual bool readScanList();
  bool linkScanLists();

  bool readAnalogContacts();
  /** Reads an analog contact. */
  virtual bool readAnalogContact();
  bool linkAnalogContacts();
};

#endif // D868UVFILEREADER_HH
