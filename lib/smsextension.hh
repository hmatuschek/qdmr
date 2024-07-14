#ifndef SMSEXTENSION_HH
#define SMSEXTENSION_HH

#include "configobject.hh"
#include "interval.hh"


/** Represents a SMS message template (pre defined message).
 * Instances of this class are held in the @c SMSExtension. */
class SMSTemplate: public ConfigObject
{
  Q_OBJECT

  /** Specifies the prefix for every ID assigned to every message during serialization. */
  Q_CLASSINFO("IdPrefix", "sms")

  /** The message text. */
  Q_PROPERTY(QString message READ message WRITE setMessage)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit SMSTemplate(QObject *parent = nullptr);

  ConfigItem *clone() const;

  /** Returns the message text. */
  const QString &message() const;
  /** Sets the message text. */
  void setMessage(const QString message);

protected:
  /** Holds the message text. */
  QString _message;
};


/** Just a list, holding the SMS templates. */
class SMSTemplates: public ConfigObjectList
{
  Q_OBJECT

public:
  /** Default constructor. */
  Q_INVOKABLE SMSTemplates(QObject *parent=nullptr);

  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());

  /** Returns the i-th message. */
  SMSTemplate *message(unsigned int i) const;
};


/** Extension collecting all settings associated with SMS messages. */
class SMSExtension : public ConfigExtension
{
  Q_OBJECT

  /** The format for the SMS. */
  Q_PROPERTY(Format format READ format WRITE setFormat)
  /** All predefined SMS messages. */
  Q_PROPERTY(SMSTemplates *smsTemplates READ smsTemplates)

public:
  /** Possible SMS formats, DMR is usually a good idea. */
  enum class Format {
    Motorola, Hytera, DMR
  };
  Q_ENUM(Format)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit SMSExtension(QObject *parent = nullptr);

  ConfigItem *clone() const;

  /** Returns the SMS format setting. */
  Format format() const;
  /** Sets the SMS format. */
  void setFormat(Format format);

  /** Retunrs a weak reference to the list of SMS templates. */
  SMSTemplates *smsTemplates() const;

protected:
  /** Holds the SMS format. */
  Format _format;
  /** Owns a reference to the list of SMS templates. */
  SMSTemplates *_smsTemplates;
};

#endif // SMSEXTENSION_HH
