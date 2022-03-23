#ifndef ANYTONEEXTENSION_HH
#define ANYTONEEXTENSION_HH

#include "configobject.hh"

/** Implements the AnyTone extensions for zones.
 * @ingroup anytone */
class AnytoneZoneExtension : public ConfigExtension
{
  Q_OBJECT

  /** If @c true, the zone is hidden in the menu. */
  Q_PROPERTY(bool hidden READ hidden WRITE enableHidden)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit AnytoneZoneExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns @c true if the zone is hidden. */
  bool hidden() const;
  /** Enables/disables hidden zone. */
  void enableHidden(bool enable);

protected:
  /** If @c true, the zone is hidden in the menu. */
  bool _hidden;
};

/** Implements the AnyTone contact extension.
 * @ingroup anytone */
class AnytoneContactExtension: public ConfigExtension
{
  Q_OBJECT

  /** Overrides the ring flag, allows to set None, Ring and Online. */
  Q_PROPERTY(AlertType alertType READ alertType WRITE setAlertType)

public:
  /** Possible ring-tone types. */
  enum class AlertType {
    None = 0,                   ///< Alert disabled.
    Ring = 1,                   ///< Ring tone.
    Online = 2                  ///< WTF?
  };
  Q_ENUM(AlertType)

public:
  /** Default constructor. */
  Q_INVOKABLE explicit AnytoneContactExtension(QObject *parent=nullptr);

  ConfigItem *clone() const;

  /** Returns the alert type for the contact. */
  AlertType alertType() const;
  /** Sets the alert type for the contact. */
  void setAlertType(AlertType type);

protected:
  /** Holds the alert type for the contact. */
  AlertType _alertType;
};

#endif // ANYTONEEXTENSION_HH
