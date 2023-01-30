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

QTEST_GUILESS_MAIN(D878UVTest)

