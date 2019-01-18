#include "settings.hh"


Settings::Settings(QObject *parent)
  : QSettings(parent)
{
  // pass...
}

QDateTime
Settings::lastRepeaterUpdate() const {
  if (! contains("lastRepeaterUpdate"))
    return QDateTime();
  return value("lastRepeaterUpdate").toDateTime();
}

void
Settings::repeaterUpdated() {
  setValue("lastRepeaterUpdate", QDateTime::currentDateTime());
}

bool
Settings::repeaterUpdateNeeded(uint period) const {
  QDateTime last = lastRepeaterUpdate();
  if (! last.isValid())
    return true;
  QDateTime now = QDateTime::currentDateTime();
  return last.daysTo(now) >= period;
}
