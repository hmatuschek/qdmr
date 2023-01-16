#include "md390_test.hh"
#include "config.hh"
#include "md390_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

MD390Test::MD390Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
MD390Test::testBasicConfigEncoding() {
  ErrorStack err;
  MD390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
MD390Test::testBasicConfigDecoding() {
  ErrorStack err;
  MD390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
MD390Test::testChannelFrequency() {
  ErrorStack err;
  MD390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           123456780ULL);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           999999990ULL);
}


QTEST_GUILESS_MAIN(MD390Test)

