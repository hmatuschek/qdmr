#ifndef OPENGD77TEST_HH
#define OPENGD77TEST_HH


#include "libdmrconfigtest.hh"
#include <QObject>
#include <QTextStream>
#include "config.hh"
#include "errorstack.hh"


class OpenGD77Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit OpenGD77Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();

  /** Regression test for #539. */
  void testChannelGroupList();
  /** Regression test for #507 */
  void testChannelPowerSettings();
  /** Regression test for #541. */
  void testOverrideChannelRadioId();
  /** Regression test for #549. */
  void testChannelSubTones();
  /** Regression test for #556. */
  void testChannelFixedLocation();

protected:
  static bool encodeDecode(Config &config, Config &decoded, const ErrorStack &err=ErrorStack());

protected:
  QTextStream _stderr;
};

#endif // OPENGD77TEST_HH
