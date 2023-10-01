#ifndef GD77FILEREADER_HH
#define GD77FILEREADER_HH

#include "gd77_codeplug.hh"

/** Methods to read manufacturer codeplug files.
 *
 * The file format of the stock CPS is pretty simple. It is a one-to-one dump of the codeplug
 * data as written to the device. This makes the decoding of the manufacturer codeplug files very
 * easy. Some memory regions, however, are not written to the deivice although they are present in
 * the codeplug file.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th></tr>
 *  <tr><td>0x00080</td> <td>0x07c00</td>  <td>0x07b80</td></tr>
 *  <tr><td>0x08000</td> <td>0x1e300</td>  <td>0x16300</td></tr>
 * </table>
 *
 * @ingroup gd77 */
class GD77FileReader
{
public:
  /** Reads manufacturer codeplug file into given codeplug object.
   * @param filename Specifies the file to read.
   * @param codeplug Specifies the codeplug object to store read codeplug.
   * @param err Error stack.
   * @returns @c true on success and @c false on error. */
  static bool read(const QString &filename, GD77Codeplug *codeplug, const ErrorStack &err=ErrorStack());
};

#endif // GD77FILEREADER_HH
