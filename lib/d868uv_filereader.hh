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

    uint16_t index() const;
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
    uint8_t numChannels() const;
    uint16_t channel(uint8_t index) const;
    QString name() const;

    size_t size() const;

  protected:
    uint8_t _numChannels;
    size_t _nameLength;
  };


public:
  D868UVFileReader(Config *config, const uint8_t *data, size_t size, QString &message);

  bool readHeader();
  bool readChannels();
  virtual bool readChannel();
  bool readRadioIDs();
  virtual bool readRadioID();
  bool readZones();
  virtual bool readZone();
  bool readScanLists();
  virtual bool readScanList();
};

#endif // D868UVFILEREADER_HH
