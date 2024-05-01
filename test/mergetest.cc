#include "mergetest.hh"

#include <QTest>
#include "config.hh"
#include "configmergevisitor.hh"


MergeTest::MergeTest(QObject *parent)
  : QObject{parent}
{
  // pass...
}

void
MergeTest::testMergeRadioIds() {
  Config *base = new Config(), *merging = new Config();

  base->radioIDs()->add(new DMRRadioID("ID 0", 1234));
  base->radioIDs()->add(new DMRRadioID("ID 1", 1234));

  merging->radioIDs()->add(new DMRRadioID("ID 1", 2345));
  merging->radioIDs()->add(new DMRRadioID("ID 2", 2345));

  ErrorStack err;
  Config *merged = ConfigMerge::merge(base, merging,
                                      ConfigMergeVisitor::ItemStrategy::Ignore,
                                      ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->radioIDs()->count(), 3);
  QCOMPARE(merged->radioIDs()->getId(0)->name(), "ID 0");
  QCOMPARE(merged->radioIDs()->getId(0)->as<DMRRadioID>()->number(), 1234);
  QCOMPARE(merged->radioIDs()->getId(1)->name(), "ID 1");
  QCOMPARE(merged->radioIDs()->getId(1)->as<DMRRadioID>()->number(), 1234);
  QCOMPARE(merged->radioIDs()->getId(2)->name(), "ID 2");
  QCOMPARE(merged->radioIDs()->getId(2)->as<DMRRadioID>()->number(), 2345);


  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Override,
                              ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->radioIDs()->count(), 3);
  QCOMPARE(merged->radioIDs()->getId(0)->name(), "ID 0");
  QCOMPARE(merged->radioIDs()->getId(0)->as<DMRRadioID>()->number(), 1234);
  QCOMPARE(merged->radioIDs()->getId(1)->name(), "ID 1");
  QCOMPARE(merged->radioIDs()->getId(1)->as<DMRRadioID>()->number(), 2345);
  QCOMPARE(merged->radioIDs()->getId(2)->name(), "ID 2");
  QCOMPARE(merged->radioIDs()->getId(2)->as<DMRRadioID>()->number(), 2345);


  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Duplicate,
                              ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->radioIDs()->count(), 4);
  QCOMPARE(merged->radioIDs()->getId(0)->name(), "ID 0");
  QCOMPARE(merged->radioIDs()->getId(0)->as<DMRRadioID>()->number(), 1234);
  QCOMPARE(merged->radioIDs()->getId(1)->name(), "ID 1");
  QCOMPARE(merged->radioIDs()->getId(1)->as<DMRRadioID>()->number(), 1234);
  QCOMPARE(merged->radioIDs()->getId(2)->name(), "ID 1 (copy)");
  QCOMPARE(merged->radioIDs()->getId(2)->as<DMRRadioID>()->number(), 2345);
  QCOMPARE(merged->radioIDs()->getId(3)->name(), "ID 2");
  QCOMPARE(merged->radioIDs()->getId(3)->as<DMRRadioID>()->number(), 2345);
}


