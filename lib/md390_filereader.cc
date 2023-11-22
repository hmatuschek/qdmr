#include "md390_filereader.hh"
#include <QFile>
#include <QFileInfo>

#define SEGMENT0_FILE_ADDR   0x00002225
#define SEGMENT0_TARGET_ADDR 0x00002000
#define SEGMENT0_SIZE        0x0003e000

bool
MD390FileReader::read(const QString &filename, MD390Codeplug *codeplug, const ErrorStack &err)
{
  // Check file properties
  QFileInfo info(filename);
  if (! info.exists()) {
    errMsg(err) << "Cannot open file '" << filename << "': File does not exisist.";
    return false;
  }
  if (262709 != info.size()) {
    errMsg(err) << "Cannot read codeplug file '" << filename << "': File size is not 262709 bytes.";
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
      errMsg(err)  << "Cannot read codeplug file '" << filename
                   << "': " << file.errorString() << ".";
      file.close();
      return false;
    }
    n -= nread;
    ptr += nread;
  }

  return true;
}
