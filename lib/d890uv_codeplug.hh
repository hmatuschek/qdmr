#ifndef DMRCONF_D890UVCODEPLUG_HH
#define DMRCONF_D890UVCODEPLUG_HH

#include "anytone_codeplug.hh"


/** AnyTone AT-D890UV codeplug. */
class D890UVCodeplug : public AnytoneCodeplug
{
public:
  /** Encodes/decodes a single channel. */
  class ChannelElement: public AnytoneCodeplug::ChannelElement
  {
  public:
    /** Constructor from pointer. */
    ChannelElement(uint8_t *ptr);

    /** Size of the element. */
    static constexpr unsigned int size() { return 0x0080; }


  protected:
    /** Some internal offsets. */
    struct Offset: Element::Offset {
      /// @cond  DO_NOT_DOCUMENT
      static constexpr unsigned int rxFrequency()            { return 0x0000; }
      static constexpr unsigned int txOffset()               { return 0x0004; }
      static constexpr Bit channelMode()                     { return {0x0008, 0}; }
      /// @endcond
    };
  };

protected:
  /** Hidden constructor. */
  D890UVCodeplug(const QString &label, QObject *parent = nullptr);

public:
  /** Default constructor with parent. */
  explicit D890UVCodeplug(QObject *parent=nullptr);


protected:
  /** Internal used offsets within the codeplug. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int channels()           { return 0x01000000; }
    /// @endcond
  };

};


#endif //QDMR_D890UVCODEPLUG_HH
