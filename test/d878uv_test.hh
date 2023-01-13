#ifndef D878UVTEST_HH
#define D878UVTEST_HH


#include <QObject>
#include "config.hh"

class D878UVTest : public QObject
{
  Q_OBJECT

public:
  explicit D878UVTest(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // D878UVTEST_HH
