#ifndef DM1701TEST_HH
#define DM1701TEST_HH


#include "libdmrconfigtest.hh"

class DM1701Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit DM1701Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // DM1701TEST_HH
