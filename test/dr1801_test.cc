#include "dr1801_test.hh"
#include "config.hh"
#include "dr1801uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

DR1801Test::DR1801Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
DR1801Test::testBasicConfigEncoding() {
  ErrorStack err;
  DR1801UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DR1801Test::testBasicConfigDecoding() {
  ErrorStack err;
  DR1801UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}


void
DR1801Test::testBasicConfigReencoding() {
  ErrorStack err;
  DR1801UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config originalConfig;
  if (! codeplug.decode(&originalConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
  // original Config now contains all default values and extensions

  // reencode it
  if (! codeplug.encode(&originalConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Decode complete config
  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // compare
  if (0 != originalConfig.compare(testConfig)) {
    QFAIL("Decoded config of BTECH DR1801UV does not match source.");
  }
}

QTEST_GUILESS_MAIN(DR1801Test)


