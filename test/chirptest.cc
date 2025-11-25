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
    QFAIL("Cannot open CHIRP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 3);
  QCOMPARE(config.channelList()->channel(0)->name(), "KD8BMI");
  QCOMPARE(config.channelList()->channel(0)->rxFrequency().inMHz(), 147.075000);
  QCOMPARE(config.channelList()->channel(0)->txFrequency().inMHz(), 147.675000);
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone(), SelectiveCall(103.5));
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), SelectiveCall());
  QCOMPARE(config.channelList()->channel(1)->rxFrequency().inMHz(), 146.760000);
  QCOMPARE(config.channelList()->channel(1)->txFrequency().inMHz(), 146.160000);
}


void
ChirpTest::testReaderCTCSS() {

  QFile file(":/data/chirp_ctcss.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHIRP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 3);

  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone().format(), SelectiveCall().format());
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), SelectiveCall());

  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->txTone(), SelectiveCall(67.0));
  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->rxTone(), SelectiveCall());

  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->txTone(), SelectiveCall(77.0));
  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->rxTone(), SelectiveCall(77.0));
}


void
ChirpTest::testReaderDCS() {

  QFile file(":/data/chirp_dcs.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHIRP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }
  SelectiveCall cmp(23, false);
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone().format(), cmp.format());
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), SelectiveCall(23, false));

  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->txTone(), SelectiveCall(23, false));
  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->rxTone(), SelectiveCall(23, true));

  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->txTone(), SelectiveCall(23, true));
  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->rxTone(), SelectiveCall(23, false));

  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->txTone(), SelectiveCall(23, true));
  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->rxTone(), SelectiveCall(23, true));
}


void
ChirpTest::testReaderCross()
{
  QFile file(":/data/chirp_cross.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHIRP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 8);

  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->txTone(), SelectiveCall());
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->rxTone(), SelectiveCall(67.0));

  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->txTone(), SelectiveCall());
  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->rxTone().format(), SelectiveCall(23, false).format());

  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->txTone(), SelectiveCall(67.0));
  QCOMPARE(config.channelList()->channel(2)->as<FMChannel>()->rxTone(), SelectiveCall());

  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->txTone(), SelectiveCall(67.0));
  QCOMPARE(config.channelList()->channel(3)->as<FMChannel>()->rxTone(), SelectiveCall(77.0));

  QCOMPARE(config.channelList()->channel(4)->as<FMChannel>()->txTone(), SelectiveCall(67.0));
  QCOMPARE(config.channelList()->channel(4)->as<FMChannel>()->rxTone(), SelectiveCall(23, false));

  QCOMPARE(config.channelList()->channel(5)->as<FMChannel>()->txTone(), SelectiveCall(23, false));
  QCOMPARE(config.channelList()->channel(5)->as<FMChannel>()->rxTone(), SelectiveCall());

  QCOMPARE(config.channelList()->channel(6)->as<FMChannel>()->txTone(), SelectiveCall(23, false));
  QCOMPARE(config.channelList()->channel(6)->as<FMChannel>()->rxTone(), SelectiveCall(67.0));

  QCOMPARE(config.channelList()->channel(7)->as<FMChannel>()->txTone(), SelectiveCall(23, false));
  QCOMPARE(config.channelList()->channel(7)->as<FMChannel>()->rxTone(), SelectiveCall(32, false));
}


