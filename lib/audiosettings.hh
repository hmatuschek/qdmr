#ifndef AUDIOSETTINGS_HH
#define AUDIOSETTINGS_HH

#include "configobject.hh"
#include "level.hh"
#include "interval.hh"


/** Collects common audio and tone settings */
class AudioSettings: public ConfigItem
{
  Q_OBJECT

  Q_PROPERTY(Level squelch READ squelch WRITE setSquelch)
  Q_PROPERTY(Level dmrSquelch READ dmrSquelch WRITE setDMRSquelch)
  Q_PROPERTY(Level micGain READ micGain WRITE setMicGain)
  Q_PROPERTY(Level fmMicGain READ fmMicGain WRITE setFMMicGain)
  Q_PROPERTY(Level m17MicGain READ m17MicGain WRITE setM17MicGain)
  Q_PROPERTY(Level maxSpeakerVolume READ maxSpeakerVolume WRITE setMaxSpeakerVolume)
  Q_PROPERTY(Level maxHeadphoneVolume READ maxHeadphoneVolume WRITE setMaxHeadphoneVolume)
  Q_PROPERTY(Level vox READ vox WRITE setVox)
  Q_PROPERTY(Interval voxDelay READ voxDelay WRITE setVOXDelay)
  Q_PROPERTY(bool speech READ speechSynthesisEnabled WRITE enableSpeechSynthesis)

public:
  /** Default constructor. */
  explicit AudioSettings(QObject *parent = nullptr);

  ConfigItem *clone() const override;

  /** Returns the default (FM) squelch. */
  Level squelch() const;
  /** Sets the default squelch. */
  void setSquelch(Level squelch);

  /** Returns @c true if the DMR squelch is set. */
  bool dmrSquelchEnabled() const;
  /** Returns the DMR squelch. */
  Level dmrSquelch() const;
  /** Set the DMR squelch. */
  void setDMRSquelch(Level dmrSquelch);
  /** Disables the DMR squelch setting. The global FM squelch is then used. */
  void disableDMRSquelch();

  /** Returns the default mic gain. */
  Level micGain() const;
  /** Sets the default mic gain. */
  void setMicGain(Level micGain);

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

  /** Returns @c true, if the VOX is enabled. */
  bool voxEnabled() const;
  /** Returns the VOX sensitivity. */
  Level vox() const;
  /** Sets the VOX sensitivity. */
  void setVox(Level vox);
  /** Disables VOX. */
  void disableVox();

  /** Returns the VOX delay. */
  Interval voxDelay() const;
  /** Sets the VOX delay. */
  void setVOXDelay(Interval ms);

  /** Returns the maximum speaker volume. */
  Level maxSpeakerVolume() const;
  /** Sets the maximum speaker volume. */
  void setMaxSpeakerVolume(Level maxSpeakerVolume);
  /** Returns the maximum headphone volume. */
  Level maxHeadphoneVolume() const;
  /** Sets the maximum headphone volume. */
  void setMaxHeadphoneVolume(Level maxHeadphoneVolume);

  /** Returns @c true if the speech synthesis is enabled. */
  bool speechSynthesisEnabled() const;
  /** Enable speech synthesis. */
  void enableSpeechSynthesis(bool enabled);

protected:
  /** Default (FM) squelch. */
  Level _squelch;
  /** DMR squelch setting. */
  Level _dmrSquelch;
  /** Default mic gain. */
  Level _micGain;
  /** The FM mic gain. */
  Level _fmMicGain;
  /** The M17 mic gain. */
  Level _m17MicGain;
  /** Maximum speaker volume. */
  Level _maxSpeakerVolume;
  /** Maximum headphone volume. */
  Level _maxHeadphoneVolume;
  /** Specifies the VOX level. */
  Level _vox;
  /** Specifies the VOX delay. */
  Interval _voxDelay;
  /** Enables speech synthesis. */
  bool _speech;
};



#endif //QDMR_AUDIOSETTINGS_HH
