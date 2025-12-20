/** @defgroup dm32uv Baofeng DM32UV
 * @ingroup dsc */
#ifndef DM32UV_HH
#define DM32UV_HH

#include "radio.hh"
#include <QObject>
#include <QHash>

// Forwar declaration
class DM32UVInterface;


/** Represents a Baofeng DM32UV.
 * @ingroup dm32uv */
class DM32UV : public Radio
{
  Q_OBJECT

public:
  /** Implements a memory map between physical and virtual codeplug addresses.
   * The addresses, the codeplug is encoded in, are virtual. These codeplug blocks are then written#
   * to different physical addresses. */
  class AddressMap {
  public:
    /** Default constructor. */
    AddressMap();
    /** Default copy constructor. */
    AddressMap(const AddressMap &other) = default;
    /** Adds an address mapping. */
    void map(uint32_t phys, uint32_t virt);
    /** Maps the given physical address to a virtual codeplug address. */
    uint32_t toVirtual(uint32_t phys) const;
    /** Maps the given virtual codeplug address to a physical memory address. */
    uint32_t toPhysical(uint32_t virt) const;

  protected:
    /** Holds only the prefix address mapping from physical to virtual addresses. That is
     *  @c addr>>12. */
    QHash<uint32_t, uint32_t> _toVirtual;
    /** Holds only the prefix address mapping from virtual to physical addresses. That is
     *  @c addr>>12. */
    QHash<uint32_t, uint32_t> _toPhyiscal;
  };

public:
  /** Default constructor. */
  explicit DM32UV(DM32UVInterface *device=nullptr, QObject *parent=nullptr);

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

protected:
  /** The interface to the radio. */
  DM32UVInterface *_dev;
  /** Holds the flags to control assembly and upload of code-plugs. */
  Codeplug::Flags _codeplugFlags;
  /** The generic configuration. */
  Config *_config;
  /** A weak reference to the user-database. */
  UserDatabase *_userDB;
};


#endif // DM32UV_HH
