/** @defgroup ir Intermediate representation.
 * This module collects visitors applied to a codeplug before the actual encoding of the device
 * specific binary codeplug. This preprocessing step eases the encoding a lot.
 *
 * @ingroup config */

#ifndef INTERMEDIATEREPRESENTATION_HH
#define INTERMEDIATEREPRESENTATION_HH

#include "visitor.hh"
#include <QList>

class Zone;
class EncryptionKey;


/** Simple visitor that splits Zones having A and B channels into two zones with A-lists only.
 * This is a pre-processing step for many radios, where zones consists of a single list of channels
 * and a zone is selected for each VFO separately.
 * @ingroup ir */
class ZoneSplitVisitor: public Visitor
{
public:
  /** Constructor. */
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
  /** Constructor. */
  explicit ZoneMergeVisitor();

  bool processList(AbstractConfigObjectList *list, const ErrorStack &err);
  bool processItem(ConfigItem *item, const ErrorStack &err);

protected:
  /** The last zone visited, @c nullptr if the first zone is processed. */
  Zone *_lastZone;
  /** Zones to be removed. */
  QList<Zone *> _mergedZones;
};



/** Filters instances by a test function.
 *
 * This visitor can be used to remove elements from the abstract codeplug, not supported by the
 * target device. Use @c ObjectFilterVisitor to remove instances of specific classes.
 */
class AbstractObjectFilterVisitor: public Visitor
{
protected:
  /** Hidden constructor. */
  explicit AbstractObjectFilterVisitor();

public:
  bool processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err);
  bool processList(AbstractConfigObjectList *list, const ErrorStack &err);

protected slots:
  /** Abstract test function. If this function returns @c true, the item will be removed. */
  virtual bool toRemove(ConfigItem *item) = 0;
};



/** Filters instance by meta object.
 *
 * This visitor can be used to remove elements from the abstract codeplug, not supported by the
 * target device. */
class ObjectFilterVisitor: public AbstractObjectFilterVisitor
{
public:
  /** Constructor from initializer list of Qt meta objects. */
  explicit ObjectFilterVisitor(const std::initializer_list<QMetaObject> &types);

protected slots:
  bool toRemove(ConfigItem *item);

protected:
  /** The list of filtered types. */
  QList<QMetaObject> _filter;
};


/** Only keeps those encryption keys, that match a specific type and size. */
class EncryptionKeyFilterVisitor: public AbstractObjectFilterVisitor
{
public:
  struct Filter {
    QMetaObject type;
    unsigned int minBits;
    unsigned int maxBits;

    Filter(const QMetaObject &type, unsigned int bits);
    Filter(const QMetaObject &type, unsigned int minBits, unsigned int maxBits);

    bool accepts(const EncryptionKey *key) const;
  };

public:
  EncryptionKeyFilterVisitor(const std::initializer_list<Filter> &filter);

protected:
  bool toRemove(ConfigItem *item);

protected:
  QList<Filter> _filter;
};

#endif // INTERMEDIATEREPRESENTATION_HH
