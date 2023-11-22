#ifndef DR1801TEST_HH
#define DR1801TEST_HH


#include "libdmrconfigtest.hh"

class DR1801Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit DR1801Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testBasicConfigReencoding();
};

#endif // DR1801TEST_HH
