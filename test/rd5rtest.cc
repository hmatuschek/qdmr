#include "rd5rtest.hh"
#include "config.hh"
#include <QTest>
#include "utils.hh"
#include <QDebug>

RD5RTest::RD5RTest(QObject *parent) : QObject(parent)
{
  // pass...
}

void
RD5RTest::initTestCase() {
  // Read simple configuration file
  Config config;
  QString errMessage;
  QVERIFY(config.readCSV("://testconfig.conf", errMessage));
  // Encode config as code-plug
  QVERIFY(_codeplug.encode(&config));
}

void
RD5RTest::cleanupTestCase() {
  // clear codeplug
  _codeplug.clear();
}

void
RD5RTest::testRadioName() {
  QCOMPARE(decode_ascii(_codeplug.data(0x000e0+0x00), 16, 0xff), QString("DM3MAT"));
}

void
RD5RTest::testDMRID() {
  QCOMPARE(decode_dmr_id_bcd(_codeplug.data(0x000e0+0x08)), 1234567U);
}

void
RD5RTest::testIntroLines() {
  QCOMPARE(decode_ascii(_codeplug.data(0x07540+0x00), 16, 0xff), QString("ABC"));
  QCOMPARE(decode_ascii(_codeplug.data(0x07540+0x10), 16, 0xff), QString("DEF"));
}

void
RD5RTest::testDigitalContacts() {
  /*
   * Test Contact 01
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x01788+0x00), 16, 0xff), QString("Lokal"));
  QCOMPARE(decode_dmr_id_bcd(_codeplug.data(0x01788+0x10)), 9U);
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x14)), 0x00); // group call
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x15)), 0x00); // receive tone (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x16)), 0x00); // ring style (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x17)), 0x00); // unused byte 0x00

  /*
   * Test Contact 02
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x01788+0x18), 16, 0xff), QString("Bln/Brb"));
  QCOMPARE(decode_dmr_id_bcd(_codeplug.data(0x01788+0x28)), 2621U);
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x2c)), 0x00); // group call
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x2d)), 0x00); // receive tone (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x2e)), 0x00); // ring style (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x2f)), 0x00); // unused byte 0x00

  /*
   * Test Contact 03
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x01788+0x30), 16, 0xff), QString("All Call"));
  QCOMPARE(decode_dmr_id_bcd(_codeplug.data(0x01788+0x40)), 16777215U);
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x44)), 0x02); // all call
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x45)), 0x00); // receive tone (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x46)), 0x00); // ring style (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x47)), 0x00); // unused byte 0x00

  /*
   * Test Contact 04
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x01788+0x48), 16, 0xff), QString("BM APRS"));
  QCOMPARE(decode_dmr_id_bcd(_codeplug.data(0x01788+0x58)), 262999U);
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x5c)), 0x01); // private call
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x5d)), 0x00); // receive tone (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x5e)), 0x00); // ring style (off)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x01788+0x5f)), 0x00); // unused byte 0x00
}

void
RD5RTest::testRXGroups() {
  /*
   * Test RX Group List table
   */
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x1d620+0x00)), 0x03); // Number of contacts in first list (+1)
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x1d620+0x01)), 0x00); // Second list disabled == 0.

  /*
   * Test RX Group List 01
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x1d620+0x80), 16, 0xff), QString("Berlin/Brand")); // Check name
  QCOMPARE((int)*((uint16_t *)_codeplug.data(0x1d620+0x90)), 0x01); // 1st member index +1
  QCOMPARE((int)*((uint16_t *)_codeplug.data(0x1d620+0x92)), 0x02); // 2nd member index +1
  QCOMPARE((int)*((uint16_t *)_codeplug.data(0x1d620+0x94)), 0x00); // 3nd member index == 0 (EOL)
}

void
RD5RTest::testDigitalChannels() {
  /*
   * Test Channel bank 0
   */
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x03780+0x00)), 0x1f); // First 5 channels enabled
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x03780+0x01)), 0x00); // rest is disabled

  /*
   * Test channel 01
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x03780+0x10), 16, 0xff), QString("BB DB0LDS TS2")); // Check name
  qDebug() << decode_frequency(*(uint32_t *)_codeplug.data(0x03780+0x20)) - 439.563;
  qDebug() << decode_frequency(*(uint32_t *)_codeplug.data(0x03780+0x24)) - 431.963;
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x03780+0x20)), 439.563); // check RX Frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x03780+0x24)), 431.963); // check TX Frequency
}


QTEST_GUILESS_MAIN(RD5RTest)
