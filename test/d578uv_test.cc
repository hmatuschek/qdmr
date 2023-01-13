#include "d578uv_test.hh"
#include "config.hh"
#include "d578uv.hh"
#include "d578uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D578UVTest::D578UVTest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
D578UVTest::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D578UVTest::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
D578UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  D578UV radio;
  D578UVCodeplug codeplug;
  codeplug.clear();
  codeplug.setBitmaps(&_basicConfig);
  codeplug.allocateUpdated();
  codeplug.allocateForEncoding();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D578UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  D578UV radio;
  D578UVCodeplug codeplug;
  codeplug.clear();
  codeplug.setBitmaps(&_basicConfig);
  codeplug.allocateUpdated();
  codeplug.allocateForEncoding();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D578UV: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

QTEST_GUILESS_MAIN(D578UVTest)

