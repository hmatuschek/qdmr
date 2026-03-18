#include "dm32uv_test.hh"
#include "config.hh"
#include "dm32uv_codeplug.hh"
#include "errorstack.hh"
#include "logger.hh"
#include <QTest>


DM32UVTest::DM32UVTest(QObject *parent)
  : UnitTestBase(parent)
{
}

void
DM32UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DM32UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}


void
DM32UVTest::testBasicConfigReencoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config originalConfig;
  if (! codeplug.decode(&originalConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
  // original Config now contains all default values and extensions

  // re-encode it
  if (! codeplug.encode(&originalConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Decode complete config
  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // compare
  if (0 != originalConfig.compare(testConfig)) {
    QString orig, test;
    QTextStream origStream(&orig), testStream(&test);
    originalConfig.toYAML(origStream);
    testConfig.toYAML(testStream);
    QFAIL(QString("Decoded config of BTECH DM32UV does not match source:\n%1\n%2").arg(orig).arg(test).toStdString().c_str());
  }
}

void
DM32UVTest::testProstProcessingOfEmptyCodeplug() {
  Config config;
  DM32UVCodeplug codeplug;
  ErrorStack err;

  config.clear();
  QVERIFY(codeplug.postprocess(&config, err));
}


void
DM32UVTest::testAMChannelReencoding() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/am_channel_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }

  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(testConfig.channelList()->count(), config.channelList()->count());
  QVERIFY(testConfig.channelList()->channel(0)->is<AMChannel>());
  QCOMPARE(testConfig.channelList()->channel(0)->rxFrequency(), Frequency::fromMHz(121.13));
  QVERIFY(testConfig.channelList()->channel(0)->txFrequency().isZero());
}



QTEST_GUILESS_MAIN(DM32UVTest)


