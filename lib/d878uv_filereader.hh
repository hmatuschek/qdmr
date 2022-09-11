#ifndef D878UVFILEREADER_HH
#define D878UVFILEREADER_HH

#include "d868uv_filereader.hh"

/** Implements a file read for the AnyTone D878UV manufacturer CPS file. */
class D878UVFileReader: public D868UVFileReader
{
public:
  /** Constructor. */
  D878UVFileReader(Config *config, const uint8_t *data, size_t size, QString &message);
};

#endif // D878UVFILEREADER_HH
