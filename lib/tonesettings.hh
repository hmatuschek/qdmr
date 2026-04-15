#ifndef TONESETTINGS_HH
#define TONESETTINGS_HH

#include "configobject.hh"
#include "level.hh"


class ToneSettings : public ConfigItem
{
  Q_OBJECT

  /** Disables all tones. */
  Q_PROPERTY(bool silent READ silent WRITE enableSilent)
  /** Key tone volume. */
  Q_PROPERTY(Level keyTone READ keyToneVolume WRITE setKeyToneVolume)

public:
  /** Default constructor. */
  explicit ToneSettings(QObject *parent = 0);

  ConfigItem *clone() const;

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

protected:
  /** If @c true, all tones are disabled. */
  bool _silent;
  /** The key tone volume. */
  Level _keyTone;
};



#endif //TONESETTINGS_HH
