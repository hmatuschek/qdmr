#include "md2017_test.hh"
#include "config.hh"
#include "md2017_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

MD2017Test::MD2017Test(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
MD2017Test::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
MD2017Test::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
MD2017Test::testBasicConfigEncoding() {
  ErrorStack err;
  MD2017Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD2017: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
MD2017Test::testBasicConfigDecoding() {
  ErrorStack err;
  MD2017Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for TyT MD2017: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for TyT MD2017: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

QTEST_GUILESS_MAIN(MD2017Test)

