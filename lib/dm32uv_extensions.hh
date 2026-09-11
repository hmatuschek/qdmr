#ifndef DM32UVEXTENSIONS_HH
#define DM32UVEXTENSIONS_HH

#include "configobject.hh"
#include "interval.hh"


/** Represents the programmable button settings of the Baofeng DM-32UV. */
class DM32UVButtonSettingsExtension: public ConfigItem
{
  Q_OBJECT

  Q_PROPERTY(Interval longPressDuration READ longPressDuration WRITE setLongPressDuration)
  Q_PROPERTY(Function sk1Short READ sk1Short WRITE setSK1Short)
  Q_PROPERTY(Function sk1Long READ sk1Long WRITE setSK1Long)
  Q_PROPERTY(Function sk2Short READ sk2Short WRITE setSK2Short)
  Q_PROPERTY(Function sk2Long READ sk2Long WRITE setSK2Long)
  Q_PROPERTY(Function p1Short READ p1Short WRITE setP1Short)
  Q_PROPERTY(Function p1Long READ p1Long WRITE setP1Long)
  Q_PROPERTY(Function p2Short READ p2Short WRITE setP2Short)
  Q_PROPERTY(Function p2Long READ p2Long WRITE setP2Long)
  Q_PROPERTY(bool sideKeyLock READ sideKeyLock WRITE enableSideKeyLock)

public:
  enum class Function {
    None = 0, PowerSelect = 1, Volt = 2, Talkaround = 3,
    DMREncryption = 4, VOX = 6, ChannelMode = 7, Alarm = 8,
    OneTouch1 = 9, OneTouch2 = 10, OneTouch3 = 11, OneTouch4 = 12,
    OneTouch5 = 13, SMS = 14, Contacts = 15, ZoneUp = 16, ZoneDown = 17,
    Scan = 18, ToggleRecord = 19, PreviousRecord = 20, NextRecord = 21,
    FMBCRadio = 22, FMBCScan = 23, GPSInformation = 24, Monitor = 25,
    ToggleMainChannel = 26, LoneWorker = 27, KeypadLock = 28, Mute = 29,
    TBST = 30, APRSTX = 31, ChannelType = 32, DisplayMode = 33,
    CTCSSDSCScan = 34, CTCSSDSCSettings = 25, SilentTone = 36,
    Roaming = 37, SubPTT = 38,
    OneKeyScanFrequency = 40, Flashlight = 41
  };
  Q_ENUM(Function)

  explicit DM32UVButtonSettingsExtension(QObject *parent=nullptr);
  ConfigItem *clone() const;

  Interval longPressDuration() const;
  void setLongPressDuration(Interval duration);
  Function sk1Short() const;
  void setSK1Short(Function function);
  Function sk1Long() const;
  void setSK1Long(Function function);
  Function sk2Short() const;
  void setSK2Short(Function function);
  Function sk2Long() const;
  void setSK2Long(Function function);
  Function p1Short() const;
  void setP1Short(Function function);
  Function p1Long() const;
  void setP1Long(Function function);
  Function p2Short() const;
  void setP2Short(Function function);
  Function p2Long() const;
  void setP2Long(Function function);
  bool sideKeyLock() const;
  void enableSideKeyLock(bool enable);

protected:
  Interval _longPressDuration;
  Function _sk1Short, _sk1Long, _sk2Short, _sk2Long;
  Function _p1Short, _p1Long, _p2Short, _p2Long;
  bool _sideKeyLock;
};


/** Device-specific settings for the Baofeng DM-32UV. */
class DM32UVSettingsExtension: public ConfigExtension
{
  Q_OBJECT
  Q_PROPERTY(DM32UVButtonSettingsExtension *buttons READ buttons)

public:
  Q_INVOKABLE explicit DM32UVSettingsExtension(QObject *parent=nullptr);
  ConfigItem *clone() const;
  DM32UVButtonSettingsExtension *buttons() const;

protected:
  DM32UVButtonSettingsExtension *_buttons;
};

#endif // DM32UVEXTENSIONS_HH
