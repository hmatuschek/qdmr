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
  ConfigCloneVisitor cloner(map);
  Channel *ch = _basicConfig.channelList()->channel(0);

  ErrorStack err;
  if (! cloner.processItem(ch, err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }

  ConfigItem *item = cloner.takeResult(err);
  QVERIFY(item);

  QCOMPARE(ch->compare(*item), 0);
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


QTEST_GUILESS_MAIN(CopyTest)
