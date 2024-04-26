#ifndef MERGETEST_HH
#define MERGETEST_HH

#include <QObject>

class MergeTest : public QObject
{
  Q_OBJECT

public:
  explicit MergeTest(QObject *parent=nullptr);

private slots:
  void testMergeRadioIds();
  void testMergeContacts();
};

#endif // MERGETEST_HH
