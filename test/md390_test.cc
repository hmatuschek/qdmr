#include "md390_test.hh"
#include "config.hh"
#include "md390_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

MD390Test::MD390Test(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
MD390Test::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
MD390Test::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
MD390Test::testBasicConfigEncoding() {
  ErrorStack err;
  MD390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
MD390Test::testBasicConfigDecoding() {
  ErrorStack err;
  MD390Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT MD390: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

QTEST_GUILESS_MAIN(MD390Test)

