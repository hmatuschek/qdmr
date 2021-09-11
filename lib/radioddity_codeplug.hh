#ifndef RADIODDITYCODEPLUG_HH
#define RADIODDITYCODEPLUG_HH

#include "codeplug.hh"
#include "signaling.hh"
#include "channel.hh"
#include "contact.hh"

class DigitalContact;
class Zone;
class RXGroupList;
class ScanList;


/** Base class of all Radioddity codeplugs. This class implements the majority of all codeplug
 * elements present in all Radioddity codeplugs (also some derivatives like OpenGD77). This eases
 * the support of several Radioddity radios, as only the differences in the codeplug to this base
 * class must be implemented.
 *
 * @ingroup radioddity */
class RadioddityCodeplug : public CodePlug
{
  Q_OBJECT

public:
  /** Implements the base for all Radioddity channel encodings.
   *
   * Memory layout of encoded channel:
   * @verbinclude radioddity_channel.txt */
  class ChannelElement: public CodePlug::Element
  {
  protected:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr, size_t size);

  public:
    /** Constructs a channel from the given memory. */
    ChannelElement(uint8_t *ptr);
    /** Destructor. */
    virtual ~ChannelElement();
  };


protected:
  /** Hidden constructor, use a device specific class to instantiate. */
  explicit RadioddityCodeplug(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~RadioddityCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear();

  /** Decodes the binary codeplug and stores its content in the given generic configuration. */
  bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config, const Flags &flags = Flags());

public:

};

#endif // RADIODDITYCODEPLUG_HH
