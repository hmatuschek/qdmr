#ifndef UTILSTEST_HH
#define UTILSTEST_HH

#include <QObject>

class UtilsTest : public QObject
{
  Q_OBJECT
public:
  explicit UtilsTest(QObject *parent = nullptr);

private slots:
  void testDecodeUnicode();
  void testEncodeUnicode();
  void testDecodeASCII();
  void testEncodeASCII();
  void testDecodeFrequency();
  void testEncodeFrequency();
  void testDecodeDMRID_bcd();
  void testEncodeDMRID_bcd();
};

#endif // UTILSTEST_HH
