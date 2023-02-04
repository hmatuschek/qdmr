#ifndef LIBDMRCONFIGTEST_HH
#define LIBDMRCONFIGTEST_HH

#include <QObject>
#include "config.hh"
#include <QTest>

class UnitTestBase : public QObject
{
  Q_OBJECT

public:
  explicit UnitTestBase(QObject *parent = nullptr);

protected slots:
  virtual void initTestCase();
  virtual void cleanupTestCase();

protected:
  Config _basicConfig;
  Config _channelFrequencyConfig;
};

#endif // LIBDMRCONFIGTEST_HH
