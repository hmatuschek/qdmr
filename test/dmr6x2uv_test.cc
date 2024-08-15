#include "dmr6x2uv_test.hh"
#include "config.hh"
#include "dmr6x2uv.hh"
#include "dmr6x2uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

DMR6X2UVTest::DMR6X2UVTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}


void
DMR6X2UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  DMR6X2UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DMR-6X2UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DMR6X2UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  DMR6X2UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DMR-6X2UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BETCH DMR-6X2UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DMR6X2UVTest::testChannelFrequency() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D868UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DMR-6X2UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DMR-6X2UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}

void
DMR6X2UVTest::testFMAPRSSettings() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/fm_aprs_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QCOMPARE(config.posSystems()->count(), 1);
  QVERIFY(config.posSystems()->get(0)->is<APRSSystem>());
  APRSSystem *aprs = config.posSystems()->get(0)->as<APRSSystem>();
  QCOMPARE(aprs->period(), 300);

  // Encode
  DMR6X2UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  Config *intermediate = codeplug.preprocess(&config, err);
  if (nullptr == intermediate) {
    QFAIL(QString("Cannot prepare codeplug for BTECH DMR-6X2UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  if (! codeplug.encode(intermediate, flags, err)) {
    delete intermediate;
    QFAIL(QString("Cannot encode codeplug for BTECH DMR-6X2UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
  delete intermediate;

  // Decode
  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DMR-6X2UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Check config
  QCOMPARE(comp_config.posSystems()->count(), 1);
  QVERIFY(comp_config.posSystems()->get(0)->is<APRSSystem>());
  APRSSystem *comp_aprs = comp_config.posSystems()->get(0)->as<APRSSystem>();

}

QTEST_GUILESS_MAIN(DMR6X2UVTest)

