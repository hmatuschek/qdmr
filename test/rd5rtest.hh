#ifndef RD5RTEST_HH
#define RD5RTEST_HH

#include "config.hh"
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
  void testGeneralDefaults();
  void testDigitalContacts();
  void testRXGroups();
  void testDigitalChannels();
  void testAnalogChannels();
  void testZones();
  void testScanLists();
  void testDecode();

protected:
  Config _config;
  RD5RCodeplug _codeplug;
};

#endif // RD5RTEST_HH
