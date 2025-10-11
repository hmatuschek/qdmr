#include "utilstest.hh"

#include <QTest>
#include <QtEndian>
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

  QCOMPARE(Frequency::fromString("100").inHz(), 100000000ULL);
  QCOMPARE(Frequency::fromString("100.0").inHz(), 100000000ULL);
}


void
UtilsTest::testLocator() {
  QGeoCoordinate coor;

  coor = loc2deg("JO62");
  QVERIFY(coor.isValid());
  QCOMPARE(deg2loc(coor, 4), "JO62");

  coor = loc2deg("JO62jl");
  QVERIFY(coor.isValid());
  QCOMPARE(deg2loc(coor, 6), "JO62jl");

  coor = loc2deg("JO62jl55");
  QVERIFY(coor.isValid());
  QCOMPARE(deg2loc(coor, 8), "JO62jl55");

  coor = loc2deg("JO62jl55jj");
  QVERIFY(coor.isValid());
  QCOMPARE(deg2loc(coor, 10), "JO62jl55jj");
}


void
UtilsTest::testEndianess() {
  char val1le[] = {0x34, 0x12}, val1be[] = {0x12,0x34};

  QCOMPARE(qFromLittleEndian(*(quint16 *)val1le), 0x1234);
  QCOMPARE(qFromBigEndian(*(quint16 *)val1be), 0x1234);

  char val2le[] = {0x78, 0x56, 0x34, 0x12}, val2be[] = {0x12, 0x34, 0x56, 0x78};

  QCOMPARE(qFromLittleEndian(*(quint32 *)val2le), 0x12345678);
  QCOMPARE(qFromBigEndian(*(quint32 *)val2be), 0x12345678);
}


void
UtilsTest::testFrequencyNearestMap() {
  auto map = Frequency::MapNearest<unsigned int>(
        {
          {Frequency::fromHz(100), 0},
          {Frequency::fromHz(200), 1},
          {Frequency::fromHz(300), 2},
          {Frequency::fromHz(400), 3},
        });

  QCOMPARE(map.key(Frequency::fromHz(10)),  Frequency::fromHz(100));
  QCOMPARE(map.key(Frequency::fromHz(100)), Frequency::fromHz(100));
  QCOMPARE(map.key(Frequency::fromHz(110)), Frequency::fromHz(100));
  QCOMPARE(map.key(Frequency::fromHz(160)), Frequency::fromHz(200));
  QCOMPARE(map.key(Frequency::fromHz(200)), Frequency::fromHz(200));
  QCOMPARE(map.key(Frequency::fromHz(210)), Frequency::fromHz(200));
  QCOMPARE(map.key(Frequency::fromHz(360)), Frequency::fromHz(400));
  QCOMPARE(map.key(Frequency::fromHz(400)), Frequency::fromHz(400));
  QCOMPARE(map.key(Frequency::fromHz(410)), Frequency::fromHz(400));

  QCOMPARE(map.value(Frequency::fromHz(10)),  0);
  QCOMPARE(map.value(Frequency::fromHz(100)), 0);
  QCOMPARE(map.value(Frequency::fromHz(110)), 0);
  QCOMPARE(map.value(Frequency::fromHz(160)), 1);
  QCOMPARE(map.value(Frequency::fromHz(200)), 1);
  QCOMPARE(map.value(Frequency::fromHz(210)), 1);
  QCOMPARE(map.value(Frequency::fromHz(360)), 3);
  QCOMPARE(map.value(Frequency::fromHz(400)), 3);
  QCOMPARE(map.value(Frequency::fromHz(410)), 3);
}


QTEST_GUILESS_MAIN(UtilsTest)