void
ChirpTest::testReaderBandwidth() {

  QFile file(":/data/chirp_bandwidth.csv");
  if (! file.open(QIODevice::ReadOnly)) {
    QFAIL("Cannot open CHIRP file.");
  }

  QTextStream stream(&file);
  Config config;
  ErrorStack err;

  if (! ChirpReader::read(stream, &config, err)) {
    QFAIL(QString("Cannot read codeplug file:\n%1").arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->count(), 2);
  QVERIFY(config.channelList()->channel(0)->is<FMChannel>());
  QCOMPARE(config.channelList()->channel(0)->as<FMChannel>()->bandwidth(), FMChannel::Bandwidth::Wide);
  QCOMPARE(config.channelList()->channel(1)->as<FMChannel>()->bandwidth(), FMChannel::Bandwidth::Narrow);
}


void
ChirpTest::testWriterBasic() {
  Config orig;

  FMChannel *fm0 = new FMChannel();
  fm0->setName("DB0SP"); fm0->setRXFrequency(Frequency::fromMHz(145.6)); fm0->setTXFrequency(Frequency::fromMHz(145.0));
  orig.channelList()->add(fm0);

  FMChannel *fm1 = new FMChannel();
  fm1->setName("DB0SP"); fm1->setRXFrequency(Frequency::fromMHz(145.6)); fm1->setTXFrequency(Frequency::fromMHz(144.4));
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
  fm0->setName("DB0SP"); fm0->setRXFrequency(Frequency::fromMHz(145.6)); fm0->setTXFrequency(Frequency::fromMHz(145.0));
  fm0->setTXTone(SelectiveCall(67.0));
  orig.channelList()->add(fm0);

  FMChannel *fm1 = new FMChannel();
  fm1->setName("DB0SP"); fm1->setRXFrequency(Frequency::fromMHz(145.6)); fm1->setTXFrequency(Frequency::fromMHz(145.0));
  fm1->setTXTone(SelectiveCall(67.0));
  fm1->setRXTone(SelectiveCall(67.0));
  orig.channelList()->add(fm1);

  FMChannel *fm2 = new FMChannel();
  fm2->setName("DB0SP"); fm2->setRXFrequency(Frequency::fromMHz(145.6)); fm2->setTXFrequency(Frequency::fromMHz(145.0));
  fm2->setTXTone(SelectiveCall(67.0));
  fm2->setRXTone(SelectiveCall(77.0));
  orig.channelList()->add(fm2);

  QString csv;
  QTextStream stream(&csv);
  ErrorStack err;
  if (! ChirpWriter::write(stream, &orig, err))
    QFAIL(QString("Cannot serialize codeplug:\n%1").arg(err.format()).toStdString().c_str());

  Config parsed;
  if (! ChirpReader::read(stream, &parsed, err))
    QFAIL(QString("Cannot parse CHIRP CSV:\n%1").arg(err.format()).toStdString().c_str());

  FMChannel *pfm1 = parsed.channelList()->channel(0)->as<FMChannel>();
  QCOMPARE(pfm1->txTone().format(), fm0->txTone().format());

  FMChannel *pfm2 = parsed.channelList()->channel(1)->as<FMChannel>();
  QCOMPARE(pfm2->txTone(), fm1->txTone());
  QCOMPARE(pfm2->rxTone(), fm1->rxTone());

  FMChannel *pfm3 = parsed.channelList()->channel(2)->as<FMChannel>();
  QCOMPARE(pfm3->txTone(), fm2->txTone());
  QCOMPARE(pfm3->rxTone(), fm2->rxTone());
}


void
ChirpTest::testWriterDCS() {
  Config orig;

  FMChannel *fm0 = new FMChannel();
  fm0->setName("DB0SP"); fm0->setRXFrequency(Frequency::fromMHz(145.6)); fm0->setTXFrequency(Frequency::fromMHz(145.0));
  fm0->setTXTone(SelectiveCall(23, false));
  fm0->setRXTone(SelectiveCall(23, false));
  orig.channelList()->add(fm0);

  FMChannel *fm1 = new FMChannel();
  fm1->setName("DB0SP"); fm1->setRXFrequency(Frequency::fromMHz(145.6)); fm1->setTXFrequency(Frequency::fromMHz(145.0));
  fm1->setTXTone(SelectiveCall(23, false));
  fm1->setRXTone(SelectiveCall(23, true));
  orig.channelList()->add(fm1);

  FMChannel *fm2 = new FMChannel();
  fm2->setName("DB0SP"); fm2->setRXFrequency(Frequency::fromMHz(145.6)); fm2->setTXFrequency(Frequency::fromMHz(145.0));
  fm2->setTXTone(SelectiveCall(23, false));
  fm2->setRXTone(SelectiveCall(77.0));
  orig.channelList()->add(fm2);

  FMChannel *fm3 = new FMChannel();
  fm3->setName("DB0SP"); fm3->setRXFrequency(Frequency::fromMHz(145.6)); fm3->setTXFrequency(Frequency::fromMHz(145.0));
  fm3->setTXTone(SelectiveCall(23, false));
  fm3->setRXTone(SelectiveCall(32, false));
  orig.channelList()->add(fm3);

  QString csv;
  QTextStream stream(&csv);
  ErrorStack err;
  if (! ChirpWriter::write(stream, &orig, err))
    QFAIL(QString("Cannot serialize codeplug:\n%1").arg(err.format()).toStdString().c_str());
  qDebug() << csv;

  Config parsed;
  if (! ChirpReader::read(stream, &parsed, err))
    QFAIL(QString("Cannot parse CHIRP CSV:\n%1").arg(err.format()).toStdString().c_str());

  FMChannel *pfm0 = parsed.channelList()->channel(0)->as<FMChannel>();
  QCOMPARE(pfm0->txTone(), fm0->txTone());
  QCOMPARE(pfm0->rxTone(), fm0->rxTone());

  FMChannel *pfm1 = parsed.channelList()->channel(1)->as<FMChannel>();
  QCOMPARE(pfm1->txTone(), fm1->txTone());
  QCOMPARE(pfm1->rxTone(), fm1->rxTone());

  FMChannel *pfm2 = parsed.channelList()->channel(2)->as<FMChannel>();
  QCOMPARE(pfm2->txTone(), fm2->txTone());
  QCOMPARE(pfm2->rxTone(), fm2->rxTone());

  FMChannel *pfm3 = parsed.channelList()->channel(3)->as<FMChannel>();
  QCOMPARE(pfm3->txTone(), fm3->txTone());
  QCOMPARE(pfm3->rxTone(), fm3->rxTone());
}


void
ChirpTest::testWriterCross() {

}


void
ChirpTest::testWriterBandwidth() {
  Config orig;

  FMChannel *fm0 = new FMChannel();
  fm0->setName("DB0SP"); fm0->setRXFrequency(Frequency::fromMHz(145.6)); fm0->setTXFrequency(Frequency::fromMHz(145.0));
  fm0->setBandwidth(FMChannel::Bandwidth::Wide);
  orig.channelList()->add(fm0);

  FMChannel *fm1 = new FMChannel();
  fm1->setName("DB0SP"); fm1->setRXFrequency(Frequency::fromMHz(145.6)); fm1->setTXFrequency(Frequency::fromMHz(145.0));
  fm1->setBandwidth(FMChannel::Bandwidth::Narrow);
  orig.channelList()->add(fm1);

  QString csv;
  QTextStream stream(&csv);
  ErrorStack err;
  if (! ChirpWriter::write(stream, &orig, err))
    QFAIL(QString("Cannot serialize codeplug:\n%1").arg(err.format()).toStdString().c_str());
  qDebug() << csv;

  Config parsed;
  if (! ChirpReader::read(stream, &parsed, err))
    QFAIL(QString("Cannot parse CHIRP CSV:\n%1").arg(err.format()).toStdString().c_str());

  FMChannel *pfm0 = parsed.channelList()->channel(0)->as<FMChannel>();
  QCOMPARE(pfm0->bandwidth(), fm0->bandwidth());

  FMChannel *pfm1 = parsed.channelList()->channel(1)->as<FMChannel>();
  QCOMPARE(pfm1->bandwidth(), fm1->bandwidth());
}


QTEST_GUILESS_MAIN(ChirpTest)
