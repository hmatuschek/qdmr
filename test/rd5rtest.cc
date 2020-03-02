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
  QCOMPARE(decode_ascii(_codeplug.data(0x03790+0x00), 16, 0xff), QString("BB DB0LDS TS2")); // Check name
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x03790+0x10)), 439.563); // check RX Frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x03790+0x14)), 431.963); // check TX Frequency
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x18), 1);       // check mode 1 == Digital
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03790+0x19), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x1b), 3);       // tot 3 == 45s
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x1c), 0);       // tot re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x1d), 2);       // admit color code
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x1e), 0x50);    // unused 0x50
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x1f), 0);       // scanlist index +1 (0=none)
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03790+0x20), 0xffff);  // rx ctcss 0xffff = none
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03790+0x22), 0xffff);  // tx ctcss 0xffff = none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x24), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x25), 0);       // TX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x26), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x27), 0);       // RX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x28), 0x16);    // unused 0x16
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x29), 0);       // priv group 0=none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x2a), 1);       // CC TX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x2b), 1);       // GprListIdx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x2c), 1);       // CC RX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x2d), 0);       // Emergency Sys 0=None
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03790+0x2e), 2);       // TX Contact idx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x30), 0);       // Em ACK + DataCallConf.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x31), 0x40);    // PrivateCallConf, Priv., TS2
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x32), 0);       // DCDM, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x33), 0x80);    // Power, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x34), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x35), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x36), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03790+0x37), 0);       // Squelch = 0

  /*
   * Test channel 02
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x037c8+0x00), 16, 0xff), QString("BB DM0TT TS2")); // Check name
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x037c8+0x10)), 439.087); // check RX Frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x037c8+0x14)), 431.487); // check TX Frequency
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x18), 1);       // check mode 1 == Digital
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x037c8+0x19), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x1b), 3);       // tot 3 == 45s
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x1c), 0);       // tot re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x1d), 2);       // admit color code
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x1e), 0x50);    // unused 0x50
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x1f), 0);       // scanlist index +1 (0=none)
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x037c8+0x20), 0xffff);  // rx ctcss 0xffff = none
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x037c8+0x22), 0xffff);  // tx ctcss 0xffff = none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x24), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x25), 0);       // TX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x26), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x27), 0);       // RX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x28), 0x16);    // unused 0x16
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x29), 0);       // priv group 0=none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x2a), 1);       // CC TX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x2b), 1);       // GprListIdx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x2c), 1);       // CC RX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x2d), 0);       // Emergency Sys 0=None
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x037c8+0x2e), 2);       // TX Contact idx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x30), 0);       // Em ACK + DataCallConf.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x31), 0x40);    // PrivateCallConf, Priv., TS2
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x32), 0);       // DCDM, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x33), 0x80);    // Power, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x34), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x35), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x36), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x037c8+0x37), 0);       // Squelch = 0

  /*
   * Test channel 03
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x03800+0x00), 16, 0xff), QString("TG9 DB0KK TS1")); // Check name
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x03800+0x10)), 439.538); // check RX Frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x03800+0x14)), 431.938); // check TX Frequency
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x18), 1);       // check mode 1 == Digital
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03800+0x19), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x1b), 3);       // tot 3 == 45s
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x1c), 0);       // tot re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x1d), 2);       // admit color code
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x1e), 0x50);    // unused 0x50
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x1f), 0);       // scanlist index +1 (0=none)
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03800+0x20), 0xffff);  // rx ctcss 0xffff = none
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03800+0x22), 0xffff);  // tx ctcss 0xffff = none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x24), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x25), 0);       // TX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x26), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x27), 0);       // RX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x28), 0x16);    // unused 0x16
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x29), 0);       // priv group 0=none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x2a), 1);       // CC TX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x2b), 1);       // GprListIdx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x2c), 1);       // CC RX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x2d), 0);       // Emergency Sys 0=None
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x03800+0x2e), 1);       // TX Contact idx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x30), 0);       // Em ACK + DataCallConf.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x31), 0x00);    // PrivateCallConf, Priv., TS2
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x32), 0);       // DCDM, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x33), 0x80);    // Power, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x34), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x35), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x36), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x03800+0x37), 0);       // Squelch = 0
}


void
RD5RTest::testAnalogChannels() {
  /*
   * Test Channel bank 0
   */
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x03780+0x00)), 0x1f); // First 5 channels enabled
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x03780+0x01)), 0x00); // rest is disabled

  /*
   * Test channel 04
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x3838+0x00), 16, 0xff), QString("DB0LDS")); // Check name
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x3838+0x10)), 439.5625); // check RX Frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x3838+0x14)), 431.9625); // check TX Frequency
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x18), 0);       // check mode 0 == Analog
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x3838+0x19), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x1b), 3);       // tot 3 == 45s
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x1c), 0);       // tot re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x1d), 1);       // admit 1 = free
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x1e), 0x50);    // unused 0x50
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x1f), 0);       // scanlist index +1 (0=none)
  QCOMPARE((uint16_t)*(uint16_t *)_codeplug.data(0x3838+0x20), encode_ctcss_tone_table(67.0));  // rx ctcss 0xffff = none
  QCOMPARE((uint16_t)*(uint16_t *)_codeplug.data(0x3838+0x22), encode_ctcss_tone_table(67.0));  // tx ctcss 0xffff = none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x24), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x25), 0);       // TX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x26), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x27), 0);       // RX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x28), 0x16);    // unused 0x16
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x29), 0);       // priv group 0=none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x2a), 0);       // CC TX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x2b), 0);       // GprListIdx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x2c), 0);       // CC RX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x2d), 0);       // Emergency Sys 0=None
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x3838+0x2e), 0);       // TX Contact idx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x30), 0);       // Em ACK + DataCallConf.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x31), 0);       // PrivateCallConf, Priv., TS2
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x32), 0);       // DCDM, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x33), 0x80);    // Power, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x34), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x35), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x36), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3838+0x37), 1);       // Squelch = 1

  /*
   * Test channel 05
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x3870+0x00), 16, 0xff), QString("DB0SP-2")); // Check name
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x3870+0x10)), 145.6); // check RX Frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x3870+0x14)), 145.0); // check TX Frequency
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x18), 0);       // check mode 0 == Analog
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x3870+0x19), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x1b), 3);       // tot 3 == 45s
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x1c), 0);       // tot re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x1d), 1);       // admit free
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x1e), 0x50);    // unused 0x50
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x1f), 0);       // scanlist index +1 (0=none)
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x3870+0x20), 0xffff);  // rx ctcss 0xffff = none
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x3870+0x22), 0xffff);  // tx ctcss 0xffff = none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x24), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x25), 0);       // TX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x26), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x27), 0);       // RX sig. 0=off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x28), 0x16);    // unused 0x16
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x29), 0);       // priv group 0=none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x2a), 0);       // CC TX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x2b), 0);       // GprListIdx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x2c), 0);       // CC RX
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x2d), 0);       // Emergency Sys 0=None
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x3870+0x2e), 0);       // TX Contact idx +1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x30), 0);       // Em ACK + DataCallConf.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x31), 0);    // PrivateCallConf, Priv., TS2
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x32), 0);       // DCDM, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x33), 0x80);    // Power, ...
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x34), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x35), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x36), 0);       // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x3870+0x37), 1);       // Squelch = 0
}

void
RD5RTest::testZones() {
  /*
   * Test zone bank
   */
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x08010+0x00)), 0x03); // First 2 zones enabled
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x08010+0x01)), 0x00); // rest is disabled

  /*
   * Test zone 01
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x08030+0x00), 16, 0xff), QString("KW A")); // Check name
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x08030+0x10), 1); // channel 1
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x08030+0x12), 3); // channel 3
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x08030+0x14), 5); // channel 5
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x08030+0x16), 0); // not set
  /*
   * Test zone 02
   */
  QCOMPARE(decode_ascii(_codeplug.data(0x08060+0x00), 16, 0xff), QString("KW B")); // Check name
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x08060+0x10), 2); // channel 2
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x08060+0x12), 4); // channel 4
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x08060+0x14), 0); // not set
}

void
RD5RTest::testScanLists() {
  /*
   * Test scan list bank
   */
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x17620+0x00)), 0x00); // No scanlist enabled.
  QCOMPARE((int)*((uint8_t *)_codeplug.data(0x17620+0x01)), 0x00); // No scanlist enabled.
}

QTEST_GUILESS_MAIN(RD5RTest)
