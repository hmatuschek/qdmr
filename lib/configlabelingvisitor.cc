#include "configlabelingvisitor.hh"

ConfigLabelingVisitor::ConfigLabelingVisitor(ConfigItem::Context &context)
  : Visitor(), _context(context)
{
  // pass..
}

bool
ConfigLabelingVisitor::processItem(ConfigItem *item, const ErrorStack &err)
{
  // First, check if item is an ConfigObject
  if (item->is<ConfigObject>()) {
    ConfigObject *obj = item->as<ConfigObject>();
    QString prefix = obj->idPrefix();

    // If no prefix is set -> skip.
    if (prefix.isEmpty())
      return Visitor::processItem(item, err);

    // Find unused ID
    unsigned n=1;
    QString id=QString("%1%2").arg(prefix).arg(n);
    while (_context.contains(id))
      id=QString("%1%2").arg(prefix).arg(++n);

    // Add to context
    if (! _context.add(id, obj)) {
      if (_context.contains(obj))
        errMsg(err) << "Object already in context with id '" << _context.getId(obj) << "'.";
      errMsg(err) << "Cannot add element '" << id << "' to context.";
      return false;
    }
  }

  return Visitor::processItem(item, err);
}
