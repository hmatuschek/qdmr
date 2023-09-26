#ifndef D878UVTEST_HH
#define D878UVTEST_HH


#include "libdmrconfigtest.hh"

class D878UVTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit D878UVTest(QObject *parent = nullptr);

private slots:
  void initTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();

  void testAnalogMicGain();
  void testRoaming();
  void testHangTime();

protected:
  Config _micGainConfig;
  QTextStream _stderr;
};

#endif // D878UVTEST_HH
