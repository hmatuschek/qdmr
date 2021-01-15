#include "codeplugcontext.hh"

CodeplugContext::CodeplugContext(Config *config)
  : _config(config), _channelTable(), _digitalContactTable(), _groupListTable()
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


bool
CodeplugContext::hasGroupList(int index) const {
  return _groupListTable.contains(index);
}

bool
CodeplugContext::addGroupList(RXGroupList *grp, int index) {
  if (_groupListTable.contains(index))
    return false;
  int cidx = _config->rxGroupLists()->addList(grp);
  if (0 > cidx)
    return false;
  _groupListTable[index] = cidx;
  return true;
}

RXGroupList *
CodeplugContext::getGroupList(int index) const {
  if (! _groupListTable.contains(index))
    return nullptr;
  return _config->rxGroupLists()->list(_groupListTable[index]);
}


bool
CodeplugContext::hasScanList(int index) const {
  return _scanListTable.contains(index);
}

ScanList *
CodeplugContext::getScanList(int index) const {
  if (! _scanListTable.contains(index))
    return nullptr;
  return _config->scanlists()->scanlist(_scanListTable[index]);
}

bool
CodeplugContext::addScanList(ScanList *lst, int index) {
  if (_scanListTable.contains(index))
    return false;
  int sidx = _config->scanlists()->count();
  if (! _config->scanlists()->addScanList(lst))
    return false;
  _scanListTable[index] = sidx;
  return true;
}

bool
CodeplugContext::hasGPSSystem(int index) const {
  return _gpsSystemTable.contains(index);
}

GPSSystem *
CodeplugContext::getGPSSystem(int index) const {
  if (! _gpsSystemTable.contains(index))
    return nullptr;
  return _config->gpsSystems()->gpsSystem(_gpsSystemTable[index]);
}

bool
CodeplugContext::addGPSSystem(GPSSystem *sys, int index) {
  if (_gpsSystemTable.contains(index))
    return false;
  int sidx = _config->gpsSystems()->count();
  if (0 > _config->gpsSystems()->addGPSSystem(sys))
    return false;
  _gpsSystemTable[index] = sidx;
  return true;
}
