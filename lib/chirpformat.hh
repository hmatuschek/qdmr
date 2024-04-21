#ifndef CHIRPFORMAT_HH
#define CHIRPFORMAT_HH

#include "errorstack.hh"
#include <QSet>

class QTextStream;
class Config;
class QStringList;


/** Some common constants for the CIRP reader/writer. */
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
    None, Tone, TSQL, DTCS, Cross
  };

  enum class Polarity {
    Normal, Reversed
  };

protected:
  static const QSet<QString> _mandatoryHeaders;
  static const QSet<QString> _knownHeaders;
  static const QHash<QString, Duplex> _duplexCodes;
  static const QHash<QString, Mode> _modeCodes;
  static const QHash<QString, ToneMode> _toneModeCodes;
};


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
};

#endif // CHIRPFORMAT_HH
