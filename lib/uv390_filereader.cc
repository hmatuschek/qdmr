#include "uv390_filereader.hh"
#include <QFile>
#include <QFileInfo>

#define SEGMENT0_FILE_ADDR   0x00002225
#define SEGMENT0_TARGET_ADDR 0x00002000
#define SEGMENT0_SIZE        0x0003e000
#define SEGMENT1_FILE_ADDR   0x00040235
#define SEGMENT1_TARGET_ADDR 0x00110000
#define SEGMENT1_SIZE        0x00090000

bool
UV390FileReader::read(const QString &filename, UV390Codeplug *codeplug, const ErrorStack &err)
{
  // Check file properties
  QFileInfo info(filename);
  if (! info.exists()) {
    errMsg(err) << "Cannot open file '" << filename << "': File does not exisist.";
    return false;
  }
  if (852533 != info.size()) {
    errMsg(err) << "Cannot read codeplug file '" << filename << "': File size is not 852533 bytes.";
    return false;
  }

  // Open file
  QFile file(filename);
  if (! file.open(QFile::ReadOnly)) {
    errMsg(err) << "Cannot open file '" << filename << "': " << file.errorString() << ".";
    return false;
  }

  // Read file content

  if (! file.seek(SEGMENT0_FILE_ADDR)) {
    errMsg(err) << "Cannot read codeplug file '" << filename
                << "': Cannot seek within file: " << file.errorString() << ".";
    file.close();
    return false;
  }
  char *ptr = (char *)codeplug->data(SEGMENT0_TARGET_ADDR);
  size_t n = SEGMENT0_SIZE;
  while (0 < n) {
    int nread = file.read(ptr, n);
    if (0 > nread) {
      errMsg(err) << "Cannot read codeplug file '" << filename
                  << "': " << file.errorString() << ".";
      file.close();
      return false;
    }
    n -= nread;
    ptr += nread;
  }

  if (! file.seek(SEGMENT1_FILE_ADDR)) {
    errMsg(err) << "Cannot read codeplug file '" << filename
                << "': Cannot seek within file: " << file.errorString() << ".";
    file.close();
    return false;
  }
  ptr = (char *)codeplug->data(SEGMENT1_TARGET_ADDR);
  n = SEGMENT1_SIZE;
  while (0 < n) {
    int nread = file.read(ptr, n);
    if (0 > nread) {
      errMsg(err) << "Cannot read codeplug file '" << filename
                  << "': " << file.errorString() << ".";
      file.close();
      return false;
    }
    n -= nread;
    ptr += nread;
  }

  return true;
}
