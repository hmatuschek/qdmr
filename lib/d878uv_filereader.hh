#ifndef D878UVFILEREADER_HH
#define D878UVFILEREADER_HH

#include "d868uv_filereader.hh"

class D878UVFileReader: public D868UVFileReader
{
public:
  D878UVFileReader(Config *config, const uint8_t *data, size_t size, QString &message);
};

#endif // D878UVFILEREADER_HH
