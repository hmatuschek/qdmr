#include "gd73_test.hh"
#include "config.hh"
#include "gd73.hh"
#include "gd73_codeplug.hh"
#include "gd73_limits.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

GD73Test::GD73Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
GD73Test::testBasicConfigEncoding() {
  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
GD73Test::testBasicConfigDecoding() {
  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
GD73Test::testChannelFrequency() {
  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}


void
GD73Test::testSMSTemplates() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  SMSTemplate *sms0 = new SMSTemplate(); sms0->setName("SMS0"); sms0->setMessage("ABC");
  SMSTemplate *sms1 = new SMSTemplate(); sms1->setName("SMS1"); sms1->setMessage("XYZ");
  config.smsExtension()->smsTemplates()->add(sms0);
  config.smsExtension()->smsTemplates()->add(sms1);

  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(decoded.smsExtension()->smsTemplates()->count(), 2);
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(0)->name(), "SMS0");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(0)->message(), "ABC");
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(1)->name(), "SMS1");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(1)->message(), "XYZ");
}


void
GD73Test::testFMSignaling() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 1");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::CTCSS_67_0Hz);
    ch->setTXTone(Signaling::SIGNALING_NONE);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 2");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::SIGNALING_NONE);
    ch->setTXTone(Signaling::CTCSS_67_0Hz);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 3");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::DCS_023N);
    ch->setTXTone(Signaling::SIGNALING_NONE);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 4");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::SIGNALING_NONE);
    ch->setTXTone(Signaling::DCS_023N);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 5");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::DCS_023I);
    ch->setTXTone(Signaling::SIGNALING_NONE);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 6");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::SIGNALING_NONE);
    ch->setTXTone(Signaling::DCS_023I);
    config.channelList()->add(ch);
  }

  GD73Codeplug codeplug;
  ErrorStack err;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config compare;
  if (! codeplug.decode(&compare, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(compare.channelList()->count(), 6);
  for (int i=0; i<compare.channelList()->count(); i++) {
    QVERIFY(compare.channelList()->channel(i)->is<FMChannel>());
    QCOMPARE((unsigned int)compare.channelList()->channel(i)->as<FMChannel>()->rxTone(),
             (unsigned int)config.channelList()->channel(i)->as<FMChannel>()->rxTone());
    QCOMPARE((unsigned int)compare.channelList()->channel(i)->as<FMChannel>()->txTone(),
             (unsigned int)config.channelList()->channel(i)->as<FMChannel>()->txTone());
  }
}

void
GD73Test::testEncryption() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));

  DMRContact *cnt = new DMRContact(DMRContact::AllCall, "All Call", 0, false);
  config.contacts()->add(cnt);

  BasicEncryptionKey *key = new BasicEncryptionKey();
  key->setName("Key 1");
  key->fromHex("1234");
  config.commercialExtension()->encryptionKeys()->add(key);
  QCOMPARE(key->toHex(), "1234");

  auto *ext = new CommercialChannelExtension();
  ext->setEncryptionKey(key);

  DMRChannel *ch = new DMRChannel();
  ch->setName("Ch");
  ch->setTXContactObj(cnt);
  ch->setTXFrequency(Frequency::fromMHz(440));
  ch->setRXFrequency(Frequency::fromMHz(440));
  ch->setCommercialExtension(ext);
  config.channelList()->add(ch);

  GD73Codeplug codeplug;
  ErrorStack err;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config compare;
  if (! codeplug.decode(&compare, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(compare.channelList()->count(), 1);
  QVERIFY(compare.channelList()->channel(0)->is<DMRChannel>());
  QVERIFY(compare.channelList()->channel(0)->as<DMRChannel>()->commercialExtension());
  QCOMPARE(compare.channelList()->channel(0)->as<DMRChannel>()->commercialExtension()->encryptionKey()->key(),
           config.channelList()->channel(0)->as<DMRChannel>()->commercialExtension()->encryptionKey()->key());
}


void
GD73Test::testEncryptionLimits() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));

  DMRContact *cnt = new DMRContact(DMRContact::GroupCall, "GroupCall", 1234, false);
  config.contacts()->add(cnt);

  RXGroupList *grpLst = new RXGroupList("GrpLst");
  grpLst->addContact(cnt);
  config.rxGroupLists()->add(grpLst);

  BasicEncryptionKey *key = new BasicEncryptionKey();
  key->setName("Key 1");
  key->fromHex("1234");
  config.commercialExtension()->encryptionKeys()->add(key);
  QCOMPARE(key->toHex(), "1234");

  auto *ext = new CommercialChannelExtension();
  ext->setEncryptionKey(key);

  DMRChannel *ch = new DMRChannel();
  ch->setName("Ch");
  ch->setTXContactObj(cnt);
  ch->setGroupListObj(grpLst);
  ch->setTXFrequency(Frequency::fromMHz(440));
  ch->setRXFrequency(Frequency::fromMHz(440));
  ch->setCommercialExtension(ext);
  config.channelList()->add(ch);

  Zone *zone = new Zone("Zone");
  zone->A()->add(ch);
  config.zones()->add(zone);

  {
    RadioLimitContext issues;
    GD73Limits().verifyConfig(&config, issues);
    QStringList status;
    for (int i=0; i<issues.count(); i++)
      status.append(issues.message(i).format());
    QVERIFY2(1 == issues.count(), status.join("\n").toLocal8Bit().constData());
  }

  // Add wrong key type
  AESEncryptionKey *key2 = new AESEncryptionKey();
  key2->setName("Key 2");
  key2->fromHex("1234567890abcdef1234567890abcdef");
  config.commercialExtension()->encryptionKeys()->add(key2);

  {
    RadioLimitContext issues;
    GD73Limits().verifyConfig(&config, issues);
    QStringList status;
    for (int i=0; i<issues.count(); i++)
      status.append(issues.message(i).format());
    QVERIFY2(2 == issues.count(), status.join("\n").toLocal8Bit().constData());
  }

  // add a larger key
  config.commercialExtension()->encryptionKeys()->del(key2);
  BasicEncryptionKey *key3 = new BasicEncryptionKey();
  key3->setName("Key 2");
  key3->fromHex("1234567890");
  config.commercialExtension()->encryptionKeys()->add(key3);
  QCOMPARE(key3->toHex(), "1234567890");

  {
    RadioLimitContext issues;
    GD73Limits().verifyConfig(&config, issues);
    QStringList status;
    for (int i=0; i<issues.count(); i++)
      status.append(issues.message(i).format());
    QVERIFY2(2 == issues.count(), status.join("\n").toLocal8Bit().constData());
  }
}

QTEST_GUILESS_MAIN(GD73Test)

