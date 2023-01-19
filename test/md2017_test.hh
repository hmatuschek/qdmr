#ifndef MD2017TEST_HH
#define MD2017TEST_HH


#include <QObject>
#include "config.hh"

class MD2017Test : public QObject
{
  Q_OBJECT

public:
  explicit MD2017Test(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // MD2017TEST_HH
