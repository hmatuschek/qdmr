#ifndef D168UVTEST_HH
#define D168UVTEST_HH

#include "libdmrconfigtest.hh"


class D168UVTest: public UnitTestBase
{
  Q_OBJECT

public:
  explicit D168UVTest(QObject *parent=nullptr);

protected:
  void encodeDecode(Config &input, Config &output);

private slots:
  void testFixedLocation();

protected:
  QTextStream _stderr;
};



#endif // D168UVTEST_HH
