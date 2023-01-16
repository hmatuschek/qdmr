#include "uv390_test.hh"
#include "config.hh"
#include "uv390_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

UV390Test::UV390Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
UV390Test::testBasicConfigEncoding() {
  ErrorStack err;
  UV390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
UV390Test::testBasicConfigDecoding() {
  ErrorStack err;
  UV390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
UV390Test::testChannelFrequency() {
  ErrorStack err;
  UV390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD-UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT MD-UV390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           123456780ULL);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           999999990ULL);
}


QTEST_GUILESS_MAIN(UV390Test)

