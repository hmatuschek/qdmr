#ifndef DM1701TEST_HH
#define DM1701TEST_HH


#include <QObject>
#include "config.hh"

class DM1701Test : public QObject
{
  Q_OBJECT

public:
  explicit DM1701Test(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // DM1701TEST_HH
