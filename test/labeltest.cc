#include "labeltest.hh"
#include "configlabelingvisitor.hh"


LabelTest::LabelTest(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
LabelTest::testLabelVisitor() {
  Config::Context ctx;
  ConfigLabelingVisitor labelPrinter(ctx);

  ErrorStack err;
  if (! labelPrinter.processItem(&_basicConfig, err)) {
    QFAIL(err.format().toLocal8Bit().constData());
  }

  YAML::Node node = _basicConfig.serialize(ctx, err);
  if (node.IsNull()) {
    QFAIL(err.format().toLocal8Bit().constData());
  }
}



QTEST_GUILESS_MAIN(LabelTest)
