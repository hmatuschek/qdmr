#ifndef D578UVTEST_HH
#define D578UVTEST_HH


#include "libdmrconfigtest.hh"

class D578UVTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit D578UVTest(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
};

#endif // D578UVTEST_HH
