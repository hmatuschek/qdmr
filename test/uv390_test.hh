#ifndef UV390TEST_HH
#define UV390TEST_HH


#include "libdmrconfigtest.hh"

class UV390Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit UV390Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // UV390TEST_HH
