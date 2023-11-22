#include "dr1801uv_filereader.hh"
#include <QFile>
#include <QFileInfo>

#define SEGMENT0_ADDR 0x00000000
#define SEGMENT0_SIZE 0x0001dd90

bool
DR1801UVFileReader::read(const QString &filename, DR1801UVCodeplug *codeplug, const ErrorStack &err)
{
  // Check file properties
  QFileInfo info(filename);
  if (! info.exists()) {
    errMsg(err) << "Cannot open file '" << filename
                << "': File does not exisist.";
    return false;
  }
  if (0x1dd90 != info.size()) {
    errMsg(err) << "Cannot read codeplug file '" << filename
                << "': File size is not 1dd90h bytes.";
    return false;
  }

  // Open file
  QFile file(filename);
  if (! file.open(QFile::ReadOnly)) {
    errMsg(err) << "Cannot open file '" << filename
                << "': " << file.errorString() << ".";
    return false;
  }

  // Read file content
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

  return true;
}
