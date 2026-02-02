#include "dm32uv_test.hh"
#include "config.hh"
#include "dm32uv_codeplug.hh"
#include "errorstack.hh"
#include <QTest>

DM32UVTest::DM32UVTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
DM32UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DR1801UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DM32UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
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
DM32UVTest::testBasicConfigReencoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
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

  // re-encode it
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

void
DM32UVTest::testProstProcessingOfEmptyCodeplug() {
  Config config;
  DM32UVCodeplug codeplug;
  ErrorStack err;

  config.clear();
  QVERIFY(codeplug.postprocess(&config, err));
}


QTEST_GUILESS_MAIN(DM32UVTest)


