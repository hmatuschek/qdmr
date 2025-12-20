#include "dm32uv.hh"
#include "dm32uv_interface.hh"


/* ********************************************************************************************* *
 * Implementation of Baofeng DM32UV weird address mapping
 * ********************************************************************************************* */
DM32UV::AddressMap::AddressMap()
  : _toVirtual(), _toPhyiscal()
{
  // pass...
}

void
DM32UV::AddressMap::map(uint32_t phys, uint32_t virt) {
  phys >>= 12; virt >>= 12;
  _toVirtual[phys] = virt;
  _toPhyiscal[virt] = phys;
}

uint32_t
DM32UV::AddressMap::toPhysical(uint32_t virt) const {
  uint32_t offset = virt & 0xfff;
  if (! _toPhyiscal.contains(virt>>12))
    return -1;
  return (_toPhyiscal[virt>>12]<<12) | offset;
}

uint32_t
DM32UV::AddressMap::toVirtual(uint32_t phys) const {
  uint32_t offset = phys & 0xfff;
  if (! _toVirtual.contains(phys>>12))
    return -1;
  return (_toVirtual[phys>>12]<<12) | offset;
}


/* ********************************************************************************************* *
 * Implementation of Baofeng DM32UV
 * ********************************************************************************************* */
DM32UV::DM32UV(DM32UVInterface *dev, QObject *parent)
  : Radio{parent}, _dev(dev)
{
  // pass...
}

RadioInfo
DM32UV::defaultRadioInfo() {
  return RadioInfo(
    RadioInfo::DM32UV, "dm32uv", "DM-32UV", "Baofeng", {DM32UVInterface::interfaceInfo()});

}
