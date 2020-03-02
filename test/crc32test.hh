#ifndef CRC32TEST_H
#define CRC32TEST_H

#include <QObject>

class CRC32Test : public QObject
{
  Q_OBJECT

public:
  explicit CRC32Test(QObject *parent = nullptr);

private slots:
  void testCRC32();
};

#endif // CRC32TEST_H
