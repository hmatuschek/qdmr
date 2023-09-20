#include "copytest.hh"
#include "configcopyvisitor.hh"


CopyTest::CopyTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
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
CopyTest::testChannelClone() {

}


QTEST_GUILESS_MAIN(CopyTest)
