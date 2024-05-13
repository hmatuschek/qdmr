#ifndef SMSEXTENSION_HH
#define SMSEXTENSION_HH

#include "configobject.hh"
#include "interval.hh"

class SMSTemplate: public ConfigObject
{
  Q_OBJECT

  Q_PROPERTY(QString message READ message WRITE setMessage)

public:
  Q_INVOKABLE explicit SMSTemplate(QObject *parent = nullptr);

  ConfigItem *clone() const;

  const QString &message() const;
  void setMessage(const QString message);

protected:
  QString _message;
};


class SMSTemplates: public ConfigObjectList
{
  Q_OBJECT

public:
  Q_INVOKABLE SMSTemplates(QObject *parent=nullptr);

  ConfigItem *allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err=ErrorStack());
};


class SMSExtension : public ConfigExtension
{
  Q_OBJECT

  Q_PROPERTY(Format format READ format WRITE setFormat)
  Q_PROPERTY(SMSTemplates *smsTemplates READ smsTemplates)

public:
  enum class Format {
    Motorola, Hytera, DMR
  };
  Q_ENUM(Format)

public:
  Q_INVOKABLE explicit SMSExtension(QObject *parent = nullptr);

  ConfigItem *clone() const;

  Format format() const;
  void setFormat(Format format);

  SMSTemplates *smsTemplates() const;

protected:
  Format _format;
  SMSTemplates *_smsTemplates;
};

#endif // SMSEXTENSION_HH
