#ifndef RD5RTEST_HH
#define RD5RTEST_HH


#include <QObject>
#include "config.hh"

class RD5RTest : public QObject
{
  Q_OBJECT

public:
  explicit RD5RTest(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testBasicConfigEncoding();
  void testBasicConfigDecoding();

  void testChannelFrequency();

protected:
  Config _basicConfig;
  Config _channelFrequencyConfig;
};

#endif // RD5RTEST_HH
