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
  ErrorStack err;

  ConfigCloneVisitor cloner(map);
  if (! cloner.process(&_basicConfig, err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }

  ConfigItem *item = cloner.takeResult(err);
  QVERIFY(nullptr != item);

  QVERIFY(0 == _basicConfig.compare(*item));
}

void
CopyTest::testChannelClone() {

}


QTEST_GUILESS_MAIN(CopyTest)
