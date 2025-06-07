#ifndef TRANSFERFLAGS_HH
#define TRANSFERFLAGS_HH


/** Controls the transfer of codeplugs, callsign DBs etc to the device. */
class TransferFlags
{
public:
  /** Default constructor. */
  TransferFlags();
  /** Constructor. */
  TransferFlags(bool blocking, bool updateDeviceClock);

  /** Returns @c true if the transfer is blocking. */
  bool blocking() const;
  /** Set if trasfer is blocking. */
  void setBlocking(bool enable);

  /** Returns @c true if the device clock gets updated during transfer. */
  bool updateDeviceClock() const;
  /** Sets if the device clock gets updated during the trasfer. */
  void setUpdateDeviceClock(bool enable);

protected:
  /** If @c true, the transfer is blocking. */
  bool _blocking;
  /** If @c true, the device clock gets updated during the transfer. */
  bool _updateDeviceClock;
};


#endif // TRANSFERFLAGS_HH
