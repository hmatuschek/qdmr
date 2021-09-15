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


#endif // OPENGD77EXTENSION_HH
