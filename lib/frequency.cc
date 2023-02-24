#include "frequency.hh"
#include <QRegularExpression>

Frequency::Frequency(unsigned long long Hz)
  : _frequency(Hz)
{
  // pass...
}

Frequency::Frequency()
  : _frequency(0)
{
  // pass...
}

Frequency::Frequency(const Frequency &other)
  : _frequency(other._frequency)
{
  // pass...
}

Frequency &
Frequency::operator =(const Frequency &other) {
  _frequency = other._frequency;
  return *this;
}

QString
Frequency::format(Format f) const {
  switch (f) {
  case Format::Automatic:
    if (10000ULL > _frequency)
      return format(Format::Hz);
    else if (10000000ULL > _frequency)
      return format(Format::kHz);
    else if (10000000000ULL > _frequency)
      return format(Format::MHz);
    return format(Format::GHz);
  case Format::Hz:
    return QString("%1 Hz").arg(inHz());
  case Format::kHz:
    return QString("%1 kHz").arg(inkHz());
  case Format::MHz:
    return QString("%1 MHz").arg(inMHz());
  case Format::GHz:
    return QString("%1 GHz").arg(inGHz());
  }
  return "";
}

bool
Frequency::parse(const QString &value) {
  QRegularExpression re("\\w*([0-9]*)(?:.([0-9]*))\\w*([kMG]?Hz)\\w*");
  QRegularExpressionMatch match = re.match(value);
  if (! match.isValid())
    return false;

  bool isFloat = match.capturedLength(2);
  bool hasUnit = match.capturedLength(3);
  QString unit = match.captured(3);
  QString decimals = match.captured(2);

  _frequency = match.captured(1).toUInt();

  if (("Hz" == unit) || (!isFloat && !hasUnit))
    return true;

  if ("kHz" == unit) {
    _frequency *= 1000ULL;
    unsigned long long factor = 100ULL;
    for (int i=0; i<std::min(3, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
  } else if (("MHz"==unit) || (isFloat && !hasUnit)) {
    _frequency *= 1000000ULL;
    unsigned long long factor = 100000ULL;
    for (int i=0; i<std::min(6, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
  } else if ("GHz"==unit) {
    _frequency *= 1000000000ULL;
    unsigned long long factor = 100000000;
    for (int i=0; i<std::min(9, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
  }

  return true;
}
