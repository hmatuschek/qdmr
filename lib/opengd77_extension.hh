#ifndef OPENGD77EXTENSION_HH
#define OPENGD77EXTENSION_HH

#include "configobject.hh"
#include "configreader.hh"
#include "opengd77_codeplug.hh"

/** Implements the channel extensions for the OpenGD77 radios.
 * @ingroup opengd77 */
class OpenGD77ChannelExtension: public ConfigExtension
{
  Q_OBJECT

  /** The channel transmit power. Overrides the common power settings. */
  Q_PROPERTY(Power power READ power WRITE setPower)

public:
  /** Use same enum as codeplug element. */
  typedef OpenGD77Codeplug::ChannelElement::Power Power;
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
  /** Reuse enum from codeplug. */
  typedef OpenGD77Codeplug::ContactElement::TimeSlotOverride TimeSlotOverride;
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
