/** @defgroup anytone_writeall "Write everything" for AnyTone radios (gsquared3d fork, D-113)
 *
 * The AnyTone erases a whole flash page (16 KB on the D878UVII) when any part of it is
 * written, and then holds only what was sent in that session (W-04 §4b). qdmr's own write
 * leaves live data unsent in some pages it writes (W-04 §4c); the vendor CPS never does.
 *
 * This write therefore copies the CPS:
 *  1. Read every page the CPS writes for this model, whole (the model's write map, learned
 *     from a USB capture of the CPS writing the radio; D-114).
 *  2. Plan the edit on that copy with the preserving planner (only the bits the edit changes).
 *     A page the edit touches that the CPS doesn't write is read whole too.
 *  3. Send, for every page read: every block the CPS sends, and every other block that holds
 *     data (not all 0xFF). In ascending address order, like the CPS.
 *  4. The radio's expected content of those pages afterwards is saved, so a second read after
 *     the radio restarts can be compared byte for byte (the in-session read-back shows old
 *     bytes, W-04 §4a).
 *
 * Every byte sent is the radio's own except the edit's. Nothing depends on qdmr's idea of
 * which areas exist.
 *
 * @ingroup anytone */
#ifndef ANYTONE_WRITEALL_HH
#define ANYTONE_WRITEALL_HH

#include <QList>
#include <QPair>
#include <QString>
#include "anytone_preserve.hh"

class AnytoneCodeplug;

class AnytoneWriteAll
{
public:
  typedef AnytonePreservingWrite::Memory Memory;
  typedef AnytonePreservingWrite::Reader Reader;
  typedef QList<QPair<unsigned, unsigned>> Ranges;   ///< [start, end) address ranges

  /** A model's write map: what the vendor CPS writes, and the flash page size. */
  struct Map {
    QString model;       ///< model name, for messages
    unsigned pageSize;   ///< flash erase size in bytes (proven on the radio)
    Ranges cps;          ///< every block the CPS writes, as ranges
    QString source;      ///< where the map came from
  };

  /** Result of planning a write-everything. */
  struct Result {
    QList<unsigned> pages;                 ///< page start addresses that will be written, sorted
    Memory image;                          ///< the radio's content of those pages, as read
    Memory send;                           ///< blocks to send, by address
    Memory expected;                       ///< every block of those pages after the write
    AnytonePreservingWrite::Plan plan;     ///< the edit (changed blocks only)
    AnytonePreservingWrite::Stats planStats;
    unsigned blocksRead = 0;               ///< blocks read for the pages
    unsigned blocksChanged = 0;            ///< blocks the edit changes
    unsigned bytesChanged = 0;             ///< bytes the edit changes
    unsigned blocksSentNotCps = 0;         ///< sent blocks the CPS would not send (data or edit)
    unsigned pagesAdded = 0;               ///< pages the edit touches outside the CPS's pages
  };

  /** The write map for this codeplug's model, or @c nullptr if there is none yet (a model
   * gets one only after a CPS capture; INV-020). */
  static const Map *forCodeplug(const AnytoneCodeplug &cp);

  /** Page start addresses the map's CPS ranges fall in. */
  static QList<unsigned> pages(const Map &map);

  /** Reads the pages through @c read, plans @c edited (already pre-processed) on them and
   * builds the blocks to send. Nothing is written.
   * @param progress Called with 0..100 while reading (may be empty). */
  static bool build(AnytoneCodeplug &cp, const Map &map, const Reader &read, Config *edited,
                    const Codeplug::Flags &flags, Result &res, const QString &currentYAML,
                    const std::function<void(int)> &progress, const ErrorStack &err=ErrorStack());

  /** Writes the pages as a ranges file ("START END" in hex per line), the format
   * "dmrconf read-addr" reads, so the radio can be read back and compared. */
  static bool writeRangesFile(const QString &filename, const QList<unsigned> &pages,
                              unsigned pageSize, const QString &header, const ErrorStack &err=ErrorStack());

  /** True if all 16 bytes are 0xFF (erased flash). */
  static bool blank(const QByteArray &block);
};

#endif // ANYTONE_WRITEALL_HH
