#ifndef LABELTEST_HH
#define LABELTEST_HH

#include "libdmrconfigtest.hh"

class LabelTest: public UnitTestBase
{
  Q_OBJECT

public:
  explicit LabelTest(QObject *parent=nullptr);

private slots:
  void testLabelVisitor();
};

#endif // LABETEST_HH
