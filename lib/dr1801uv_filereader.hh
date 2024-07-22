#ifndef DR1801UVFILEREADER_HH
#define DR1801UVFILEREADER_HH

#include "dr1801uv_codeplug.hh"

/** Methods to read manufacturer codeplug files.
 *
 * The file format of the stock CPS is pretty simple. It is a one-to-one dump of the codeplug
 * data as written to the device. This makes the decoding of the manufacturer codeplug files very
 * easy.
 *
 * @ingroup dr1801uv */
class DR1801UVFileReader
{
public:
  /** Reads manufacturer codeplug file into given codeplug object.
   * @param filename Specifies the file to read.
   * @param codeplug Specifies the codeplug object to store read codeplug.
   * @param err On error, contains an error message.
   * @returns @c true on success and @c false on error. */
  static bool read(const QString &filename, DR1801UVCodeplug *codeplug, const ErrorStack &err=ErrorStack());
};

#endif // GD77FILEREADER_HH
