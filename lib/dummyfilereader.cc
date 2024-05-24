#include "dummyfilereader.hh"

bool
DummyFileReader::read(const QString &filename, Codeplug *codeplug, const ErrorStack &err) {
  Q_UNUSED(codeplug); Q_UNUSED(filename);
  errMsg(err) << "Cannot read manufacturer codeplug file: not implemented yet.";
  return false;
}
