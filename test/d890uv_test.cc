#include "d890uv_test.hh"
#include "config.hh"
#include "d890uv_codeplug.hh"
#include "errorstack.hh"
#include <QTest>


// Codeplug read from an AT-D890UV (FW V100) holding the factory configuration.
static const char *factoryCodeplug = ":/data/d890uv_factory.dfu";


/* Prepares the codeplug like AnytoneRadio::upload() does: Starting from a cleared codeplug, every
 * allocated element (bitmaps and preserved regions) is "read" from the given radio image. */
static void
readFromRadio(D890UVCodeplug &codeplug, D890UVCodeplug &radio) {
  codeplug.clear();
  for (int i=0; i<codeplug.image(0).numElements(); i++) {
    uint32_t addr = codeplug.image(0).element(i).address(),
        size = codeplug.image(0).element(i).data().size();
    if (radio.isAllocated(addr, 0) && radio.isAllocated(addr+size-1, 0))
      memcpy(codeplug.data(addr), radio.data(addr), size);
  }
}


D890UVTest::D890UVTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}


void
D890UVTest::testDecodeFactory() {
  ErrorStack err;
  D890UVCodeplug codeplug;
  if (! codeplug.read(factoryCodeplug, err))
    QFAIL(err.format().toStdString().c_str());

  Config config;
  if (! codeplug.decode(&config, err))
    QFAIL(err.format().toStdString().c_str());

  QCOMPARE(config.radioIDs()->count(), 1);
  QCOMPARE(config.radioIDs()->get(0)->name(), QString("My Radio"));
  QCOMPARE(config.radioIDs()->get(0)->as<DMRRadioID>()->number(), 12345678U);

  QCOMPARE(config.contacts()->count(), 1);
  DMRContact *contact = config.contacts()->contact(0)->as<DMRContact>();
  QVERIFY(contact);
  QCOMPARE(contact->name(), QString("Contact1"));
  QCOMPARE(contact->number(), 12345678U);
  QCOMPARE(contact->type(), DMRContact::GroupCall);

  QCOMPARE(config.rxGroupLists()->count(), 1);
  // The stray 0x00 at 0x22 of the factory group list is not a member.
  QCOMPARE(config.rxGroupLists()->list(0)->count(), 1);

  QCOMPARE(config.channelList()->count(), 8);
  Channel *ch1 = config.channelList()->channel(0);
  QVERIFY(ch1->is<FMChannel>());
  QCOMPARE(ch1->name(), QString("Channel 1"));
  QCOMPARE(ch1->rxFrequency(), Frequency::fromMHz(446.325));
  QCOMPARE(ch1->txFrequency(), Frequency::fromMHz(446.325));
  QCOMPARE(ch1->power(), Channel::Power::Max);
  QCOMPARE(ch1->as<FMChannel>()->bandwidth(), FMChannel::Bandwidth::Wide);
  Channel *ch2 = config.channelList()->channel(1);
  QVERIFY(ch2->is<DMRChannel>());
  QCOMPARE(ch2->as<DMRChannel>()->colorCode(), 1U);
  QCOMPARE(ch2->as<DMRChannel>()->contact(), contact);

  QCOMPARE(config.zones()->count(), 1);
  QCOMPARE(config.zones()->zone(0)->name(), QString("Zone 1"));
  QCOMPARE(config.zones()->zone(0)->A()->count(), 8);

  // The single 0x00 at 0x94 of the factory scan list is not a member.
  QCOMPARE(config.scanlists()->count(), 1);
  QCOMPARE(config.scanlists()->scanlist(0)->count(), 8);
}


