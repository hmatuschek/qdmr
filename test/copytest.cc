#include "copytest.hh"
#include "configcopyvisitor.hh"


CopyTest::CopyTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
CopyTest::testChannelClone() {
  QHash<ConfigObject *, ConfigObject *> map;

  ErrorStack err;
  ConfigCloneVisitor cloner(map);

  Config config;
  if (! config.readYAML(":/data/config_test.yaml"))
    QFAIL("Cannot load config.");
  auto ch = config.channelList()->channel(0);

  {
    if (! cloner.processItem(ch, err))
      QFAIL(err.format().toLocal8Bit().constData());

    ConfigItem *item = cloner.takeResult(err);
    QVERIFY(item);
    QCOMPARE(ch->compare(*item), 0);
    delete item;
  }

  {
    ch->setPower(Channel::Power::Min);
    ch->setTimeout(60);
    ch->setVOX(3);

    if (! cloner.processItem(ch, err))
      QFAIL(err.format().toLocal8Bit().constData());

    ConfigItem *item = cloner.takeResult(err);
    QVERIFY(item);
    QVERIFY(item->is<Channel>());
    QCOMPARE(item->as<Channel>()->power(), Channel::Power::Min);
    QCOMPARE(item->as<Channel>()->timeout(), 60);
    QCOMPARE(item->as<Channel>()->vox(), 3);
    delete item;
  }

  {
    ch->setDefaultPower();
    ch->setDefaultTimeout();
    ch->setVOXDefault();

    if (! cloner.processItem(ch, err))
      QFAIL(err.format().toLocal8Bit().constData());

    ConfigItem *item = cloner.takeResult(err);
    QVERIFY(item);
    QVERIFY(item->is<Channel>());
    QVERIFY(item->as<Channel>()->defaultPower());
    QVERIFY(item->as<Channel>()->defaultTimeout());
    QVERIFY(item->as<Channel>()->defaultVOX());
    delete item;
  }
}


void
CopyTest::testConfigClone() {
  QHash<ConfigObject *, ConfigObject *> map;
  ConfigCloneVisitor cloner(map);

  ErrorStack err;
  if (! cloner.process(&_basicConfig, err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }

  ConfigItem *item = cloner.takeResult(err);
  QVERIFY(item);

  QCOMPARE(_basicConfig.compare(*item), 0);
}

void
CopyTest::testConfigCopy() {
  QHash<ConfigObject *, ConfigObject *> map;
  ConfigCloneVisitor cloner(map);
  FixReferencesVisistor fixer(map);

  ErrorStack err;
  if (! cloner.process(&_basicConfig, err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }

  ConfigItem *item = cloner.takeResult(err);
  QVERIFY(item);
  QVERIFY(item->is<Config>());

  if (! fixer.process(item->as<Config>(), err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }

  QCOMPARE(_basicConfig.compare(*item), 0);
}

void
CopyTest::testAPRSSystemCopy() {
  // Regression test for issue #468.

  QHash<ConfigObject *, ConfigObject *> map;
  ConfigCloneVisitor cloner(map);
  FixReferencesVisistor fixer(map);

  Config config; ErrorStack err;
  if (! config.readYAML(":/data/fm_aprs_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1\n")
          .arg(err.format()).toStdString().c_str());
  }

  if (! cloner.process(&config, err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }

  ConfigItem *item = cloner.takeResult(err);
  QVERIFY(item);
  QVERIFY(item->is<Config>());

  if (! fixer.process(item->as<Config>(), err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }
  Config *comp_config = item->as<Config>();
  QCOMPARE(config.compare(*item), 0);

  QCOMPARE(config.posSystems()->count(), 1);
  QCOMPARE(comp_config->posSystems()->count(), config.posSystems()->count());
  QVERIFY(config.posSystems()->get(0)->is<APRSSystem>());
  QVERIFY(comp_config->posSystems()->get(0)->is<APRSSystem>());

  APRSSystem *aprs = config.posSystems()->get(0)->as<APRSSystem>(),
      *comp_aprs = comp_config->posSystems()->get(0)->as<APRSSystem>();
  QCOMPARE(comp_aprs->name(), aprs->name());
  QCOMPARE(comp_aprs->period(), aprs->period());
  QCOMPARE(comp_aprs->destination(), aprs->destination()); QCOMPARE(comp_aprs->destSSID(), aprs->destSSID());
  QCOMPARE(comp_aprs->source(), aprs->source()); QCOMPARE(comp_aprs->srcSSID(), aprs->srcSSID());
  QCOMPARE(comp_aprs->path(), aprs->path());
  QCOMPARE(comp_aprs->icon(), aprs->icon());
  QCOMPARE(comp_aprs->message(), aprs->message());
}


QTEST_GUILESS_MAIN(CopyTest)
