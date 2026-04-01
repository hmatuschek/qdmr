#include "d168uv_codeplug.hh"
#include "config.hh"

#include <QTimeZone>
#include <QtEndian>

/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug::GeneralSettingsElement::TimeZone
 * ******************************************************************************************** */
QVector<QTimeZone>
D168UVCodeplug::GeneralSettingsElement::TimeZone::_timeZones = {
  QTimeZone(-43200), QTimeZone(-39600), QTimeZone(-36000), QTimeZone(-32400), QTimeZone(-28800),
  QTimeZone(-25200), QTimeZone(-21600), QTimeZone(-18000), QTimeZone(-14400), QTimeZone(-12600),
  QTimeZone(-10800), QTimeZone(- 7200), QTimeZone(- 3600), QTimeZone(     0), QTimeZone(  3600),
  QTimeZone(  7200), QTimeZone( 10800), QTimeZone( 12600), QTimeZone( 14400), QTimeZone( 16200),
  QTimeZone( 18000), QTimeZone( 19800), QTimeZone( 20700), QTimeZone( 21600), QTimeZone( 25200),
  QTimeZone( 28600), QTimeZone( 30600), QTimeZone( 32400), QTimeZone( 36000), QTimeZone( 39600),
  QTimeZone( 43200), QTimeZone( 46800) };

QTimeZone
D168UVCodeplug::GeneralSettingsElement::TimeZone::decode(uint8_t code) {
  if (code >= _timeZones.size())
    return _timeZones.back();
  return _timeZones.at(code);
}
uint8_t
D168UVCodeplug::GeneralSettingsElement::TimeZone::encode(const QTimeZone &zone) {
  if (! _timeZones.contains(zone))
    return 13; //<- UTC
  return _timeZones.indexOf(zone);
}


/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
D168UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D878UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}


QTimeZone
D168UVCodeplug::GeneralSettingsElement::gpsTimeZone() const {
  return TimeZone::decode(getUInt8(Offset::gpsTimeZone()));
}

void
D168UVCodeplug::GeneralSettingsElement::setGPSTimeZone(const QTimeZone &zone) {
  setUInt8(Offset::gpsTimeZone(), TimeZone::encode(zone)); // <- Set to UTC
}



/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug
 * ******************************************************************************************** */
D168UVCodeplug::D168UVCodeplug(const QString &label, QObject *parent)
  : D878UVCodeplug(label, parent)
{
  // pass...
}

D168UVCodeplug::D168UVCodeplug(QObject *parent)
  : D878UVCodeplug("AnyTone AT-D168UV Codeplug", parent)
{
  // pass...
}



bool
D168UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).fromConfig(flags, ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).fromConfig(flags, ctx);
  return true;
}
bool
D168UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).updateConfig(ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).updateConfig(ctx);
  return true;
}
bool
D168UVCodeplug::linkGeneralSettings(Context &ctx, const ErrorStack &err) {
  if (! GeneralSettingsElement(data(Offset::settings())).linkSettings(ctx.config()->settings(), ctx, err)) {
    errMsg(err) << "Cannot link general settings extension.";
    return false;
  }

  if (! ExtendedSettingsElement(data(Offset::settingsExtension())).linkConfig(ctx, err)) {
    errMsg(err) << "Cannot link general settings extension.";
    return false;
  }

  return true;
}
