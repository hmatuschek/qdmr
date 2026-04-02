//
// Created by hannes on 02.04.26.
//

#ifndef BOOTSETTINGS_HH
#define BOOTSETTINGS_HH

#include "configobject.hh"
#include "configreference.hh"


/** Collects all common boot settings across devices.
 * @ingroup conf */
class BootSettings: public ConfigExtension
{
  Q_OBJECT

  /** The boot display setting. */
  Q_PROPERTY(BootDisplay display READ bootDisplay WRITE setBootDisplay)
  /** If @c true, the boot password is enabled. */
  Q_PROPERTY(bool passwordEnabled READ bootPasswordEnabled WRITE enableBootPassword)
  /** Holds the boot password. */
  Q_PROPERTY(QString password READ bootPassword WRITE setBootPassword)

  Q_CLASSINFO("defaultChannelDescription", "If enabled, the default channels are selected at boot.")
  /** If enabled, the default channels are selected at boot. */
  Q_PROPERTY(bool defaultChannel READ defaultChannelEnabled WRITE enableDefaultChannel)
  Q_CLASSINFO("zoneADescription", "The default zone for VFO A.")
  /** The default zone for VFO A. */
  Q_PROPERTY(ZoneReference* zoneA READ zoneA)
  Q_CLASSINFO("channelADescription", "The default channel for VFO A. Must be within zone A.")
  /** The default channel for VFO A. */
  Q_PROPERTY(ChannelReference* channelA READ channelA)
  Q_CLASSINFO("zoneBDescription", "The default zone for VFO B.")
  /** The current zone for VFO B. */
  Q_PROPERTY(ZoneReference* zoneB READ zoneB)
  Q_CLASSINFO("channelBDescription", "The default channel for VFO B. Must be within zone B.")
  /** The default channel for VFO B. */
  Q_PROPERTY(ChannelReference* channelB READ channelB)

  /** Allows a factory reset during boot. Sometimes needed to enter FW programming mode. */
  Q_PROPERTY(bool allowFactoryReset READ resetEnabled WRITE enableReset)

public:
  /** What to display during boot. */
  enum class BootDisplay {
    Logo, ///< Default or build in logo.
    Text, ///< Custom text.
    Image ///< Custom image.
  };
  Q_ENUM(BootDisplay)

public:
  /** Default constructor. */
  explicit BootSettings(QObject *parent=nullptr);

  ConfigItem *clone() const override;

  /** Returns the boot display setting. */
  BootDisplay bootDisplay() const;
  /** Sets the boot display. */
  void setBootDisplay(BootDisplay mode);
  /** Returns @c true if the boot password is enabled.*/
  bool bootPasswordEnabled() const;
  /** Enables the boot password. */
  void enableBootPassword(bool enable);
  /** Returns the boot password. */
  const QString &bootPassword() const;
  /** Sets the boot password. */
  void setBootPassword(const QString &pass);

  /** If @c true, the radio switches to the default channel at boot. */
  bool defaultChannelEnabled() const;
  /** Enables/disables boot default channel. */
  void enableDefaultChannel(bool enable);
  /** Returns a reference to the default zone for VFO A. */
  ZoneReference *zoneA() const;
  /** Returns a reference to the default channel for VFO A. */
  ChannelReference *channelA() const;
  /** Returns a reference to the default zone for VFO B. */
  ZoneReference *zoneB() const;
  /** Returns a reference to the default channel for VFO B. */
  ChannelReference *channelB() const;

  /** Returns @c true if the MCU is reset on boot. */
  bool resetEnabled() const;
  /** Enables/disables MCU reset on boot. */
  void enableReset(bool enable);

protected:
  BootDisplay _bootDisplay;        ///< The boot display property.
  bool _bootPasswordEnabled;       ///< If true, the boot password is enabled.
  QString _bootPassword;           ///< The boot password
  bool _defaultChannel;            ///< Change to the default channel on boot.
  ZoneReference *_zoneA;           ///< Default zone for VFO A.
  ChannelReference *_channelA;     ///< Default channel for VFO A, must be member of zone for VFO A.
  ZoneReference *_zoneB;           ///< Default zone for VFO B.
  ChannelReference *_channelB;     ///< Default channel for VFO B, must be member of zone for VFO B.
  bool _reset;                     ///< Enables MCU reset on boot.
};


#endif //BOOTSETTINGS_HH
