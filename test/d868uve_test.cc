#include "d868uve_test.hh"
#include "config.hh"
#include "d868uv.hh"
#include "d868uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D868UVETest::D868UVETest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
D868UVETest::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D868UVETest::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
D868UVETest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug = false;
  D868UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D868UVETest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D868UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

QTEST_GUILESS_MAIN(D868UVETest)

