#include "rt4d_test.hh"
#include "config.hh"
#include "rt4d_codeplug.hh"
#include "errorstack.hh"
#include "logger.hh"
#include <QTest>


RT4DTest::RT4DTest(QObject *parent)
  : UnitTestBase(parent)
{
}

void
RT4DTest::testBasicConfigEncoding() {
  ErrorStack err;
  RT4DCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radtel RT-4D: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
RT4DTest::testBasicConfigDecoding() {
  ErrorStack err;
  RT4DCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radtel RT-4D: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radtel RT-4D: %1")
          .arg(err.format()).toStdString().c_str());
  }
}


void
RT4DTest::testBasicConfigReencoding() {
  ErrorStack err;
  RT4DCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radtel RT-4D: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config originalConfig;
  if (! codeplug.decode(&originalConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for Radtel RT-4D: %1")
          .arg(err.format()).toStdString().c_str());
  }
  // original Config now contains all default values and extensions

  // re-encode it
  if (! codeplug.encode(&originalConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radtel RT-4D: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Decode complete config
  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for Radtel RT-4D: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // compare
  if (0 != originalConfig.compare(testConfig)) {
    QString orig, test;
    QTextStream origStream(&orig), testStream(&test);
    originalConfig.toYAML(origStream);
    testConfig.toYAML(testStream);
    QFAIL(QString("Decoded config of Radtel RT-4D does not match source:\n%1\n%2").arg(orig).arg(test).toStdString().c_str());
  }
}


void
RT4DTest::testAMChannelReencoding() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/am_channel_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }

  RT4DCodeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radtel RT-4D: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for Radtel RT-4D: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(testConfig.channelList()->count(), config.channelList()->count());
  QVERIFY(testConfig.channelList()->channel(0)->is<AMChannel>());
  QCOMPARE(testConfig.channelList()->channel(0)->rxFrequency(), Frequency::fromMHz(121.13));
  QVERIFY(testConfig.channelList()->channel(0)->rxOnly());
}




QTEST_GUILESS_MAIN(RT4DTest)


