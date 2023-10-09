#ifndef CONFIGTEST_HH
#define CONFIGTEST_HH

#include <QObject>
#include "libdmrconfigtest.hh"
#include "config.hh"


class ConfigTest : public UnitTestBase
{
  Q_OBJECT

public:
  explicit ConfigTest(QObject *parent = nullptr);

private slots:
  void initTestCase();

  void testImmediateRefInvalidation();

  void testCloneChannelBasic();

  /** Regression test for issue #388. */
  void testMultipleRadioIDs();

  void testMelodyLilypond();
  void testMelodyEncoding();
  void testMelodyDecoding();
};

#endif // CONFIGTEST_HH
