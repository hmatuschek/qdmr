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
    QFAIL(QString("Cannot open codeplug file: %1\n")
          .arg(err.format()).toStdString().c_str());
  }

  auto ch_ext = new AnytoneFMChannelExtension();
  ch_ext->setAPRSPTT(AnytoneChannelExtension::APRSPTT::Start);
  config.channelList()->channel(1)->as<FMChannel>()->setAnytoneChannelExtension(ch_ext);

  // Check config
  QCOMPARE(config.posSystems()->count(), 1);
  QVERIFY(config.posSystems()->get(0)->is<APRSSystem>());

  APRSSystem *aprs = config.posSystems()->get(0)->as<APRSSystem>();
  QCOMPARE(aprs->source(), "DM3MAT"); QCOMPARE(aprs->srcSSID(), 7);
  QCOMPARE(aprs->destination(), "APAT81"); QCOMPARE(aprs->destSSID(), 0);
  QCOMPARE(aprs->path(), "WIDE1-1,WIDE2-1");
  QCOMPARE(aprs->period(), 300);

  // test extension settings
  auto ext = new AnytoneFMAPRSSettingsExtension();
  ext->setPreWaveDelay(Interval::fromMilliseconds(100));
  ext->setTXDelay(Interval::fromMilliseconds(200));
  aprs->setAnytoneExtension(ext);

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
  QCOMPARE(comp_aprs->source(), aprs->source()); QCOMPARE(comp_aprs->srcSSID(), aprs->srcSSID());
  QCOMPARE(comp_aprs->destination(), aprs->destination()); QCOMPARE(comp_aprs->destSSID(), aprs->destSSID());
  QCOMPARE(comp_aprs->path(), aprs->path());
  QCOMPARE(comp_aprs->period(), aprs->period());

  // check extension settings
  QVERIFY(nullptr != comp_aprs->anytoneExtension());
  QCOMPARE(comp_aprs->anytoneExtension()->preWaveDelay().milliseconds(),
           100);
  QCOMPARE(comp_aprs->anytoneExtension()->txDelay().milliseconds(),
           200);

  // Check revert channel
  QCOMPARE(comp_config.channelList()->count(), 2);
  QVERIFY(comp_config.channelList()->channel(0)->is<FMChannel>());
  QCOMPARE(comp_config.channelList()->channel(0)->rxFrequency(),
           config.channelList()->channel(0)->rxFrequency());
  QCOMPARE(comp_config.channelList()->channel(0)->txFrequency(),
           config.channelList()->channel(0)->txFrequency());

  // Check channel extension properties
  QVERIFY(nullptr != comp_config.channelList()->channel(1)->as<FMChannel>()->anytoneChannelExtension());
  auto comp_ch_ext = comp_config.channelList()->channel(1)->as<FMChannel>()->anytoneChannelExtension();
  QCOMPARE(comp_ch_ext->aprsPTT(), ch_ext->aprsPTT());
}

QTEST_GUILESS_MAIN(DMR6X2UVTest)