void
MergeTest::testMergeContacts() {
  Config *base = new Config(), *merging = new Config();

  base->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 0", 1234));
  base->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 1", 1234));

  merging->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 1", 2345));
  merging->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 2", 2345));

  ErrorStack err;
  Config *merged = ConfigMerge::merge(base, merging,
                                      ConfigMergeVisitor::ItemStrategy::Ignore,
                                      ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 3);
  QCOMPARE(merged->contacts()->get(0)->as<DMRContact>()->name(), "ID 0");
  QCOMPARE(merged->contacts()->get(0)->as<DMRContact>()->number(), 1234);
  QCOMPARE(merged->contacts()->get(1)->as<DMRContact>()->name(), "ID 1");
  QCOMPARE(merged->contacts()->get(1)->as<DMRContact>()->number(), 1234);
  QCOMPARE(merged->contacts()->get(2)->as<DMRContact>()->name(), "ID 2");
  QCOMPARE(merged->contacts()->get(2)->as<DMRContact>()->number(), 2345);

  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Override,
                              ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 3);
  QCOMPARE(merged->contacts()->get(0)->as<DMRContact>()->name(), "ID 0");
  QCOMPARE(merged->contacts()->get(0)->as<DMRContact>()->number(), 1234);
  QCOMPARE(merged->contacts()->get(1)->as<DMRContact>()->name(), "ID 1");
  QCOMPARE(merged->contacts()->get(1)->as<DMRContact>()->number(), 2345);
  QCOMPARE(merged->contacts()->get(2)->as<DMRContact>()->name(), "ID 2");
  QCOMPARE(merged->contacts()->get(2)->as<DMRContact>()->number(), 2345);

  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Duplicate,
                              ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 4);
  QCOMPARE(merged->contacts()->get(0)->as<DMRContact>()->name(), "ID 0");
  QCOMPARE(merged->contacts()->get(0)->as<DMRContact>()->number(), 1234);
  QCOMPARE(merged->contacts()->get(1)->as<DMRContact>()->name(), "ID 1");
  QCOMPARE(merged->contacts()->get(1)->as<DMRContact>()->number(), 1234);
  QCOMPARE(merged->contacts()->get(2)->as<DMRContact>()->name(), "ID 1 (copy)");
  QCOMPARE(merged->contacts()->get(2)->as<DMRContact>()->number(), 2345);
  QCOMPARE(merged->contacts()->get(3)->as<DMRContact>()->name(), "ID 2");
  QCOMPARE(merged->contacts()->get(3)->as<DMRContact>()->number(), 2345);
}


void
MergeTest::testMergeGroupLists() {
  Config *base = new Config(), *merging = new Config();

  base->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 0", 1234));
  base->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 1", 1234));

  {
    RXGroupList *lst = new RXGroupList("List 0");
    lst->addContact(base->contacts()->contact(0)->as<DMRContact>());
    lst->addContact(base->contacts()->contact(1)->as<DMRContact>());
    base->rxGroupLists()->add(lst);
  }

  merging->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 1", 2345));
  merging->contacts()->add(new DMRContact(DMRContact::PrivateCall, "ID 2", 2345));

  {
    RXGroupList *lst = new RXGroupList("List 0");
    lst->addContact(merging->contacts()->contact(0)->as<DMRContact>());
    lst->addContact(merging->contacts()->contact(1)->as<DMRContact>());
    merging->rxGroupLists()->add(lst);
  }


  ErrorStack err;
  Config *merged = ConfigMerge::merge(base, merging,
                                      ConfigMergeVisitor::ItemStrategy::Ignore,
                                      ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 3);
  QCOMPARE(merged->rxGroupLists()->count(), 1);
  QCOMPARE(merged->rxGroupLists()->list(0)->count(), 2);
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(0), merged->contacts()->contact(0));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(1), merged->contacts()->contact(1));


  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Ignore,
                              ConfigMergeVisitor::SetStrategy::Merge, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 3);
  QCOMPARE(merged->rxGroupLists()->count(), 1);
  QCOMPARE(merged->rxGroupLists()->list(0)->count(), 3);
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(0), merged->contacts()->contact(0));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(1), merged->contacts()->contact(1));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(2), merged->contacts()->contact(2));

  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Override,
                              ConfigMergeVisitor::SetStrategy::Merge, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 3);
  QCOMPARE(merged->rxGroupLists()->count(), 1);
  QCOMPARE(merged->rxGroupLists()->list(0)->count(), 3);
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(0), merged->contacts()->contact(0));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(1), merged->contacts()->contact(1));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(2), merged->contacts()->contact(2));

  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Ignore,
                              ConfigMergeVisitor::SetStrategy::Override, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 3);
  QCOMPARE(merged->rxGroupLists()->count(), 1);
  QCOMPARE(merged->rxGroupLists()->list(0)->count(), 2);
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(0), merged->contacts()->contact(1));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(1), merged->contacts()->contact(2));

  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Duplicate,
                              ConfigMergeVisitor::SetStrategy::Merge, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 4);
  QCOMPARE(merged->rxGroupLists()->count(), 1);
  QCOMPARE(merged->rxGroupLists()->list(0)->count(), 4);
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(0), merged->contacts()->contact(0));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(1), merged->contacts()->contact(1));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(2), merged->contacts()->contact(2));
  QCOMPARE(merged->rxGroupLists()->list(0)->contact(3), merged->contacts()->contact(3));
}