void
D890UVTest::testBasicConfigRoundTrip() {
  ErrorStack err;
  D890UVCodeplug codeplug;
  codeplug.clear();
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  if (! codeplug.encode(&_basicConfig, flags, err))
    QFAIL(err.format().toStdString().c_str());

  Config config;
  if (! codeplug.decode(&config, err))
    QFAIL(err.format().toStdString().c_str());

  QCOMPARE(config.channelList()->count(), 4);
  DMRChannel *ch1 = config.channelList()->channel(0)->as<DMRChannel>();
  QVERIFY(ch1);
  QCOMPARE(ch1->name(), QString("L9 DB0LDS"));
  QCOMPARE(ch1->rxFrequency(), Frequency::fromMHz(439.5625));
  QCOMPARE(ch1->txFrequency(), Frequency::fromMHz(431.9625));
  QCOMPARE(ch1->timeSlot(), DMRChannel::TimeSlot::TS2);
  QCOMPARE(ch1->contact()->name(), QString("Local"));
  QCOMPARE(ch1->groupList()->name(), QString("Local"));
  QVERIFY(nullptr == ch1->scanList());

  QCOMPARE(config.contacts()->count(), 6);
  QCOMPARE(config.contacts()->contact(4)->as<DMRContact>()->type(), DMRContact::PrivateCall);
  QCOMPARE(config.rxGroupLists()->count(), 3);
  QCOMPARE(config.rxGroupLists()->list(0)->count(), 3);
  QCOMPARE(config.rxGroupLists()->list(0)->contact(2)->name(), QString("BB"));

  QCOMPARE(config.zones()->count(), 2);
  QCOMPARE(config.zones()->zone(1)->name(), QString("Zu Hause2"));
  QCOMPARE(config.zones()->zone(1)->A()->count(), 3);
  QCOMPARE(config.zones()->zone(1)->A()->get(0)->name(), QString("DL DB0LDS"));

  QCOMPARE(config.scanlists()->count(), 1);
  QCOMPARE(config.scanlists()->scanlist(0)->count(), 3);
  QCOMPARE(config.scanlists()->scanlist(0)->channel(2)->name(), QString("DL DB0LDS"));
}


void
D890UVTest::testDualZoneRoundTrip() {
  ErrorStack err;
  QVERIFY(_basicConfig.zones()->zone(0)->B()->count());

  // A zone with channels for VFO A and B is split into two zones.
  D890UVCodeplug codeplug;
  codeplug.clear();
  Config *intermediate = codeplug.preprocess(&_basicConfig, err);
  if (nullptr == intermediate)
    QFAIL(err.format().toStdString().c_str());
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  bool encoded = codeplug.encode(intermediate, flags, err);
  delete intermediate;
  if (! encoded)
    QFAIL(err.format().toStdString().c_str());

  Config config;
  if (! codeplug.decode(&config, err))
    QFAIL(err.format().toStdString().c_str());
  QCOMPARE(config.zones()->count(), 2*_basicConfig.zones()->count());
  QVERIFY(config.zones()->zone(0)->name().endsWith(" A"));
  QVERIFY(config.zones()->zone(1)->name().endsWith(" B"));

  // ... and merged again.
  if (! codeplug.postprocess(&config, err))
    QFAIL(err.format().toStdString().c_str());
  QCOMPARE(config.zones()->count(), _basicConfig.zones()->count());
  for (int i=0; i<config.zones()->count(); i++) {
    Zone *orig = _basicConfig.zones()->zone(i), *zone = config.zones()->zone(i);
    QCOMPARE(zone->name(), orig->name());
    QCOMPARE(zone->A()->count(), orig->A()->count());
    QCOMPARE(zone->B()->count(), orig->B()->count());
    QCOMPARE(zone->B()->get(0)->name(), orig->B()->get(0)->name());
  }
}


