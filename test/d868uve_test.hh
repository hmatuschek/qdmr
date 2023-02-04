#ifndef D868UVETEST_HH
#define D868UVETEST_HH


#include "libdmrconfigtest.hh"

class D868UVETest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit D868UVETest(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // D878UV2TEST_HH