void
MergeTest::testMergeChannels() {
  Config *base = new Config(), *merging = new Config();

  base->contacts()->add(new DMRContact(DMRContact::GroupCall, "ID 0", 1234));

  {
    RXGroupList *lst = new RXGroupList("List 0");
    lst->addContact(base->contacts()->contact(0)->as<DMRContact>());
    base->rxGroupLists()->add(lst);

    FMChannel *ach = new FMChannel();
    ach->setName("FM 0");
    ach->setRXFrequency(Frequency::fromMHz(144.0));
    ach->setTXFrequency(Frequency::fromMHz(144.0));
    base->channelList()->add(ach);

    DMRChannel *dch = new DMRChannel();
    dch->setName("DMR 0");
    dch->setRXFrequency(Frequency::fromMHz(144.0));
    dch->setTXFrequency(Frequency::fromMHz(144.0));
    dch->setTXContactObj(base->contacts()->contact(0)->as<DMRContact>());
    dch->setGroupListObj(lst);
    base->channelList()->add(dch);
  }

  merging->contacts()->add(new DMRContact(DMRContact::GroupCall, "ID 0", 2345));
  merging->contacts()->add(new DMRContact(DMRContact::GroupCall, "ID 1", 3456));

  {
    RXGroupList *lst = new RXGroupList("List 0");
    lst->addContact(merging->contacts()->contact(0)->as<DMRContact>());
    merging->rxGroupLists()->add(lst);

    FMChannel *ach = new FMChannel();
    ach->setName("FM 0");
    ach->setRXFrequency(Frequency::fromMHz(145.0));
    ach->setTXFrequency(Frequency::fromMHz(145.0));
    merging->channelList()->add(ach);

    DMRChannel *dch = new DMRChannel();
    dch->setName("DMR 0");
    dch->setRXFrequency(Frequency::fromMHz(145.0));
    dch->setTXFrequency(Frequency::fromMHz(145.0));
    dch->setTXContactObj(merging->contacts()->contact(0)->as<DMRContact>());
    dch->setGroupListObj(lst);
    merging->channelList()->add(dch);
  }


  ErrorStack err;
  Config *merged = ConfigMerge::merge(base, merging,
                                      ConfigMergeVisitor::ItemStrategy::Ignore,
                                      ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 2);
  QCOMPARE(merged->rxGroupLists()->count(), 1);
  QCOMPARE(merged->channelList()->count(), 2);

  QVERIFY(merged->channelList()->channel(0)->is<FMChannel>());
  QCOMPARE(merged->channelList()->channel(0)->rxFrequency().inMHz(), 144.0);

  QVERIFY(merged->channelList()->channel(1)->is<DMRChannel>());
  QCOMPARE(merged->channelList()->channel(1)->rxFrequency().inMHz(), 144.0);
  QCOMPARE(merged->channelList()->channel(1)->as<DMRChannel>()->txContactObj(), merged->contacts()->contact(0)->as<DMRContact>());
  QCOMPARE(merged->channelList()->channel(1)->as<DMRChannel>()->groupListObj(), merged->rxGroupLists()->list(0));


  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Override,
                              ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->contacts()->count(), 2);
  QCOMPARE(merged->rxGroupLists()->count(), 1);
  QCOMPARE(merged->channelList()->count(), 2);

  QVERIFY(merged->channelList()->channel(0)->is<FMChannel>());
  QCOMPARE(merged->channelList()->channel(0)->rxFrequency().inMHz(), 145.0);

  QVERIFY(merged->channelList()->channel(1)->is<DMRChannel>());
  QCOMPARE(merged->channelList()->channel(1)->rxFrequency().inMHz(), 145.0);
  QCOMPARE(merged->channelList()->channel(1)->as<DMRChannel>()->txContactObj(), merged->contacts()->contact(0)->as<DMRContact>());
  QCOMPARE(merged->channelList()->channel(1)->as<DMRChannel>()->groupListObj(), merged->rxGroupLists()->list(0));
}


