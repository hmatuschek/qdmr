#include "hd1_test.hh"
#include "config.hh"
#include "hd1_codeplug.hh"
#include "errorstack.hh"
#include "rxgrouplist.hh"
#include "radioid.hh"
#include "zone.hh"
#include "configcopyvisitor.hh"
#include <QtEndian>
#include <cstring>
#include <QTest>


HD1Test::HD1Test(QObject *parent)
  : UnitTestBase(parent)
{
}

void
HD1Test::testBasicConfigEncoding() {
  ErrorStack err;
  HD1Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err))
    QFAIL(QString("Cannot encode codeplug for HD1: %1").arg(err.format()).toStdString().c_str());
}

void
HD1Test::testBasicConfigDecoding() {
  ErrorStack err;
  HD1Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err))
    QFAIL(QString("Cannot encode codeplug for HD1: %1").arg(err.format()).toStdString().c_str());
  Config config;
  if (! codeplug.decode(&config, err))
    QFAIL(QString("Cannot decode codeplug for HD1: %1").arg(err.format()).toStdString().c_str());
  QCOMPARE(config.channelList()->count(), _basicConfig.channelList()->count());
  QCOMPARE(config.zones()->count(), _basicConfig.zones()->count());
}

void
HD1Test::testBasicConfigReencoding() {
  ErrorStack err;
  HD1Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err))
    QFAIL(QString("Cannot encode codeplug for HD1: %1").arg(err.format()).toStdString().c_str());
  Config originalConfig;
  if (! codeplug.decode(&originalConfig, err))
    QFAIL(QString("Cannot decode codeplug for HD1: %1").arg(err.format()).toStdString().c_str());
  if (! codeplug.encode(&originalConfig, Codeplug::Flags(), err))
    QFAIL(QString("Cannot re-encode codeplug for HD1: %1").arg(err.format()).toStdString().c_str());
  Config testConfig;
  if (! codeplug.decode(&testConfig, err))
    QFAIL(QString("Cannot decode codeplug for HD1: %1").arg(err.format()).toStdString().c_str());
  if (0 != originalConfig.compare(testConfig)) {
    QString orig, test;
    QTextStream origStream(&orig), testStream(&test);
    originalConfig.toYAML(origStream);
    testConfig.toYAML(testStream);
    QFAIL(QString("Decoded config of HD1 does not match source:\n%1\n%2").arg(orig).arg(test)
          .toStdString().c_str());
  }
}

void
HD1Test::testPostProcessingOfEmptyCodeplug() {
  Config config;
  HD1Codeplug codeplug;
  ErrorStack err;
  config.clear();
  QVERIFY(codeplug.postprocess(&config, err));
}

void
HD1Test::testGroupListMembers() {
  ErrorStack err;
  HD1Codeplug codeplug;
  QVERIFY(codeplug.encode(&_basicConfig, Codeplug::Flags(), err));
  Config config;
  QVERIFY(codeplug.decode(&config, err));
  // Every digital channel with a group list in the source keeps a list with the same members.
  for (int i=0; i<_basicConfig.channelList()->count(); i++) {
    DMRChannel *src = _basicConfig.channelList()->channel(i)->as<DMRChannel>();
    if ((nullptr == src) || (nullptr == src->groupList()))
      continue;
    DMRChannel *dst = config.channelList()->channel(i)->as<DMRChannel>();
    QVERIFY(dst && dst->groupList());
    QCOMPARE(dst->groupList()->count(), src->groupList()->count());
    QCOMPARE(dst->groupList()->name(), src->groupList()->name().left(10));  // 10 characters on the HD1
  }
}

