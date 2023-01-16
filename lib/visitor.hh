#ifndef VISITOR_HH
#define VISITOR_HH

#include <QObject>
#include "errorstack.hh"

// Forward declarations
class Config;
class ConfigItem;
class ConfigObjectReference;
class AbstractConfigObjectList;


/** Base visitor class for the config tree.
 *
 *  This class can be used to implement a convenient tree taversal for the entrie configuration.
 *
 * @ingroup config */
class Visitor
{
protected:
  /** Hidden constructor. */
  Visitor();

public:
  /** Destructor. */
  virtual ~Visitor();

  /** Traverses the properties of the configuration recursively.
   * @returns @c true on success. Error information can be found in the error stack, if passed. */
  virtual bool process(Config *config, const ErrorStack &err=ErrorStack());

  /** Processes the specified property of the item.
   * This method dispatches to the type-specific methods like @c processEnum, @c processBool,
   * @c processInt, @c processUInt, @c processDouble, @c processString, @c processItem,
   * @c processReference, @c processList, and @c processUnknownType, depending on the type of the
   * property. Do not override this method unless you need to handle all properties differently. */
  virtual bool processProperty(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());

  /** Handles an enum typed property.
   * @param item Specifies the config item holding this property.
   * @param prop Specifies the property.
   * @param err Specifies the error stack to pass on. */
  virtual bool processEnum(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  /** Handles a boolean typed property.
   * @param item Specifies the config item holding this property.
   * @param prop Specifies the property.
   * @param err Specifies the error stack to pass on. */
  virtual bool processBool(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  /** Handles an integer typed property.
   * @param item Specifies the config item holding this property.
   * @param prop Specifies the property.
   * @param err Specifies the error stack to pass on. */
  virtual bool processInt(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  /** Handles an unsigned integer typed property.
   * @param item Specifies the config item holding this property.
   * @param prop Specifies the property.
   * @param err Specifies the error stack to pass on. */
  virtual bool processUInt(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  /** Handles a double precision float typed property.
   * @param item Specifies the config item holding this property.
   * @param prop Specifies the property.
   * @param err Specifies the error stack to pass on. */
  virtual bool processDouble(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  /** Handles a string typed property.
   * @param item Specifies the config item holding this property.
   * @param prop Specifies the property.
   * @param err Specifies the error stack to pass on. */
  virtual bool processString(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());
  /** Handles a property of unknown type.
   * Returns always @c false.
   * @param item Specifies the config item holding this property.
   * @param prop Specifies the property.
   * @param err Specifies the error stack to pass on. */
  virtual bool processUnknownType(ConfigItem *item, const QMetaProperty &prop, const ErrorStack &err=ErrorStack());

  /** Traverses the specified config item.
   * This method calles @c processProperty on all properties of the item. */
  virtual bool processItem(ConfigItem *item, const ErrorStack &err=ErrorStack());

  /** Traverses the list of objects or references.
   * If @c list is a list of @c ConfigItem, the visitor will traverse those by calling
   * @c processItem on every of these element. If it is a list of @c ConfigObjectRef references,
   * the visitor stop here and simply return @c true. By default, the visitor does not follow
   * references. */
  virtual bool processList(AbstractConfigObjectList *list, const ErrorStack &err=ErrorStack());

  /** Handles references to config objects.
   * By default, the method will simply return @c true. The visitor does not follow references. */
  virtual bool processReference(ConfigObjectReference*, const ErrorStack &err=ErrorStack());
};

#endif // VISITOR_HH
