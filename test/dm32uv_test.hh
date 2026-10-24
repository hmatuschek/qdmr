#ifndef DR1801TEST_HH
#define DR1801TEST_HH


#include "libdmrconfigtest.hh"

class DM32UVTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit DM32UVTest(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testBasicConfigReencoding();
  void testProstProcessingOfEmptyCodeplug();
  void testAMChannelReencoding();
  /** Regression test for #873 */
  void testChannelBankEncoding();
  /** Regression test fo #967 */
  void testCTCSSHigherFrequencies();
  /** All 250 zones need the 9th zone bank. */
  void testZoneBankEncoding();
};

#endif // DR1801TEST_HH
