#ifndef GD73TEST_HH
#define GD73TEST_HH


#include "libdmrconfigtest.hh"

class GD73Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit GD73Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
  void testSMSTemplates();
  void testFMSignaling();
};

#endif // GD73TEST_HH
