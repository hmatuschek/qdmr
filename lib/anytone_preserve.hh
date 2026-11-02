/** @defgroup anytone_preserve Preserving write for AnyTone radios
 *
 * A "preserving" write changes only what an edit actually changes and leaves every other byte
 * in the radio exactly as it was, including settings qdmr cannot represent.
 *
 * Method (gsquared3d fork, research note W-02):
 *  1. Read from the radio everything qdmr would read for an update (bitmaps, then the
 *     update areas) and for a full decode. Together this is @c orig.
 *  2. Decode @c orig into C0 (the radio's current set-up) and pass C0 through YAML exactly like
 *     an edited file is, so YAML round-trip effects cancel out.
 *  3. Encode C0 and the edited set-up C1 the same way, each on top of @c orig (update mode):
 *     E0 and E1.
 *  4. For every bit: take E1 where E1 differs from E0, otherwise keep @c orig. Bitmaps hold eight
 *     channels per byte, so this is done bit by bit, not byte by byte.
 *  5. Write only the 16-byte blocks that end up different from @c orig. A block the radio does
 *     not hold is written only if the edit touches it.
 *
 * The same planner is used for a real write and for an offline dry run on a memory file, so
 * the tests exercise exactly the code that writes the radio.
 *
 * @ingroup anytone */
#ifndef ANYTONE_PRESERVE_HH
#define ANYTONE_PRESERVE_HH

#include <QMap>
#include <QByteArray>
#include <QString>
#include <functional>
#include "codeplug.hh"
#include "errorstack.hh"

class AnytoneCodeplug;
class Config;

/** Plans (and describes) a preserving write for any AnyTone codeplug. No per-model code. */
class AnytonePreservingWrite
{
public:
  /** Size of one radio memory block in bytes. */
  static constexpr unsigned BlockSize = 16;
  /** Radio memory, one 16-byte block per address. */
  typedef QMap<unsigned, QByteArray> Memory;
  /** Reads @c size bytes at @c addr from the radio (or a memory file) into @c dst. */
  typedef std::function<bool(unsigned addr, uint8_t *dst, unsigned size, const ErrorStack &err)> Reader;

  /** One planned block write: what the radio holds now (empty if it holds nothing there) and
   * what will be written. */
  struct Block {
    QByteArray before;
    QByteArray after;
  };
  /** The plan: blocks to write, by address. */
  typedef QMap<unsigned, Block> Plan;

  /** Counters for the summary. */
  struct Stats {
    unsigned blocksRead = 0;       ///< blocks read from the radio (or memory file)
    unsigned blocksConsidered = 0; ///< blocks in the encoded edited set-up (E1)
    unsigned blocksToWrite = 0;    ///< blocks in the plan
    unsigned blocksNew = 0;        ///< planned blocks the radio copy did not hold
    unsigned bytesChanged = 0;     ///< bytes that differ between radio and plan
  };

public:
  /** Reads the radio through @c read, then plans the preserving write of @c edited (already
   * pre-processed with the codeplug's own @c preprocess, as for a normal write).
   * @param cp A codeplug of the radio's type; it is cleared and reused.
   * @param flags The caller's encoding flags; update mode is forced on.
   * @param currentYAML If not empty, the radio's current set-up (C0) is saved there as YAML.
   * @param radioMemory If not null, receives everything read from the radio. */
  static bool plan(AnytoneCodeplug &cp, const Reader &read, Config *edited,
                   const Codeplug::Flags &flags, Plan &plan, Stats &stats,
                   const QString &currentYAML=QString(), Memory *radioMemory=nullptr,
                   const ErrorStack &err=ErrorStack());

  /** Writes the plan as text: one line per block, "address before after" in hex
   * ("--" for a block the radio did not hold). Lines starting with '#' are comments. */
  static bool writePlanFile(const QString &filename, const Plan &plan, const Stats &stats,
                            const QString &header, const ErrorStack &err=ErrorStack());

  /** Reads a radio memory file: one line per block, "address data" in hex (32 hex digits). */
  static bool readMemoryFile(const QString &filename, Memory &mem, const ErrorStack &err=ErrorStack());
  /** Writes a radio memory file in the format @c readMemoryFile reads. */
  static bool writeMemoryFile(const QString &filename, const Memory &mem, const ErrorStack &err=ErrorStack());

  /** A reader that serves blocks from @c mem (for offline dry runs and tests). */
  static Reader memoryReader(const Memory &mem);

protected:
  /** Reads every block of the codeplug's elements not yet in @c mem, and fills the codeplug
   * from @c mem. */
  static bool readMissing(AnytoneCodeplug &cp, const Reader &read, Memory &mem, Stats &stats,
                          const ErrorStack &err);
  /** Copies every block of the codeplug's elements that @c mem holds into the codeplug. */
  static void fill(AnytoneCodeplug &cp, const Memory &mem);
  /** Copies the codeplug's elements into a memory map. */
  static Memory dump(AnytoneCodeplug &cp);
  /** Rebuilds the codeplug in update mode on top of @c mem and encodes @c config into it. */
  static bool encodeOn(AnytoneCodeplug &cp, const Memory &mem, Config *config,
                       const Codeplug::Flags &flags, Memory &out, const ErrorStack &err);
};

#endif // ANYTONE_PRESERVE_HH
