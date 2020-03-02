#include "configtest.hh"
#include "config.hh"
#include <QTest>


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
ConfigTest::testMicLevel() {
  QCOMPARE(_config.micLevel(), 3u);
}

void
ConfigTest::testSpeechSynthesis() {
  QCOMPARE(_config.speech(), false);
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
  QCOMPARE(_config.channelList()->channel(0)->name(), QString("BB DB0LDS TS2")); // Check name
  QCOMPARE(_config.channelList()->channel(0)->rxFrequency(), 439.563); // check RX Frequency
  QCOMPARE(_config.channelList()->channel(0)->txFrequency(), 431.963); // check TX Frequency
  QCOMPARE(_config.channelList()->channel(0)->power(), Channel::HighPower); // check power
  QCOMPARE(_config.channelList()->channel(0)->scanList(), nullptr); // Scanlist
  QCOMPARE(_config.channelList()->channel(0)->txTimeout(), 45U);
  QCOMPARE(_config.channelList()->channel(0)->rxOnly(), false);
  QCOMPARE(_config.channelList()->channel(0)->as<DigitalChannel>()->admit(), DigitalChannel::AdmitColorCode);
  QCOMPARE(_config.channelList()->channel(0)->as<DigitalChannel>()->colorCode(), 1u);
  QCOMPARE(_config.channelList()->channel(0)->as<DigitalChannel>()->timeslot(), DigitalChannel::TimeSlot2);
  QCOMPARE(_config.rxGroupLists()->indexOf(
             _config.channelList()->channel(0)->as<DigitalChannel>()->rxGroupList()), 0); // RX Group list index
  QCOMPARE(_config.contacts()->indexOfDigital(
             _config.channelList()->channel(0)->as<DigitalChannel>()->txContact()), 1);   // TX contact index
  QCOMPARE(_config.gpsSystems()->indexOf(
             _config.channelList()->channel(0)->as<DigitalChannel>()->gpsSystem()), 0);   // GPS system index

  /*
   * Test channel 02
   */
  QCOMPARE(_config.channelList()->channel(1)->name(), QString("BB DM0TT TS2")); // Check name
  QCOMPARE(_config.channelList()->channel(1)->rxFrequency(), 439.0870); // check RX Frequency
  QCOMPARE(_config.channelList()->channel(1)->txFrequency(), 431.4870); // check TX Frequency
  QCOMPARE(_config.channelList()->channel(1)->power(), Channel::HighPower); // check power
  QCOMPARE(_config.channelList()->channel(1)->scanList(), nullptr); // Scanlist
  QCOMPARE(_config.channelList()->channel(1)->txTimeout(), 45U);
  QCOMPARE(_config.channelList()->channel(1)->rxOnly(), false);
  QCOMPARE(_config.channelList()->channel(1)->as<DigitalChannel>()->admit(), DigitalChannel::AdmitColorCode);
  QCOMPARE(_config.channelList()->channel(1)->as<DigitalChannel>()->colorCode(), 1u);
  QCOMPARE(_config.channelList()->channel(1)->as<DigitalChannel>()->timeslot(), DigitalChannel::TimeSlot2);
  QCOMPARE(_config.rxGroupLists()->indexOf(
             _config.channelList()->channel(1)->as<DigitalChannel>()->rxGroupList()), 0); // RX Group list index
  QCOMPARE(_config.contacts()->indexOfDigital(
             _config.channelList()->channel(1)->as<DigitalChannel>()->txContact()), 1);   // TX contact index
  QCOMPARE(_config.channelList()->channel(1)->as<DigitalChannel>()->gpsSystem(), nullptr);   // GPS system none

  /*
   * Test channel 03
   */
  QCOMPARE(_config.channelList()->channel(2)->name(), QString("TG9 DB0KK TS1")); // Check name
  QCOMPARE(_config.channelList()->channel(2)->rxFrequency(), 439.5380); // check RX Frequency
  QCOMPARE(_config.channelList()->channel(2)->txFrequency(), 431.9380); // check TX Frequency
  QCOMPARE(_config.channelList()->channel(2)->power(), Channel::HighPower); // check power
  QCOMPARE(_config.channelList()->channel(2)->scanList(), nullptr); // Scanlist
  QCOMPARE(_config.channelList()->channel(2)->txTimeout(), 45U);
  QCOMPARE(_config.channelList()->channel(2)->rxOnly(), false);
  QCOMPARE(_config.channelList()->channel(2)->as<DigitalChannel>()->admit(), DigitalChannel::AdmitColorCode);
  QCOMPARE(_config.channelList()->channel(2)->as<DigitalChannel>()->colorCode(), 1u);
  QCOMPARE(_config.channelList()->channel(2)->as<DigitalChannel>()->timeslot(), DigitalChannel::TimeSlot1);
  QCOMPARE(_config.rxGroupLists()->indexOf(
             _config.channelList()->channel(2)->as<DigitalChannel>()->rxGroupList()), 0); // RX Group list index
  QCOMPARE(_config.contacts()->indexOfDigital(
             _config.channelList()->channel(2)->as<DigitalChannel>()->txContact()), 0);   // TX contact index
  QCOMPARE(_config.channelList()->channel(2)->as<DigitalChannel>()->gpsSystem(), nullptr);   // GPS system index
}

