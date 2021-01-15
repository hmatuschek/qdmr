#ifndef CSVWRITER_HH
#define CSVWRITER_HH

#include <QTextStream>

class Config;


/** Writing config files.
 * @ingroup conf */
class CSVWriter
{
public:
  /** Writes the given @c config into the @c stream.
   * @returns @c true on success. */
  static bool write(const Config *config, QTextStream &stream, QString &errorMessage);
};

#endif // CSVWRITER_HH
