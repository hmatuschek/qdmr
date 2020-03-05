#include "crc32test.hh"
#include "crc32.hh"
#include <QTest>

CRC32Test::CRC32Test(QObject *parent) : QObject(parent)
{
  // pass...
}

void
CRC32Test::testCRC32() {
  QString txt("The quick brown fox jumps over the lazy dog");
  CRC32 crc;
  crc.update(txt.toLocal8Bit());
  QCOMPARE(crc.get(), 0x414FA339U^0xFFFFFFFF);
}

QTEST_GUILESS_MAIN(CRC32Test)
