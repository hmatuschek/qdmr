#include "uv390test.hh"
#include "config.hh"
#include <QTest>
#include "utils.hh"
#include <QDebug>

UV390Test::UV390Test(QObject *parent) : QObject(parent)
{
  // pass...
}

void
UV390Test::initTestCase() {
  // Read simple configuration file
  QString errMessage;
  QVERIFY(_config.readCSV("://testconfig.conf", errMessage));
  // Encode config as code-plug
  QVERIFY(_codeplug.encode(&_config));
}

void
UV390Test::cleanupTestCase() {
  // clear codeplug
  _codeplug.clear();
}

void
UV390Test::testRadioName() {

}

void
UV390Test::testDMRID() {

}

void
UV390Test::testIntroLines() {
}

void
UV390Test::testGeneralDefaults() {
}

void
UV390Test::testDigitalContacts() {
}

void
UV390Test::testRXGroups() {
}

void
UV390Test::testDigitalChannels() {
}


void
UV390Test::testAnalogChannels() {
}

void
UV390Test::testZones() {
}

void
UV390Test::testScanLists() {
}

void
UV390Test::testDecode() {
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
      QCOMPARE(nullptr != dec->gpsSystem(), nullptr != ori->gpsSystem());
      QCOMPARE(decoded.gpsSystems()->indexOf(dec->gpsSystem()),
               _config.gpsSystems()->indexOf(ori->gpsSystem()));
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

  // Compare Zones.
  QCOMPARE(decoded.zones()->count(), _config.zones()->count());
  for (int i=0; i<_config.zones()->count(); i++) {
    QCOMPARE(decoded.zones()->zone(i)->name(), _config.zones()->zone(i)->name());
    QCOMPARE(decoded.zones()->zone(i)->A()->count(), _config.zones()->zone(i)->A()->count());
    for (int j=0; j<_config.zones()->zone(0)->A()->count(); j++) {
      QCOMPARE(decoded.channelList()->indexOf(decoded.zones()->zone(i)->A()->channel(j)),
               _config.channelList()->indexOf(_config.zones()->zone(i)->A()->channel(j)));
    }
    QCOMPARE(decoded.zones()->zone(i)->B()->count(), _config.zones()->zone(i)->B()->count());
    for (int j=0; j<_config.zones()->zone(0)->B()->count(); j++) {
      QCOMPARE(decoded.channelList()->indexOf(decoded.zones()->zone(i)->B()->channel(j)),
               _config.channelList()->indexOf(_config.zones()->zone(i)->B()->channel(j)));
    }
  }

  // Compare scanlist
  QCOMPARE(decoded.scanlists()->count(), _config.scanlists()->count());

  // Compare GPS systems
  QCOMPARE(decoded.gpsSystems()->count(), _config.gpsSystems()->count());
  for (int i=0; i<_config.gpsSystems()->count(); i++) {
    QCOMPARE(decoded.gpsSystems()->gpsSystem(i)->name(), _config.gpsSystems()->gpsSystem(i)->name());
    QCOMPARE(decoded.gpsSystems()->gpsSystem(i)->period(), _config.gpsSystems()->gpsSystem(i)->period());
    QCOMPARE(decoded.contacts()->indexOf(decoded.gpsSystems()->gpsSystem(i)->contact()),
             _config.contacts()->indexOf(_config.gpsSystems()->gpsSystem(i)->contact()));
    QCOMPARE(decoded.channelList()->indexOf(decoded.gpsSystems()->gpsSystem(i)->revertChannel()),
             _config.channelList()->indexOf(_config.gpsSystems()->gpsSystem(i)->revertChannel()));
  }
}

QTEST_GUILESS_MAIN(UV390Test)
