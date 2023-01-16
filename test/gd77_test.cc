#include "gd77_test.hh"
#include "config.hh"
#include "gd77.hh"
#include "gd77_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

GD77Test::GD77Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
GD77Test::testBasicConfigEncoding() {
  ErrorStack err;
  GD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
GD77Test::testBasicConfigDecoding() {
  ErrorStack err;
  GD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD77: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
GD77Test::testChannelFrequency() {
  ErrorStack err;
  GD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           123456780ULL);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           999999990ULL);
}

QTEST_GUILESS_MAIN(GD77Test)

