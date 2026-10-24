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
  /** The contact index as the CPS writes it. */
  void testContactIndexEncoding();
  /** The radio holds more than one radio ID. */
  void testRadioIdLimits();
  /** Blocks the radio has no page for get the lowest free page. */
  void testAddressMapAllocation();
  /** Disabled encryption keys (the CPS's default "Encrypt 1") survive an encode. */
  void testEncryptionKeysKeepDisabled();
  /** A scan list holds 16 channels. */
  void testScanListChannels();
  /** The CPS's default APRS destination adds no contact on decode and survives an encode. */
  void testAPRSDefaultDestination();
  /** Names are written as the CPS does: text and a terminating 0, the rest is kept. */
  void testNamesKeepTrailingBytes();
  /** Channels take the radio's middle power level. */
  void testChannelPowerMid();
};

#endif // DR1801TEST_HH
