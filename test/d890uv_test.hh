#ifndef D890UVTEST_HH
#define D890UVTEST_HH

#include "libdmrconfigtest.hh"

class D890UVTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit D890UVTest(QObject *parent = nullptr);

private slots:
  void testDecodeFactory();
  void testBasicConfigRoundTrip();
  void testDualZoneRoundTrip();
  void testEncodeKeepsUnmodelledRegions();
  void testScanListTemplate();
  void testContactIDMap();
  void testRefuseNoZones();
};

#endif // D890UVTEST_HH
