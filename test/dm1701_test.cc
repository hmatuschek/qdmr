#include "dm1701_test.hh"
#include "config.hh"
#include "dm1701_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

DM1701Test::DM1701Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
DM1701Test::testBasicConfigEncoding() {
  ErrorStack err;
  DM1701Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH MD1701: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DM1701Test::testBasicConfigDecoding() {
  ErrorStack err;
  DM1701Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM1701: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM1701: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DM1701Test::testChannelFrequency() {
  ErrorStack err;
  DM1701Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM1701: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM1701: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           123456780ULL);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           999999990ULL);
}

QTEST_GUILESS_MAIN(DM1701Test)

