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


void
DM32UVTest::testChannelBankEncoding() {
  static const uint32_t bank0=0x12000, bank1 = 0x13000, bank2 = 0x14000;
  Config config;

  config.radioIDs()->add(new DMRRadioID("test", 123456));
  // Add 84 channels to fill bank 0
  for (int i=0; i<84; i++) {
    auto ch = new FMChannel();
    ch->setName(QString("Channel %1").arg(i));
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.0));
    config.channelList()->add(ch);
  }

  {
    DM32UVCodeplug codeplug; ErrorStack err;
    if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
      QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
              .arg(err.format()).toStdString().c_str());
    }
    QVERIFY(codeplug.isAllocated(bank0));
    QVERIFY(!codeplug.isAllocated(bank1));
    QVERIFY(!codeplug.isAllocated(bank2));
  }

  // Add another 85 channel to fill bank 1
  for (int i=0; i<85; i++) {
    auto ch = new FMChannel();
    ch->setName(QString("Channel %1").arg(i));
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.0));
    config.channelList()->add(ch);
  }

  {
    DM32UVCodeplug codeplug; ErrorStack err;
    if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
      QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
              .arg(err.format()).toStdString().c_str());
    }
    QVERIFY(codeplug.isAllocated(bank0));
    QVERIFY(codeplug.isAllocated(bank1));
    QVERIFY(!codeplug.isAllocated(bank2));
  }

  // Add yet another channel to allocate bank 2
  auto ch = new FMChannel();
  ch->setName(QString("Channel %1").arg(config.channelList()->count()));
  ch->setRXFrequency(Frequency::fromMHz(144.0));
  ch->setTXFrequency(Frequency::fromMHz(144.0));
  config.channelList()->add(ch);

  {
    DM32UVCodeplug codeplug; ErrorStack err;
    if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
      QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
              .arg(err.format()).toStdString().c_str());
    }
    QVERIFY(codeplug.isAllocated(bank0));
    QVERIFY(codeplug.isAllocated(bank1));
    QVERIFY(codeplug.isAllocated(bank2));
  }
}


QTEST_GUILESS_MAIN(DM32UVTest)


