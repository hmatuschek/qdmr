#include "configtest.hh"
#include "config.hh"
#include "gnsssettings.hh"
#include "errorstack.hh"
#include "melody.hh"
#include <iostream>
#include <QTest>
#include "logger.hh"
#include <iostream>
#include "gd73_limits.hh"

#include "configcopyvisitor.hh"

ConfigTest::ConfigTest(QObject *parent)
  : UnitTestBase(parent), _stderr(stderr)
{
  Logger::get().addHandler(new StreamLogHandler(_stderr, LogMessage::DEBUG));
}


void
ConfigTest::initTestCase() {
  UnitTestBase::initTestCase();

  ErrorStack err;
  if (! _ctcssCopyTest.readYAML(":/data/ctcss_copy_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
ConfigTest::testImmediateRefInvalidation() {
  ErrorStack err;
  Config *copy = ConfigCopy::copy(&_basicConfig, err)->as<Config>();
  if (nullptr == copy)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(_basicConfig.posSystems()->count(), 1);
  QVERIFY(_basicConfig.channelList()->get(1));
  QVERIFY(_basicConfig.channelList()->get(1)->is<DMRChannel>());
  QCOMPARE(_basicConfig.channelList()->get(1)->as<DMRChannel>()->aprs(),
           _basicConfig.posSystems()->get(0)->as<DMRAPRSSystem>());

  // Delete DMR APRS system and check if reference to it (channel 2) is removed as well.
  QVERIFY(copy->posSystems()->del(copy->posSystems()->get(0)));
  QCOMPARE(copy->posSystems()->count(), 0);
  QCOMPARE(copy->channelList()->get(1)->as<DMRChannel>()->aprs(), nullptr);

  QVERIFY(_basicConfig.channelList()->get(2));
  QCOMPARE(_basicConfig.zones()->get(0)->as<Zone>()->B()->count(), 1);
  QCOMPARE(_basicConfig.zones()->get(0)->as<Zone>()->B()->get(0),
           _basicConfig.channelList()->get(2));

  // Delete channel 3, check if zone 1, list B is empty
  copy->channelList()->del(copy->channelList()->get(2));
  QCOMPARE(copy->zones()->get(0)->as<Zone>()->B()->count(), 0);
}


void
ConfigTest::testCloneChannelBasic() {
  // Check if a channel can be cloned
  Channel *clone = _basicConfig.channelList()->channel(0)->clone()->as<Channel>();
  // Check if channels are the same
  QCOMPARE(clone->compare(*_basicConfig.channelList()->channel(0)), 0);
}

void
ConfigTest::testCloneChannelCTCSS() {
  // Check if a channel can be cloned
  QHash<ConfigObject*, ConfigObject*> table;
  ConfigCloneVisitor visitor(table);
  ErrorStack err;
  if (! visitor.processItem(_ctcssCopyTest.channelList()->channel(0)))
    QFAIL(err.format().toLocal8Bit().constData());

  Channel *clone = visitor.takeResult(err)->as<Channel>();
  if (nullptr == clone)
    QFAIL(err.format().toLocal8Bit().constData());

  // Check if channels are the same
  QCOMPARE(clone->compare(*_ctcssCopyTest.channelList()->channel(0)), 0);
  QCOMPARE(clone->as<FMChannel>()->txTone(),
           _ctcssCopyTest.channelList()->channel(0)->as<FMChannel>()->txTone());
  QCOMPARE(clone->as<FMChannel>()->rxTone(),
           _ctcssCopyTest.channelList()->channel(0)->as<FMChannel>()->rxTone());
}

void
ConfigTest::testMultipleRadioIDs() {
  ErrorStack err;
  Config multipleRadioIds;
  if (! multipleRadioIds.readYAML(":/data/audio_settings_extension.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config::Context ctx;
  if (! multipleRadioIds.label(ctx, err)) {
    QFAIL(QString("Cannot label codeplug: %1")
          .arg(err.format()).toStdString().c_str());
  }

  YAML::Node node = multipleRadioIds.serialize(ctx, err);
  if (node.IsNull()) {
    QFAIL(QString("Cannot serialize codeplug: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
ConfigTest::testMelodyLilypond() {
  QString lilypond = "a8 b e2 cis4 d";
  Melody melody; melody.fromLilypond(lilypond);
  QString serialized = melody.toLilypond();
  QCOMPARE(serialized, lilypond);

  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  QCOMPARE(std::round(tones[0].first*100), 44000); QCOMPARE(tones[0].second, 300);
  QCOMPARE(std::round(tones[1].first*100), 49388); QCOMPARE(tones[1].second, 300);
  QCOMPARE(std::round(tones[2].first*100), 32963); QCOMPARE(tones[2].second,1200);
  QCOMPARE(std::round(tones[3].first*100), 27718); QCOMPARE(tones[3].second, 600);
  QCOMPARE(std::round(tones[4].first*100), 29366); QCOMPARE(tones[4].second, 600);
}

void
ConfigTest::testMelodyEncoding() {
  QString lilypond = "a8 b e2 cis4 d";
  Melody melody; melody.fromLilypond(lilypond);

  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  QCOMPARE(std::round(tones[0].first*100), 44000); QCOMPARE(tones[0].second, 300);
  QCOMPARE(std::round(tones[1].first*100), 49388); QCOMPARE(tones[1].second, 300);
  QCOMPARE(std::round(tones[2].first*100), 32963); QCOMPARE(tones[2].second,1200);
  QCOMPARE(std::round(tones[3].first*100), 27718); QCOMPARE(tones[3].second, 600);
  QCOMPARE(std::round(tones[4].first*100), 29366); QCOMPARE(tones[4].second, 600);
}

void
ConfigTest::testMelodyDecoding() {
  QString lilypond = "a8 b e2 cis4 d";
  QVector<QPair<double, unsigned int>> tones = {
    {440.00, 300}, {493.88, 300}, {329.63, 1200}, {277.18, 600}, {293.66, 600}
  };

  Melody melody; melody.infer(tones);
  QCOMPARE(melody.toLilypond(), lilypond);
  // infer() should also infer a BPM of 100
  QCOMPARE(melody.bpm(), 100);
}


void
ConfigTest::testGNSSSettings() {
  Config config; config.readYAML(":/data/config_test.yaml");
  config.settings()->gnss()->setFixedPositionLocator("JO62jl45");
  config.settings()->gnss()->enableFixedPosition(true);
  config.settings()->gnss()->setSystems(GNSSSettings::System::GPS | GNSSSettings::System::Beidou);

  qDebug() << "Before" << config.settings()->gnss()->fixedPositionLocator();

  QString buffer;
  QTextStream stream(&buffer);
  ErrorStack err;
  if (! config.toYAML(stream, err))
    QFAIL(err.format().toLocal8Bit().constData());

  Config testConfig;
  Config::Context ctx;
  YAML::Node doc = YAML::Load(buffer.toStdString());
  if (! testConfig.parse(doc, ctx, err))
    QFAIL(err.format().toLocal8Bit().constData());
  if (! testConfig.link(doc, ctx, err))
    QFAIL(err.format().toLocal8Bit().constData());

  qDebug() << "After" << testConfig.settings()->gnss()->fixedPositionLocator();

  QCOMPARE(testConfig.settings()->gnss()->fixedPositionEnabled(), true);
  QCOMPARE(testConfig.settings()->gnss()->fixedPositionLocator(), "JO62jl45");
  QCOMPARE(testConfig.settings()->gnss()->systems(), GNSSSettings::System::GPS | GNSSSettings::System::Beidou);
}


void
ConfigTest::testAudioSettings() {
  Config config; config.readYAML(":/data/audio_settings_extension.yaml");
  QCOMPARE(config.settings()->audio()->micGain(), Level::fromValue(3));
  QCOMPARE(config.settings()->audio()->fmMicGain(), Level::fromValue(6));

  Config copyConfig;
  QVERIFY(copyConfig.copy(config));
  QCOMPARE(copyConfig.settings()->audio()->micGain(), config.settings()->audio()->micGain());
  QCOMPARE(copyConfig.settings()->audio()->fmMicGain(), config.settings()->audio()->fmMicGain());
}


void
ConfigTest::testCTCSSNull() {
  ErrorStack err;
  Config ctcssConfig;
  if (! ctcssConfig.readYAML(":/data/ctcss_null_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Test backward compatibility
  QVERIFY(ctcssConfig.channelList()->channel(0)->is<FMChannel>());
  QCOMPARE(SelectiveCall() ,ctcssConfig.channelList()->channel(0)->as<FMChannel>()->rxTone());
  QCOMPARE(SelectiveCall(67.0) ,ctcssConfig.channelList()->channel(0)->as<FMChannel>()->txTone());

  // Test new format
  QVERIFY(ctcssConfig.channelList()->channel(1)->is<FMChannel>());
  QCOMPARE(SelectiveCall(37, true), ctcssConfig.channelList()->channel(1)->as<FMChannel>()->rxTone());
  QCOMPARE(SelectiveCall(67.0) ,ctcssConfig.channelList()->channel(1)->as<FMChannel>()->txTone());
}


void
ConfigTest::testSelectiveCallDCS() {
  ErrorStack err;

  auto dcsFromOctal = SelectiveCall(0, false);
  auto dcsFromBinary = SelectiveCall::fromBinaryDCS(0x0, false);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "n000");
  QCOMPARE(dcsFromOctal.binCode(), 0x0);
  QCOMPARE(dcsFromOctal.octalCode(), 0);
  QCOMPARE(dcsFromOctal.isInverted(), false);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(1, false);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x1, false);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "n001");
  QCOMPARE(dcsFromOctal.binCode(), 0x1);
  QCOMPARE(dcsFromOctal.octalCode(), 1);
  QCOMPARE(dcsFromOctal.isInverted(), false);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(7, false);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x7, false);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "n007");
  QCOMPARE(dcsFromOctal.binCode(), 0x7);
  QCOMPARE(dcsFromOctal.octalCode(), 7);
  QCOMPARE(dcsFromOctal.isInverted(), false);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(10, false);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x8, false);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "n010");
  QCOMPARE(dcsFromOctal.binCode(), 0x8);
  QCOMPARE(dcsFromOctal.octalCode(), 10);
  QCOMPARE(dcsFromOctal.isInverted(), false);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(42, false);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x22, false);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "n042");
  QCOMPARE(dcsFromOctal.binCode(), 0x22);
  QCOMPARE(dcsFromOctal.octalCode(), 42);
  QCOMPARE(dcsFromOctal.isInverted(), false);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(777, false);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x1ff, false);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "n777");
  QCOMPARE(dcsFromOctal.binCode(), 0x1ff);
  QCOMPARE(dcsFromOctal.octalCode(), 777);
  QCOMPARE(dcsFromOctal.isInverted(), false);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(0, true);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x200 - 512, true);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "i000");
  QCOMPARE(dcsFromOctal.binCode(), 0x0);
  QCOMPARE(dcsFromOctal.octalCode(), 0);
  QCOMPARE(dcsFromOctal.isInverted(), true);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(7, true);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x207 - 512, true);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "i007");
  QCOMPARE(dcsFromOctal.binCode(), 0x7);
  QCOMPARE(dcsFromOctal.octalCode(), 7);
  QCOMPARE(dcsFromOctal.isInverted(), true);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(10, true);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x208 - 512, true);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "i010");
  QCOMPARE(dcsFromOctal.binCode(), 0x8);
  QCOMPARE(dcsFromOctal.octalCode(), 10);
  QCOMPARE(dcsFromOctal.isInverted(), true);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(42, true);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x222 - 512, true);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "i042");
  QCOMPARE(dcsFromOctal.binCode(), 0x22);
  QCOMPARE(dcsFromOctal.octalCode(), 42);
  QCOMPARE(dcsFromOctal.isInverted(), true);
  QCOMPARE(dcsFromOctal, dcsFromBinary);

  dcsFromOctal = SelectiveCall(777, true);
  dcsFromBinary = SelectiveCall::fromBinaryDCS(0x3ff - 512, true);
  QVERIFY(dcsFromOctal.isDCS());
  QCOMPARE(dcsFromOctal.format(), "i777");
  QCOMPARE(dcsFromOctal.binCode(), 0x1ff);
  QCOMPARE(dcsFromOctal.octalCode(), 777);
  QCOMPARE(dcsFromOctal.isInverted(), true);
  QCOMPARE(dcsFromOctal, dcsFromBinary);
}


void
ConfigTest::testDMRIdVerification() {
  GD73Limits limits;
  RadioLimitContext ctx;

  ErrorStack err;
  Config cfg;

  if (! cfg.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  /*limits.verifyConfig(&cfg, ctx);
  for (unsigned int i=0; i<ctx.count(); i++)
    qDebug() << ctx.message(i).format();*/

  QVERIFY(limits.verifyConfig(&cfg, ctx));
  QVERIFY(ctx.maxSeverity() < RadioLimitIssue::Severity::Critical);

  cfg.radioIDs()->get(0)->as<DMRRadioID>()->setNumber(0x12345678);

  QVERIFY(!limits.verifyConfig(&cfg, ctx));
  QVERIFY(ctx.maxSeverity() == RadioLimitIssue::Severity::Critical);
}


QTEST_GUILESS_MAIN(ConfigTest)

