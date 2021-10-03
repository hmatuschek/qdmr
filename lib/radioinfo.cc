#include "radioinfo.hh"

QHash<QString, RadioInfo::Radio>
RadioInfo::_radiosByName = QHash<QString, RadioInfo::Radio>{
  {"opengd77", RadioInfo::OpenGD77},
  {"rd5r", RadioInfo::RD5R},
  {"gd77", RadioInfo::GD77},
  {"uv390", RadioInfo::UV390},
  {"rt3s", RadioInfo::RT3S},
  {"md2017", RadioInfo::MD2017},
  {"rt82", RadioInfo::RT82},
  {"d868uv", RadioInfo::D868UV},
  {"d868uve", RadioInfo::D868UVE},
  {"dmr6x2", RadioInfo::DMR6X2},
  {"d878uv", RadioInfo::D878UV},
  {"d878uv2", RadioInfo::D878UVII},
  {"d578uv", RadioInfo::D578UV},
};

QHash<unsigned, RadioInfo>
RadioInfo::_radiosById= QHash<unsigned, RadioInfo>{
  {RadioInfo::OpenGD77, RadioInfo(RadioInfo::OpenGD77, "OpenGD77", "OpenGD77 Project")},
  {RadioInfo::RD5R, RadioInfo(RadioInfo::RD5R, "RD5R", "Radioddity")},
  {RadioInfo::GD77, RadioInfo(RadioInfo::GD77, "GD77", "Radioddity")},
  {RadioInfo::UV390, RadioInfo(RadioInfo::UV390, "UV390", "TyT",
                               QList<RadioInfo>{
                                 RadioInfo(RadioInfo::RT3S, "RT3S", "Retevis")
                               })},
  {RadioInfo::MD2017, RadioInfo(RadioInfo::MD2017, "MD2017", "TyT",
                                QList<RadioInfo>{
                                  RadioInfo(RadioInfo::RT82, "RT82", "Retevis")
                                })},
  {RadioInfo::D868UV, RadioInfo(RadioInfo::D868UV, "D868UV", "AnyTone")},
  {RadioInfo::D868UVE, RadioInfo(RadioInfo::D868UVE, "D868UVE", "AnyTone",
                                 QList<RadioInfo>{
                                   RadioInfo(RadioInfo::DMR6X2, "DMR6X2", "BTech")
                                 })},
  {RadioInfo::D878UV, RadioInfo(RadioInfo::D878UV, "D878UV", "AnyTone")},
  {RadioInfo::D878UVII, RadioInfo(RadioInfo::D878UVII, "D878UV2", "AnyTone")},
  {RadioInfo::D578UV, RadioInfo(RadioInfo::D578UV, "D578UV", "AnyTone")}
};

RadioInfo::RadioInfo(
    Radio radio, const QString &name, const QString manufacturer, const QList<RadioInfo> &alias)
  : _radio(radio), _key(name.toLower()), _name(name), _manufacturer(manufacturer), _alias(alias)
{
  // pass...
}

RadioInfo::RadioInfo(
    Radio radio, const QString &key, const QString &name, const QString manufacturer, const QList<RadioInfo> &alias)
  : _radio(radio), _key(key), _name(name), _manufacturer(manufacturer), _alias(alias)
{
  // pass...
}

RadioInfo::RadioInfo()
  : _key("")
{
  // pass...
}

RadioInfo::RadioInfo(const RadioInfo &other)
  : _radio(other._radio), _key(other._key), _name(other._name), _manufacturer(other._manufacturer),
    _alias(other._alias)
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
