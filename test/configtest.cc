#include "configtest.hh"
#include "config.hh"
#include <QTest>
#include <iostream>


ConfigTest::ConfigTest(QObject *parent) : QObject(parent)
{
  // pass...
}


void
ConfigTest::initTestCase() {
  // Read simple configuration file
  QString errMessage;
  QVERIFY(_config.readCSV("://testconfig.conf", errMessage));
}

void
ConfigTest::cleanupTestCase() {
  // clear codeplug
  _config.reset();
}

void
ConfigTest::testRadioName() {
  QCOMPARE(_config.name(), QString("DM3MAT"));
}

void
ConfigTest::testDMRID() {
  QCOMPARE(_config.id(), 1234567U);
}

void
ConfigTest::testIntroLines() {
  QCOMPARE(_config.introLine1(), QString("ABC"));
  QCOMPARE(_config.introLine2(), QString("DEF"));
}

void
ConfigTest::testDigitalContacts() {
  QCOMPARE(_config.contacts()->digitalCount(), 4); // Number of digital contacts
  /*
   * Test Contact 01
   */
  QCOMPARE(_config.contacts()->digitalContact(0)->name(), QString("Lokal"));
  QCOMPARE(_config.contacts()->digitalContact(0)->number(), 9U);
  QCOMPARE(_config.contacts()->digitalContact(0)->type(), DigitalContact::GroupCall);
  QCOMPARE(_config.contacts()->digitalContact(0)->rxTone(), false); // receive tone (off)

  /*
   * Test Contact 02
   */
  QCOMPARE(_config.contacts()->digitalContact(1)->name(), QString("Bln/Brb"));
  QCOMPARE(_config.contacts()->digitalContact(1)->number(), 2621U);
  QCOMPARE(_config.contacts()->digitalContact(1)->type(), DigitalContact::GroupCall); // group call
  QCOMPARE(_config.contacts()->digitalContact(1)->rxTone(), false); // receive tone (off)

  /*
   * Test Contact 03
   */
  QCOMPARE(_config.contacts()->digitalContact(2)->name(), QString("All Call"));
  QCOMPARE(_config.contacts()->digitalContact(2)->number(), 16777215U);
  QCOMPARE(_config.contacts()->digitalContact(2)->type(), DigitalContact::AllCall); // all call
  QCOMPARE(_config.contacts()->digitalContact(2)->rxTone(), false); // receive tone (off)

  /*
   * Test Contact 04
   */
  QCOMPARE(_config.contacts()->digitalContact(3)->name(), QString("BM APRS"));
  QCOMPARE(_config.contacts()->digitalContact(3)->number(), 262999U);
  QCOMPARE(_config.contacts()->digitalContact(3)->type(), DigitalContact::PrivateCall); // private call
  QCOMPARE(_config.contacts()->digitalContact(3)->rxTone(), false); // receive tone (off)
}

void
ConfigTest::testRXGroups() {
  /*
   * Test RX Group List table
   */
  QCOMPARE(_config.rxGroupLists()->count(), 1); // Number group lists

  /*
   * Test RX Group List 01
   */
  QCOMPARE(_config.rxGroupLists()->list(0)->count(), 2); // Number group lists
  QCOMPARE(_config.rxGroupLists()->list(0)->name(), QString("Berlin/Brand")); // Check name
  QCOMPARE(_config.contacts()->indexOfDigital(_config.rxGroupLists()->list(0)->contact(0)), 0x00); // 1st member index +1
  QCOMPARE(_config.contacts()->indexOfDigital(_config.rxGroupLists()->list(0)->contact(1)), 0x01); // 2nd member index +1
}

void
ConfigTest::testDigitalChannels() {
  /*
   * Test Channel bank 0
   */
  QCOMPARE(_config.channelList()->count(), 0x5); // First 5 channels enabled

  /*
   * Test channel 01
   */
  std::cerr.precision(17);
  std::cerr << "Got RX: " << std::fixed << _config.channelList()->channel(0)->rxFrequency() << std::endl;
  QCOMPARE(_config.channelList()->channel(0)->name(), QString("BB DB0LDS TS2")); // Check name
  QCOMPARE(_config.channelList()->channel(0)->rxFrequency(), 439.563); // check RX Frequency
  QCOMPARE(_config.channelList()->channel(0)->txFrequency(), 431.963); // check TX Frequency
}



QTEST_GUILESS_MAIN(ConfigTest)