void
HD1Test::testCustomMemberSet() {
  ErrorStack err;
  HD1Codeplug codeplug;
  QVERIFY(codeplug.encode(&_basicConfig, Codeplug::Flags(), err));
  // Give channel 1 a member set no list has: drop all group lists from the table.
  memset(codeplug.bankData(HD1Interface::Bulk, 0x21d8*0x400), 0xff,
         HD1Codeplug::GroupListBankElement::size());
  Config config;
  QVERIFY(codeplug.decode(&config, err));
  // Each set gets one list, named after the first channel that uses it.
  QHash<RXGroupList *, QString> firstUser;
  for (int i=0; i<config.channelList()->count(); i++) {
    DMRChannel *dmr = config.channelList()->channel(i)->as<DMRChannel>();
    if (dmr && dmr->groupList() && (! firstUser.contains(dmr->groupList())))
      firstUser.insert(dmr->groupList(), dmr->name());
  }
  QVERIFY(! firstUser.isEmpty());
  for (RXGroupList *list: firstUser.keys())
    QCOMPARE(list->name(), firstUser.value(list));
}

void
HD1Test::testManyGroupLists() {
  ErrorStack err;
  QScopedPointer<Config> owned(ConfigCopy::copy(&_basicConfig, err)->as<Config>());
  QVERIFY(owned);
  Config &config = *owned;
  DMRContact *contact = nullptr;
  for (int i=0; i<config.contacts()->count() && (nullptr == contact); i++)
    contact = config.contacts()->contact(i)->as<DMRContact>();
  QVERIFY(contact);
  while (config.rxGroupLists()->count() < 260) {
    RXGroupList *list = new RXGroupList(QString("L%1").arg(config.rxGroupLists()->count()));
    list->addContact(contact);
    config.rxGroupLists()->add(list);
  }
  HD1Codeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err))
    QFAIL(QString("Cannot encode 260 group lists: %1").arg(err.format()).toStdString().c_str());
}

void
HD1Test::testCTCSS() {
  uint8_t data[HD1Codeplug::ChannelElement::size()];
  HD1Codeplug::ChannelElement ch(data);
  ch.clear();
  ch.setRXTone(SelectiveCall(254.1));
  ch.setTXTone(SelectiveCall(67.0));
  QCOMPARE(data[0xa4], uint8_t(0x41));
  QCOMPARE(data[0xa5], uint8_t(0x25));
  QCOMPARE(data[0xa6], uint8_t(0x70));
  QCOMPARE(data[0xa7], uint8_t(0x06));
  QCOMPARE(ch.rxTone().mHz(), 254100U);
  QCOMPARE(ch.txTone().mHz(), 67000U);
  ch.setRXTone(SelectiveCall());
  QCOMPARE(data[0xa4], uint8_t(0xff));
  QVERIFY(! ch.rxTone().isValid());
}

void
HD1Test::testDCS() {
  uint8_t data[HD1Codeplug::ChannelElement::size()];
  HD1Codeplug::ChannelElement ch(data);
  ch.clear();
  ch.setRXTone(SelectiveCall(754, false));
  ch.setTXTone(SelectiveCall(743, true));
  QCOMPARE(data[0xa4], uint8_t(0x54));
  QCOMPARE(data[0xa5], uint8_t(0x87));
  QCOMPARE(data[0xa6], uint8_t(0x43));
  QCOMPARE(data[0xa7], uint8_t(0xc7));
  QVERIFY(ch.rxTone().isDCS() && (! ch.rxTone().isInverted()) && (754 == ch.rxTone().octalCode()));
  QVERIFY(ch.txTone().isDCS() && ch.txTone().isInverted() && (743 == ch.txTone().octalCode()));
}

void
HD1Test::testOffsetDirection() {
  uint8_t data[HD1Codeplug::ChannelElement::size()];
  HD1Codeplug::ChannelElement ch(data);
  ch.clear();
  ch.setRXFrequency(Frequency::fromMHz(145.5));
  ch.setTXFrequency(Frequency::fromMHz(146.1));
  QCOMPARE(data[0xa1] & 0x30, 0x10);
  ch.setTXFrequency(Frequency::fromMHz(144.9));
  QCOMPARE(data[0xa1] & 0x30, 0x20);
  ch.setTXFrequency(Frequency::fromMHz(145.5));
  QCOMPARE(data[0xa1] & 0x30, 0x00);
}

