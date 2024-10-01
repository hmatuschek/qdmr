#include "smstemplatetest.hh"

SMSTemplateTest::SMSTemplateTest(QObject *parent)
  : UnitTestBase{parent}
{
  // pass...
}

void
SMSTemplateTest::serializationTest() {
  Config config;

  auto msg1 = new SMSTemplate();
  msg1->setName("Message 1");
  msg1->setMessage("Some message");
  config.smsExtension()->smsTemplates()->add(msg1);

  ErrorStack err;
  QString buffer;
  QTextStream stream(&buffer);
  if (! config.toYAML(stream, err))
    QFAIL(err.format().toLocal8Bit().constData());

  Config comp_config;
  Config::Context ctx;
  YAML::Node doc = YAML::Load(buffer.toStdString());
  if (! comp_config.parse(doc, ctx, err))
    QFAIL(err.format().toLocal8Bit().constData());
  if (! comp_config.link(doc, ctx, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(comp_config.smsExtension()->smsTemplates()->count(),
           config.smsExtension()->smsTemplates()->count());
  QCOMPARE(comp_config.smsExtension()->smsTemplates()->message(0)->name(),
           config.smsExtension()->smsTemplates()->message(0)->name());
  QCOMPARE(comp_config.smsExtension()->smsTemplates()->message(0)->message(),
           config.smsExtension()->smsTemplates()->message(0)->message());
}


QTEST_GUILESS_MAIN(SMSTemplateTest)
