#include "rd5r_test.hh"
#include "config.hh"
#include "rd5r.hh"
#include "rd5r_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

RD5RTest::RD5RTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
RD5RTest::testBasicConfigEncoding() {
  ErrorStack err;
  RD5RCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity RD5R: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
RD5RTest::testBasicConfigDecoding() {
  ErrorStack err;
  RD5RCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity RD5R: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity RD5R: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
RD5RTest::testChannelFrequency() {
  ErrorStack err;
  RD5RCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity RD5R: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity RD5R: %1")
          .arg(err.format()).toStdString().c_str());
  }

  /*QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           123456780ULL);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           1234567890ULL);*/
}

void
RD5RTest::testSMSTemplates() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  SMSTemplate *sms0 = new SMSTemplate(); sms0->setName("SMS0"); sms0->setMessage("ABC");
  SMSTemplate *sms1 = new SMSTemplate(); sms1->setName("SMS1"); sms1->setMessage("XYZ");
  config.smsExtension()->smsTemplates()->add(sms0);
  config.smsExtension()->smsTemplates()->add(sms1);

  ErrorStack err;
  RD5RCodeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity RD5R: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity RD5R: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(decoded.smsExtension()->smsTemplates()->count(), 2);
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(0)->name(), "SMS0");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(0)->message(), "ABC");
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(1)->name(), "SMS1");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(1)->message(), "XYZ");
}


QTEST_GUILESS_MAIN(RD5RTest)

