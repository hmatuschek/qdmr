#ifndef CONFIGLABELINGVISITOR_HH
#define CONFIGLABELINGVISITOR_HH

#include "visitor.hh"
#include "configobject.hh"

/** A visitor to label the entire configuration.
 * That is, assigning unique labels to each @c ConfigObject within the configuration.
 * @ingroup conf */
class ConfigLabelingVisitor: protected Visitor
{
public:
  /** Use the static method @c label to label the configuration. */
  ConfigLabelingVisitor(ConfigItem::Context &context);

  bool processItem(ConfigItem *item, const ErrorStack &err=ErrorStack());

  /** Labels the configuration and stores the labels in the given context. */
  static bool label(Config *config, ConfigItem::Context &context);

protected:
  /** Holds a weak reference to the parser/serializer context.
   * That is, the id<->obj table. */
  ConfigItem::Context &_context;
};

#endif // CONFIGLABELINGVISITOR_HH