void
ConfigTest::testAnalogChannels() {
  /*
   * Test Channel bank 0
   */
  QCOMPARE(_config.channelList()->count(), 0x5); // First 5 channels enabled

  /*
   * Test channel 04
   */
  QCOMPARE(_config.channelList()->channel(3)->name(), QString("DB0LDS")); // Check name
  QCOMPARE(_config.channelList()->channel(3)->rxFrequency(), 439.5625); // check RX Frequency
  QCOMPARE(_config.channelList()->channel(3)->txFrequency(), 431.9625); // check TX Frequency
  QCOMPARE(_config.channelList()->channel(3)->power(), Channel::HighPower); // check power
  QCOMPARE(_config.channelList()->channel(3)->scanList(), nullptr); // Scanlist
  QCOMPARE(_config.channelList()->channel(3)->txTimeout(), 45U);
  QCOMPARE(_config.channelList()->channel(3)->rxOnly(), false);
  QCOMPARE(_config.channelList()->channel(3)->as<AnalogChannel>()->admit(), AnalogChannel::AdmitFree); // admit
  QCOMPARE(_config.channelList()->channel(3)->as<AnalogChannel>()->squelch(), 1u); // squelch
  QCOMPARE(_config.channelList()->channel(3)->as<AnalogChannel>()->rxTone(), 67.0); // RX CTCSS tone
  QCOMPARE(_config.channelList()->channel(3)->as<AnalogChannel>()->txTone(), 67.0); // TX CTCSS tone

  /*
   * Test channel 05
   */
  QCOMPARE(_config.channelList()->channel(4)->name(), QString("DB0SP-2")); // Check name
  QCOMPARE(_config.channelList()->channel(4)->rxFrequency(), 145.6000); // check RX Frequency
  QCOMPARE(_config.channelList()->channel(4)->txFrequency(), 145.0000); // check TX Frequency
  QCOMPARE(_config.channelList()->channel(4)->power(), Channel::HighPower); // check power
  QCOMPARE(_config.channelList()->channel(4)->scanList(), nullptr); // Scanlist
  QCOMPARE(_config.channelList()->channel(4)->txTimeout(), 45U);
  QCOMPARE(_config.channelList()->channel(4)->rxOnly(), false);
  QCOMPARE(_config.channelList()->channel(4)->as<AnalogChannel>()->admit(), AnalogChannel::AdmitFree); // admit
  QCOMPARE(_config.channelList()->channel(4)->as<AnalogChannel>()->squelch(), 1u); // squelch
  QCOMPARE(_config.channelList()->channel(4)->as<AnalogChannel>()->rxTone(), 0.0); // RX CTCSS tone
  QCOMPARE(_config.channelList()->channel(4)->as<AnalogChannel>()->txTone(), 0.0); // TX CTCSS tone
}

void
ConfigTest::testZones() {
  // Check number of zones (1)
  QCOMPARE(_config.zones()->count(), 1);

  /*
   * Check zone 01
   */
  QCOMPARE(_config.zones()->zone(0)->name(), QString("KW"));
  QCOMPARE(_config.zones()->zone(0)->A()->count(), 3);
  QCOMPARE(_config.zones()->zone(0)->B()->count(), 2);
  QCOMPARE(_config.channelList()->indexOf(
             _config.zones()->zone(0)->A()->channel(0)), 0);
  QCOMPARE(_config.channelList()->indexOf(
             _config.zones()->zone(0)->A()->channel(1)), 2);
  QCOMPARE(_config.channelList()->indexOf(
             _config.zones()->zone(0)->A()->channel(2)), 4);
  QCOMPARE(_config.channelList()->indexOf(
             _config.zones()->zone(0)->B()->channel(0)), 1);
  QCOMPARE(_config.channelList()->indexOf(
             _config.zones()->zone(0)->B()->channel(1)), 3);
}

void
ConfigTest::testScanLists() {
  // pass...
}

void
ConfigTest::testGPSSystems() {
  // Check number of GPS systems (1)
  QCOMPARE(_config.gpsSystems()->count(), 1);

  /*
   * Check GPS system 01
   */
  QCOMPARE(_config.gpsSystems()->gpsSystem(0)->name(), QString("BM APRS"));
  QCOMPARE(_config.contacts()->indexOfDigital(
             _config.gpsSystems()->gpsSystem(0)->contact()), 3);
  QCOMPARE(_config.gpsSystems()->gpsSystem(0)->period(), 300u);
  QCOMPARE(_config.gpsSystems()->gpsSystem(0)->revertChannel(), nullptr);
}


QTEST_GUILESS_MAIN(ConfigTest)
