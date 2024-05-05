#include "tableformattest.hh"
#include "config.hh"
#include "csvreader.hh"
#include <QTest>


TableFormatTest::TableFormatTest(QObject *parent)
  : QObject{parent}
{
  // pass...
}

void
TableFormatTest::testFrequencyParser() {
  Config config;

  QString data;
  QTextStream stream(&data);

  stream << "ID: 2621370" << Qt::endl
         << "Name: \"DM3MAT\"" << Qt::endl
         << "Digital Name       Receive    Transmit   Power Scan TOT RO Admit  CC TS RxGL TxC GPS Roam ID" << Qt::endl
         << "1       \"test 0\" 439.56250  -7.60000   High  -    -   -  Color  1  1  -    -   -   +    -  # Local" << Qt::endl
         << "2       \"test 1\" 439.56250  +7.60000   High  -    -   -  Color  1  1  -    -   -   +    -  # Sa/Th" << Qt::endl
         << "3       \"test 2\" 439.56250 439.56250   High  -    -   -  Color  1  2  -    -   -   +    -  # Regional" << Qt::endl;

  QString errMsg;
  QVERIFY2(CSVReader::read(&config, stream, errMsg), errMsg.toStdString().c_str());

  QCOMPARE(config.channelList()->count(), 3);
  QCOMPARE(config.channelList()->channel(0)->rxFrequency(), Frequency::fromMHz(439.56250));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(), Frequency::fromMHz(431.96250));
  QCOMPARE(config.channelList()->channel(1)->txFrequency(), Frequency::fromMHz(447.16250));
  QCOMPARE(config.channelList()->channel(2)->txFrequency(), Frequency::fromMHz(439.56250));
}

QTEST_GUILESS_MAIN(TableFormatTest)


