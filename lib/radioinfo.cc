#include "radioinfo.hh"

#include "opengd77.hh"
#include "gd77.hh"
#include "rd5r.hh"
#include "md390.hh"
#include "uv390.hh"
#include "md2017.hh"
#include "d868uv.hh"
#include "d878uv.hh"
#include "d878uv2.hh"
#include "d578uv.hh"
#include "dm1701.hh"
#include "cdr300uv.hh"


QHash<QString, RadioInfo::Radio>
RadioInfo::_radiosByName = QHash<QString, RadioInfo::Radio>{
  {"opengd77", RadioInfo::OpenGD77},
  {"rd5r",     RadioInfo::RD5R},
  {"gd77",     RadioInfo::GD77},
  {"md390",    RadioInfo::MD390},
  {"uv380",    RadioInfo::UV380},
  {"rt8",      RadioInfo::RT8},
  {"uv390",    RadioInfo::UV390},
  {"rt3s",     RadioInfo::RT3S},
  {"md2017",   RadioInfo::MD2017},
  {"rt82",     RadioInfo::RT82},
  {"dm1701",   RadioInfo::DM1701},
  {"rt84",     RadioInfo::RT84},
  {"d868uv",   RadioInfo::D868UV},
  {"d868uve",  RadioInfo::D868UVE},
  {"dmr6x2",   RadioInfo::DMR6X2},
  {"d878uv",   RadioInfo::D878UV},
  {"d878uv2",  RadioInfo::D878UVII},
  {"d578uv",   RadioInfo::D578UV},
  {"cdr300uv", RadioInfo::CDR300UV},
  {"rt73",     RadioInfo::RT73},
  {"db25d",    RadioInfo::DB25D}
};

QHash<unsigned, RadioInfo>
RadioInfo::_radiosById = QHash<unsigned, RadioInfo>{
  {RadioInfo::OpenGD77, OpenGD77::defaultRadioInfo()},
  {RadioInfo::RD5R,     RD5R::defaultRadioInfo()},
  {RadioInfo::GD77,     GD77::defaultRadioInfo()},
  {RadioInfo::MD390,    MD390::defaultRadioInfo()},
  {RadioInfo::UV390,    UV390::defaultRadioInfo()},
  {RadioInfo::MD2017,   MD2017::defaultRadioInfo()},
  {RadioInfo::DM1701,   DM1701::defaultRadioInfo()},
  {RadioInfo::D868UVE,  D868UV::defaultRadioInfo()},
  {RadioInfo::D878UV,   D878UV::defaultRadioInfo()},
  {RadioInfo::D878UVII, D878UV2::defaultRadioInfo()},
  {RadioInfo::D578UV,   D578UV::defaultRadioInfo()},
  {RadioInfo::CDR300UV, CDR300UV::defaultRadioInfo()}
};



/* ********************************************************************************************* *
 * Implementation of RadioInfo
 * ********************************************************************************************* */
RadioInfo::RadioInfo(Radio radio, const QString &name, const QString manufacturer,
                     const USBDeviceInfo &interface, const QList<RadioInfo> &alias)
  : _radio(radio), _key(name.toLower()), _name(name), _manufacturer(manufacturer), _alias(alias),
    _interface(interface)
{
  // pass...
}

RadioInfo::RadioInfo(Radio radio, const QString &key, const QString &name,
                     const QString manufacturer, const USBDeviceInfo &interface,
                     const QList<RadioInfo> &alias)
  : _radio(radio), _key(key), _name(name), _manufacturer(manufacturer), _alias(alias),
    _interface(interface)
{
  // pass...
}

RadioInfo::RadioInfo()
  : _key("")
{
  // pass...
}

bool
RadioInfo::isValid() const {
  return ! _key.isEmpty();
}

const QString &
RadioInfo::key() const {
  return _key;
}

const QString &
RadioInfo::name() const {
  return _name;
}

const QString &
RadioInfo::manufactuer() const {
  return _manufacturer;
}

const USBDeviceInfo &
RadioInfo::interface() const {
  return _interface;
}

bool
RadioInfo::hasAlias() const {
  return 0 != _alias.count();
}
const QList<RadioInfo> &
RadioInfo::alias() const {
  return _alias;
}

RadioInfo::Radio
RadioInfo::id() const {
  return _radio;
}

bool
RadioInfo::hasRadioKey(const QString &key) {
  return _radiosByName.contains(key);
}

RadioInfo
RadioInfo::byKey(const QString &key) {
  if (! hasRadioKey(key))
    return RadioInfo();
  return byID(_radiosByName[key]);
}

RadioInfo
RadioInfo::byID(Radio radio) {
  return _radiosById[radio];
}

QList<RadioInfo>
RadioInfo::allRadios(bool flat) {
  QList<RadioInfo> radios;
  QHash<unsigned, RadioInfo>::const_iterator it = _radiosById.constBegin();
  for (; it!=_radiosById.constEnd(); it++) {
    radios.push_back(*it);
    if (flat)
      radios.append(it->_alias);
  }
  std::sort(radios.begin(), radios.end(), [](const RadioInfo &a, const RadioInfo &b) {
    return a.id()<b.id();
  });
  return radios;
}

QList<RadioInfo>
RadioInfo::allRadios(const USBDeviceInfo &interface, bool flat) {
  QList<RadioInfo> radios;
  QHash<unsigned, RadioInfo>::const_iterator it = _radiosById.constBegin();
  for (; it!=_radiosById.constEnd(); it++) {
    if (it->interface() != interface)
      continue;
    radios.push_back(*it);
    if (flat)
      radios.append(it->_alias);
  }
  std::sort(radios.begin(), radios.end(), [](const RadioInfo &a, const RadioInfo &b) {
    return a.id()<b.id();
  });
  return radios;
}
