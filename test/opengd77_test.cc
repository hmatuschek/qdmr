#include "opengd77_test.hh"
#include "config.hh"
#include "opengd77_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

OpenGD77Test::OpenGD77Test(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
OpenGD77Test::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
OpenGD77Test::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
OpenGD77Test::testBasicConfigEncoding() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
OpenGD77Test::testBasicConfigDecoding() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for OpenGD77: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

QTEST_GUILESS_MAIN(OpenGD77Test)

