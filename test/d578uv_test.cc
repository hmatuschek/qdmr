#include "d578uv_test.hh"
#include "config.hh"
#include "d578uv_codeplug.hh"
#include "errorstack.hh"
#include <QTest>


D578UVTest::D578UVTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
D578UVTest::encodeDecode(Config &input, Config &output) {
  ErrorStack err;
  D578UVCodeplug codeplug;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  if (! codeplug.encode(&input, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
  if (! codeplug.decode(&output, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
}


void
D578UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D578UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D578UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D578UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D578UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D578UVTest::testChannelFrequency() {
  ErrorStack err;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D578UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D578UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D578UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}


void
D578UVTest::testChannelDataACK() {
  ErrorStack err;
  Config config;

  if (! config.readYAML(":/data/anytone_channel_data_ack.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 2);
  QVERIFY(config.channelList()->channel(0)->is<DMRChannel>());
  QCOMPARE(config.channelList()->channel(0)->as<DMRChannel>()
             ->extended()->dataConfirm(), false);
  QVERIFY(config.channelList()->channel(1)->is<DMRChannel>());
  QCOMPARE(config.channelList()->channel(1)->as<DMRChannel>()
             ->extended()->dataConfirm(), true);

  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D578UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(testConfig.channelList()->channel(0)->as<DMRChannel>()
             ->extended()->dataConfirm(), false);
  QCOMPARE(testConfig.channelList()->channel(1)->as<DMRChannel>()
             ->extended()->dataConfirm(), true);
}


void
D578UVTest::testSettingsDisplayVolumeChangePrompt() {
  ErrorStack err;
  Config config;

  if (! config.readYAML(":/data/anytone_settings_display.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.settings()->anytoneExtension()->displaySettings()->volumeChangePromptEnabled(), false);

  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D578UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(testConfig.settings()->anytoneExtension()->displaySettings()->volumeChangePromptEnabled(), false);
}

void
D578UVTest::testMicGain() {
  Config copy, config; config.copy(_basicConfig);
  config.settings()->audio()->setMicGain(Level::fromValue(10));
  encodeDecode(config, copy);
  QCOMPARE(copy.settings()->audio()->micGain(), Level::fromValue(10));
  QVERIFY(copy.settings()->anytoneExtension());
  // FM mic gain enabled only if it differs from DMR gain
  QVERIFY(! copy.settings()->audio()->fmMicGainEnabled());

  QList<QPair<unsigned int,unsigned int>> pairs = {{1,1}, {2,1}, {3,1}, {4,3}, {5,3}, {6,5}, {7,5}, {8,7}, {9,7}, {10,10}};
  for (auto pair: pairs) {
    config.settings()->audio()->setMicGain(Level::fromValue(pair.first));
    encodeDecode(config, copy);
    QCOMPARE(copy.settings()->audio()->micGain(), Level::fromValue(pair.second));
  }
}


void
D578UVTest::testSettingsRoamingNotificationCount() {
  ErrorStack err;
  Config config;

  if (! config.readYAML(":/data/anytone_settings_roaming.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.settings()->anytoneExtension()->roamingSettings()->notificationCount(), 1);

  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D578UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(testConfig.settings()->anytoneExtension()->roamingSettings()->notificationCount(), 1);
}

void
D578UVTest::testIdleToneEnable() {
  ErrorStack err;
  Config config, testConfig;

  if (! config.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }

  config.settings()->tone()->setChannelIdle(Channel::Type::FM);
  encodeDecode(config, testConfig);
  QCOMPARE(testConfig.settings()->tone()->channelIdle(), Channel::Type::FM);

  config.settings()->tone()->setChannelIdle(Channel::Type::DMR);
  encodeDecode(config, testConfig);
  QCOMPARE(testConfig.settings()->tone()->channelIdle(), Channel::Type::DMR);

  config.settings()->tone()->setChannelIdle(Channel::Type::FM|Channel::Type::DMR);
  encodeDecode(config, testConfig);
  QCOMPARE(testConfig.settings()->tone()->channelIdle(), Channel::Type::FM|Channel::Type::DMR);

  config.settings()->tone()->setChannelIdle(Channel::Type::None);
  encodeDecode(config, testConfig);
  QCOMPARE(testConfig.settings()->tone()->channelIdle(), Channel::Type::None);
}

void
D578UVTest::testARC4Encryption() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/arc4_encryption.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file:\n%1")
            .arg(err.format(" ")).toStdString().c_str());
  }

  D578UVCodeplug codeplug;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D578UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  // Verify existence of key
  QVERIFY(nullptr != decoded.commercialExtension());
  QCOMPARE(decoded.commercialExtension()->encryptionKeys()->count(), 1);
  QCOMPARE(decoded.commercialExtension()->encryptionKeys()->key(0)->key(), QByteArray::fromHex("1122334455"));

  // Verify link to channel
  QVERIFY(nullptr != decoded.channelList()->channel(0)->as<DMRChannel>()->commercialExtension());
  QCOMPARE(decoded.channelList()->channel(0)->as<DMRChannel>()->commercialExtension()->encryptionKey(),
           decoded.commercialExtension()->encryptionKeys()->key(0));
}


void
D578UVTest::testAMChannel() {
  // Load config from file
  ErrorStack err;
  Config config, testConfig;
  if (! config.readYAML(":/data/am_channel_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file:\n%1")
            .arg(err.format(" ")).toStdString().c_str());
  }

  encodeDecode(config, testConfig);

  // There must be an AM and a FM channel. The order is not defined.
  QCOMPARE(testConfig.channelList()->count(), 2);
  auto am = testConfig.channelList()->channel(0)->is<AMChannel>() ?
              testConfig.channelList()->channel(0)->as<AMChannel>() :
              testConfig.channelList()->channel(1)->as<AMChannel>();
  QCOMPARE(am->name(), config.channelList()->channel(0)->as<AMChannel>()->name());
  QCOMPARE(am->rxFrequency(), config.channelList()->channel(0)->as<AMChannel>()->rxFrequency());
}


QTEST_GUILESS_MAIN(D578UVTest)

