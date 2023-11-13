#include "dr1801_test.hh"
#include "config.hh"
#include "dr1801uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

DR1801Test::DR1801Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
DR1801Test::testBasicConfigEncoding() {
  ErrorStack err;
  DR1801UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH MD1701: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DR1801Test::testBasicConfigDecoding() {
  ErrorStack err;
  DR1801UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM1701: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM1701: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DR1801Test::testChannelFrequency() {
  ErrorStack err;
  DR1801UVCodeplug codeplug;
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM1701: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM1701: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}

QTEST_GUILESS_MAIN(DR1801Test)

