#ifndef DMR6X2UVTEST_HH
#define DMR6X2UVTEST_HH


#include <QObject>
#include "config.hh"

class DMR6X2UVTest : public QObject
{
  Q_OBJECT

public:
  explicit DMR6X2UVTest(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // DMR6X2UVTEST_HH
