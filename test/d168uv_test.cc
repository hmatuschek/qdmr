#include "d168uv_test.hh"
#include "logger.hh"
#include "d168uv_codeplug.hh"


D168UVTest::D168UVTest(QObject *parent)
  : UnitTestBase(parent), _stderr(stderr)
{
  Logger::get().addHandler(new StreamLogHandler(_stderr, LogMessage::DEBUG));
}

void
D168UVTest::encodeDecode(Config &input, Config &output) {
  ErrorStack err;
  D168UVCodeplug codeplug;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  if (! codeplug.encode(&input, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D878UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
  if (! codeplug.decode(&output, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
}


void
D168UVTest::testFixedLocation() {
  ErrorStack err;
  Config decoded, config, reencoded; config.copy(_basicConfig);

  config.settings()->gnss()->setFixedPositionLocator("JO62kk45");
  config.settings()->gnss()->enableFixedPosition(true);
  encodeDecode(config, decoded);

  QVERIFY(decoded.settings()->gnss()->fixedPositionEnabled());
  QCOMPARE(decoded.settings()->gnss()->fixedPositionLocator(), QString("JO62kk45"));

  encodeDecode(decoded, reencoded);
  QVERIFY(reencoded.settings()->gnss()->fixedPositionEnabled());
  QCOMPARE(reencoded.settings()->gnss()->fixedPositionLocator(), QString("JO62kk45"));
}


void
D168UVTest::testShortKeyFunctions() {
  ErrorStack err;
  Config config; config.copy(_basicConfig);

  if (nullptr == config.settings()->anytoneExtension())
    config.settings()->setAnytoneExtension(new AnytoneSettingsExtension());
  auto keys = config.settings()->anytoneExtension()->keySettings();
  keys->setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction::Squelch);
  keys->setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction::DIMShut);

  D168UVCodeplug codeplug;
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D168UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  // Check device specific codes of short key functions
  uint8_t *settings = codeplug.data(0x02500000);
  QCOMPARE(settings[0x10], (uint8_t)0x37); // PF1 short: Squelch
  QCOMPARE(settings[0x11], (uint8_t)0x34); // PF2 short: DIMShut

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D168UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QVERIFY2(decoded.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  keys = decoded.settings()->anytoneExtension()->keySettings();
  QCOMPARE(keys->funcKeyAShort(), AnytoneKeySettingsExtension::KeyFunction::Squelch);
  QCOMPARE(keys->funcKeyBShort(), AnytoneKeySettingsExtension::KeyFunction::DIMShut);
}


QTEST_GUILESS_MAIN(D168UVTest)

