#include "smsextension.hh"


/* ********************************************************************************************* *
 * Implementation of SMSTemplate
 * ********************************************************************************************* */
SMSTemplate::SMSTemplate(QObject *parent)
  : ConfigObject{parent}, _message()
{
  // pass...
}

ConfigItem *
SMSTemplate::clone() const {
  ConfigItem *item = new SMSTemplate();
  if (! item->copy(*this)) {
    delete item;
    return nullptr;
  }
  return item;
}

const QString &
SMSTemplate::message() const {
  return _message;
}

void
SMSTemplate::setMessage(const QString message) {
  if (_message == message)
    return;
  _message = message;
  emit modified(this);
}



/* ********************************************************************************************* *
 * Implementation of SMSTemplates
 * ********************************************************************************************* */
SMSTemplates::SMSTemplates(QObject *parent)
  : ConfigObjectList(SMSTemplate::staticMetaObject, parent)
{
  // pass...
}

ConfigItem *
SMSTemplates::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(node); Q_UNUSED(ctx); Q_UNUSED(err);
  return new SMSTemplate();
}



/* ********************************************************************************************* *
 * Implementation of RadioSettingsExtension
 * ********************************************************************************************* */
SMSExtension::SMSExtension(QObject *parent)
  : ConfigExtension{parent}, _format(Format::DMR), _smsTemplates(new SMSTemplates(this))
{
  // pass...
}

SMSExtension::Format
SMSExtension::format() const {
  return _format;
}
void
SMSExtension::setFormat(Format format) {
  if (_format == format)
    return;
  _format = format;
  emit modified(this);
}

ConfigItem *
SMSExtension::clone() const {
  ConfigItem *item = new SMSExtension();
  if (! item->copy(*this)) {
    delete item;
    return nullptr;
  }
  return item;
}

SMSTemplates *
SMSExtension::smsTemplates() const {
  return _smsTemplates;
}

