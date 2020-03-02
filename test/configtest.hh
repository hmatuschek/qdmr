#ifndef CONFIGTEST_HH
#define CONFIGTEST_HH

#include <QObject>
#include "config.hh"


class ConfigTest : public QObject
{
  Q_OBJECT

public:
  explicit ConfigTest(QObject *parent = nullptr);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testDMRID();
  void testRadioName();
  void testIntroLines();
  void testMicLevel();
  void testSpeechSynthesis();
  void testDigitalContacts();
  void testRXGroups();
  void testDigitalChannels();
  void testAnalogChannels();
  void testZones();
  void testScanLists();
  void testGPSSystems();

protected:
  Config _config;
};

#endif // CONFIGTEST_HH
