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

QTEST_GUILESS_MAIN(MergeTest)
