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


QTEST_GUILESS_MAIN(D168UVTest)

