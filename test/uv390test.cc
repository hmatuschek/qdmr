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
  /*
   * Test Channel 01
   */
  // Mode digi, BW 12.5kHz, Autoscan off, lone worker off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x00), 0x62);
  // Talkaround off, RX only off, TS 2, CC 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x01), 0x19);
  // Priv #0, priv none, prv. call conf. off, data call conf. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x02), 0x00);
  // rx ref freq low, emrg. ack off, PTT id off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x03), 0xe0);
  // tx ref freq low, VOX off, admit none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x04), 0xe4);
  // in-call always, turn-off freq off.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x05), 0xc0);
  // TX contact 2
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x110800+0x06), 0x02);
  // TOT 45s <-> 0x03
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x08), 0x03);
  // TOT re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x09), 0x00);
  // Emerg. sys none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x0a), 0x00);
  // Scanlist none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x0b), 0x00);
  // RX group list 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x0c), 0x01);
  // GPS system 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x0d), 0x01);
  // DTMF decode none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x0e), 0x00);
  // Squelch none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x0f), 0x00);
  // RX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110800+0x10)), 439.5630);
  // TX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110800+0x14)), 431.9630);
  // RX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110800+0x18)), 0.0);
  // TX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110800+0x1a)), 0.0);
  // RX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x1c), 0x00);
  // TX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x1d), 0x00);
  // Power high
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x1e), 0xff);
  // GPS on (inv), allow interrupt off (inv), DCDM off (inv), Leader/MS off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110800+0x1f), 0xfc);
  // Name
  QCOMPARE(decode_unicode((uint16_t *)_codeplug.data(0x110800+0x20), 16, 0), QString("BB DB0LDS TS2"));

  /*
   * Test Channel 02
   */
  // Mode digi, BW 12.5kHz, Autoscan off, lone worker off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x00), 0x62);
  // Talkaround off, RX only off, TS 2, CC 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x01), 0x19);
  // Priv #0, priv none, prv. call conf. off, data call conf. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x02), 0x00);
  // rx ref freq low, emrg. ack off, PTT id off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x03), 0xe0);
  // tx ref freq low, VOX off, admit none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x04), 0xe4);
  // in-call always, turn-off freq off.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x05), 0xc0);
  // TX contact 2
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x110840+0x06), 0x02);
  // TOT 45s <-> 0x03
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x08), 0x03);
  // TOT re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x09), 0x00);
  // Emerg. sys none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x0a), 0x00);
  // Scanlist none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x0b), 0x00);
  // RX group list 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x0c), 0x01);
  // GPS system 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x0d), 0x00);
  // DTMF decode none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x0e), 0x00);
  // Squelch none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x0f), 0x00);
  // RX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110840+0x10)), 439.0870);
  // TX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110840+0x14)), 431.4870);
  // RX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110840+0x18)), 0.0);
  // TX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110840+0x1a)), 0.0);
  // RX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x1c), 0x00);
  // TX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x1d), 0x00);
  // Power high
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x1e), 0xff);
  // GPS off (inv), allow interrupt off (inv), DCDM off (inv), Leader/MS off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110840+0x1f), 0xff);
  // Name
  QCOMPARE(decode_unicode((uint16_t *)_codeplug.data(0x110840+0x20), 16, 0), QString("BB DM0TT TS2"));

  /*
   * Test Channel 03
   */
  // Mode digi, BW 12.5kHz, Autoscan off, lone worker off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x00), 0x62);
  // Talkaround off, RX only off, TS 1, CC 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x01), 0x15);
  // Priv #0, priv none, prv. call conf. off, data call conf. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x02), 0x00);
  // rx ref freq low, emrg. ack off, PTT id off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x03), 0xe0);
  // tx ref freq low, VOX off, admit none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x04), 0xe4);
  // in-call always, turn-off freq off.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x05), 0xc0);
  // TX contact 2
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x110880+0x06), 0x01);
  // TOT 45s <-> 0x03
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x08), 0x03);
  // TOT re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x09), 0x00);
  // Emerg. sys none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x0a), 0x00);
  // Scanlist none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x0b), 0x00);
  // RX group list 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x0c), 0x01);
  // GPS system 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x0d), 0x00);
  // DTMF decode none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x0e), 0x00);
  // Squelch none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x0f), 0x00);
  // RX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110880+0x10)), 439.5380);
  // TX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110880+0x14)), 431.9380);
  // RX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110880+0x18)), 0.0);
  // TX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110880+0x1a)), 0.0);
  // RX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x1c), 0x00);
  // TX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x1d), 0x00);
  // Power high
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x1e), 0xff);
  // GPS off (inv), allow interrupt off (inv), DCDM off (inv), Leader/MS off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110880+0x1f), 0xff);
  // Name
  QCOMPARE(decode_unicode((uint16_t *)_codeplug.data(0x110880+0x20), 16, 0), QString("TG9 DB0KK TS1"));
}


