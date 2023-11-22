#include "trafotest.hh"
#include "intermediaterepresentation.hh"
#include "configcopyvisitor.hh"

TrafoTest::TrafoTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
TrafoTest::testZoneSplitVisitor() {
  ErrorStack err;
  Config *copy = ConfigCopy::copy(&_basicConfig, err)->as<Config>();
  if (nullptr == copy)
    QFAIL(err.format().toLocal8Bit().constData());

  ZoneSplitVisitor splitter;
  if (! splitter.process(copy, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(copy->zones()->count(), 2);
  QVERIFY(copy->zones()->get(0)->name().endsWith(" A"));
  QVERIFY(copy->zones()->get(1)->name().endsWith(" B"));
  QCOMPARE(copy->zones()->get(0)->name().chopped(2),
           copy->zones()->get(1)->name().chopped(2));
  QCOMPARE(copy->zones()->get(0)->as<Zone>()->A()->count(), 3);
  QCOMPARE(copy->zones()->get(1)->as<Zone>()->A()->count(), 1);
}


void
TrafoTest::testZoneMergeVisitor() {
  ErrorStack err;
  Config *copy = ConfigCopy::copy(&_basicConfig, err)->as<Config>();
  if (nullptr == copy)
    QFAIL(err.format().toLocal8Bit().constData());

  ZoneSplitVisitor splitter;
  if (! splitter.process(copy, err))
    QFAIL(err.format().toLocal8Bit().constData());

  ZoneMergeVisitor merger;
  if (! merger.process(copy, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(_basicConfig.compare(*copy), 0);
}


void
TrafoTest::testListElementRemoval() {
  ErrorStack err;
  Config *copy = ConfigCopy::copy(&_basicConfig, err)->as<Config>();
  if (nullptr == copy)
    QFAIL(err.format().toLocal8Bit().constData());

  ObjectFilterVisitor filter({GPSSystem::staticMetaObject});
  if (! filter.process(copy, err))
    QFAIL(err.format().toLocal8Bit().constData());

  // Check filter
  QCOMPARE(copy->posSystems()->count(), 0);
  // Check references to deleted objects
  QCOMPARE(copy->channelList()->channel(1)->as<DMRChannel>()->aprsObj(), nullptr);
}

void
TrafoTest::testPropertyRemoval() {
  ErrorStack err;
  Config config;
  if (! config.readYAML(":/data/anytone_call_hangtime.yaml", err))
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  QVERIFY2(config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");

  ObjectFilterVisitor filter({AnytoneSettingsExtension::staticMetaObject});
  if (! filter.process(&config, err))
    QFAIL(err.format().toLocal8Bit().constData());

  // Check filter
  QCOMPARE(config.settings()->anytoneExtension(), nullptr);
}

QTEST_GUILESS_MAIN(TrafoTest)
