#ifndef UV390TEST_HH
#define UV390TEST_HH

#include "config.hh"
#include "uv390_codeplug.hh"

#include <QObject>


class UV390Test : public QObject
{
  Q_OBJECT

public:
  explicit UV390Test(QObject *parent = nullptr);

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
  UV390Codeplug _codeplug;
};

#endif // UV390TEST_HH
