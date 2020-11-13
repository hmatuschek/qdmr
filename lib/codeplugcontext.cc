#include "codeplugcontext.hh"

CodeplugContext::CodeplugContext(Config *config)
  : _config(config), _channelTable()
{
  // pass...
}

Config *
CodeplugContext::config() const {
  return _config;
}

bool
CodeplugContext::hasChannel(int index) const {
  return _channelTable.contains(index);
}

bool
CodeplugContext::addChannel(Channel *ch, int index) {
  if (_channelTable.contains(index))
    return false;
  int cidx = _config->channelList()->addChannel(ch);
  if (0 > cidx)
    return false;
  _channelTable[index] = cidx;
  return true;
}

Channel *
CodeplugContext::getChannel(int index) const {
  if (! _channelTable.contains(index))
    return nullptr;
  return _config->channelList()->channel(_channelTable[index]);
}
