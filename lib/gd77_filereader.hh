#ifndef GD77FILEREADER_HH
#define GD77FILEREADER_HH

#include "gd77_codeplug.hh"

/** Methods to read manufacturer codeplug files.
 * @ingroup gd77 */
class GD77FileReader
{
public:
  /** Reads manufacturer codeplug file into given codeplug object.
   * @param filename Specifies the file to read.
   * @param codeplug Specifies the codeplug object to store read codeplug.
   * @param errorMessage On error, contains an error message.
   * @returns @c true on success and @c false on error. */
  static bool read(const QString &filename, GD77Codeplug *codeplug, QString &errorMessage);
};

#endif // GD77FILEREADER_HH
