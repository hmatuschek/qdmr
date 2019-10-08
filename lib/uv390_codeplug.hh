#ifndef RT3S_GPS_CODEPLUG_HH
#define RT3S_GPS_CODEPLUG_HH

#include <QObject>

class RT3SGPSCodeplug : public QObject
{
  Q_OBJECT
public:
  explicit RT3SGPSCodeplug(QObject *parent = nullptr);

signals:

public slots:
};

#endif // RT3S_GPS_CODEPLUG_HH