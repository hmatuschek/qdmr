#ifndef RD5RTEST_HH
#define RD5RTEST_HH
#include "rd5r_codeplug.hh"

#include <QObject>

class RD5RTest : public QObject
{
  Q_OBJECT

public:
  explicit RD5RTest(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testDMRID();
  void testRadioName();
  void testIntroLines();
  void testDigitalContacts();
  void testRXGroups();
  void testDigitalChannels();

protected:
  RD5RCodeplug _codeplug;
};

#endif // RD5RTEST_HH
