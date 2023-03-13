#include "d878uv_test.hh"
#include "config.hh"
#include "d878uv.hh"
#include "d878uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>
#include "logger.hh"

D878UVTest::D878UVTest(QObject *parent)
  : QObject(parent), _stderr(stderr)
{
  Logger::get().addHandler(new StreamLogHandler(_stderr, LogMessage::DEBUG));
}

void
D878UVTest::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  if (! _micGainConfig.readYAML(":/data/anytone_audio_settings_extension.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
D878UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D878UVTest::testAnalogMicGain() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D878UVCodeplug codeplug;
  if (! codeplug.encode(&_micGainConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QVERIFY(config.settings()->anytoneExtension()->audioSettings()->analogMicGainEnabled());
  QCOMPARE(config.settings()->anytoneExtension()->audioSettings()->analogMicGain(), 6);
}


QTEST_GUILESS_MAIN(D878UVTest)

