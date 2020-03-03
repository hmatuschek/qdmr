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
  QString errMessage;
  QVERIFY(_config.readCSV("://testconfig.conf", errMessage));
  // Encode config as code-plug
  QVERIFY(_codeplug.encode(&_config));
}

void
RD5RTest::cleanupTestCase() {
  // clear codeplug
  _codeplug.clear();
}

void
RD5RTest::testRadioName() {
  QCOMPARE(decode_ascii(_codeplug.data(0x000e0+0x00), 8, 0xff), QString("DM3MAT"));
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
RD5RTest::testGeneralDefaults() {
  // Unused 0x00
  QCOMPARE((int)*(uint32_t *)_codeplug.data(0x000e0+0x0c), 0);
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x10), 0);
  // TX preamble default=0x06
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x11), 6);
  // Monitor type 0=open SQ
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x12), 0);
  // VOX sensitivity default=3
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x13), 3);
  // low batt interval default=6
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x14), 6);
  // Call alert dur default=0x18
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x15), 0x18);
  // Lone worker response default=1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x16), 1);
   // Lone worker reminder default=10
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x17), 10);
  // Group call hang time default=6
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x18), 6);
  // Private call hang time default=6
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x19), 6);
  // Up/Down-Ch mode, RST tone, unk. numb. tone, ARTS tone, default=0x40
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x1a), 0x40);
  // perm. tone analog/digi, self-test tone, CH freq. ind. tone, dis. all tones, save bat, default=0xc4
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x1b), 0xc4);
   // dis. LEDs, quick-key override, default=0x80
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x1c), 0x80);
  // TX exit tone, TX on act. CH, animation, scan mode, default=0x10
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x1d), 0x10);
  // repeat. delay & STE, default=0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x1e), 0);
  // unused 0x00
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x000e0+0x1f), 0);
  // prog passwd., disabled=0xffffffffffffffff
  QCOMPARE(*(uint64_t *)_codeplug.data(0x000e0+0x20), 0xffffffffffffffffUL);
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
  QCOMPARE(decode_ascii(_codeplug.data(0x1d6a0+0x00), 16, 0xff), QString("Berlin/Brand")); // Check name
  QCOMPARE((int)*((uint16_t *)_codeplug.data(0x1d6a0+0x10)), 0x01); // 1st member index +1
  QCOMPARE((int)*((uint16_t *)_codeplug.data(0x1d6a0+0x12)), 0x02); // 2nd member index +1
  QCOMPARE((int)*((uint16_t *)_codeplug.data(0x1d6a0+0x14)), 0x00); // 3nd member index == 0 (EOL)
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

