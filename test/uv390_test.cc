#include "uv390_test.hh"
#include "config.hh"
#include "uv390_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

UV390Test::UV390Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
UV390Test::testBasicConfigEncoding() {
  ErrorStack err;
  UV390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
UV390Test::testBasicConfigDecoding() {
  ErrorStack err;
  UV390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT UV390: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT UV390: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
UV390Test::testChannelFrequency() {
  ErrorStack err;
  UV390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD-UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT MD-UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}

void
UV390Test::testSMSTemplates() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  SMSTemplate *sms0 = new SMSTemplate(); sms0->setName("SMS0"); sms0->setMessage("ABC");
  SMSTemplate *sms1 = new SMSTemplate(); sms1->setName("SMS1"); sms1->setMessage("XYZ");
  config.smsExtension()->smsTemplates()->add(sms0);
  config.smsExtension()->smsTemplates()->add(sms1);

  ErrorStack err;
  UV390Codeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD-UV390: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT MD-UV390: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(decoded.smsExtension()->smsTemplates()->count(), 2);
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(0)->name(), "SMS0");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(0)->message(), "ABC");
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(1)->name(), "SMS1");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(1)->message(), "XYZ");
}


QTEST_GUILESS_MAIN(UV390Test)

