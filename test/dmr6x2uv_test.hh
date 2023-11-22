#ifndef DMR6X2UVTEST_HH
#define DMR6X2UVTEST_HH


#include "libdmrconfigtest.hh"

class DMR6X2UVTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit DMR6X2UVTest(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // DMR6X2UVTEST_HH
