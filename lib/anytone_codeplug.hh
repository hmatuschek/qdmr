#ifndef ANYTONECODEPLUG_HH
#define ANYTONECODEPLUG_HH

#include "codeplug.hh"

/** Base class interface for all Anytone radio codeplugs.
 * This class extends the generic @c CodePlug to provide an interface to the multi-step up and
 * download of the binary codeplug. In contrast to the majority of radios, the Anytone codeplugs
 * are heavily segmented and only valid sections are read from a written to the device. */
class AnytoneCodeplug : public CodePlug
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AnytoneCodeplug(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~AnytoneCodeplug();

  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear() = 0;

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
