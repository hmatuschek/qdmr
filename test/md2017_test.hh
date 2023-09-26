#ifndef MD2017TEST_HH
#define MD2017TEST_HH


#include "libdmrconfigtest.hh"

class MD2017Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit MD2017Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // MD2017TEST_HH
