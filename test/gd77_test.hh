#ifndef GD77TEST_HH
#define GD77TEST_HH


#include <QObject>
#include "config.hh"

class GD77Test : public QObject
{
  Q_OBJECT

public:
  explicit GD77Test(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // GD77TEST_HH
