#include "anytone_interface_test.hh"
#include "anytone_interface.hh"
#include "errorstack.hh"
#include <QTest>

/** Gives access to the interface state, to simulate a device that is lost in program mode. */
class LostAnytoneInterface: public AnytoneGD32Interface
{
public:
  LostAnytoneInterface()
    : AnytoneGD32Interface(USBDeviceDescriptor())
  {
    // The port is not open, so every command fails, like with an unplugged radio.
    _state = STATE_PROGRAM;
  }
};


AnytoneInterfaceTest::AnytoneInterfaceTest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
AnytoneInterfaceTest::testLostDeviceInProgramMode() {
  LostAnytoneInterface device;
  QVERIFY(! device.isOpen());

  // Must fail, not try to leave program mode again and again until the stack overflows
  uint8_t data[16] = {0};
  QVERIFY(! device.write(0, 0x00800000, data, sizeof(data)));
  QVERIFY(! device.isOpen());
  device.close();
}

QTEST_GUILESS_MAIN(AnytoneInterfaceTest)
