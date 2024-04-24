#include "configmergevisitor.hh"
#include "configobject.hh"
#include "config.hh"
#include "channel.hh"


ConfigMergeVisitor::ConfigMergeVisitor(Config* destination, ItemStrategy itemStrategy, SetStrategy setStrategy)
  : Visitor(), _destination(destination), _itemStrategy(itemStrategy), _setStrategy(setStrategy)
{
  // pass...
}

bool
ConfigMergeVisitor::processItem(ConfigItem *item, const ErrorStack &err) {
  // Dispatch by type
  if (item->is<Channel>()) {
    if (_destination->channelList()->findItemsByName(item->as<Channel>()->name()).count()) {

    }
  }
}
