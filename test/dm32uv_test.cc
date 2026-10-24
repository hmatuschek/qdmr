#include "dm32uv_test.hh"
#include "config.hh"
#include "dm32uv_codeplug.hh"
#include "errorstack.hh"
#include "logger.hh"
#include "dm32uv_limits.hh"
#include "dm32uv.hh"
#include <QTest>


DM32UVTest::DM32UVTest(QObject *parent)
  : UnitTestBase(parent)
{
}

void
DM32UVTest::testBasicConfigEncoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
DM32UVTest::testBasicConfigDecoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
}


void
DM32UVTest::testBasicConfigReencoding() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config originalConfig;
  if (! codeplug.decode(&originalConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }
  // original Config now contains all default values and extensions

  // re-encode it
  if (! codeplug.encode(&originalConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // Decode complete config
  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  // compare
  if (0 != originalConfig.compare(testConfig)) {
    QString orig, test;
    QTextStream origStream(&orig), testStream(&test);
    originalConfig.toYAML(origStream);
    testConfig.toYAML(testStream);
    QFAIL(QString("Decoded config of BTECH DM32UV does not match source:\n%1\n%2").arg(orig).arg(test).toStdString().c_str());
  }
}

void
DM32UVTest::testProstProcessingOfEmptyCodeplug() {
  Config config;
  DM32UVCodeplug codeplug;
  ErrorStack err;

  config.clear();
  QVERIFY(codeplug.postprocess(&config, err));
}


void
DM32UVTest::testAMChannelReencoding() {
  ErrorStack err;

  // Load config from file
  Config config;
  if (! config.readYAML(":/data/am_channel_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }

  DM32UVCodeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  Config testConfig;
  if (! codeplug.decode(&testConfig, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(testConfig.channelList()->count(), config.channelList()->count());
  QVERIFY(testConfig.channelList()->channel(0)->is<AMChannel>());
  QCOMPARE(testConfig.channelList()->channel(0)->rxFrequency(), Frequency::fromMHz(121.13));
  QVERIFY(testConfig.channelList()->channel(0)->txFrequency().isZero());
}


void
DM32UVTest::testChannelBankEncoding() {
  static const uint32_t bank0=0x12000, bank1 = 0x13000, bank2 = 0x14000;
  Config config;

  config.radioIDs()->add(new DMRRadioID("test", 123456));
  // Add 84 channels to fill bank 0
  for (int i=0; i<84; i++) {
    auto ch = new FMChannel();
    ch->setName(QString("Channel %1").arg(i));
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.0));
    config.channelList()->add(ch);
  }

  {
    DM32UVCodeplug codeplug; ErrorStack err;
    if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
      QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
              .arg(err.format()).toStdString().c_str());
    }
    QVERIFY(codeplug.isAllocated(bank0));
    QVERIFY(!codeplug.isAllocated(bank1));
    QVERIFY(!codeplug.isAllocated(bank2));
  }

  // Add another 85 channel to fill bank 1
  for (int i=0; i<85; i++) {
    auto ch = new FMChannel();
    ch->setName(QString("Channel %1").arg(i));
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.0));
    config.channelList()->add(ch);
  }

  {
    DM32UVCodeplug codeplug; ErrorStack err;
    if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
      QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
              .arg(err.format()).toStdString().c_str());
    }
    QVERIFY(codeplug.isAllocated(bank0));
    QVERIFY(codeplug.isAllocated(bank1));
    QVERIFY(!codeplug.isAllocated(bank2));
  }

  // Add yet another channel to allocate bank 2
  auto ch = new FMChannel();
  ch->setName(QString("Channel %1").arg(config.channelList()->count()));
  ch->setRXFrequency(Frequency::fromMHz(144.0));
  ch->setTXFrequency(Frequency::fromMHz(144.0));
  config.channelList()->add(ch);

  {
    DM32UVCodeplug codeplug; ErrorStack err;
    if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
      QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
              .arg(err.format()).toStdString().c_str());
    }
    QVERIFY(codeplug.isAllocated(bank0));
    QVERIFY(codeplug.isAllocated(bank1));
    QVERIFY(codeplug.isAllocated(bank2));
  }
}


void
DM32UVTest::testCTCSSHigherFrequencies() {
  ErrorStack err;
  DM32UVCodeplug codeplug;
  Config config;
  if (! config.readYAML(":/data/ctcss_copy_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
            .arg(err.format()).toStdString().c_str());
  }
  config.channelList()->channel(0)->as<FMChannel>()->setRXTone(SelectiveCall(100.0));
  config.channelList()->channel(0)->as<FMChannel>()->setTXTone(SelectiveCall(107.2));

  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QVERIFY(decoded.channelList()->channel(0)->is<FMChannel>());
  QCOMPARE(decoded.channelList()->channel(0)->as<FMChannel>()->rxTone().Hz(), 100.0);
  QCOMPARE(decoded.channelList()->channel(0)->as<FMChannel>()->txTone().Hz(), 107.2);
}


