#ifndef OPENGD77TEST_HH
#define OPENGD77TEST_HH


#include "libdmrconfigtest.hh"
#include <QObject>
#include <QTextStream>
#include "config.hh"

class OpenGD77Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit OpenGD77Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();
  void testSMSTemplates();

protected:
  QTextStream _stderr;
};

#endif // OPENGD77TEST_HH
