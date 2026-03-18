#include "level.hh"


Level::Level()
  : _level(std::numeric_limits<unsigned int>::max())
{
  // pass...
}


QString
Level::format() const{
  if (isNull())
    return "off";
  else if (isInvalid())
    return "none";
  return QString::number(_level);
}


bool
Level::parse(const QString &value) {
  if ("off" == value.toLower().simplified()) {
    _level = 0;
    return true;
  }

  bool ok;
  _level = std::min(10U, value.toUInt(&ok));
  return ok;
}


