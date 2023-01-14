#include "d878uv2_test.hh"
#include "config.hh"
#include "d878uv2.hh"
#include "d878uv2_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D878UV2Test::D878UV2Test(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
D878UV2Test::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UV2Test::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
D878UV2Test::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UV2Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UVII: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UV2Test::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UV2Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UVII: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

QTEST_GUILESS_MAIN(D878UV2Test)

