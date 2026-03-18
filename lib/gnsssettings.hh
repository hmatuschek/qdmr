#ifndef GNSSSETTINGS_HH
#define GNSSSETTINGS_HH

#include "configobject.hh"
#include <QGeoCoordinate>

/** Some common global GNSS settings. */
class GNSSSettings : public ConfigExtension
{
  Q_OBJECT

  Q_CLASSINFO("description", "Some common global GNSS settings.");

  Q_CLASSINFO("fixedPositionEnabledDescription", "If enabled, the fixed position is used instead of the GNSS position.")
  Q_PROPERTY(bool fixedPositionEnabled READ fixedPositionEnabled WRITE enableFixedPosition FINAL)

  Q_CLASSINFO("fixedPositionDescription", "Some fixed position (locator).")
  Q_PROPERTY(QString fixedPosition READ fixedPositionLocator WRITE setFixedPositionLocator FINAL)

  Q_CLASSINFO("systemsDescription", "Enabled GNSSs.")
  Q_PROPERTY(Systems systems READ systems WRITE setSystems FINAL)

  Q_CLASSINFO("unitsDescrption", "Specifies unit system.")
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

  ConfigItem *clone() const override;

  bool fixedPositionEnabled() const;
  void enableFixedPosition(bool use);

  const QGeoCoordinate &fixedPosition() const;
  void setFixedPosition(const QGeoCoordinate &pos);
  QString fixedPositionLocator() const;
  void setFixedPositionLocator(const QString &locator);

  Systems systems() const;
  void setSystems(Systems systems);

  Units units() const;
  void setUnits(Units units);

protected:
  bool _fixedPositionEnabled;
  QGeoCoordinate _fixedPosition;
  Systems _systems;
  Units _units;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GNSSSettings::Systems)

namespace YAML
{
  /** Implements the conversion to and from YAML::Node. */
  template<>
  struct convert<QGeoCoordinate>
  {
    /** Serializes the frequency. */
    static Node encode(const QGeoCoordinate& rhs) {
      if (! rhs.isValid())
        return Node(YAML::Null);

      Node list;
      list["longitude"] = rhs.longitude();
      list["latitude"]  = rhs.latitude();
      if (std::isfinite(rhs.altitude()))
        list["altitude"]  = rhs.altitude();
      return list;
    }

    /** Parses the frequency. */
    static bool decode(const Node& node, QGeoCoordinate& rhs) {
      if (node.IsNull()) {
        rhs = QGeoCoordinate();
        return true;
      }
      if (! node.IsMap())
        return false;
      if ((!node["longitude"]) || (!node["longitude"].IsScalar()))
        return false;
      if ((!node["latitude"]) || (!node["latitude"].IsScalar()))
        return false;
      rhs.setLongitude(node["longitude"].as<double>(std::numeric_limits<double>::quiet_NaN()));
      rhs.setLatitude(node["latitude"].as<double>(std::numeric_limits<double>::quiet_NaN()));
      if (node["altitude"] && node["altitude"].IsScalar())
        rhs.setAltitude(node["altitude"].as<double>(std::numeric_limits<double>::quiet_NaN()));
      return true;
    }
  };
}

#endif // GNSSSETTINGS_HH
