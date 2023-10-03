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

QTEST_GUILESS_MAIN(D878UVTest)