void
RD5RTest::testDecode() {
  /* Decode config */
  Config decoded;
  QVERIFY(_codeplug.decode(&decoded));

  // Compare basic config
  QCOMPARE(decoded.name(), _config.name());
  QCOMPARE(decoded.id(), _config.id());
  QCOMPARE(decoded.introLine1(), _config.introLine1());
  QCOMPARE(decoded.introLine2(), _config.introLine2());
  QCOMPARE(decoded.micLevel(), _config.micLevel());
  QCOMPARE(decoded.speech(), _config.speech());

  // Compare contacts
  QCOMPARE(decoded.contacts()->count(), _config.contacts()->count());
  for (int i=0; i<_config.contacts()->count(); i++) {
    // Compare name
    QCOMPARE(decoded.contacts()->contact(i)->name(), _config.contacts()->contact(i)->name());
    // Compare number
    QCOMPARE(decoded.contacts()->contact(i)->as<DigitalContact>()->number(),
             _config.contacts()->contact(i)->as<DigitalContact>()->number());
    // Compare type
    QCOMPARE(decoded.contacts()->contact(i)->as<DigitalContact>()->type(),
             _config.contacts()->contact(i)->as<DigitalContact>()->type());
    // Compare tone
    QCOMPARE(decoded.contacts()->contact(i)->rxTone(),
             _config.contacts()->contact(i)->rxTone());
  }

  // Compare RX Groups
  QCOMPARE(decoded.rxGroupLists()->count(), _config.rxGroupLists()->count());
  for (int i=0; i<_config.rxGroupLists()->count(); i++) {
    // Compare name
    QCOMPARE(decoded.rxGroupLists()->list(i)->name(), _config.rxGroupLists()->list(i)->name());
    // Compare number of entries
    QCOMPARE(decoded.rxGroupLists()->list(i)->count(), _config.rxGroupLists()->list(i)->count());
    // Compare entries
    for (int j=0; j<_config.rxGroupLists()->list(i)->count(); j++) {
      QCOMPARE(decoded.contacts()->indexOf(decoded.rxGroupLists()->list(i)->contact(j)),
               _config.contacts()->indexOf(_config.rxGroupLists()->list(i)->contact(j)));
    }
  }

  // Compare Channels
  QCOMPARE(decoded.channelList()->count(), _config.channelList()->count());
  for (int i=0; i<_config.channelList()->count(); i++) {
    // Compare name
    QCOMPARE(decoded.channelList()->channel(i)->name(), _config.channelList()->channel(i)->name());
    // RX Frequency
    QCOMPARE(decoded.channelList()->channel(i)->rxFrequency(), _config.channelList()->channel(i)->rxFrequency());
    // TX Frequency
    QCOMPARE(decoded.channelList()->channel(i)->txFrequency(), _config.channelList()->channel(i)->txFrequency());
    // Power
    QCOMPARE(decoded.channelList()->channel(i)->power(), _config.channelList()->channel(i)->power());
    // TOT
    QCOMPARE(decoded.channelList()->channel(i)->txTimeout(), _config.channelList()->channel(i)->txTimeout());
    // RX only flag
    QCOMPARE(decoded.channelList()->channel(i)->rxOnly(), _config.channelList()->channel(i)->rxOnly());
    // Scanlist
    QCOMPARE(decoded.scanlists()->indexOf(decoded.channelList()->channel(i)->scanList()),
             _config.scanlists()->indexOf(_config.channelList()->channel(i)->scanList()));
    // Check type
    QCOMPARE(decoded.channelList()->channel(i)->is<DigitalChannel>(),
             _config.channelList()->channel(i)->is<DigitalChannel>());
    // Dispatch by type
    if (_config.channelList()->channel(i)->is<DigitalChannel>()) {
      DigitalChannel *dec = decoded.channelList()->channel(i)->as<DigitalChannel>();
      DigitalChannel *ori = _config.channelList()->channel(i)->as<DigitalChannel>();
      // Compare admit criterion
      QCOMPARE(dec->admit(), ori->admit());
      // color code
      QCOMPARE(dec->colorCode(), ori->colorCode());
      // time slot
      QCOMPARE(dec->timeslot(), ori->timeslot());
      // RX group list
      QCOMPARE(decoded.rxGroupLists()->indexOf(dec->rxGroupList()),
               _config.rxGroupLists()->indexOf(ori->rxGroupList()));
      // TX contact
      QCOMPARE(decoded.contacts()->indexOf(dec->txContact()),
               _config.contacts()->indexOf(ori->txContact()));
      // do not check GSP system (RD5R has no GPS).
      QCOMPARE(dec->gpsSystem(), nullptr);
    } else {
      AnalogChannel *dec = decoded.channelList()->channel(i)->as<AnalogChannel>();
      AnalogChannel *ori = _config.channelList()->channel(i)->as<AnalogChannel>();
      // Compare admit criterion
      QCOMPARE(dec->admit(), ori->admit());
      // squelch
      QCOMPARE(dec->squelch(), ori->squelch());
      // RX Tone
      QCOMPARE(dec->rxTone(), ori->rxTone());
      // TX Tone
      QCOMPARE(dec->txTone(), ori->txTone());
      // Bandwidth
      QCOMPARE(dec->bandwidth(), ori->bandwidth());
    }
  }

  // Compare Zones. Note Zones with VFO A/B are split into individual zones for RD5R
  QCOMPARE(decoded.zones()->count(), 2*_config.zones()->count());
  QCOMPARE(decoded.zones()->zone(0)->name(), _config.zones()->zone(0)->name()+" A");
  QCOMPARE(decoded.zones()->zone(0)->A()->count(), _config.zones()->zone(0)->A()->count());
  for (int i=0; i<_config.zones()->zone(0)->A()->count(); i++) {
    QCOMPARE(decoded.channelList()->indexOf(decoded.zones()->zone(0)->A()->channel(i)),
             _config.channelList()->indexOf(_config.zones()->zone(0)->A()->channel(i)));
  }
  QCOMPARE(decoded.zones()->zone(1)->name(), _config.zones()->zone(0)->name()+" B");
  QCOMPARE(decoded.zones()->zone(1)->A()->count(), _config.zones()->zone(0)->B()->count());
  for (int i=0; i<_config.zones()->zone(0)->B()->count(); i++) {
    QCOMPARE(decoded.channelList()->indexOf(decoded.zones()->zone(1)->A()->channel(i)),
             _config.channelList()->indexOf(_config.zones()->zone(0)->B()->channel(i)));
  }

  // Compare scanlist
  QCOMPARE(decoded.scanlists()->count(), _config.scanlists()->count());

  // Do not compare GPS systems (RD5R has no GPS).
  QCOMPARE(decoded.gpsSystems()->count(), 0);
}

QTEST_GUILESS_MAIN(RD5RTest)
