#ifndef TRAFOTEST_HH
#define TRAFOTEST_HH

#include "libdmrconfigtest.hh"

class TrafoTest: public UnitTestBase
{
  Q_OBJECT

public:
  explicit TrafoTest(QObject *parent=nullptr);

private slots:
  void testZoneSplitVisitor();
  void testZoneMergeVisitor();
  void testListElementRemoval();
  void testPropertyRemoval();
};

#endif // TRAFOTEST_HH
