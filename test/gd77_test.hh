#ifndef GD77TEST_HH
#define GD77TEST_HH


#include "libdmrconfigtest.hh"

class GD77Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit GD77Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // GD77TEST_HH
