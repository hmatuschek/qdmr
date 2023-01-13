#ifndef D878UV2TEST_HH
#define D878UV2TEST_HH


#include <QObject>
#include "config.hh"

class D878UV2Test : public QObject
{
  Q_OBJECT

public:
  explicit D878UV2Test(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

protected:
  Config _basicConfig;
};

#endif // D878UV2TEST_HH
