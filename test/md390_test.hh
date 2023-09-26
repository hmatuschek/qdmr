#ifndef MD390TEST_HH
#define MD390TEST_HH


#include "libdmrconfigtest.hh"

class MD390Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit MD390Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // MD390TEST_HH
