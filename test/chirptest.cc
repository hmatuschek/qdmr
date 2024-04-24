#include "chirptest.hh"

#include <QTest>
#include "chirpformat.hh"
#include "config.hh"


ChirpTest::ChirpTest(QObject *parent)
  : QObject{parent}
{
  // pass...
}


void
ChirpTest::testReaderBasic() {

  QFile file(":/data/chirp_simple.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHRIP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 3);
  QCOMPARE(config.channelList()->channel(0)->name(), "KD8BMI");
  QCOMPARE(config.channelList()->channel(0)->rxFrequency(), 147.075000);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(), 147.675000);
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone(), Signaling::CTCSS_103_5Hz);
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), Signaling::SIGNALING_NONE);
  QCOMPARE(config.channelList()->channel(1)->rxFrequency(), 146.760000);
  QCOMPARE(config.channelList()->channel(1)->txFrequency(), 146.160000);
}


void
ChirpTest::testReaderCTCSS() {

  QFile file(":/data/chirp_ctcss.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHRIP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 3);

  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone(), Signaling::SIGNALING_NONE);
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), Signaling::SIGNALING_NONE);

  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->txTone(), Signaling::CTCSS_67_0Hz);
  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->rxTone(), Signaling::SIGNALING_NONE);

  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->txTone(), Signaling::CTCSS_77_0Hz);
  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->rxTone(), Signaling::CTCSS_77_0Hz);
}


void
ChirpTest::testReaderDCS() {

  QFile file(":/data/chirp_dcs.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHRIP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone(), Signaling::DCS_023N);
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), Signaling::DCS_023N);

  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->txTone(), Signaling::DCS_023N);
  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->rxTone(), Signaling::DCS_023I);

  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->txTone(), Signaling::DCS_023I);
  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->rxTone(), Signaling::DCS_023N);

  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->txTone(), Signaling::DCS_023I);
  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->rxTone(), Signaling::DCS_023I);
}


void
ChirpTest::testReaderCross()
{
  QFile file(":/data/chirp_cross.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHRIP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 8);

  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone(), Signaling::SIGNALING_NONE);
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), Signaling::CTCSS_67_0Hz);

  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->txTone(), Signaling::SIGNALING_NONE);
  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->rxTone(), Signaling::DCS_023N);

  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->txTone(), Signaling::CTCSS_67_0Hz);
  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->rxTone(), Signaling::SIGNALING_NONE);

  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->txTone(), Signaling::CTCSS_67_0Hz);
  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->rxTone(), Signaling::CTCSS_77_0Hz);

  QCOMPARE(config.channelList()->channel(4)->as<FMChannel>()->txTone(), Signaling::CTCSS_67_0Hz);
  QCOMPARE(config.channelList()->channel(4)->as<FMChannel>()->rxTone(), Signaling::DCS_023N);

  QCOMPARE(config.channelList()->channel(5)->as<FMChannel>()->txTone(), Signaling::DCS_023N);
  QCOMPARE(config.channelList()->channel(5)->as<FMChannel>()->rxTone(), Signaling::SIGNALING_NONE);

  QCOMPARE(config.channelList()->channel(6)->as<FMChannel>()->txTone(), Signaling::DCS_023N);
  QCOMPARE(config.channelList()->channel(6)->as<FMChannel>()->rxTone(), Signaling::CTCSS_67_0Hz);

  QCOMPARE(config.channelList()->channel(7)->as<FMChannel>()->txTone(), Signaling::DCS_023N);
  QCOMPARE(config.channelList()->channel(7)->as<FMChannel>()->rxTone(), Signaling::DCS_032N);
}


void
ChirpTest::testWriterBasic() {
  Config orig;

  FMChannel *fm0 = new FMChannel();
  fm0->setName("DB0SP"); fm0->setRXFrequency(145.6); fm0->setTXFrequency(145.0);
  orig.channelList()->add(fm0);

  FMChannel *fm1 = new FMChannel();
  fm1->setName("DB0SP"); fm1->setRXFrequency(145.6); fm1->setTXFrequency(144.4);
  orig.channelList()->add(fm1);

  QString csv;
  QTextStream stream(&csv);
  ErrorStack err;
  if (! ChirpWriter::write(stream, &orig, err))
    QFAIL(QString("Cannot serialize codeplug:\n%1").arg(err.format()).toStdString().c_str());

  Config parsed;
  if (! ChirpReader::read(stream, &parsed, err))
    QFAIL(QString("Cannot parse CHIRP CSV:\n%1").arg(err.format()).toStdString().c_str());

  QCOMPARE(parsed.channelList()->count(), orig.channelList()->count());
  QCOMPARE(parsed.channelList()->channel(0)->name(), orig.channelList()->channel(0)->name());
  QCOMPARE(parsed.channelList()->channel(0)->rxFrequency(), orig.channelList()->channel(0)->rxFrequency());
  QCOMPARE(parsed.channelList()->channel(0)->txFrequency(), orig.channelList()->channel(0)->txFrequency());

  QCOMPARE(parsed.channelList()->channel(1)->rxFrequency(), orig.channelList()->channel(1)->rxFrequency());
  QCOMPARE(parsed.channelList()->channel(1)->txFrequency(), orig.channelList()->channel(1)->txFrequency());
}


void
ChirpTest::testWriterCTCSS() {
  Config orig;

  FMChannel *fm0 = new FMChannel();
  fm0->setName("DB0SP"); fm0->setRXFrequency(145.6); fm0->setTXFrequency(145.0);
  fm0->setTXTone(Signaling::CTCSS_67_0Hz);
  orig.channelList()->add(fm0);

  FMChannel *fm1 = new FMChannel();
  fm1->setName("DB0SP"); fm1->setRXFrequency(145.6); fm1->setTXFrequency(145.0);
  fm1->setTXTone(Signaling::CTCSS_67_0Hz);
  fm1->setRXTone(Signaling::CTCSS_67_0Hz);
  orig.channelList()->add(fm1);

  FMChannel *fm2 = new FMChannel();
  fm2->setName("DB0SP"); fm2->setRXFrequency(145.6); fm2->setTXFrequency(145.0);
  fm2->setTXTone(Signaling::CTCSS_67_0Hz);
  fm2->setRXTone(Signaling::CTCSS_77_0Hz);
  orig.channelList()->add(fm2);

  QString csv;
  QTextStream stream(&csv);
  ErrorStack err;
  if (! ChirpWriter::write(stream, &orig, err))
    QFAIL(QString("Cannot serialize codeplug:\n%1").arg(err.format()).toStdString().c_str());
  qDebug() << csv;

  Config parsed;
  if (! ChirpReader::read(stream, &parsed, err))
    QFAIL(QString("Cannot parse CHIRP CSV:\n%1").arg(err.format()).toStdString().c_str());

  FMChannel *pfm1 = parsed.channelList()->channel(0)->as<FMChannel>();
  QCOMPARE(pfm1->txTone(), fm0->txTone());

  FMChannel *pfm2 = parsed.channelList()->channel(1)->as<FMChannel>();
  QCOMPARE(pfm2->txTone(), fm1->txTone());
  QCOMPARE(pfm2->rxTone(), fm1->rxTone());

  FMChannel *pfm3 = parsed.channelList()->channel(2)->as<FMChannel>();
  QCOMPARE(pfm3->txTone(), fm2->txTone());
  QCOMPARE(pfm3->rxTone(), fm2->rxTone());
}


void
ChirpTest::testWriterDCS() {

}


void
ChirpTest::testWriterCross() {

}


QTEST_GUILESS_MAIN(ChirpTest)