void
UV390Test::testAnalogChannels() {
  /*
   * Test Channel 04
   */
  // Mode analog, BW 12.5kHz, Autoscan off, lone worker off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x00), 0x61);
  // Talkaround off, RX only off, TS 1, CC 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x01), 0x15);
  // Priv #0, priv none, prv. call conf. off, data call conf. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x02), 0x00);
  // rx ref freq low, emrg. ack off, PTT id off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x03), 0xe0);
  // tx ref freq low, VOX off, admit CH admit tone
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x04), 0xa4);
  // in-call always, turn-off freq off.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x05), 0xc0);
  // TX contact none
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x1108c0+0x06), 0x00);
  // TOT 45s <-> 0x03
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x08), 0x03);
  // TOT re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x09), 0x00);
  // Emerg. sys none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x0a), 0x00);
  // Scanlist none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x0b), 0x00);
  // RX group list 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x0c), 0x00);
  // GPS system none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x0d), 0x00);
  // DTMF decode none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x0e), 0x00);
  // Squelch 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x0f), 0x01);
  // RX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x1108c0+0x10)), 439.5625);
  // TX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x1108c0+0x14)), 431.9625);
  // RX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x1108c0+0x18)), 67.0);
  // TX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x1108c0+0x1a)), 67.0);
  // RX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x1c), 0x00);
  // TX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x1d), 0x00);
  // Power high
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x1e), 0xff);
  // GPS off (inv), allow interrupt off (inv), DCDM off (inv), Leader/MS off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x1108c0+0x1f), 0xff);
  // Name
  QCOMPARE(decode_unicode((uint16_t *)_codeplug.data(0x1108c0+0x20), 16, 0), QString("DB0LDS"));

  /*
   * Test Channel 05
   */
  // Mode analog, BW 12.5kHz, Autoscan off, lone worker off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x00), 0x61);
  // Talkaround off, RX only off, TS 1, CC 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x01), 0x15);
  // Priv #0, priv none, prv. call conf. off, data call conf. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x02), 0x00);
  // rx ref freq low, emrg. ack off, PTT id off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x03), 0xe0);
  // tx ref freq low, VOX off, admit CH free
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x04), 0x64);
  // in-call always, turn-off freq off.
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x05), 0xc0);
  // TX contact none
  QCOMPARE((int)*(uint16_t *)_codeplug.data(0x110900+0x06), 0x00);
  // TOT 45s <-> 0x03
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x08), 0x03);
  // TOT re-key delay 0
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x09), 0x00);
  // Emerg. sys none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x0a), 0x00);
  // Scanlist none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x0b), 0x00);
  // RX group list none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x0c), 0x00);
  // GPS system none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x0d), 0x00);
  // DTMF decode none
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x0e), 0x00);
  // Squelch 1
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x0f), 0x01);
  // RX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110900+0x10)), 145.6000);
  // TX frequency
  QCOMPARE(decode_frequency(*(uint32_t *)_codeplug.data(0x110900+0x14)), 145.0000);
  // RX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110900+0x18)), 0.0);
  // TX CTCSS none
  QCOMPARE(decode_ctcss_tone_table(*(uint16_t *)_codeplug.data(0x110900+0x1a)), 0.0);
  // RX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x1c), 0x00);
  // TX DTMF sig. off
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x1d), 0x00);
  // Power high
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x1e), 0xff);
  // GPS off (inv), allow interrupt off (inv), DCDM off (inv), Leader/MS off (inv)
  QCOMPARE((int)*(uint8_t  *)_codeplug.data(0x110900+0x1f), 0xff);
  // Name
  QCOMPARE(decode_unicode((uint16_t *)_codeplug.data(0x110900+0x20), 16, 0), QString("DB0SP-2"));
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
    // Do not compare GPS system name UV390 cannot save GPS system name.
    QCOMPARE(decoded.gpsSystems()->gpsSystem(i)->name(), QString("GPS System"));
    QCOMPARE(decoded.gpsSystems()->gpsSystem(i)->period(), _config.gpsSystems()->gpsSystem(i)->period());
    QCOMPARE(decoded.contacts()->indexOf(decoded.gpsSystems()->gpsSystem(i)->contact()),
             _config.contacts()->indexOf(_config.gpsSystems()->gpsSystem(i)->contact()));
    QCOMPARE(decoded.channelList()->indexOf(decoded.gpsSystems()->gpsSystem(i)->revertChannel()),
             _config.channelList()->indexOf(_config.gpsSystems()->gpsSystem(i)->revertChannel()));
  }
}

QTEST_GUILESS_MAIN(UV390Test)
