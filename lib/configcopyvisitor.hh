#ifndef CONFIGCOPYVISITOR_HH
#define CONFIGCOPYVISITOR_HH

#include "visitor.hh"

class ConfigObject;
class Channel;

/** This visitor traverses the the given configuration and clones it. All references are still
 *  pointing to the originals.
 * @ingroup conf */
class ConfigCloneVisitor : public Visitor
{
public:
  /** Constructor.
   * @param map Specifies the mapping table to be filled with the created clones. */
  ConfigCloneVisitor(QHash<ConfigObject *, ConfigObject*> &map);

  bool processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  bool processItem(ConfigItem *item, const ErrorStack &err=ErrorStack());
  bool processList(AbstractConfigObjectList *list, const ErrorStack &err=ErrorStack());

  /** Specialized handler for channels, must traverse the channel object. */
  virtual bool processChannel(Channel *item, const ErrorStack &err=ErrorStack());

  /** Extracts the cloned item. */
  ConfigItem *takeResult(const ErrorStack &err=ErrorStack());

protected:
  /** Stack of the current object. */
  QList<QObject *> _stack;
  /** Reference to the translation table origial -> cloned object. */
  QHash<ConfigObject *, ConfigObject*> &_map;
};


/** Replaces references using a specified map.
 * This can be considered a second step in copying an entire codeplug, first all objects are cloned
 * and in a second step, all references are fixed using this class.
 * @ingroup conf */
class FixReferencesVisistor: public Visitor
{
public:
  /** Constructor. */
  FixReferencesVisistor(QHash<ConfigObject *, ConfigObject*> &map, bool keepUnknown=false);

  bool processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  bool processList(AbstractConfigObjectList *list, const ErrorStack &err=ErrorStack());

protected:
  /** Reference to the translation table origial -> cloned object. */
  QHash<ConfigObject *, ConfigObject*> &_map;
  /** If false, an unmapped reference is an error. */
  bool _keepUnknown;
};

/** Just a name space to hold the copy function. */
class ConfigCopy {
public:
  /** Copies the given item. */
  static ConfigItem *copy(ConfigItem *original, const ErrorStack &err=ErrorStack());
};

#endif // CONFIGCOPYVISITOR_HH
