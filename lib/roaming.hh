#ifndef ROAMINGZONE_HH
#define ROAMINGZONE_HH

#include <QObject>

class RoamingZone : public QObject
{
  Q_OBJECT
public:
  explicit RoamingZone(QObject *parent = nullptr);

signals:

};

#endif // ROAMINGZONE_HH
