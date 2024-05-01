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
  void testMergeGroupLists();
  void testMergeChannels();
  void testMergeZones();
};

#endif // MERGETEST_HH
