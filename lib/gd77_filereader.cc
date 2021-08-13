#include "gd77_filereader.hh"
#include <QFile>
#include <QFileInfo>

#define SEGMENT0_ADDR 0x00000080
#define SEGMENT0_SIZE 0x00007b80
#define SEGMENT1_ADDR 0x00008000
#define SEGMENT1_SIZE 0x00016300

bool
GD77FileReader::read(const QString &filename, GD77Codeplug *codeplug, QString &errorMessage)
{
  // Check file properties
  QFileInfo info(filename);
  if (! info.exists()) {
    errorMessage = QObject::tr("Cannot open file '%1': File does not exisist.").arg(filename);
    return false;
  }
  if (131072 != info.size()) {
    errorMessage = QObject::tr("Cannot read codeplug file '%1': File size is not 131072 bytes.").arg(filename);
    return false;
  }

  // Open file
  QFile file(filename);
  if (! file.open(QFile::ReadOnly)) {
    errorMessage = QObject::tr("Cannot open file '%1': %2").arg(filename, file.errorString());
    return false;
  }

  // Read file content

  if (! file.seek(SEGMENT0_ADDR)) {
    errorMessage = QObject::tr("Cannot read codeplug file '%1': Cannot seek within file: %2")
        .arg(filename, file.errorString());
    file.close();
    return false;
  }
  char *ptr = (char *)codeplug->data(SEGMENT0_ADDR);
  size_t n = SEGMENT0_SIZE;
  while (0 < n) {
    int nread = file.read(ptr, n);
    if (0 > nread) {
      errorMessage = QObject::tr("Cannot read codeplug file '%1': %2")
          .arg(filename,file.errorString());
      file.close();
      return false;
    }
    n -= nread;
    ptr += nread;
  }

  if (! file.seek(SEGMENT1_ADDR)) {
    errorMessage = QObject::tr("Cannot read codeplug file '%1': Cannot seek within file: %2")
        .arg(filename, file.errorString());
    file.close();
    return false;
  }
  ptr = (char *)codeplug->data(SEGMENT1_ADDR);
  n = SEGMENT1_SIZE;
  while (0 < n) {
    int nread = file.read(ptr, n);
    if (0 > nread) {
      errorMessage = QObject::tr("Cannot read codeplug file '%1': %2")
          .arg(filename,file.errorString());
      file.close();
      return false;
    }
    n -= nread;
    ptr += nread;
  }

  return true;
}