void
HD1Test::testDefaultIdNotFirst() {
  ErrorStack err;
  QScopedPointer<Config> owned(ConfigCopy::copy(&_basicConfig, err)->as<Config>());
  QVERIFY(owned);
  Config &config = *owned;
  DMRRadioID *second = new DMRRadioID("Second", 2620099);
  config.radioIDs()->add(second);
  config.settings()->setDefaultId(second);
  HD1Codeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err))
    QFAIL(err.format().toStdString().c_str());
  // Channels using the default ID point to the second record.
  for (int i=0; i<config.channelList()->count(); i++) {
    DMRChannel *dmr = config.channelList()->channel(i)->as<DMRChannel>();
    if (dmr && dmr->radioIdRef()->is<DefaultRadioID>()) {
      HD1Codeplug::ChannelElement el(codeplug.bankData(HD1Interface::Bulk, 0x1bcc*0x400
                                                       + i*HD1Codeplug::ChannelElement::size()));
      QCOMPARE(el.radioIdIndex(), unsigned(config.radioIDs()->indexOf(second)));
    }
  }
}

void
HD1Test::testDualZone() {
  ErrorStack err;
  QScopedPointer<Config> owned(ConfigCopy::copy(&_basicConfig, err)->as<Config>());
  QVERIFY(owned);
  Config &config = *owned;
  QVERIFY(config.zones()->count() > 0);
  Zone *zone = config.zones()->zone(0);
  zone->B()->add(config.channelList()->channel(0));
  int countB = zone->B()->count();

  HD1Codeplug codeplug;
  Config *copy = codeplug.preprocess(&config, err);
  QVERIFY2(copy, err.format().toStdString().c_str());
  QVERIFY2(codeplug.encode(copy, Codeplug::Flags(), err), err.format().toStdString().c_str());
  delete copy;
  Config decoded;
  QVERIFY(codeplug.decode(&decoded, err));
  QVERIFY(codeplug.postprocess(&decoded, err));
  QCOMPARE(decoded.zones()->count(), config.zones()->count());
  QCOMPARE(decoded.zones()->zone(0)->B()->count(), countB);
}

void
HD1Test::testAMChannelFiltered() {
  ErrorStack err;
  QScopedPointer<Config> owned(ConfigCopy::copy(&_basicConfig, err)->as<Config>());
  QVERIFY(owned);
  Config &config = *owned;
  int count = config.channelList()->count();
  AMChannel *am = new AMChannel();
  am->setName("AM");
  am->setRXFrequency(Frequency::fromMHz(144.0));
  am->setTXFrequency(Frequency::fromMHz(144.0));
  config.channelList()->add(am);

  HD1Codeplug codeplug;
  Config *copy = codeplug.preprocess(&config, err);
  QVERIFY2(copy, err.format().toStdString().c_str());
  QVERIFY2(codeplug.encode(copy, Codeplug::Flags(), err), err.format().toStdString().c_str());
  delete copy;
  Config decoded;
  QVERIFY(codeplug.decode(&decoded, err));
  QCOMPARE(decoded.channelList()->count(), count);
}

void
HD1Test::testUnmodelledBytesMove() {
  ErrorStack err;
  HD1Codeplug codeplug;
  QVERIFY2(codeplug.encode(&_basicConfig, Codeplug::Flags(), err), err.format().toStdString().c_str());
  // Give channel 2 a kill code (0xac, not modelled by qdmr).
  uint8_t *ch2 = codeplug.bankData(HD1Interface::Bulk, 0x1bcc*0x400 + HD1Codeplug::ChannelElement::size());
  ch2[0xac] = 7;

  // Decode, delete channel 1, encode: channel 2 moves to slot 1 and keeps its kill code.
  Config config;
  QVERIFY(codeplug.decode(&config, err));
  QVERIFY(config.channelList()->count() > 1);
  QString name = config.channelList()->channel(1)->name();
  config.channelList()->del(config.channelList()->channel(0));
  QVERIFY2(codeplug.encode(&config, Codeplug::Flags(), err), err.format().toStdString().c_str());
  uint8_t *slot1 = codeplug.bankData(HD1Interface::Bulk, 0x1bcc*0x400);
  QCOMPARE(HD1Codeplug::ChannelElement(slot1).name(), name.left(14));
  QCOMPARE(slot1[0xac], uint8_t(7));
}

