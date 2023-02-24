#include "interval.hh"
#include <QRegularExpression>

Interval::Interval()
  : _duration(0)
{
  // pass...
}

Interval::Interval(unsigned long long ms)
  : _duration(ms)
{
  // pass...
}

Interval::Interval(const Interval &other)
  : _duration(other._duration)
{
  // pass...
}

QString
Interval::format(Format f) const {
  if (0 == _duration)
    return "0";
  switch (f) {
  case Format::Automatic:
    if (0 == _duration % 60000UL)
      return format(Format::Minutes);
    else if (0 == _duration % 1000UL)
      return format(Format::Seconds);
    return format(Format::Milliseconds);
  case Format::Minutes:
    return QString("%1 min").arg(_duration/60000UL);
  case Format::Seconds:
    return QString("%1 s").arg(_duration/1000UL);
  case Format::Milliseconds:
    return QString("%1 ms").arg(_duration);
  }
  return QString("%1 ms").arg(_duration);
}

bool
Interval::parse(const QString &value) {
  QRegularExpression ex("\\w*([0-9])+\\w*(|min|s|ms)\\w*");
  QRegularExpressionMatch match = ex.match(value);
  if (! match.isValid())
    return false;

  bool hasUnit = match.capturedLength(2);
  QString unit = match.captured(2);
  QString dur = match.captured(1);

  if (hasUnit && ("s"==unit))
    _duration = dur.toULongLong()*1000ULL;
  else if (hasUnit && ("s"==unit))
    _duration = dur.toULongLong()*60000ULL;
  else
    _duration = dur.toULongLong();
  return true;
}

