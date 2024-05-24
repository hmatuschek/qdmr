#ifndef ROAMINGCHANNEL_HH
#define ROAMINGCHANNEL_HH

#include "channel.hh"

/** Represents a roaming channel.
 *
 *  That is, an incomplete DMR channel, that overrides some channel
 *  settings to allow for roaming between repeaters. To this end, the roaming channel may override
 *  only those channel settings, that are repeater specific like frequencies and color-codes but
 *  keeps DMR contact settings. Some of these properties are overridden optionally (time slot and
 *  color code) while the RX/TX frequencies are overridden always.
 *
 *  @ingroup config */
class RoamingChannel : public ConfigObject
{
  Q_OBJECT
  Q_CLASSINFO("IdPrefix", "rch")

  /** Holds the RX frequency in Hz. */
  Q_PROPERTY(Frequency rxFrequency READ rxFrequency WRITE setRXFrequency SCRIPTABLE false)
  /** Holds the TX frequency in Hz. */
  Q_PROPERTY(Frequency txFrequency READ txFrequency WRITE setTXFrequency SCRIPTABLE false)
  /** If @c true, the color code of the channel gets overridden by the one specified in @c colorCode. */
  Q_PROPERTY(bool overrideColorCode READ colorCodeOverridden WRITE overrideColorCode SCRIPTABLE false)
  /** If @c overrideColorCode is @c true, specifies the color code. */
  Q_PROPERTY(unsigned int colorCode READ colorCode WRITE setColorCode SCRIPTABLE false)
  /** If @c true, the time slot of the channel gets overridden by the one specified in @c timeSlot. */
  Q_PROPERTY(bool overrideTimeSlot READ timeSlotOverridden WRITE overrideTimeSlot SCRIPTABLE false)
  /** If @c overrideTimeSlot is @c true, specifies the time slot. */
  Q_PROPERTY(DMRChannel::TimeSlot timeSlot READ timeSlot WRITE setTimeSlot SCRIPTABLE false)

public:
  /** Default constructor for a roaming channel. */
  Q_INVOKABLE explicit RoamingChannel(QObject *parent = nullptr);
  /** Copy constructor. */
  RoamingChannel(const RoamingChannel &other, QObject *parent=nullptr);

  ConfigItem *clone() const;
  void clear();

  /** Returns the RX frequency in Hz. */
  Frequency rxFrequency() const;
  /** Sets the RX frquency in Hz. */
  void setRXFrequency(Frequency f);
  /** Returns the TX frequency in Hz. */
  Frequency txFrequency() const;
  /** Sets the TX frquency in Hz. */
  void setTXFrequency(Frequency f);

  /** Returns @c true, if the color code of the channel gets overridden. */
  bool colorCodeOverridden() const;
  /** Enables/disables overriding the color code of the channel. */
  void overrideColorCode(bool override);
  /** Returns the color code. */
  unsigned int colorCode() const;
  /** Sets the color code. */
  void setColorCode(unsigned int cc);

  /** Returns @c true, if the time slot of the channel gets overridden. */
  bool timeSlotOverridden() const;
  /** Enables/disables overriding the time slot of the channel. */
  void overrideTimeSlot(bool override);
  /** Returns the time slot. */
  DMRChannel::TimeSlot timeSlot() const;
  /** Sets the time slot. */
  void setTimeSlot(DMRChannel::TimeSlot ts);

  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err);

public:
  /** Helper method to construct a Roaming channel from a given DMR channel. Optionally with
   * reference to a third one. */
  static RoamingChannel *fromDMRChannel(DMRChannel *ch, DMRChannel *ref=nullptr);

protected:
  bool populate(YAML::Node &node, const Context &context, const ErrorStack &err);

protected:
  /** Holds the RX frequency in Hz. */
  Frequency _rxFrequency;
  /** Holds the TX frequency in Hz. */
  Frequency _txFrequency;
  /** If @c true, the color code of the channel gets overridden by the one specified in @c _colorCode. */
  bool _overrideColorCode;
  /** If @c _overrideColorCode is @c true, specifies the color code. */
  unsigned int _colorCode;
  /** If @c true, the time slot of the channel gets overridden by the one specified in @c _timeSlot. */
  bool _overrideTimeSlot;
  /** If @c _overrideTimeSlot is @c true, specifies the time slot. */
  DMRChannel::TimeSlot _timeSlot;
};


/** Represents the list of roaming channels within the abstract device configuration.
 *
 * @ingroup config */
class RoamingChannelList: public ConfigObjectList
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RoamingChannelList(QObject *parent=nullptr);

  /** Returns the roaming channel at the given index. */
  RoamingChannel *channel(int idx) const;

  int add(ConfigObject *obj, int row=-1, bool unique=true);

public:
  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};

#endif // ROAMINGCHANNEL_HH