void
MergeTest::testMergeZones() {
  Config *base = new Config(), *merging = new Config();

  base->contacts()->add(new DMRContact(DMRContact::GroupCall, "ID 0", 1234));

  {
    RXGroupList *lst = new RXGroupList("List 0");
    lst->addContact(base->contacts()->contact(0)->as<DMRContact>());
    base->rxGroupLists()->add(lst);

    FMChannel *ach = new FMChannel();
    ach->setName("FM 0");
    ach->setRXFrequency(Frequency::fromMHz(144.0));
    ach->setTXFrequency(Frequency::fromMHz(144.0));
    base->channelList()->add(ach);

    DMRChannel *dch = new DMRChannel();
    dch->setName("DMR 0");
    dch->setRXFrequency(Frequency::fromMHz(144.0));
    dch->setTXFrequency(Frequency::fromMHz(144.0));
    dch->setTXContactObj(base->contacts()->contact(0)->as<DMRContact>());
    dch->setGroupListObj(lst);
    base->channelList()->add(dch);

    Zone *zone = new Zone("Zone 0");
    zone->A()->add(ach);
    zone->A()->add(dch);
    base->zones()->add(zone);
  }

  merging->contacts()->add(new DMRContact(DMRContact::GroupCall, "ID 0", 2345));
  merging->contacts()->add(new DMRContact(DMRContact::GroupCall, "ID 1", 3456));

  {
    RXGroupList *lst = new RXGroupList("List 0");
    lst->addContact(merging->contacts()->contact(0)->as<DMRContact>());
    merging->rxGroupLists()->add(lst);

    FMChannel *ach = new FMChannel();
    ach->setName("FM 0");
    ach->setRXFrequency(Frequency::fromMHz(145.0));
    ach->setTXFrequency(Frequency::fromMHz(145.0));
    merging->channelList()->add(ach);

    DMRChannel *dch = new DMRChannel();
    dch->setName("DMR 1");
    dch->setRXFrequency(Frequency::fromMHz(145.0));
    dch->setTXFrequency(Frequency::fromMHz(145.0));
    dch->setTXContactObj(merging->contacts()->contact(0)->as<DMRContact>());
    dch->setGroupListObj(lst);
    merging->channelList()->add(dch);

    Zone *zone = new Zone("Zone 0");
    zone->A()->add(ach);
    zone->A()->add(dch);
    merging->zones()->add(zone);
  }


  ErrorStack err;
  Config *merged = ConfigMerge::merge(base, merging,
                                      ConfigMergeVisitor::ItemStrategy::Ignore,
                                      ConfigMergeVisitor::SetStrategy::Ignore, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->channelList()->count(), 3);
  QCOMPARE(merged->zones()->count(), 1);
  QCOMPARE(merged->zones()->zone(0)->A()->count(), 2);
  QCOMPARE(merged->zones()->zone(0)->A()->get(0)->as<Channel>(),
           merged->channelList()->channel(0));
  QCOMPARE(merged->zones()->zone(0)->A()->get(1)->as<Channel>(),
           merged->channelList()->channel(1));


  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Ignore,
                              ConfigMergeVisitor::SetStrategy::Override, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->channelList()->count(), 3);
  QCOMPARE(merged->zones()->count(), 1);
  QCOMPARE(merged->zones()->zone(0)->A()->count(), 2);
  QCOMPARE(merged->zones()->zone(0)->A()->get(0)->as<Channel>(),
           merged->channelList()->channel(0));
  QCOMPARE(merged->zones()->zone(0)->A()->get(1)->as<Channel>(),
           merged->channelList()->channel(2));


  merged = ConfigMerge::merge(base, merging,
                              ConfigMergeVisitor::ItemStrategy::Ignore,
                              ConfigMergeVisitor::SetStrategy::Merge, err);
  if (nullptr == merged)
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(merged->channelList()->count(), 3);
  QCOMPARE(merged->zones()->count(), 1);
  QCOMPARE(merged->zones()->zone(0)->A()->count(), 3);
  QCOMPARE(merged->zones()->zone(0)->A()->get(0)->as<Channel>(),
           merged->channelList()->channel(0));
  QCOMPARE(merged->zones()->zone(0)->A()->get(1)->as<Channel>(),
           merged->channelList()->channel(1));
  QCOMPARE(merged->zones()->zone(0)->A()->get(2)->as<Channel>(),
           merged->channelList()->channel(2));
}


QTEST_GUILESS_MAIN(MergeTest)
