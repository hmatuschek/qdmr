#ifndef TABLEFORMATTEST_HH
#define TABLEFORMATTEST_HH

#include <QObject>


class TableFormatTest : public QObject
{
  Q_OBJECT

public:
  explicit TableFormatTest(QObject *parent = nullptr);

private slots:
  void testFrequencyParser();
};


#endif // TABLEFORMATTEST_HH
