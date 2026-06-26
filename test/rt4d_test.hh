#ifndef LIBDMRCONF_RT4D_TEST_HH
#define LIBDMRCONF_RT4D_TEST_HH


#include "libdmrconfigtest.hh"

class RT4DTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit RT4DTest(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testBasicConfigReencoding();
  void testAMChannelReencoding();
};

#endif // LIBDMRCONF_RT4D_TEST_HH
