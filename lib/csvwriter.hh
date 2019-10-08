#ifndef CSVWRITER_HH
#define CSVWRITER_HH

#include <QObject>

class CSVWriter : public QObject
{
  Q_OBJECT
public:
  explicit CSVWriter(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CSVWRITER_HH