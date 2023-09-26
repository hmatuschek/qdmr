#ifndef RD5RTEST_HH
#define RD5RTEST_HH

#include "libdmrconfigtest.hh"

class RD5RTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit RD5RTest(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

  void testChannelFrequency();
};

#endif // RD5RTEST_HH
