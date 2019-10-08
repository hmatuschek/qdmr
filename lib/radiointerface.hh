#ifndef RADIOINFERFACE_HH
#define RADIOINFERFACE_HH

#include <QObject>

class RadioInferface : public QObject
{
  Q_OBJECT
public:
  explicit RadioInferface(QObject *parent = nullptr);

signals:

public slots:
};

#endif // RADIOINFERFACE_HH