#include "addressmap.hh"
#include <algorithm>

AddressMap::AddressMap()
  : _items()
{
  // pass...
}

AddressMap::AddressMap(const AddressMap &other)
  : _items(other._items)
{
  // pass...
}

AddressMap &
AddressMap::operator =(const AddressMap &other) {
  _items = other._items;
  return *this;
}


void
AddressMap::clear() {
  _items.clear();
}

bool
AddressMap::add(uint32_t addr, uint32_t len, int idx) {
  if (0 > idx)
    idx = _items.size();
  AddrMapItem item(addr, len, idx);

  std::vector<AddrMapItem>::iterator at = std::lower_bound(_items.begin(), _items.end(), item);
  if (_items.end() == at)
    _items.push_back(item);
  else
    _items.insert(at, item);
  return true;
}

bool
AddressMap::rem(uint32_t idx) {
  std::vector<AddrMapItem>::iterator at = _items.begin();
  for (; at!=_items.end(); at++) {
    if (at->index == idx)
      break;
  }
  if (_items.end() == at)
    return false;
  _items.erase(at);
  return true;
}

bool
AddressMap::contains(uint32_t addr) const {
  return 0 <= find(addr);
}

int
AddressMap::find(uint32_t addr) const {
  std::vector<AddrMapItem>::const_iterator at = std::lower_bound(_items.begin(), _items.end(), addr);
  if (_items.end() == at)
    return -1;
  return at->contains(addr) ? at->index : -1;
}
