/** @defgroup ir Intermediate representation.
 * This module collects visitors applied to a codeplug before the actual encoding of the device
 * specific binary codeplug. This preprocessing step eases the encoding a lot.
 *
 * @ingroup config */

#ifndef INTERMEDIATEREPRESENTATION_HH
#define INTERMEDIATEREPRESENTATION_HH

#include "visitor.hh"

class Zone;


/** Simple visitor that splits Zones having A and B channels into two zones with A-lists only.
 * This is a pre-processing step for many radios, where zones consists of a single list of channels
 * and a zone is selected for each VFO separately.
 * @ingroup ir */
class ZoneSplitVisitor: public Visitor
{
public:
  explicit ZoneSplitVisitor();

  bool processItem(ConfigItem *item, const ErrorStack &err);
};

/** Simple visitor that merges zones. This is the reverse step of the @c ZoneSplitVisitor.
 * Two subsequent zones are only merged into one, if both zones have empty B lists, the name of the
 * first zone ends on "... A" and the name of the second ends on "... B". That is, if the two zones
 * where likely split by qdmr. */
class ZoneMergeVisitor: public Visitor
{
public:
  explicit ZoneMergeVisitor();

  bool processList(AbstractConfigObjectList *list, const ErrorStack &err);
  bool processItem(ConfigItem *item, const ErrorStack &err);

protected:
  Zone *_lastZone;
  QList<Zone *> _mergedZones;
};

#endif // INTERMEDIATEREPRESENTATION_HH
