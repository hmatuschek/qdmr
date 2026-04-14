#ifndef AUDIOSETTINGS_HH
#define AUDIOSETTINGS_HH

#include "configobject.hh"
#include "level.hh"
#include "interval.hh"


/** Collects common audio and tone settings */
class AudioSettings: public ConfigItem
{
  Q_OBJECT

  Q_PROPERTY(Level fmMicGain READ fmMicGain WRITE setFMMicGain)
  Q_PROPERTY(Level m17MicGain READ m17MicGain WRITE setM17MicGain)
  Q_PROPERTY(Level satMicGain READ satMicGain WRITE setSatMicGain)
  Q_PROPERTY(Interval voxDelay READ voxDelay WRITE setVOXDelay)

public:
  /** Default constructor. */
  explicit AudioSettings(QObject *parent = nullptr);

  ConfigItem *clone() const override;

  /** Returns @c true if the FM mic gain is enabled. */
  bool fmMicGainEnabled() const;
  /** Returns the FM mic gain. If null, the dmr mic gain is used. */
  Level fmMicGain() const;
  /** Sets the FM mic gain. If set to null, the global DMR mic gain is used. */
  void setFMMicGain(Level fmMicGain);
  /** Disables FM mic gain. */
  void disableFMMicGain();

  /** Returns @c true if the M17 mic gain is enabled. */
  bool m17MicGainEnabled() const;
  /** Returns the M17 mic gain. If null, the dmr mic gain is used. */
  Level m17MicGain() const;
  /** Sets the M17 mic gain. If set to null, the global DMR mic gain is used. */
  void setM17MicGain(Level m17MicGain);
  /** Disables M17 mic gain. */
  void disableM17MicGain();

  /** Returns @c true if the FM mic gain for satellite operation is enabled. */
  bool satMicGainEnabled() const;
  /** Returns the FM mic gain for satellite operation. If null, the dmr mic gain is used. */
  Level satMicGain() const;
  /** Sets the FM mic gain for satellite operation.
   * If set to null, the global DMR mic gain is used. */
  void setSatMicGain(Level satMicGain);
  /** Disables FM mic gain for sat operation. */
  void disableSatMicGain();

  /** Returns the VOX delay. */
  Interval voxDelay() const;
  /** Sets the VOX delay. */
  void setVOXDelay(Interval ms);

protected:
  /** The FM mic gain. */
  Level _fmMicGain;
  /** The M17 mic gain. */
  Level _m17MicGain;
  /** The FM mic gain for satellite operation. */
  Level _satMicGain;
  /** Specifies the VOX delay. */
  Interval _voxDelay;
};



#endif //QDMR_AUDIOSETTINGS_HH
