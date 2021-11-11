#ifndef RADIOSETTINGS_HH
#define RADIOSETTINGS_HH

#include "configobject.hh"
#include "channel.hh"

/** Represents the common radio-global settings.
 * @ingroup conf */
class RadioSettings : public ConfigItem
{
  Q_OBJECT
  /** The first intro line. */
  Q_PROPERTY(QString introLine1 READ introLine1 WRITE setIntroLine1)
  /** The scond intro line. */
  Q_PROPERTY(QString introLine2 READ introLine2 WRITE setIntroLine2)
  /** The mic amplification level. */
  Q_PROPERTY(unsigned micLevel READ micLevel WRITE setMicLevel)
  /** Speech synthesis flag. */
  Q_PROPERTY(bool speech READ speech WRITE enableSpeech)
  /** The default channel power */
  Q_PROPERTY(Channel::Power power READ power WRITE setPower)
  /** The squelch level. */
  Q_PROPERTY(unsigned squelch READ squelch WRITE setSquelch)
  /** The default vox sensitivity */
  Q_PROPERTY(unsigned vox READ vox WRITE setVOX)
  /** The default transmit timeout */
  Q_PROPERTY(unsigned tot READ tot WRITE setTOT)

public:
  /** Default constructor. */
  explicit RadioSettings(QObject *parent=nullptr);

  bool copy(const ConfigItem &other);
  ConfigItem *clone() const;

  /** Resets the settings. */
  void clear();

  /** Returns the first intro line. */
  const QString &introLine1() const;
  /** (Re-)Sets the first intro line. */
  void setIntroLine1(const QString &line);

  /** Returns the second intro line. */
  const QString &introLine2() const;
  /** (Re-)Sets the second intro line. */
  void setIntroLine2(const QString &line);

  /** Returns the MIC amplification level [1,10]. */
  unsigned micLevel() const;
  /** (Re-)Sets the MIC amplification level [1,10]. */
  void setMicLevel(unsigned value);

  /** Returns @c true if the speech synthesis is enabled. */
  bool speech() const;
  /** Enables/disables the speech synthesis. */
  void enableSpeech(bool enabled);

  /** Returns the default squelch level [0-10]. */
  unsigned squelch() const;
  /** Sets the default squelch level. */
  void setSquelch(unsigned squelch);

  /** Returns the default channel power. */
  Channel::Power power() const;
  /** Sets the default channel power. */
  void setPower(Channel::Power power);

  /** Retuns @c true if VOX is disabled by default. */
  bool voxDisabled() const;
  /** Returns the default VOX level [0-10], 0=disabled. */
  unsigned vox() const;
  /** Sets the default VOX level [0-10], 0=disabled. */
  void setVOX(unsigned level);
  /** Disables VOX by default. */
  void disableVOX();

  /** Returns @c true if the transmit timeout (TOT) is disabled. */
  bool totDisabled() const;
  /** Returns the default transmit timeout (TOT) in seconds, 0=disabled. */
  unsigned tot() const;
  /** Sets the default transmit timeout (TOT) in seconds, 0=disabled. */
  void setTOT(unsigned sec);
  /** Disables the transmit timeout (TOT). */
  void disableTOT();

public:
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx);

protected:
  /** Holds the first intro line. */
  QString _introLine1;
  /** Holds the second intro line. */
  QString _introLine2;
  /** Holds the mic amplification level. */
  unsigned _micLevel;
  /** Holds the speech synthesis flag. */
  bool _speech;
  /** Holds the global squelch setting. */
  unsigned _squelch;
  /** Holds the global power setting. */
  Channel::Power _power;
  /** Holds the global VOX level. */
  unsigned _vox;
  /** Holds the global transmit timeout. */
  unsigned _transmitTimeOut;
};

#endif // RADIOCONFIG_HH
