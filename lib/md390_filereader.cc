#include "md390_filereader.hh"
#include <QFile>
#include <QFileInfo>

#define SEGMENT0_FILE_ADDR   0x00002225
#define SEGMENT0_TARGET_ADDR 0x00002000
#define SEGMENT0_SIZE        0x0003e000

bool
MD390FileReader::read(const QString &filename, MD390Codeplug *codeplug, QString &errorMessage)
{
  // Check file properties
  QFileInfo info(filename);
  if (! info.exists()) {
    errorMessage = QObject::tr("Cannot open file '%1': File does not exisist.").arg(filename);
    return false;
  }
  if (262709 != info.size()) {
    errorMessage = QObject::tr("Cannot read codeplug file '%1': File size is not 262709 bytes.").arg(filename);
    return false;
  }

  // Open file
  QFile file(filename);
  if (! file.open(QFile::ReadOnly)) {
    errorMessage = QObject::tr("Cannot open file '%1': %2").arg(filename, file.errorString());
    return false;
  }

  // Read file content

  if (! file.seek(SEGMENT0_FILE_ADDR)) {
    errorMessage = QObject::tr("Cannot read codeplug file '%1': Cannot seek within file: %2")
        .arg(filename, file.errorString());
    file.close();
    return false;
  }
  char *ptr = (char *)codeplug->data(SEGMENT0_TARGET_ADDR);
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

  return true;
}
