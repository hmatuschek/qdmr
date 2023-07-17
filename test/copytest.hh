#ifndef COPYTEST_HH
#define COPYTEST_HH

#include "libdmrconfigtest.hh"

class CopyTest: public UnitTestBase
{
  Q_OBJECT

public:
  explicit CopyTest(QObject *parent=nullptr);

private slots:
  void testChannelClone();
  void testConfigClone();
};

#endif // COPYTEST_HH
