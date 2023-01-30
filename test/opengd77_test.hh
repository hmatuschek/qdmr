#ifndef OPENGD77TEST_HH
#define OPENGD77TEST_HH


#include <QObject>
#include "config.hh"

class OpenGD77Test : public QObject
{
  Q_OBJECT

public:
  explicit OpenGD77Test(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // OPENGD77TEST_HH
