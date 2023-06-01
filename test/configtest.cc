#include "configtest.hh"
#include "config.hh"
#include "errorstack.hh"
#include "melody.hh"
#include <iostream>
#include <QTest>


ConfigTest::ConfigTest(QObject *parent) : QObject(parent)
{
  // pass...
}


void
ConfigTest::initTestCase() {
  ErrorStack err;
  if (! _config.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1").arg(err.format()).toStdString().c_str());
  }
}

void
ConfigTest::cleanupTestCase() {
  // clear codeplug
  _config.clear();
}

void
ConfigTest::testCloneChannelBasic() {
  // Check if a channel can be cloned
  Channel *clone = _config.channelList()->channel(0)->clone()->as<Channel>();
  // Check if channels are the same
  QCOMPARE(clone->compare(*_config.channelList()->channel(0)), 0);
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

QTEST_GUILESS_MAIN(ConfigTest)

