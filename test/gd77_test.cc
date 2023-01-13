#include "gd77_test.hh"
#include "config.hh"
#include "gd77.hh"
#include "gd77_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

GD77Test::GD77Test(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
GD77Test::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
GD77Test::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
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

QTEST_GUILESS_MAIN(GD77Test)

