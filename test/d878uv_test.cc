#include "d878uv_test.hh"
#include "config.hh"
#include "d878uv.hh"
#include "d878uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D878UVTest::D878UVTest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
D878UVTest::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
  if (! _roamingConfig.readYAML(":/data/roaming_channel_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
D878UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::testRoaming() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_roamingConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.roamingChannels()->count(), 3);
  QCOMPARE(config.roamingZones()->count(), 2);
  QCOMPARE(config.roamingZones()->count(), 2);

  QCOMPARE(config.roamingZones()->get(0)->as<RoamingZone>()->count(), 2);
  QCOMPARE(config.roamingZones()->get(0)->as<RoamingZone>()->channel(0),
           config.roamingChannels()->get(0)->as<RoamingChannel>());
  QCOMPARE(config.roamingZones()->get(0)->as<RoamingZone>()->channel(1),
           config.roamingChannels()->get(1)->as<RoamingChannel>());

  QCOMPARE(config.roamingZones()->get(1)->as<RoamingZone>()->count(), 2);
  QCOMPARE(config.roamingZones()->get(1)->as<RoamingZone>()->channel(0),
           config.roamingChannels()->get(0)->as<RoamingChannel>());
  QCOMPARE(config.roamingZones()->get(1)->as<RoamingZone>()->channel(1),
           config.roamingChannels()->get(2)->as<RoamingChannel>());
}

QTEST_GUILESS_MAIN(D878UVTest)

