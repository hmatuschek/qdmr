#ifndef UV390FILEREADER_HH
#define UV390FILEREADER_HH

#include "uv390_codeplug.hh"

/** Methods to read manufacturer codeplug files.
 *
 * The file format of the stock CPS is still pretty simple. The first part of the file consists of
 * a mal-formed DFU file. This contains a single image with a single element containing the
 * first section of the memory written to the device. The second section is then added as-is
 * to the end of the file. Due to the DFU header/footer, the file and memory offsets differ.
 *
 * <table>
 *  <tr><th>File Start</th> <th>Memory Start</th> <th>Size</th></tr>
 *  <tr><td>0x002225</td>   <td>0x002000</td>     <td>0x03e000</td></tr>
 *  <tr><td>0x040235</td>   <td>0x110000</td>     <td>0x090000</td></tr>
 * </table>
 *
 * @ingroup uv390 */
class UV390FileReader
{
public:
  /** Reads manufacturer codeplug file into given codeplug object.
   * @param filename Specifies the file to read.
   * @param codeplug Specifies the codeplug object to store read codeplug.
   * @param errorMessage On error, contains an error message.
   * @returns @c true on success and @c false on error. */
  static bool read(const QString &filename, UV390Codeplug *codeplug, QString &errorMessage);
};

#endif // UV390FILEREADER_HH
