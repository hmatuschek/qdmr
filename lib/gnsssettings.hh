#ifndef GNSSSETTINGS_HH
#define GNSSSETTINGS_HH

#include "configobject.hh"
#include <QGeoCoordinate>


class GNSSSettings : public ConfigItem
{
  Q_OBJECT

  Q_PROPERTY(QGeoCoordinate fixedPosition READ fixedPosition WRITE setFixedPosition FINAL)
  Q_PROPERTY(Systems systems READ systems WRITE setSystems FINAL)
  Q_PROPERTY(Units units READ units WRITE setUnits FINAL)

public:
  enum class Units {
    Metric, Archaic
  };
  Q_ENUM(Units)

  enum class System {
    Fixed = 0, GPS = 1, Glonass = 2, Galileo = 4,  Beidou = 8
  };
  Q_DECLARE_FLAGS(Systems, System)
  Q_FLAGS(Systems)


public:
  explicit GNSSSettings(QObject *parent = nullptr);

  const QGeoCoordinate &fixedPosition() const;
  void setFixedPosition(const QGeoCoordinate &pos);

  Systems systems() const;
  void setSystems(Systems systems);

  Units units() const;
  void setUnits(Units units);

protected:
  QGeoCoordinate _fixedPosition;
  Systems _systems;
  Units _units;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GNSSSettings::Systems)


#endif // GNSSSETTINGS_HH
