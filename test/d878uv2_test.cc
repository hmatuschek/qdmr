#include "d878uv2_test.hh"
#include "config.hh"
#include "d878uv2.hh"
#include "d878uv2_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D878UV2Test::D878UV2Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
D878UV2Test::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D878UV2Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UVII: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UV2Test::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D878UV2Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UVII: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UV2Test::testChannelFrequency() {
  ErrorStack err;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  D868UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D878UV II: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D878UV II: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}

void
D878UV2Test::testKeyFunctions() {
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
  D878UV2Codeplug codeplug;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Decode
  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: %1")
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
D878UV2Test::testAESEncryption() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/aes_encryption.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file:\n%1")
            .arg(err.format(" ")).toStdString().c_str());
  }

  D878UVCodeplug codeplug;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  // Verify existence of key
  QVERIFY(nullptr != decoded.commercialExtension());
  QCOMPARE(decoded.commercialExtension()->encryptionKeys()->count(), 1);
  QCOMPARE(decoded.commercialExtension()->encryptionKeys()->key(0)->key(), QByteArray::fromHex("11223344556677889900AABBCCDDEEFF"));

  // Verify link to channel
  QVERIFY(nullptr != decoded.channelList()->channel(0)->as<DMRChannel>()->commercialExtension());
  QCOMPARE(decoded.channelList()->channel(0)->as<DMRChannel>()->commercialExtension()->encryptionKey(),
           decoded.commercialExtension()->encryptionKeys()->key(0));
}


QTEST_GUILESS_MAIN(D878UV2Test)

