#include "dmr6x2uv_test.hh"
#include "config.hh"
#include "dmr6x2uv.hh"
#include "dmr6x2uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

DMR6X2UVTest::DMR6X2UVTest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
DMR6X2UVTest::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DMR6X2UVTest::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
DMR6X2UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  DMR6X2UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DMR-6X2UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DMR6X2UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  DMR6X2UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DMR-6X2UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BETCH DMR-6X2UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

QTEST_GUILESS_MAIN(DMR6X2UVTest)

