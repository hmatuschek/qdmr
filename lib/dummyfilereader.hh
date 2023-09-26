#ifndef DUMMYFILEREADER_HH
#define DUMMYFILEREADER_HH

#include "errorstack.hh"

class Codeplug;

/** Just a class implementing the filereader "interface" that fails. */
class DummyFileReader
{
public:
  /** Does not read the specified file. Just returns an error. */
  static bool read(const QString &filename, Codeplug *codeplug, const ErrorStack &err=ErrorStack());
};

#endif // DUMMYFILEREADER_HH
