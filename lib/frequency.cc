#include "frequency.hh"
#include "logger.hh"
#include <QRegularExpression>


inline const QString HzString = QStringLiteral("Hz");
inline const QString KHzString = QStringLiteral("MHz");
inline const QString MHzString = QStringLiteral("MHz");
inline const QString GHzString = QStringLiteral("GHz");

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
FrequencyBase::format(Unit unit) const {
  switch (unit) {
  case Unit::Auto:
    if (10000LL > std::abs(_frequency)) {
      return format(Unit::Hz);
    } else if (10000000LL > std::abs(_frequency)) {
      return format(Unit::kHz);
    } else if (10000000000LL > std::abs(_frequency)) {
      return format(Unit::MHz);
    }
    return format(Unit::GHz);

  case Unit::Hz:
    return QString("%1 Hz").arg(inHz());

  case Unit::kHz:
    return QString("%1 kHz").arg(inkHz(), 0, 'g', 6);

  case Unit::MHz:
    return QString("%1 MHz").arg(inMHz(), 0, 'g', 9);

  case Unit::GHz:
    return QString("%1 GHz").arg(inGHz(), 0, 'g', 12);

  }
  return "";
}

bool
FrequencyBase::parse(const QString &value, Qt::CaseSensitivity caseSensitivity) {

  QRegularExpression::PatternOption patternOption = caseSensitivity == Qt::CaseInsensitive
    ? QRegularExpression::PatternOption::CaseInsensitiveOption : QRegularExpression::PatternOption::NoPatternOption;

  static QRegularExpression re(R"(\s*(\+|-)?\s*([0-9]+)(?:\.([0-9]*)|)\s*([kMG]|[kMG]?Hz|)\s*)", patternOption);
  QRegularExpressionMatch match = re.match(value);

  if (!match.hasMatch())
    return false;

  bool isNegative = ("-" == match.captured(1));
  auto unit = unitFromString(match.captured(4));
  QString decimals = match.captured(3);
  QString leading = match.captured(2);
  _frequency = leading.toUInt();

  switch (unit)  {
  case Unit::kHz: {
    _frequency *= 1000ULL;
    unsigned long long factor = 100ULL;
    for (int i=0; i<std::min((qsizetype)3, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
    // Rounding to proper Hz
    if ((decimals.size()>3) && (decimals[3].digitValue()>=5))
      _frequency += 1;
  }
  break;
  case Unit::MHz: {
    _frequency *= 1000000ULL;
    unsigned long long factor = 100000ULL;
    for (int i=0; i<std::min((qsizetype)6, decimals.size()); i++) {
      _frequency += decimals[i].digitValue()*factor;
      factor /= 10;
    }
    // Rounding to proper Hz
    if ((decimals.size()>6) && (decimals[6].digitValue()>=5))
      _frequency += 1;
  }
  break;
  case Unit::GHz: {
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
  break;
  case Unit::Hz:
  default:
    ; // Do Nothing
  }

  if (isNegative)
    _frequency = -_frequency;

  return true;
}

FrequencyBase::Unit FrequencyBase::unit() const {
  qint64 absFreq = std::abs(_frequency);
  if (absFreq >= 1000000000LL)
    return Unit::GHz;
  else if (absFreq >= 1000000LL)
    return Unit::MHz;
  else if (absFreq >= 1000LL)
    return Unit::kHz;
  else
    return Unit::Hz;
}

FrequencyBase::Unit FrequencyBase::unitFromString(const QString& input) const {
  Unit result;
  if (input.length() == 0) {
    return Unit::MHz;
  }

  auto multiplier = input[0].toLower();

  if (multiplier == 'k') {
    result = Unit::kHz;
  } else if (multiplier == 'm') {
    result = Unit::MHz;
  } else if (multiplier == 'g') {
    result = Unit::GHz;
  } else {
    result = Unit::Hz;
  }
  return result;
}

bool FrequencyBase::isMultipleOf(Unit u) const {
  switch(u) {
  case Unit::kHz: return (_frequency % 1000LL) == 0;
  case Unit::MHz: return (_frequency % 1000000LL) == 0;
  case Unit::GHz: return (_frequency % 1000000000LL) == 0;
  case Unit::Hz:
    //fallthrough
  default: return true;
  }
  return false; // Unreachable
}

QString FrequencyBase::unitName(Unit unit) {
  switch(unit) {
  case Unit::kHz: return KHzString;
  case Unit::MHz: return MHzString;
  case Unit::GHz: return GHzString;
  case Unit::Hz:
    //fallthrough
  default: return HzString;
  }
  return QString();
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

FrequencyOffset
Frequency::operator-(const Frequency &other) const {
  return FrequencyOffset(_frequency - other._frequency);
}

Frequency
Frequency::fromString(const QString &freq) {
  Frequency f;
  if (! f.parse(freq)) {
    logWarn() << "Cannot parse frequency '" << freq << "'.";
  }
  return f;
}
