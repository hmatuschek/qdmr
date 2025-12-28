/** @defgroup dm32uv Baofeng DM32UV
 * @ingroup dsc */
#ifndef DM32UV_HH
#define DM32UV_HH

#include "radio.hh"
#include "dm32uv_codeplug.hh"
#include <QObject>
#include <QHash>

// Forward declaration
class DM32UVInterface;


/** Represents a Baofeng DM32UV.
 * @ingroup dm32uv */
class DM32UV : public Radio
{
  Q_OBJECT

public:
  /** Implements a memory map between physical and virtual codeplug addresses.
   * The addresses, the codeplug is encoded in, are virtual. These codeplug blocks are then written
   * to different physical addresses. */
  class AddressMap {
  public:
    /** Default constructor. */
    AddressMap();
    /** Default copy constructor. */
    AddressMap(const AddressMap &other) = default;

    /** Adds an address mapping. */
    void map(uint32_t phys, uint32_t virt);

    /** Returns @c true, if the virtual address is mapped. */
    bool virtualIsMapped(uint32_t virt) const;
    /** Returns @c true, if the physical address is mapped. */
    bool physicalIsMapped(uint32_t virt) const;

    /** Maps the given physical address to a virtual codeplug address.
      * If physical address is unknown -- that is, not mapped --
      * @c std::numeric_limits<uint32_t>::max() gets returned. */
    uint32_t toVirtual(uint32_t phys) const;
    /** Maps the given virtual codeplug address to a physical memory address.
     * If virtual address is unknown -- that is, not mapped --
     * @c std::numeric_limits<uint32_t>::max() gets returned. */
    uint32_t toPhysical(uint32_t virt) const;

    /** Returns the map from physical to virtual addresses, sorted by physical addresses. */
    inline QList<uint32_t> mappedPhysical() const {
      QList<uint32_t> addrs;
      for (auto prefix: _toVirtual.keys())
        addrs.append(prefix<<12);
      return addrs;
    }

    /** Returns the map from virtual to physical addresses, sorted by virtual addresses. */
    inline QList<uint32_t> mappedVirtual() const {
      QList<uint32_t> addrs;
      for (auto prefix: _toPhysical.keys())
        addrs.append(prefix<<12);
      return addrs;
    }

  protected:
    /** Holds only the prefix address mapping from physical to virtual addresses. That is
     *  @c addr>>12. */
    QMap<uint32_t, uint32_t> _toVirtual;
    /** Holds only the prefix address mapping from virtual to physical addresses. That is
     *  @c addr>>12. */
    QMap<uint32_t, uint32_t> _toPhysical;
  };

public:
  /** Default constructor. */
  explicit DM32UV(DM32UVInterface *device=nullptr, QObject *parent=nullptr);

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

  const QString &name() const override;

  const RadioLimits &limits() const override;

  const Codeplug &codeplug() const override;
  Codeplug &codeplug() override;

  bool startDownload(const TransferFlags &flags, const ErrorStack &err=ErrorStack()) override;
  bool startUpload(Config *config, const Codeplug::Flags &flags, const ErrorStack &err) override;
  bool startUploadCallsignDB(UserDatabase *db, const CallsignDB::Flags &selection, const ErrorStack &err) override;
  bool startUploadSatelliteConfig(SatelliteDatabase *db, const TransferFlags &flags, const ErrorStack &err) override;


public:
  /** Some constants. */
  struct Offset {
    /** Size for each codeplug block. */
    static constexpr uint32_t blockSize() { return 0x1000; }
  };

private:
  virtual bool download(const ErrorStack &err=ErrorStack());
  virtual bool upload(const ErrorStack &err=ErrorStack());

protected:
  /** Thread main routine, performs all blocking IO operations for codeplug up- and download. */
  void run() override;

protected:
  /** The interface to the radio. */
  DM32UVInterface *_dev;
  /** The name of the device. */
  QString _radioName;
  /** Holds the flags to control assembly and upload of code-plugs. */
  Codeplug::Flags _codeplugFlags;
  /** Encoded / read codeplug. */
  DM32UVCodeplug _codeplug;
  /** The generic configuration. */
  Config *_config;
  /** A weak reference to the user-database. */
  UserDatabase *_userDB;


private:
  /** Holds the singleton instance of the radio limits. */
  static RadioLimits *_limits;
};


#endif // DM32UV_HH
