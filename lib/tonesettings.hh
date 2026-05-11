#ifndef TONESETTINGS_HH
#define TONESETTINGS_HH

#include "configobject.hh"
#include "level.hh"
#include "channel.hh"


class ToneSettings : public ConfigItem
{
  Q_OBJECT

  /** Disables all tones. */
  Q_PROPERTY(bool silent READ silent WRITE enableSilent)
  /** Key tone volume. */
  Q_PROPERTY(Level keyTone READ keyToneVolume WRITE setKeyToneVolume)

  /** Enables notification tone for SMS reception. */
  Q_PROPERTY(bool smsTone READ smsToneEnabled WRITE enableSMSTone)
  /** Enables ringtones. */
  Q_PROPERTY(bool ringtone READ ringtoneEnabled WRITE enableRingtone);
  /** Enables boot melody. */
  Q_PROPERTY(bool bootTone READ bootToneEnabled WRITE enableBootTone)
  /** Enables boot melody. */
  Q_PROPERTY(Melody * bootMelody READ bootMelody)
  /** Enables talk-permit tone for different channel types.
   * This tone sounds, whenever the PTT is pressed and the admit criterion is met. */
  Q_PROPERTY(Channel::Types talkPermit READ talkPermit WRITE setTalkPermit)
  /** Enables call-start tone for different channel types. */
  Q_PROPERTY(Channel::Types callStart READ callStart WRITE setCallStart)
  /** The call-start melody. */
  Q_PROPERTY(Melody * callStartMelody READ callStartMelody)
  /** Enables call-end tone for different channel types. */
  Q_PROPERTY(Channel::Types callEnd READ callEnd WRITE setCallEnd)
  /** The call-end melody. */
  Q_PROPERTY(Melody * callEndMelody READ callEndMelody)
  /** Enables channel-idle tone for different channel types.
   * This tone sounds, once the channel becomes free after a call. */
  Q_PROPERTY(Channel::Types channelIdle READ channelIdle WRITE setChannelIdle)
  /** The channel-idle melody. */
  Q_PROPERTY(Melody * channelIdleMelody READ channelIdleMelody)
  /** Enables call reset tone.
   * This tone sounds, once the (private or group-call) hang-time passed.
   * This affects only DMR and M17 channels. */
  Q_PROPERTY(bool callReset READ callResetEnabled WRITE enableCallReset)
  /** The call-reset melody. */
  Q_PROPERTY(Melody * callResetMelody READ callResetMelody)

public:
  /** Default constructor. */
  explicit ToneSettings(QObject *parent = nullptr);

  ConfigItem *clone() const override;

  /** Returns @c true if all tones are disabled. */
  bool silent() const;
  /** Disables all tones. */
  void enableSilent(bool enable);

  /** Returns @c true, if the key-tones are enabled. */
  bool keyToneEnabled() const;
  /** Returns the key tone volume. */
  Level keyToneVolume() const;
  /** Sets key tone volume. */
  void setKeyToneVolume(Level volume);
  /** Disables key-tones. */
  void disableKeyTone();

  /** Returns @c true, if the SMS tone is enabled. */
  bool smsToneEnabled() const;
  /** Enables/disables SMS tone. */
  void enableSMSTone(bool enable);

  /** Returns @c true, if the ringtone is enabled. */
  bool ringtoneEnabled() const;
  /** Enables ringtone. */
  void enableRingtone(bool enable);

  /** Returns @c true, if the boot-tone is enabled. */
  bool bootToneEnabled() const;
  /** Enables boot-tone. */
  void enableBootTone(bool enable);
  /** Returns a reference to the boot melody. */
  Melody *bootMelody() const;

  /** Returns channel types, for which the talk-permit tone is enabled. */
  Channel::Types talkPermit() const;
  /** Sets channel types, for which the talk-permit tone is enabled. */
  void setTalkPermit(Channel::Types permit);

  /** Returns channel types, for which the call-start tone is enabled. */
  Channel::Types callStart() const;
  /** Sets channel types, for which the call-start tone is enabled. */
  void setCallStart(Channel::Types permit);
  /** Returns a reference to the call-start melody. */
  Melody *callStartMelody() const;

  /** Returns channel types, for which the call-end tone is enabled. */
  Channel::Types callEnd() const;
  /** Sets channel types, for which the call-end tone is enabled. */
  void setCallEnd(Channel::Types permit);
  /** Returns a reference to the call-end melody. */
  Melody *callEndMelody() const;

  /** Returns channel types, for which the channel-idle tone is enabled. */
  Channel::Types channelIdle() const;
  /** Sets channel types, for which the channel-idle tone is enabled. */
  void setChannelIdle(Channel::Types permit);
  /** Returns a reference to the channel-idle melody. */
  Melody *channelIdleMelody() const;

  /** Returns @c true, if the call-reset tone is enabled. */
  bool callResetEnabled() const;
  /** Enables call-reset tone. */
  void enableCallReset(bool enable);
  /** Returns a reference to the call-reset melody. */
  Melody *callResetMelody() const;

protected:
  /** If @c true, all tones are disabled. */
  bool _silent;
  /** The key tone volume. */
  Level _keyTone;
  /** Enables SMS tones. */
  bool _smsTone;
  /** Enables ringtones. */
  bool _ringtone;
  /** Enables boot-tone. */
  bool _bootTone;
  /** The boot melody. */
  Melody *_bootMelody;
  /** Enables talk-permit tones for several channel types. */
  Channel::Types _talkPermit;
  /** Enables call-start tones for several channel types. */
  Channel::Types _callStart;
  Melody *_callStartMelody;             ///< Call melody.
  /** Enables call-end tones for several channel types. */
  Channel::Types _callEnd;
  Melody *_callEndMelody;          ///< Call end melody.
  /** Enables channel-idle tones for several channel types. */
  Channel::Types _channelIdle;
  Melody *_channelIdleMelody;             ///< Idle melody.
  /** Enables call-reset tones. */
  bool _callReset;
  Melody *_callResetMelody;            ///< Reset melody.
};



#endif //TONESETTINGS_HH
