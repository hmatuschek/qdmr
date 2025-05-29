#include "d868uve_test.hh"
#include "config.hh"
#include "d868uv.hh"
#include "d868uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D868UVETest::D868UVETest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
D868UVETest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug = false;
  D868UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D868UVETest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D868UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D868UVETest::testChannelFrequency() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D868UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D868UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D868UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}


void
D868UVETest::testAutoRepeaterOffset() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/anytone_auto_repeater_extension.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QVERIFY2(config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  AnytoneAutoRepeaterSettingsExtension *ext = config.settings()->anytoneExtension()->autoRepeaterSettings();
  // There should be two offset frequencies
  QCOMPARE(ext->offsets()->count(), 2);
  QCOMPARE(ext->offsets()->get(0)->as<AnytoneAutoRepeaterOffset>()->offset().inHz(),  600000);
  QCOMPARE(ext->offsets()->get(1)->as<AnytoneAutoRepeaterOffset>()->offset().inHz(), 7600000);
  // check if VHF and UHF frequency offsets are correct
  QVERIFY(! ext->vhfRef()->isNull()); QVERIFY(! ext->uhfRef()->isNull());
  QCOMPARE(ext->vhfRef()->as<AnytoneAutoRepeaterOffset>(), ext->offsets()->get(0)->as<AnytoneAutoRepeaterOffset>());
  QCOMPARE(ext->uhfRef()->as<AnytoneAutoRepeaterOffset>(), ext->offsets()->get(1)->as<AnytoneAutoRepeaterOffset>());

  // Encode
  D868UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Decode
  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Compare
  QVERIFY2(comp_config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  ext = comp_config.settings()->anytoneExtension()->autoRepeaterSettings();

  // There should be two offset frequencies
  QCOMPARE(ext->offsets()->count(), 2);
  QCOMPARE(ext->offsets()->get(0)->as<AnytoneAutoRepeaterOffset>()->offset().inHz(),  600000);
  QCOMPARE(ext->offsets()->get(1)->as<AnytoneAutoRepeaterOffset>()->offset().inHz(), 7600000);

  // check if VHF and UHF frequency offsets are correct
  QVERIFY(! ext->vhfRef()->isNull()); QVERIFY(! ext->uhfRef()->isNull());
  QCOMPARE(ext->vhfRef()->as<AnytoneAutoRepeaterOffset>(), ext->offsets()->get(0)->as<AnytoneAutoRepeaterOffset>());
  QCOMPARE(ext->uhfRef()->as<AnytoneAutoRepeaterOffset>(), ext->offsets()->get(1)->as<AnytoneAutoRepeaterOffset>());
}

void
D868UVETest::testDTMFContacts() {
  // Assemble config
  Config base;

  DTMFContact *contact0 = new DTMFContact();
  contact0->setName("Contact 0"); contact0->setNumber("0123456789ABCD#*");
  base.contacts()->add(contact0);

  // Encode
  D868UVCodeplug codeplug; ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  if (! codeplug.encode(&base, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }


  // Decode
  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check contacts
  QCOMPARE(comp_config.contacts()->count(), 1);
  QVERIFY(comp_config.contacts()->get(0)->is<DTMFContact>());
  QCOMPARE(comp_config.contacts()->get(0)->as<DTMFContact>()->name(), "Contact 0");
  // Contacts are limited to 14 digit numbers.
  QCOMPARE(comp_config.contacts()->get(0)->as<DTMFContact>()->number(), "0123456789ABCD");
}


void
D868UVETest::testSMSTemplates() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  SMSTemplate *sms0 = new SMSTemplate(); sms0->setName("SMS0"); sms0->setMessage("ABC");
  SMSTemplate *sms1 = new SMSTemplate(); sms1->setName("SMS1"); sms1->setMessage("XYZ");
  config.smsExtension()->smsTemplates()->add(sms0);
  config.smsExtension()->smsTemplates()->add(sms1);

  ErrorStack err;
  D868UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // For now, messages are not encoded
  QCOMPARE(decoded.smsExtension()->smsTemplates()->count(), 2);
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(0)->name(), "SMS0");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(0)->message(), "ABC");
  //QCOMPARE_NE(decoded.smsExtension()->smsTemplates()->message(1)->name(), "SMS1");
  QCOMPARE(decoded.smsExtension()->smsTemplates()->message(1)->message(), "XYZ");
}


void
D868UVETest::testRegressionSMSTemplateOffset() {
  // Regression test for issue #469, message index offset error if more than one bank is used
  // (i.e., more than 8 SMS).
  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));
  for (int i=0; i<9; i++) {
    SMSTemplate *sms = new SMSTemplate(); sms->setName(QString("SMS%1").arg(i)); sms->setMessage("ABC");
    config.smsExtension()->smsTemplates()->add(sms);
  }

  ErrorStack err;
  D868UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // For now, messages are not encoded
  QCOMPARE(decoded.smsExtension()->smsTemplates()->count(), 9);
}


void
D868UVETest::testRegressionSMSCount() {
  // Regression test for issue #482 (tries to encode too many SMS)
    Config config;
    config.radioIDs()->add(new DMRRadioID("ID", 1234567));
    for (int i=0; i<101; i++) {
        SMSTemplate *sms = new SMSTemplate(); sms->setName(QString("SMS%1").arg(i)); sms->setMessage("ABC");
        config.smsExtension()->smsTemplates()->add(sms);
    }

    ErrorStack err;
    D868UVCodeplug codeplug;
    Codeplug::Flags flags; flags.updateCodePlug=false;
    if (! codeplug.encode(&config, flags, err)) {
        QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
                  .arg(err.format()).toStdString().c_str());
    }
}


void
D868UVETest::testRegressionDefaultChannel() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
  config.settings()->setAnytoneExtension(new AnytoneSettingsExtension());
  config.settings()->anytoneExtension()->bootSettings()->zoneA()->set(config.zones()->zone(0));
  config.settings()->anytoneExtension()->bootSettings()->channelA()->set(config.zones()->zone(0)->A()->get(0));
  config.settings()->anytoneExtension()->bootSettings()->zoneB()->set(config.zones()->zone(1));
  config.settings()->anytoneExtension()->bootSettings()->channelB()->set(config.zones()->zone(1)->A()->get(0));
  config.settings()->anytoneExtension()->bootSettings()->enableDefaultChannel(true);

  D868UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
      QFAIL(QString("Cannot decode codeplug for AnyTone AT-D868UVE: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QVERIFY(decoded.settings()->anytoneExtension());
  QVERIFY(decoded.settings()->anytoneExtension()->bootSettings()->defaultChannelEnabled());
  QCOMPARE(decoded.settings()->anytoneExtension()->bootSettings()->zoneA()->as<Zone>(),
           decoded.zones()->zone(0));
  QCOMPARE(decoded.settings()->anytoneExtension()->bootSettings()->channelA()->as<Channel>()->name(),
           decoded.zones()->zone(0)->A()->get(0)->name());
  QCOMPARE(decoded.settings()->anytoneExtension()->bootSettings()->zoneB()->as<Zone>(),
           decoded.zones()->zone(1));
  QCOMPARE(decoded.settings()->anytoneExtension()->bootSettings()->channelB()->as<Channel>()->name(),
           decoded.zones()->zone(1)->A()->get(0)->name());
}


QTEST_GUILESS_MAIN(D868UVETest)

