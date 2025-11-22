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
    ch->setRXTone(SelectiveCall(67.0));
    ch->setTXTone(SelectiveCall());
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 2");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(SelectiveCall());
    ch->setTXTone(SelectiveCall(67.0));
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 3");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(SelectiveCall(23,false));
    ch->setTXTone(SelectiveCall());
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 4");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(SelectiveCall());
    ch->setTXTone(SelectiveCall(23, false));
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 5");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(SelectiveCall(23, true));
    ch->setTXTone(SelectiveCall());
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 6");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(SelectiveCall());
    ch->setTXTone(SelectiveCall(23, true));
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
    QCOMPARE(compare.channelList()->channel(i)->as<FMChannel>()->rxTone(),
             config.channelList()->channel(i)->as<FMChannel>()->rxTone());
    QCOMPARE(compare.channelList()->channel(i)->as<FMChannel>()->txTone(),
             config.channelList()->channel(i)->as<FMChannel>()->txTone());
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
    QVERIFY(GD73Limits().verifyConfig(&config, issues));
    QStringList status;
    for (int i=0; i<issues.count(); i++)
      if (RadioLimitIssue::Severity::Critical == issues.message(i).severity())
        status.append(issues.message(i).format());
    QCOMPARE(status.count(), 0);
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
      if (RadioLimitIssue::Severity::Critical == issues.message(i).severity())
        status.append(issues.message(i).format());
    QVERIFY2(1 == status.count(), status.join("\n").toLocal8Bit().constData());
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
      if (RadioLimitIssue::Severity::Critical == issues.message(i).severity())
        status.append(issues.message(i).format());
    QVERIFY2(1 == status.count(), status.join("\n").toLocal8Bit().constData());
  }
}


void
GD73Test::testChannelTypeEcoding() {
  Config config, new_config;
  GD73Codeplug codeplug;
  ErrorStack err;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));

  {
    DMRChannel *ch = new DMRChannel();
    ch->setName("Channel 1");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    config.channelList()->add(ch);
  }

  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  new_config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 1");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    new_config.channelList()->add(ch);
  }

  // Re-encode in the same binary codeplug
  if (! codeplug.encode(&new_config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config compare;
  if (! codeplug.decode(&compare, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(compare.channelList()->count(), 1);
  QVERIFY(compare.channelList()->channel(0)->is<FMChannel>());
}


void
GD73Test::testPowerEcoding() {
  Config config;
  GD73Codeplug codeplug;
  ErrorStack err;

  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  DMRChannel *ch = new DMRChannel();
  ch->setName("Channel 1");
  ch->setRXFrequency(Frequency::fromMHz(144.0));
  ch->setTXFrequency(Frequency::fromMHz(144.6));
  ch->setPower(Channel::Power::High);
  config.channelList()->add(ch);

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
  QCOMPARE(compare.channelList()->channel(0)->power(), Channel::Power::High);
}

void
GD73Test::testSquelchEcoding() {
  Config config;
  GD73Codeplug codeplug;
  ErrorStack err;

  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  config.settings()->setSquelch(1);

  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config compare;
  if (! codeplug.decode(&compare, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(compare.settings()->squelch(), 1);
}


QTEST_GUILESS_MAIN(GD73Test)

