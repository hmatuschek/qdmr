#ifndef D868UVETEST_HH
#define D868UVETEST_HH


#include <QObject>
#include "config.hh"

class D868UVETest : public QObject
{
  Q_OBJECT

public:
  explicit D868UVETest(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // D878UV2TEST_HH
