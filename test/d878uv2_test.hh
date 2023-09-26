#ifndef D878UV2TEST_HH
#define D878UV2TEST_HH


#include "libdmrconfigtest.hh"

class D878UV2Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit D878UV2Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // D878UV2TEST_HH
