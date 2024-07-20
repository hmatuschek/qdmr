#include "rd5r_filereader.hh"
#include <QFile>
#include <QFileInfo>

#define SEGMENT0_ADDR 0x00000080
#define SEGMENT0_SIZE 0x00007b80
#define SEGMENT1_ADDR 0x00008000
#define SEGMENT1_SIZE 0x00016300

bool
RD5RFileReader::read(const QString &filename, RD5RCodeplug *codeplug, const ErrorStack &err)
{
  // Check file properties
  QFileInfo info(filename);
  if (! info.exists()) {
    errMsg(err) << "Cannot open file '" << filename << "': File does not exisist.";
    return false;
  }
  if (131072 != info.size()) {
    errMsg(err) << "Cannot read codeplug file '" << filename << "': File size is not 131072 bytes.";
    return false;
  }

  // Open file
  QFile file(filename);
  if (! file.open(QFile::ReadOnly)) {
    errMsg(err) << "Cannot open file '" << filename << "': " << file.errorString() << ".";
    return false;
  }

  // Read file content

  if (! file.seek(SEGMENT0_ADDR)) {
    errMsg(err) << "Cannot read codeplug file '" << filename
                << "': Cannot seek within file: " << file.errorString() << ".";
    file.close();
    return false;
  }
  char *ptr = (char *)codeplug->data(SEGMENT0_ADDR);
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

  if (! file.seek(SEGMENT1_ADDR)) {
    errMsg(err) << "Cannot read codeplug file '" << filename
                << "': Cannot seek within file: " << file.errorString() << ".";
    file.close();
    return false;
  }
  ptr = (char *)codeplug->data(SEGMENT1_ADDR);
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