void
DM32UVTest::testZoneBankEncoding() {
  static const uint32_t lastBank = 0x64000;
  Config config;

  config.radioIDs()->add(new DMRRadioID("test", 123456));
  auto ch = new FMChannel();
  ch->setName("Channel");
  ch->setRXFrequency(Frequency::fromMHz(144.0));
  ch->setTXFrequency(Frequency::fromMHz(144.0));
  config.channelList()->add(ch);
  for (int i=0; i<250; i++) {
    auto zone = new Zone();
    zone->setName(QString("Zone %1").arg(i));
    zone->A()->add(ch);
    config.zones()->add(zone);
  }

  DM32UVCodeplug codeplug; ErrorStack err;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
  QVERIFY(codeplug.isAllocated(lastBank));

  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
  QCOMPARE(decoded.zones()->count(), 250);
  QCOMPARE(decoded.zones()->zone(249)->name(), QString("Zone 249"));
}


void
DM32UVTest::testContactIndexEncoding() {
  Config config;
  config.radioIDs()->add(new DMRRadioID("test", 123456));
  config.contacts()->add(new DMRContact(DMRContact::GroupCall, "b", 3));
  config.contacts()->add(new DMRContact(DMRContact::PrivateCall, "a", 1));
  config.contacts()->add(new DMRContact(DMRContact::GroupCall, "B", 2));

  DM32UVCodeplug codeplug; ErrorStack err;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }

  auto entry = [&codeplug](uint32_t addr) -> unsigned int {
    const uint8_t *ptr = codeplug.data(addr);
    return ((unsigned int)ptr[1] << 8) | ptr[0];
  };
  // Header: 3 contacts, 2 group calls, then 0 (the CPS writes 0, even with private calls)
  QCOMPARE(entry(0xb000), 3U);
  QCOMPARE(entry(0xb002), 2U);
  QCOMPARE(entry(0xb004), 0U);
  // Sorted by name, case sensitive: "B", "a", "b"
  QCOMPARE(entry(0xb100), 0x4003U);
  QCOMPARE(entry(0xb102), 0x3002U);
  QCOMPARE(entry(0xb104), 0x4001U);
  QCOMPARE(entry(0xb106), 0xffffU);
  // Sorted by number: 1, 2, 3
  QCOMPARE(entry(0xb740), 0x3002U);
  QCOMPARE(entry(0xb742), 0x4003U);
  QCOMPARE(entry(0xb744), 0x4001U);
  QCOMPARE(entry(0xb746), 0xffffU);
  QCOMPARE(entry(0xbffc), 0xffffU);

  // The padding must keep the block tag (last byte), as read from the radio
  *codeplug.data(0xbfff) = 0x0b;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot re-encode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
  QCOMPARE((unsigned int)*codeplug.data(0xbfff), 0x0bU);
  QCOMPARE((unsigned int)*codeplug.data(0xbffe), 0xffU);
}


void
DM32UVTest::testRadioIdLimits() {
  auto issueCount = [](const Config &config) -> int {
    RadioLimitContext issues;
    DM32UVLimits().verifyConfig(&config, issues);
    return issues.count();
  };

  Config config;
  config.radioIDs()->add(new DMRRadioID("First ID", 123456));
  auto cnt = new DMRContact(DMRContact::GroupCall, "Group", 91);
  config.contacts()->add(cnt);
  auto ch = new DMRChannel();
  ch->setName("Channel");
  ch->setRXFrequency(Frequency::fromMHz(439.0));
  ch->setTXFrequency(Frequency::fromMHz(439.0));
  ch->setContact(cnt);
  config.channelList()->add(ch);
  auto zone = new Zone(); zone->setName("Zone");
  zone->A()->add(ch);
  config.zones()->add(zone);

  int baseIssues = issueCount(config);
  auto second = new DMRRadioID("Second ID", 1234567);
  config.radioIDs()->add(second);
  ch->setRadioId(second);
  QCOMPARE(issueCount(config), baseIssues);

  DM32UVCodeplug codeplug; ErrorStack err;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
  Config decoded;
  if (! codeplug.decode(&decoded, err)) {
    QFAIL(QString("Cannot decode codeplug for BTECH DM32UV: %1")
            .arg(err.format()).toStdString().c_str());
  }
  QCOMPARE(decoded.radioIDs()->count(), 2);
  auto dch = decoded.channelList()->channel(0)->as<DMRChannel>();
  QVERIFY(nullptr != dch);
  QVERIFY(nullptr != dch->radioId());
  QCOMPARE(dch->radioId()->number(), 1234567U);
}


void
DM32UVTest::testAddressMapAllocation() {
  DM32UV::AddressMap map;
  map.map(0x02000, 0x03000);
  map.addFree(0x23000);
  map.addFree(0x05000);

  // Unmapped blocks get the lowest free page, each its own
  QCOMPARE(map.allocate(0x0b000), 0x05000U);
  QCOMPARE(map.toPhysical(0x0b000), 0x05000U);
  QCOMPARE(map.toVirtual(0x05000), 0x0b000U);
  QCOMPARE(map.allocate(0x0c000), 0x23000U);
  // None left
  QCOMPARE(map.allocate(0x0d000), std::numeric_limits<uint32_t>::max());
  QVERIFY(! map.virtualIsMapped(0x0d000));
  QCOMPARE(map.toPhysical(0x03000), 0x02000U);
}


QTEST_GUILESS_MAIN(DM32UVTest)