void
HD1Test::testRenamedChannelKeepsBytes() {
  ErrorStack err;
  HD1Codeplug codeplug;
  QVERIFY2(codeplug.encode(&_basicConfig, Codeplug::Flags(), err), err.format().toStdString().c_str());
  uint8_t *ch1 = codeplug.bankData(HD1Interface::Bulk, 0x1bcc*0x400);
  ch1[0xa8] = 0x4f;  // enhanced encryption, key 16 (not modelled)
  Config config;
  QVERIFY(codeplug.decode(&config, err));
  config.channelList()->channel(0)->setName("Renamed");
  QVERIFY2(codeplug.encode(&config, Codeplug::Flags(), err), err.format().toStdString().c_str());
  QCOMPARE(HD1Codeplug::ChannelElement(ch1).name(), QString("Renamed"));
  QCOMPARE(ch1[0xa8], uint8_t(0x4f));
}

void
HD1Test::testDanglingContactKept() {
  ErrorStack err;
  HD1Codeplug codeplug;
  QVERIFY2(codeplug.encode(&_basicConfig, Codeplug::Flags(), err), err.format().toStdString().c_str());
  int dmrIdx = -1;
  for (int i=0; (i<_basicConfig.channelList()->count()) && (dmrIdx < 0); i++)
    if (_basicConfig.channelList()->channel(i)->is<DMRChannel>())
      dmrIdx = i;
  QVERIFY(dmrIdx >= 0);
  HD1Codeplug::ChannelElement el(codeplug.bankData(HD1Interface::Bulk, 0x1bcc*0x400
                                                   + dmrIdx*HD1Codeplug::ChannelElement::size()));
  el.setTXContact(HD1Codeplug::ContactRef::base() + 999);  // empty contact slot
  Config config;
  QVERIFY(codeplug.decode(&config, err));
  QVERIFY(nullptr == config.channelList()->channel(dmrIdx)->as<DMRChannel>()->contact());
  QVERIFY2(codeplug.encode(&config, Codeplug::Flags(), err), err.format().toStdString().c_str());
  QCOMPARE(el.txContact(), HD1Codeplug::ContactRef::base() + 999);
}


void
HD1Test::testChannelBitmap() {
  ErrorStack err;
  HD1Codeplug codeplug;
  // A slot left over from an earlier codeplug: its bit must be set again.
  uint8_t *bitmap = codeplug.bankData(HD1Interface::Small, HD1Codeplug::Offset::channelBitmap());
  memset(bitmap, 0x00, HD1Codeplug::ChannelBitmapElement::size());
  QVERIFY2(codeplug.encode(&_basicConfig, Codeplug::Flags(), err), err.format().toStdString().c_str());
  unsigned int n = _basicConfig.channelList()->count();
  for (unsigned int i=0; i<3000; i++)
    QCOMPARE(bool(bitmap[i/8] & (1 << (i%8))), i >= n);
}

void
HD1Test::testNewChannelTXContact() {
  // The manufacturer's software writes 0 for "no TX contact", on analog and digital channels.
  HD1Codeplug codeplug;
  HD1Codeplug::ChannelElement el(codeplug.bankData(HD1Interface::Bulk, HD1Codeplug::Offset::channels()));
  el.clear();
  QCOMPARE(el.txContact(), HD1Codeplug::ContactRef::noTXContact());
}

QTEST_GUILESS_MAIN(HD1Test)
