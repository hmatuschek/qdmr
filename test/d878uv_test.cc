#include "d878uv_test.hh"
#include "config.hh"
#include "d878uv.hh"
#include "d878uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>
#include "logger.hh"

D878UVTest::D878UVTest(QObject *parent)
  : UnitTestBase(parent), _stderr(stderr)
{
  Logger::get().addHandler(new StreamLogHandler(_stderr, LogMessage::DEBUG));
}

void
D878UVTest::initTestCase() {
  UnitTestBase::initTestCase();

  ErrorStack err;
  if (! _micGainConfig.readYAML(":/data/anytone_audio_settings_extension.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::testAnalogMicGain() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_micGainConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QVERIFY(config.settings()->anytoneExtension()->audioSettings()->fmMicGainEnabled());
  QCOMPARE(config.settings()->anytoneExtension()->audioSettings()->fmMicGain(), 6);
}

void
D878UVTest::testChannelFrequency() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D868UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}

void
D878UVTest::testRoaming() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_roamingConfig, flags, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.roamingChannels()->count(), 3);
  QCOMPARE(config.roamingZones()->count(), 2);
  QCOMPARE(config.roamingZones()->count(), 2);

  QCOMPARE(config.roamingZones()->get(0)->as<RoamingZone>()->count(), 2);
  QCOMPARE(config.roamingZones()->get(0)->as<RoamingZone>()->channel(0),
           config.roamingChannels()->get(0)->as<RoamingChannel>());
  QCOMPARE(config.roamingZones()->get(0)->as<RoamingZone>()->channel(1),
           config.roamingChannels()->get(1)->as<RoamingChannel>());

  QCOMPARE(config.roamingZones()->get(1)->as<RoamingZone>()->count(), 2);
  QCOMPARE(config.roamingZones()->get(1)->as<RoamingZone>()->channel(0),
           config.roamingChannels()->get(0)->as<RoamingChannel>());
  QCOMPARE(config.roamingZones()->get(1)->as<RoamingZone>()->channel(1),
           config.roamingChannels()->get(2)->as<RoamingChannel>());
}

void
D878UVTest::testHangTime() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/anytone_call_hangtime.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QVERIFY2(config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  AnytoneDMRSettingsExtension *ext = config.settings()->anytoneExtension()->dmrSettings();

  QCOMPARE(ext->privateCallHangTime().seconds(), 4ULL);
  QCOMPARE(ext->groupCallHangTime().seconds(), 5ULL);

  // Encode
  D878UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  Config *intermediate = codeplug.preprocess(&config, err);
  if (nullptr == intermediate) {
    QFAIL(QString("Cannot prepare codeplug for AnyTone AT-D878UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  if (! codeplug.encode(intermediate, flags, err)) {
    delete intermediate;
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }
  delete intermediate;

  // Decode
  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QVERIFY2(comp_config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  ext = comp_config.settings()->anytoneExtension()->dmrSettings();

  QCOMPARE(ext->privateCallHangTime().seconds(), 4ULL);
  QCOMPARE(ext->groupCallHangTime().seconds(), 5ULL);

}

void
D878UVTest::testKeyFunctions() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/anytone_key_function.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QVERIFY2(config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  AnytoneKeySettingsExtension *ext = config.settings()->anytoneExtension()->keySettings();

  QCOMPARE(ext->funcKey1Short(), AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel);
  QCOMPARE(ext->funcKey1Long(), AnytoneKeySettingsExtension::KeyFunction::SubChannel);
  QCOMPARE(ext->funcKey2Short(), AnytoneKeySettingsExtension::KeyFunction::ToggleVFO);
  QCOMPARE(ext->funcKey2Long(), AnytoneKeySettingsExtension::KeyFunction::ChannelType);
  QCOMPARE(ext->funcKeyAShort(), AnytoneKeySettingsExtension::KeyFunction::Off);
  QCOMPARE(ext->funcKeyALong(), AnytoneKeySettingsExtension::KeyFunction::Encryption);
  QCOMPARE(ext->funcKeyBShort(), AnytoneKeySettingsExtension::KeyFunction::Voltage);
  QCOMPARE(ext->funcKeyBLong(), AnytoneKeySettingsExtension::KeyFunction::Call);
  QCOMPARE(ext->funcKeyBShort(), AnytoneKeySettingsExtension::KeyFunction::Voltage);
  QCOMPARE(ext->funcKeyBLong(), AnytoneKeySettingsExtension::KeyFunction::Call);

  // Encode
  D878UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Decode
  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QVERIFY2(comp_config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  ext = comp_config.settings()->anytoneExtension()->keySettings();

  QCOMPARE(ext->funcKey1Short(), AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel);
  QCOMPARE(ext->funcKey1Long(), AnytoneKeySettingsExtension::KeyFunction::SubChannel);
  QCOMPARE(ext->funcKey2Short(), AnytoneKeySettingsExtension::KeyFunction::ToggleVFO);
  QCOMPARE(ext->funcKey2Long(), AnytoneKeySettingsExtension::KeyFunction::ChannelType);
  QCOMPARE(ext->funcKeyAShort(), AnytoneKeySettingsExtension::KeyFunction::Off);
  QCOMPARE(ext->funcKeyALong(), AnytoneKeySettingsExtension::KeyFunction::Encryption);
  QCOMPARE(ext->funcKeyBShort(), AnytoneKeySettingsExtension::KeyFunction::Voltage);
  QCOMPARE(ext->funcKeyBLong(), AnytoneKeySettingsExtension::KeyFunction::Call);
  QCOMPARE(ext->funcKeyBShort(), AnytoneKeySettingsExtension::KeyFunction::Voltage);
  QCOMPARE(ext->funcKeyBLong(), AnytoneKeySettingsExtension::KeyFunction::Call);
}


void
D878UVTest::testFMAPRSSettings() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/fm_aprs_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1\n")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QCOMPARE(config.posSystems()->count(), 1);
  QVERIFY(config.posSystems()->get(0)->is<APRSSystem>());

  APRSSystem *aprs = config.posSystems()->get(0)->as<APRSSystem>();
  QCOMPARE(aprs->source(), "DM3MAT"); QCOMPARE(aprs->srcSSID(), 7);
  QCOMPARE(aprs->destination(), "APAT81"); QCOMPARE(aprs->destSSID(), 0);
  QCOMPARE(aprs->path(), "WIDE1-1,WIDE2-1");
  QCOMPARE(aprs->period(), 300);

  // Encode
  D878UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  Config *intermediate = codeplug.preprocess(&config, err);
  if (nullptr == intermediate) {
    QFAIL(QString("Cannot prepare codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  if (! codeplug.encode(intermediate, flags, err)) {
    delete intermediate;
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
  delete intermediate;

  // Decode
  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QCOMPARE(comp_config.posSystems()->count(), 1);
  QVERIFY(comp_config.posSystems()->get(0)->is<APRSSystem>());

  APRSSystem *comp_aprs = comp_config.posSystems()->get(0)->as<APRSSystem>();
  QCOMPARE(comp_aprs->source(), aprs->source()); QCOMPARE(comp_aprs->srcSSID(), aprs->srcSSID());
  QCOMPARE(comp_aprs->destination(), aprs->destination()); QCOMPARE(comp_aprs->destSSID(), aprs->destSSID());
  QCOMPARE(comp_aprs->path(), aprs->path());
  QCOMPARE(comp_aprs->period(), aprs->period());
}



void
D878UVTest::testRegressionDefaultChannel() {
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
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
      QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
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


QTEST_GUILESS_MAIN(D878UVTest)

