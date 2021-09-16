#ifndef OPENGD77EXTENSION_HH
#define OPENGD77EXTENSION_HH

#include "configobject.hh"
#include "configreader.hh"

/** Implements the channel extensions for the OpenGD77 radios.
 * @ingroup opengd77 */
class OpenGD77ChannelExtension: public ConfigExtension
{
  Q_OBJECT

  /** The channel transmit power. Overrides the common power settings. */
  Q_PROPERTY(Power power READ power WRITE setPower)

public:
  /** All possible power settings. */
  enum class Power {
    Global  =  0,              ///< Use global power setting.
    P50mW   =  1,              ///< About 50mW.
    P250mW  =  2,              ///< About 250mW.
    P500mW  =  3,              ///< About 500mW.
    P750mW  =  4,              ///< About 750mW.
    P1W     =  5,              ///< About 1W.
    P2W     =  6,              ///< About 2W.
    P3W     =  7,              ///< About 3W.
    P4W     =  8,              ///< About 4W.
    P5W     =  9,              ///< About 5W.
    Max     = 10,              ///< Maximum power (5.5W on UHF, 7W on VHF).
  };
  Q_ENUM(Power)

public:
  /** Constructor. */
  explicit OpenGD77ChannelExtension(QObject *parent=nullptr);

public:
  /** Returns the power settings for the channel. */
  Power power() const;
  /** Sets the power setting. */
  void setPower(Power power);

protected:
  /** Holds the power setting. */
  Power _power;
};

/** Implements the config reader for OpenGD77 channel extensions. */
class OpenGD77ChannelExtensionReader: public ExtensionReader
{
  Q_OBJECT
  Q_CLASSINFO("name", "openGD77")

public:
  /** Constructor. */
  explicit OpenGD77ChannelExtensionReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);

private:
  /** Holds the instance of the config reader. */
  static AbstractConfigReader *instance;
};


/** Implements the contact extensions for the OpenGD77 radios.
 * @ingroup opengd77 */
class OpenGD77ContactExtension: public ConfigExtension
{
  Q_OBJECT

  /** If set, overrides the channel time slot if this contact is selected as the transmit contact. */
  Q_PROPERTY(TimeSlotOverride timeSlotOverride READ timeSlotOverride WRITE setTimeSlotOverride)

public:
  /** Possible values for the time-slot override option. */
  enum class TimeSlotOverride {
    None = 0x01,                  ///< Do not override time-slot of channel.
    TS1  = 0x00,                  ///< Force time-slot to TS1.
    TS2  = 0x02                   ///< Force time-slot to TS2.
  };
  Q_ENUM(TimeSlotOverride)

public:
  /** Constructor. */
  explicit OpenGD77ContactExtension(QObject *parent=nullptr);

  /** Returns the time slot override. */
  TimeSlotOverride timeSlotOverride() const;
  /** Sets the time slot override. */
  void setTimeSlotOverride(TimeSlotOverride ts);

protected:
  /** Holds the time slot override. */
  TimeSlotOverride _timeSlotOverride;
};

/** Implements the config reader for OpenGD77 contact extensions. */
class OpenGD77ContactExtensionReader: public ExtensionReader
{
  Q_OBJECT
  Q_CLASSINFO("name", "openGD77")

public:
  /** Constructor. */
  explicit OpenGD77ContactExtensionReader(QObject *parent=nullptr);

  ConfigObject *allocate(const YAML::Node &node, const ConfigObject::Context &ctx);

private:
  /** Holds the instance of the config reader. */
  static AbstractConfigReader *instance;
};




#endif // OPENGD77EXTENSION_HH
