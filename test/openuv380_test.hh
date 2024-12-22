#ifndef OPENUV380TEST_HH
#define OPENUV380TEST_HH


#include "libdmrconfigtest.hh"
#include <QObject>
#include <QTextStream>
#include "config.hh"

class OpenUV380Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit OpenUV380Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testChannelFrequency();

protected:
  QTextStream _stderr;
};

#endif // OPENUV380TEST_HH
