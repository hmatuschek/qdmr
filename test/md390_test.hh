#ifndef MD390TEST_HH
#define MD390TEST_HH


#include <QObject>
#include "config.hh"

class MD390Test : public QObject
{
  Q_OBJECT

public:
  explicit MD390Test(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // MD390TEST_HH
