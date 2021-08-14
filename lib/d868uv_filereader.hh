#ifndef D868UVFILEREADER_HH
#define D868UVFILEREADER_HH

#include "d868uv_codeplug.hh"
#include "anytone_filereader.hh"

class D868UVFileReader: public AnytoneFileReader
{
public:
  class ChannelElement: public AnytoneFileReader::Element
  {
  public:
    ChannelElement(const uint8_t *ptr);

    uint16_t index() const;
    D868UVCodeplug::channel_t::Mode mode() const;

    double rxFrequency() const;
    double txFrequency() const;
    Channel::Power power() const;
    AnalogChannel::Bandwidth bandwidth() const;
    bool rxOnly() const;
    Signaling::Code rxSignaling() const;
    Signaling::Code txSignaling() const;
    DigitalChannel::Admit admitDigital() const;
    AnalogChannel::Admit admitAnalog() const;
    uint8_t colorCode() const;
    DigitalChannel::TimeSlot timeSlot() const;
    QString name() const;

    size_t size() const;

  protected:
    size_t _nameLength;
  };

  class RadioIDElement: public AnytoneFileReader::Element
  {
  public:
    RadioIDElement(const uint8_t *ptr);

    uint8_t index() const;
    uint32_t id() const;
    QString name() const;

    size_t size() const;

  protected:
    size_t _nameLength;
  };

  class ZoneElement: public AnytoneFileReader::Element
  {
  public:
    ZoneElement(const uint8_t *ptr);

    uint8_t index() const;
    uint16_t channelA() const;
    uint16_t channelB() const;
    uint8_t numChannels() const;
    uint16_t channel(uint8_t index) const;
    QString name() const;

    size_t size() const;

  protected:
    uint8_t _numChannels;
    size_t _nameLength;
  };

  class ScanListElement: public AnytoneFileReader::Element
  {
  public:
    ScanListElement(const uint8_t *ptr);

    uint8_t index() const;
    uint8_t numChannels() const;
    D868UVCodeplug::scanlist_t::PriChannel prioChannelSelect() const;
    uint16_t prioChannel1() const;
    uint16_t prioChannel2() const;
    uint16_t channel(uint8_t index) const;
    QString name() const;

    size_t size() const;

  protected:
    uint8_t _numChannels;
    size_t _nameLength;
  };

  class AnalogContactElement: public AnytoneFileReader::Element
  {
  public:
    AnalogContactElement(const uint8_t *ptr);

    uint8_t index() const;
    QString number() const;
    QString name() const;

    size_t size() const;

  protected:
    uint8_t _numberLength;
    size_t _nameLength;
  };


public:
  D868UVFileReader(Config *config, const uint8_t *data, size_t size, QString &message);

  bool linkHeader();
  bool readHeader();

  bool readChannels();
  virtual bool readChannel();
  bool linkChannels();
  virtual bool linkChannel();

  bool readRadioIDs();
  virtual bool readRadioID();
  bool linkRadioIDs();

  bool readZones();
  virtual bool readZone();
  bool linkZones();

  bool readScanLists();
  virtual bool readScanList();
  bool linkScanLists();

  bool readAnalogContacts();
  virtual bool readAnalogContact();
  bool linkAnalogContacts();
};

#endif // D868UVFILEREADER_HH
