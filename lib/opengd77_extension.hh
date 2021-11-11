/** @defgroup ogd77ex OpenGD77 Extensions
 * This module collects classes that implement the firmware specific extensions to the common
 * codeplug configuration for radios running the OpenGD77 firmware.
 *
 * @ingroup ogd77 */
#ifndef OPENGD77EXTENSION_HH
#define OPENGD77EXTENSION_HH

#include "configobject.hh"
#include "configreader.hh"

/** Implements the channel extensions for the OpenGD77 radios.
 * @since 0.9.0
 * @ingroup ogd77ex */
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
  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx);

  /** Returns the power settings for the channel. */
  Power power() const;
  /** Sets the power setting. */
  void setPower(Power power);

protected:
  /** Holds the power setting. */
  Power _power;
};

/** Implements the contact extensions for the OpenGD77 radios.
 * @since 0.9.0
 * @ingroup ogd77ex */
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

  ConfigItem *allocateChild(QMetaProperty &prop, const YAML::Node &node, const Context &ctx);

  /** Returns the time slot override. */
  TimeSlotOverride timeSlotOverride() const;
  /** Sets the time slot override. */
  void setTimeSlotOverride(TimeSlotOverride ts);

protected:
  /** Holds the time slot override. */
  TimeSlotOverride _timeSlotOverride;
};


#endif // OPENGD77EXTENSION_HH
