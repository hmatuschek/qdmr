#include "uv390_test.hh"
#include "config.hh"
#include "uv390_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

UV390Test::UV390Test(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
UV390Test::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
UV390Test::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
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

QTEST_GUILESS_MAIN(UV390Test)

