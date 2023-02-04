#include "d578uv_test.hh"
#include "config.hh"
#include "d578uv.hh"
#include "d578uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D578UVTest::D578UVTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
D578UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D578UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D578UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D578UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D578UVTest::testChannelFrequency() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D578UVCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           123456780ULL);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           999999990ULL);
}

QTEST_GUILESS_MAIN(D578UVTest)

