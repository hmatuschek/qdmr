#ifndef D868UVETEST_HH
#define D868UVETEST_HH


#include "libdmrconfigtest.hh"

class D868UVETest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit D868UVETest(QObject *parent = nullptr);

protected:
  void encodeDecode(Config &input, Config &output);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
  void testAutoRepeaterOffset();
  void testDTMFContacts();
  void testSMSTemplates();
  void testBasicEncryption();

  void testRegressionSMSTemplateOffset();
  void testRegressionSMSCount();
  void testRegressionDefaultChannel();
  void testChannelDataACK(); ///< Regression test for #813
  void testMicGain();
};

#endif // D878UV2TEST_HH
