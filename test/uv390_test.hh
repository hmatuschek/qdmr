#ifndef UV390TEST_HH
#define UV390TEST_HH


#include <QObject>
#include "config.hh"

class UV390Test : public QObject
{
  Q_OBJECT

public:
  explicit UV390Test(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // UV390TEST_HH
