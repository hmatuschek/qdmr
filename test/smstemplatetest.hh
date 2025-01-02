#ifndef SMSTEMPLATETEST_HH
#define SMSTEMPLATETEST_HH

#include "libdmrconfigtest.hh"

class SMSTemplateTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit SMSTemplateTest(QObject *parent=nullptr);

private slots:
  void serializationTest();

  /** Regression test for #511. */
  void testMessageDuplication();

};

#endif // SMSTEMPLATETEST_HH
