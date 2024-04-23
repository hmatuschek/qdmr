#include "utilstest.hh"

#include <QTest>
#include "utils.hh"
#include "frequency.hh"
#include "chirpformat.hh"
#include "config.hh"


UtilsTest::UtilsTest(QObject *parent)
  : QObject{parent}
{
  // pass...
}

void
UtilsTest::initTestCase() {
}

void
UtilsTest::testDecodeUnicode() {
  QString testString("abc123äöü");
  QCOMPARE(decode_unicode((uint16_t *)testString.data(), testString.size()), testString);
}

void
UtilsTest::testEncodeUnicode() {
  QString testString("abc123äöü");
  QByteArray bufferTrue(32, 0x00), bufferTest(32, 0xff);
  memcpy(bufferTrue.data(), testString.data(), 18);
  encode_unicode((uint16_t *)bufferTest.data(), testString, 16);
  QCOMPARE(bufferTest, bufferTrue);
}

void
UtilsTest::testEncodeASCII() {
  const char *testString = "abc";
  QByteArray bufferTrue(16, 0xff), bufferTest(16,0x00);
  memcpy(bufferTrue.data(), testString, 3);
  encode_ascii((uint8_t *)bufferTest.data(), testString, 16, 0xff);
  QCOMPARE(bufferTest, bufferTrue);
}

void
UtilsTest::testDecodeASCII() {
  const char *testString = "abc\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";
  QString res = decode_ascii((const uint8_t *)testString, 16, 0xff);
  QCOMPARE(res, QString("abc"));
}

void
UtilsTest::testDecodeFrequency() {
  uint32_t bcd = 0x12345678U;
  double freq  = 123.45678;
  QCOMPARE(decode_frequency(bcd), freq);
}

void
UtilsTest::testEncodeFrequency() {
  uint32_t bcd = 0x12345678U;
  double freq  = 123.45678;
  QCOMPARE(encode_frequency(freq), bcd);

  QCOMPARE(decode_frequency(encode_frequency(439.5630)), 439.5630);
}

void
UtilsTest::testDecodeDMRID_bcd() {
  uint8_t bcd[4] = {0x12, 0x34, 0x56, 0x78};
  uint32_t num = 12345678U;
  QCOMPARE(decode_dmr_id_bcd((uint8_t *)&bcd), num);
}

void
UtilsTest::testEncodeDMRID_bcd() {
  const char bcd[4] = {0x12, 0x34, 0x56, 0x78};
  uint32_t num = 12345678U;
  QByteArray res(4, 0);
  encode_dmr_id_bcd((uint8_t *)res.data(), num);
  QCOMPARE(res, QByteArray(bcd, 4));
}

void
UtilsTest::testFrequencyParser() {
  QCOMPARE(Frequency::fromString("100Hz").inHz(), 100ULL);
  QCOMPARE(Frequency::fromString("100 Hz").inHz(), 100ULL);
  QCOMPARE(Frequency::fromString("100kHz").inHz(), 100000ULL);
  QCOMPARE(Frequency::fromString("100 kHz").inHz(), 100000ULL);
  QCOMPARE(Frequency::fromString("100MHz").inHz(), 100000000ULL);
  QCOMPARE(Frequency::fromString("100 MHz").inHz(), 100000000ULL);
  QCOMPARE(Frequency::fromString("100GHz").inHz(), 100000000000ULL);
  QCOMPARE(Frequency::fromString("100 GHz").inHz(), 100000000000ULL);

  QCOMPARE(Frequency::fromString("100").inHz(), 100ULL);
  QCOMPARE(Frequency::fromString("100.0").inHz(), 100000000ULL);
}

void
UtilsTest::testChirpReaderBasic() {

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
UtilsTest::testChirpReaderCTCSS() {

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
UtilsTest::testChirpReaderDCS() {

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
UtilsTest::testChirpReaderCross() {

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
UtilsTest::testChirpWriter() {

}

QTEST_GUILESS_MAIN(UtilsTest)
