#ifndef CSVREADER_HH
#define CSVREADER_HH

#include <QObject>

class CSVReader : public QObject
{
  Q_OBJECT
public:
  explicit CSVReader(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CSVREADER_HH