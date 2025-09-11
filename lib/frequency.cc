#include "frequency.hh"
#include "logger.hh"
#include <QRegularExpression>


/* ******************************************************************************************** *
 * Implementation of FrequencyBase
 * ******************************************************************************************** */
FrequencyBase::FrequencyBase(const FrequencyBase &other)
  : _frequency(other._frequency)
{
  // pass...
}

FrequencyBase &
FrequencyBase::operator =(const FrequencyBase &other) {
  _frequency = other._frequency;
  return *this;
}

QString
FrequencyBase::format(Format f) const {
  switch (f) {
  case Format::Automatic:
    if (10000LL > std::abs(_frequency))
      return format(Format::Hz);
    else if (10000000LL > std::abs(_frequency))
      return format(Format::kHz);
    else if (10000000000LL > std::abs(_frequency))
      return format(Format::MHz);
    return format(Format::GHz);
  case Format::Hz:
    return QString("%1 Hz").arg(inHz());
  case Format::kHz:
    return QString("%1 kHz").arg(inkHz(), 0, 'g', 6);
  case Format::MHz:
    return QString("%1 MHz").arg(inMHz(), 0, 'g', 9);
  case Format::GHz:
    return QString("%1 GHz").arg(inGHz(), 0, 'g', 12);
  }
  return "";
}

bool
FrequencyBase::parse(const QString &value) {
  QRegularExpression re(R"(\s*(+-|)\s*([0-9]+)(?:\.([0-9]*)|)\s*([kMG]?Hz|)\s*)");
  QRegularExpressionMatch match = re.match(value);
  if (! match.hasMatch())
    return false;

  bool isNegative = ("-" == match.captured(1));
  bool hasUnit = match.capturedLength(4);
  QString unit = match.captured(4);
  QString decimals = match.captured(3);
  QString leading = match.captured(2);
  _frequency = leading.toUInt();

  if ("Hz" == unit)
    return true;

  if ("kHz" == unit) {
    _frequency *= 1000ULL;
    unsigned long long factor = 100ULL;
    for (int i=0; i<std::min((qsizetype)3, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
    // Rounding to proper Hz
    if ((decimals.size()>3) && (decimals[3].digitValue()>=5))
      _frequency += 1;
  } else if (("MHz"==unit) || (!hasUnit)) {
    _frequency *= 1000000ULL;
    unsigned long long factor = 100000ULL;
    for (int i=0; i<std::min((qsizetype)6, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
    // Rounding to proper Hz
    if ((decimals.size()>6) && (decimals[6].digitValue()>=5))
      _frequency += 1;
  } else if ("GHz"==unit) {
    _frequency *= 1000000000ULL;
    unsigned long long factor = 100000000;
    for (int i=0; i<std::min((qsizetype)9, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
    // Rounding to proper Hz
    if ((decimals.size()>9) && (decimals[9].digitValue()>=5))
      _frequency+=1;
  }

  if (isNegative)
    _frequency *= -1;

  return true;
}



/* ******************************************************************************************** *
 * Implementation of FrequencyOffset
 * ******************************************************************************************** */
FrequencyOffset::FrequencyOffset()
  : FrequencyBase(0)
{
  // pass...
}

FrequencyOffset::FrequencyOffset(const FrequencyOffset &other)
  : FrequencyBase(other)
{
  // pass...
}

FrequencyOffset &
FrequencyOffset::operator =(const FrequencyOffset &other) {
  FrequencyBase::operator=(other);
  return *this;
}

FrequencyOffset
FrequencyOffset::fromString(const QString &freq) {
  FrequencyOffset f;
  if (! f.parse(freq)) {
    logWarn() << "Cannot parse offset '" << freq << "'.";
  }
  return f;
}



/* ******************************************************************************************** *
 * Implementation of Frequency
 * ******************************************************************************************** */
Frequency::Frequency(quint64 Hz)
  : FrequencyBase(Hz)
{
  // pass...
}

Frequency::Frequency()
  : FrequencyBase(0)
{
  // pass...
}

Frequency::Frequency(const Frequency &other)
  : FrequencyBase(other)
{
  // pass...
}

Frequency &
Frequency::operator =(const Frequency &other) {
  FrequencyBase::operator=(other);
  return *this;
}

Frequency
Frequency::operator+(const FrequencyOffset &offset) const {
  if ((0 > offset.inHz()) && (std::abs(offset.inHz()) > _frequency))
    return Frequency();
  return Frequency(_frequency + offset.inHz());
}

bool
Frequency::parse(const QString &value) {
  QRegularExpression re(R"(\s*([0-9]+)(?:\.([0-9]*)|)\s*([kMG]?Hz|)\s*)");
  QRegularExpressionMatch match = re.match(value);
  if (! match.hasMatch())
    return false;
  return FrequencyBase::parse(value);
}

Frequency
Frequency::fromString(const QString &freq) {
  Frequency f;
  if (! f.parse(freq)) {
    logWarn() << "Cannot parse frequency '" << freq << "'.";
  }
  return f;
}
