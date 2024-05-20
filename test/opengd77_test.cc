#include "opengd77_test.hh"
#include "config.hh"
#include "opengd77_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>
#include <iostream>
#include "logger.hh"


OpenGD77Test::OpenGD77Test(QObject *parent)
  : UnitTestBase(parent), _stderr(stderr)
{
  Logger::get().addHandler(new StreamLogHandler(_stderr, LogMessage::DEBUG));
}

void
OpenGD77Test::testBasicConfigEncoding() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
OpenGD77Test::testBasicConfigDecoding() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
OpenGD77Test::testChannelFrequency() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity RD5R: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}

void
OpenGD77Test::testSMSTemplates() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  SMSTemplate *sms0 = new SMSTemplate(); sms0->setName("SMS0"); sms0->setMessage("ABC");
  SMSTemplate *sms1 = new SMSTemplate(); sms1->setName("SMS1"); sms1->setMessage("XYZ");
  config.smsExtension()->smsTemplates()->add(sms0);
  config.smsExtension()->smsTemplates()->add(sms1);

  ErrorStack err;
  OpenGD77Codeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // For now, messages are not encoded
  QCOMPARE(decoded.smsExtension()->smsTemplates()->count(), 2);
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(0)->name(), "SMS0");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(0)->message(), "ABC");
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(1)->name(), "SMS1");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(1)->message(), "XYZ");
}


QTEST_GUILESS_MAIN(OpenGD77Test)

