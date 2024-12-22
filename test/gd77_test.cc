#include "gd77_test.hh"
#include "config.hh"
#include "gd77.hh"
#include "gd77_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

GD77Test::GD77Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
GD77Test::testBasicConfigEncoding() {
  ErrorStack err;
  GD77Codeplug codeplug;
  codeplug.clear();

  auto intermediate = codeplug.preprocess(&_basicConfig, err);
  if (nullptr == intermediate)
    QFAIL(err.format().toLocal8Bit().constData());

  if (! codeplug.encode(intermediate, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  delete intermediate;
}

void
GD77Test::testBasicConfigDecoding() {
  ErrorStack err;
  GD77Codeplug codeplug;
  codeplug.clear();

  auto intermediate = codeplug.preprocess(&_basicConfig, err);
  if (nullptr == intermediate)
    QFAIL(err.format().toLocal8Bit().constData());

  if (! codeplug.encode(intermediate, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
  delete intermediate;

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
  codeplug.clear();
}

void
GD77Test::testChannelFrequency() {
  ErrorStack err;
  GD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}

void
GD77Test::testSMSTemplates() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  SMSTemplate *sms0 = new SMSTemplate(); sms0->setName("SMS0"); sms0->setMessage("ABC");
  SMSTemplate *sms1 = new SMSTemplate(); sms1->setName("SMS1"); sms1->setMessage("XYZ");
  config.smsExtension()->smsTemplates()->add(sms0);
  config.smsExtension()->smsTemplates()->add(sms1);

  ErrorStack err;
  GD77Codeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(decoded.smsExtension()->smsTemplates()->count(), 2);
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(0)->name(), "SMS0");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(0)->message(), "ABC");
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(1)->name(), "SMS1");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(1)->message(), "XYZ");
}

QTEST_GUILESS_MAIN(GD77Test)

