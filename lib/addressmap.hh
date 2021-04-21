#ifndef ADDRESSMAP_HH
#define ADDRESSMAP_HH

#include <inttypes.h>
#include <vector>

/** This class represents a memory map.
 * That is, it maintains a vector of memory regions (address and length) that can be searched
 * efficiently. This should speedup the generation of codeplugs consisting of many small memory
 * sections.
 *
 * @ingroup util*/
class AddressMap
{
public:
  /** Empty constructor. */
  AddressMap();
  /** Copy constructor. */
  AddressMap(const AddressMap &other);

  /** Copy assignment. */
  AddressMap &operator=(const AddressMap &other);

  /** Clears the address map. */
  void clear();
  /** Adds an item to the address map. */
  bool add(uint32_t addr, uint32_t len, int idx=-1);
  /** Removes an item from the address map associated with the given index. */
  bool rem(uint32_t idx);
  /** Returns @c true if the given address is contained in any of the memory regions. */
  bool contains(uint32_t addr) const;
  /** Finds the index of the memory region containing the given address. If no such region is found,
   * -1 is returned. */
  int find(uint32_t addr) const;

protected:
  /** Memory map item.
   * That is, a collection of address, length and associated index. */
  struct AddrMapItem {
    uint32_t address;   ///< The start address of the item.
    uint32_t length;    ///< The size/length of the memory item.
    uint32_t index;     ///< The associated (element) index.

    /** Contstructor. */
    inline AddrMapItem(uint32_t addr, uint32_t len, uint32_t idx)
      : address(addr), length(len), index(idx) {
      // pass...
    }
    /** Comparison operator. */
    inline bool operator<(const AddrMapItem &other) const {
      return address < other.address;
    }
    /** Comparison operator. */
    inline bool operator<(uint32_t addr) const {
      return address < addr;
    }
    /** Returns @c true if the given address is contained within this memory region. */
    inline bool contains(uint32_t addr) const {
      return (address <= addr) && ((address+length) > addr);
    }
  };

protected:
  /** Holds the vector of memory items, the order of these items is maintained. */
  std::vector<AddrMapItem> _items;
};

#endif // ADDRESSMAP_HH
