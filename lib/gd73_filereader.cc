#include "gd73_filereader.hh"
#include <QFile>
#include <QFileInfo>

#define SEGMENT0_ADDR 0x00000000
#define SEGMENT0_SIZE 0x00022014

bool
GD73FileReader::read(const QString &filename, GD73Codeplug *codeplug, const ErrorStack &err)
{
  // Check file properties
  QFileInfo info(filename);
  if (! info.exists()) {
    errMsg(err) << "Cannot open file '" << filename << "': File does not exisist.";
    return false;
  }
  if (SEGMENT0_SIZE != info.size()) {
    errMsg(err) << "Cannot read codeplug file '" << filename << "': File size is not " << SEGMENT0_SIZE << " bytes.";
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

  return true;
}
