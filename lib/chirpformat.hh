/** @defgroup chrip Import/Export from/to CHIRP CSV format
 * @ingroup util
 *
 * These classes implement the (partial) import and export of FM channels from and to the CHIRP
 * CSV format.
 */

#ifndef CHIRPFORMAT_HH
#define CHIRPFORMAT_HH

#include "errorstack.hh"
#include <QSet>

class QTextStream;
class Config;
class QStringList;
class FMChannel;


/** Some common constants for the CIRP reader/writer.
 * @ingroup chirp */
class ChirpFormat
{
protected:
  enum class Duplex {
    None, Positive, Negative, Split, Off
  };

  enum class Mode {
    FM, NFM, WFM, AM, DV, DN
  };

  enum class ToneMode {
    None, Tone, TSQL, TSQL_R, DTCS, DTCS_R, Cross
  };

  enum class Polarity {
    Normal, Reversed
  };

  enum class CrossMode {
    NoneTone, NoneDTCS, ToneNone, ToneTone, ToneDTCS, DTCSNone, DTCSTone, DTCSDTCS
  };

protected:
  static const QSet<QString> _mandatoryHeaders;
  static const QSet<QString> _knownHeaders;
  static const QHash<QString, Duplex> _duplexCodes;
  static const QHash<QString, Mode> _modeCodes;
  static const QHash<QString, ToneMode> _toneModeCodes;
  static const QHash<QString, CrossMode> _crossModes;

};


/** Implements the CHIRP CSV reader.
 @ingroup chirp */
class ChirpReader: public ChirpFormat
{

public:
  static bool read(QTextStream &stream, Config *config, const ErrorStack &err=ErrorStack());

protected:
  static bool readLine(QTextStream &stream, QStringList &list, const ErrorStack &err=ErrorStack());

  static bool processLine(const QStringList &header, const QStringList &line,
                          Config *config, const ErrorStack &err=ErrorStack());

  static bool processDuplex(const QString &code, Duplex &duplex, const ErrorStack &err=ErrorStack());
  static bool processMode(const QString &code, Mode &mode, const ErrorStack &err=ErrorStack());
  static bool processToneMode(const QString &code, ToneMode &mode, const ErrorStack &err=ErrorStack());
  static bool processPolarity(const QString &code, Polarity &txPol, Polarity &rxPol, const ErrorStack &err=ErrorStack());
  static bool processCrossMode(const QString &code, CrossMode &crossMode, const ErrorStack &err = ErrorStack());
};


/** Implements the chirp writer. */
class ChirpWriter: public ChirpFormat
{
public:
  static bool write(QTextStream &stream, Config *config, const ErrorStack &err=ErrorStack());

protected:
  static bool writeHeader(QTextStream &stream, const ErrorStack &err = ErrorStack());
  static bool writeChannel(QTextStream &stream, int i, FMChannel *channel, const ErrorStack &err=ErrorStack());
  static bool encodeFrequency(QTextStream &stream, FMChannel *channel, const ErrorStack &err = ErrorStack());
  static bool encodeSubTone(QTextStream &stream, FMChannel *channel, const ErrorStack &err = ErrorStack());
  static bool encodeBandwidth(QTextStream &stream, FMChannel *channel, const ErrorStack &err = ErrorStack());
};


#endif // CHIRPFORMAT_HH
