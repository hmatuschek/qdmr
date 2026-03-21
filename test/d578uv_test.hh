#ifndef D578UVTEST_HH
#define D578UVTEST_HH


#include "libdmrconfigtest.hh"

class D578UVTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit D578UVTest(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
  void testARC4Encryption();
  void testMicGain(); // Retression test for #773
  void testChannelDataACK(); // Regression test for #813
  void testSettingsDisplayVolumeChangePrompt(); // Regression test for #813
  void testSettingsRoamingNotificationCount(); // Regression test for #813
};

#endif // D578UVTEST_HH
