#ifndef ANYTONEINTERFACETEST_HH
#define ANYTONEINTERFACETEST_HH

#include <QObject>

class AnytoneInterfaceTest : public QObject
{
  Q_OBJECT

public:
  explicit AnytoneInterfaceTest(QObject *parent = nullptr);

private slots:
  void testLostDeviceInProgramMode();
};

#endif // ANYTONEINTERFACETEST_HH
