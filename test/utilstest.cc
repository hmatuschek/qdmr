#include "utilstest.hh"

#include <QTest>
#include "utils.hh"

UtilsTest::UtilsTest(QObject *parent) : QObject(parent)
{
  // pass...
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


QTEST_GUILESS_MAIN(UtilsTest)
