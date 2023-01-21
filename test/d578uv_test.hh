#ifndef D578UVTEST_HH
#define D578UVTEST_HH


#include <QObject>
#include "config.hh"

class D578UVTest : public QObject
{
  Q_OBJECT

public:
  explicit D578UVTest(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // D578UVTEST_HH
