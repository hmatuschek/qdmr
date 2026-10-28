#ifndef ANYTONECODEPLUGBASE_HH
#define ANYTONECODEPLUGBASE_HH

#include "codeplug.hh"


/** Common interface of all AnyTone codeplug generations.
 *
 * This is the part of the codeplug @c AnytoneRadio needs to transfer it to and from the device:
 * AnyTone codeplugs are heavily segmented and only valid sections are read from and written to
 * the device. The codeplug decides which sections these are. Everything else, i.e., the actual
 * binary format, is left to the generations, see @c AnytoneCodeplugGen1.
 *
 * @ingroup anytone */
class AnytoneCodeplugBase : public Codeplug
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AnytoneCodeplugBase(QObject *parent=nullptr);

public:
  /** Clears and resets the complete codeplug to some default values. */
  virtual void clear() = 0;

protected:
  /** Allocate all code-plug elements that must be written back to the device to maintain a working
   * codeplug. These elements might be updated during encoding. */
  virtual void allocateUpdated() = 0;
  /** Allocate all code-plug elements that must be downloaded for decoding. All code-plug elements
   * within the radio that are not represented within the common Config are omitted. */
  virtual void allocateForDecoding() = 0;

  // Allow the radio to access the allocation methods.
  friend class AnytoneRadio;
};

#endif // ANYTONECODEPLUGBASE_HH
