#ifndef DFUFILE_HH
#define DFUFILE_HH

#include <QObject>

class DFUFile : public QObject
{
  Q_OBJECT
public:
  explicit DFUFile(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DFUFILE_HH