#include "codeplugcontext.hh"

CodeplugContext::CodeplugContext(Config *config)
  : _config(config), _channelTable(), _digitalContactTable()
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

bool
CodeplugContext::hasDigitalContact(int index) const {
  return _digitalContactTable.contains(index);
}

bool
CodeplugContext::addDigitalContact(DigitalContact *con, int index) {
  if (_digitalContactTable.contains(index))
    return false;
  int cidx = _config->contacts()->addContact(con);
  if (0 > cidx)
    return false;
  _digitalContactTable[index] = cidx;
  return true;
}

DigitalContact *
CodeplugContext::getDigitalContact(int index) const {
  if (! _digitalContactTable.contains(index))
    return nullptr;
  return _config->contacts()->digitalContact(_digitalContactTable[index]);
}
