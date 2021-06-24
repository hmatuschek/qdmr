#include "codeplugcontext.hh"

CodeplugContext::CodeplugContext(Config *config)
  : _config(config), _radioIDTable(), _channelTable(), _digitalContactTable(), _analogContactTable(),
    _groupListTable(), _scanListTable(), _gpsSystemTable(), _aprsSystemTable()
{
  // pass...
}

Config *
CodeplugContext::config() const {
  return _config;
}


bool
CodeplugContext::hasRadioId(int index) const {
  return _radioIDTable.contains(index);
}

bool
CodeplugContext::setDefaultRadioId(uint32_t id, int index) {
  if (_radioIDTable.contains(0))
    return false;
  _config->radioIDs()->getId(0)->setId(id);
  _radioIDTable[index] = 0;
  return true;
}

bool
CodeplugContext::addRadioId(uint32_t id, int index) {
  if (_radioIDTable.contains(index))
    return false;
  int cidx = _config->radioIDs()->addId(id);
  if (cidx < 0)
    return false;
  _radioIDTable[index] = cidx;
  return true;
}

RadioID *
CodeplugContext::getRadioId(int idx) const {
  if (! _radioIDTable.contains(idx))
    return nullptr;
  return _config->radioIDs()->getId(_radioIDTable[idx]);
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
CodeplugContext::hasAnalogContact(int index) const {
  return _analogContactTable.contains(index);
}

bool
CodeplugContext::addAnalogContact(DTMFContact *con, int index) {
  if (_analogContactTable.contains(index))
    return false;
  int cidx = _config->contacts()->addContact(con);
  if (0 > cidx)
    return false;
  _analogContactTable[index] = cidx;
  return true;
}

DTMFContact *
CodeplugContext::getAnalogContact(int index) const {
  if (! _analogContactTable.contains(index))
    return nullptr;
  return _config->contacts()->dtmfContact(_analogContactTable[index]);
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
  return _config->posSystems()->gpsSystem(_gpsSystemTable[index]);
}

bool
CodeplugContext::addGPSSystem(GPSSystem *sys, int index) {
  if (_gpsSystemTable.contains(index))
    return false;
  int sidx = _config->posSystems()->gpsCount();
  if (0 > _config->posSystems()->addSystem(sys))
    return false;
  _gpsSystemTable[index] = sidx;
  return true;
}


bool
CodeplugContext::hasAPRSSystem(int index) const {
  return _aprsSystemTable.contains(index);
}

APRSSystem *
CodeplugContext::getAPRSSystem(int index) const {
  if (! _aprsSystemTable.contains(index))
    return nullptr;
  return _config->posSystems()->aprsSystem(_aprsSystemTable[index]);
}

bool
CodeplugContext::addAPRSSystem(APRSSystem *sys, int index) {
  if (_aprsSystemTable.contains(index))
    return false;
  int sidx = _config->posSystems()->aprsCount();
  if (0 > _config->posSystems()->addSystem(sys))
    return false;
  _aprsSystemTable[index] = sidx;
  return true;
}


bool
CodeplugContext::hasRoamingZone(int index) const {
  return _roamingZoneTable.contains(index);
}

RoamingZone *
CodeplugContext::getRoamingZone(int index) const {
  if (! _roamingZoneTable.contains(index))
    return nullptr;
  return _config->roaming()->zone(_roamingZoneTable[index]);
}

bool
CodeplugContext::addRoamingZone(RoamingZone *zone, int index) {
  if (_roamingZoneTable.contains(index))
    return false;
  int sidx = _config->roaming()->count();
  if (! _config->roaming()->addZone(zone))
    return false;
  _roamingZoneTable[index] = sidx;
  return true;
}


bool
CodeplugContext::hasRoamingChannel(int index) const {
  return _roamingChannelTable.contains(index);
}

DigitalChannel *
CodeplugContext::getRoamingChannel(int index) const {
  if (! _roamingChannelTable.contains(index))
    return nullptr;
  return _config->channelList()->channel(_roamingChannelTable[index])->as<DigitalChannel>();
}

bool
CodeplugContext::addRoamingChannel(DigitalChannel *ch, int index) {
  if (_roamingChannelTable.contains(index))
    return false;
  int sidx = _config->channelList()->indexOf(ch);
  _roamingChannelTable[index] = sidx;
  return true;
}