void
D890UVTest::testEncodeKeepsUnmodelledRegions() {
  ErrorStack err;
  D890UVCodeplug radio, codeplug;
  if (! radio.read(factoryCodeplug, err))
    QFAIL(err.format().toStdString().c_str());

  // Update the codeplug read from the radio.
  readFromRadio(codeplug, radio);
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err))
    QFAIL(err.format().toStdString().c_str());

  // Writing erases the complete erase unit. Hence every region of the radio, that shares an erase
  // unit with a written element but is not modelled, must be written back unchanged.
  auto modelled = [](uint32_t addr) {
    return ((addr >= 0x01000000) && (addr < 0x01f81000)) ||   // channel banks
        ((addr >= 0x02000000) && (addr < 0x02020000)) ||       // zone channels
        ((addr >= 0x02100000) && (addr < 0x0210c800)) ||       // scan lists
        ((addr >= 0x03600000) && (addr < 0x03604000)) ||       // zone names
        ((addr >= 0x03680000) && (addr < 0x03684000)) ||       // radio IDs
        ((addr >= 0x03780000) && (addr < 0x0379f400)) ||       // group lists
        ((addr >= 0x03a00000) && (addr < 0x03f00000)) ||       // contacts
        (0x03f00000 == addr) ||                                // contact ID map
        (0x03482a00 == addr) || (0x03482c00 == addr) || (0x03482c40 == addr) ||
        (0x03482c60 == addr) || (0x03701510 == addr) || (0x03980000 == addr);
  };
  QSet<uint32_t> writtenUnits;
  for (int i=0; i<codeplug.image(0).numElements(); i++)
    writtenUnits.insert(codeplug.image(0).element(i).address() & ~0x3ffffu);
  for (int i=0; i<radio.image(0).numElements(); i++) {
    const DFUFile::Element &el = radio.image(0).element(i);
    if (modelled(el.address()) || (! writtenUnits.contains(el.address() & ~0x3ffffu)))
      continue;
    QVERIFY2(codeplug.isAllocated(el.address(), 0),
             QString("Region at 0x%1 is not written back.").arg(el.address(), 0, 16).toStdString().c_str());
    QVERIFY2(0 == memcmp(el.data().constData(), codeplug.data(el.address()), el.data().size()),
             QString("Region at 0x%1 changed.").arg(el.address(), 0, 16).toStdString().c_str());
  }

  // Erase units holding no modelled region are not written at all.
  QVERIFY(! writtenUnits.contains(0x03500000)); // general settings
  QVERIFY(! writtenUnits.contains(0x03580000)); // encryption keys
  QVERIFY(! writtenUnits.contains(0x02980000)); // messages

  // Bits of the channel bitmap beyond channel 4000 (VFO A and B) are kept.
  QCOMPARE(QByteArray((const char *)codeplug.data(0x03482a00+500), 12),
           QByteArray((const char *)radio.data(0x03482a00+500), 12));

  // The result decodes like the config.
  Config config;
  if (! codeplug.decode(&config, err))
    QFAIL(err.format().toStdString().c_str());
  QCOMPARE(config.channelList()->count(), 4);
  QCOMPARE(config.contacts()->count(), 6);
  QCOMPARE(config.zones()->count(), 2);
}


void
D890UVTest::testScanListTemplate() {
  QByteArray buffer(D890UVCodeplug::ScanListElement::size(), 0x00);
  D890UVCodeplug::ScanListElement scan((uint8_t *)buffer.data());
  scan.clear();
  scan.setName("Scan List 1");
  for (unsigned int i=0; i<8; i++)
    scan.setMember(i, i);

  // Must match the factory record, except for the bytes behind the name's terminator.
  D890UVCodeplug factory;
  ErrorStack err;
  if (! factory.read(factoryCodeplug, err))
    QFAIL(err.format().toStdString().c_str());
  QByteArray expected((const char *)factory.data(0x02100000), D890UVCodeplug::ScanListElement::size());
  memset(expected.data()+0x26, 0x00, 0x08);
  QCOMPARE(buffer, expected);
}


void
D890UVTest::testContactIDMap() {
  ErrorStack err;
  D890UVCodeplug factory, codeplug;
  if (! factory.read(factoryCodeplug, err))
    QFAIL(err.format().toStdString().c_str());

  // Decoding and encoding the factory config reproduces the factory ID map.
  Config config;
  if (! factory.decode(&config, err))
    QFAIL(err.format().toStdString().c_str());
  readFromRadio(codeplug, factory);
  if (! codeplug.encode(&config, Codeplug::Flags(), err))
    QFAIL(err.format().toStdString().c_str());

  QCOMPARE(QByteArray((const char *)codeplug.data(0x03f00000), 16),
           QByteArray((const char *)factory.data(0x03f00000), 16));
}


void
D890UVTest::testRefuseNoZones() {
  ErrorStack err;
  Config config;
  config.copy(_channelFrequencyConfig);
  QCOMPARE(config.zones()->count(), 0);

  D890UVCodeplug codeplug;
  codeplug.clear();
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  QVERIFY(! codeplug.encode(&config, flags, err));
}


QTEST_GUILESS_MAIN(D890UVTest)
