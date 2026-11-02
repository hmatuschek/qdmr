#ifndef ANYTONE_PRESERVE_TEST_HH
#define ANYTONE_PRESERVE_TEST_HH

#include <QObject>

/** Tests for the preserving AnyTone write (gsquared3d fork, TD-004 / W-02). Needs no radio:
 * the "radio" is a memory image made by encoding a mixed analog/digital test codeplug. */
class AnytonePreserveTest : public QObject
{
  Q_OBJECT

public:
  explicit AnytonePreserveTest(QObject *parent=nullptr);

private slots:
  void testNoEditWritesNothing_data();
  void testNoEditWritesNothing();
  void testAnalogEditLeavesDigitalAlone_data();
  void testAnalogEditLeavesDigitalAlone();
  void testAddAnalogChannelLeavesDigitalAlone_data();
  void testAddAnalogChannelLeavesDigitalAlone();
  void testMemoryFileRoundTrip();
};

#endif // ANYTONE_PRESERVE_TEST_HH
