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
  /** Possible values for the "duplex" column. Specifies the frequency offset direction for the
   *  transmit frequency wrt to the receive frequency. */
  enum class Duplex {
    None,      ///< No offset at all. That is, the TX and RX frequencies are equal.
    Positive,  ///< Positive frequency offset, TX above RX frequency.
    Negative,  ///< Negative frequency offset, TX below RX frequency.
    Split,     ///< Explicit transmit frequency.
    Off        ///< No transmit frequency specified. Channel is RX only.
  };

  /** Possible CHIRP channel modes. */
  enum class Mode {
    FM,  ///< 25kHz FM (still NBFM).
    NFM, ///< 12.5kHz FM (also NBFM).
    WFM, ///< 100kHz FM (WBFM, broadcast).
    AM,  ///< AM, usually airband (not supported by qdmr yet).
    DV,  ///< D-STAR (not supported by qdmr yet).
    DN   ///< SystemFusion (not supported by qdmr yet).
  };

  /** Possible modes for transmitting and processing sub tones. */
  enum class ToneMode {
    None,    ///< No transmission of subtones.
    Tone,    ///< Transmission of a CTCSS tone.
    TSQL,    ///< CTCSS tone squelch.
    TSQL_R,  ///< Inverted, CTCSS tone squelch. That is, the squelch opens if a specific CTCSS tone is not received.
    DTCS,    ///< Transmission of a DCS code and also DCS squelch.
    DTCS_R,  ///< Transmission of a DCS code and also DCS squelch.
    Cross    ///< More complex setting (see @c CrossMode).
  };

  /** Polarity of DCS codes. */
  enum class Polarity {
    Normal,   ///< Normal DCS encoding.
    Reversed  ///< Reversed DCS encoding.
  };

  /** Generic mode for sub tones. There is no reason to use any other mode. This one covers
   * everything. */
  enum class CrossMode {
    NoneTone,   ///< No TX, RX CTCSS
    NoneDTCS,   ///< No TX, RX DCS
    ToneNone,   ///< TX CTCSS, no RX
    ToneTone,   ///< TX CTCSS, RX CTCSS
    ToneDTCS,   ///< TX CTCSS, RX DCS
    DTCSNone,   ///< TX DCS, no RX
    DTCSTone,   ///< TX DCS, RX CTCSS
    DTCSDTCS    ///< TX DCS, RX DCS
  };

protected:
  /** Internal set of mandatory headers. */
  static const QSet<QString> _mandatoryHeaders;
  /** Internal used set of known headers. */
  static const QSet<QString> _knownHeaders;
  /** Mapping of duplex codes. */
  static const QHash<QString, Duplex> _duplexCodes;
  /** Mapping of mode codes. */
  static const QHash<QString, Mode> _modeCodes;
  /** Mapping of tone mode codes. */
  static const QHash<QString, ToneMode> _toneModeCodes;
  /** Mapping of cross mode codes. */
  static const QHash<QString, CrossMode> _crossModes;
};


/** Implements the CHIRP CSV reader.
 * @ingroup chirp */
class ChirpReader: public ChirpFormat
{

public:
  /** Reads a CHIRP CSV file from the given stream and updates the given configuration.
   * Please note, that the CHRIP generic CSV does not contain a functional DMR codeplug. */
  static bool read(QTextStream &stream, Config *config, const ErrorStack &err=ErrorStack());

protected:
  /** Internal used method to read a line from the given stream.
   * This method also implements the proper quotation parsing of strings. */
  static bool readLine(QTextStream &stream, QStringList &list, const ErrorStack &err=ErrorStack());

  /** Line parser, the header must be read before and passed to this method. The parsed channel is
   * added to the configuration. */
  static bool processLine(const QStringList &header, const QStringList &line,
                          Config *config, const ErrorStack &err=ErrorStack());

  /** Helper function to parse a duplex column. */
  static bool processDuplex(const QString &code, Duplex &duplex, const ErrorStack &err=ErrorStack());
  /** Helper function to parse a mode column. */
  static bool processMode(const QString &code, Mode &mode, const ErrorStack &err=ErrorStack());
  /** Helper function to parse a tone mode column. */
  static bool processToneMode(const QString &code, ToneMode &mode, const ErrorStack &err=ErrorStack());
  /** Helper function to parse a polarity column. */
  static bool processPolarity(const QString &code, Polarity &txPol, Polarity &rxPol, const ErrorStack &err=ErrorStack());
  /** Helper function to parse a cross mode column. */
  static bool processCrossMode(const QString &code, CrossMode &crossMode, const ErrorStack &err = ErrorStack());
};


/** Implements the chirp writer.
 * @ingroup chirp */
class ChirpWriter: public ChirpFormat
{
public:
  /** Writes the (FM channels) of the given codeplug as a CHIRP CSV file into the given stream. */
  static bool write(QTextStream &stream, Config *config, const ErrorStack &err=ErrorStack());

protected:
  /** Writes the header into the given stream. */
  static bool writeHeader(QTextStream &stream, const ErrorStack &err = ErrorStack());
  /** Writes a channel into the given stream. */
  static bool writeChannel(QTextStream &stream, int i, FMChannel *channel, const ErrorStack &err=ErrorStack());
  /** Writes frequency related columns to the given stream. */
  static bool encodeFrequency(QTextStream &stream, FMChannel *channel, const ErrorStack &err = ErrorStack());
  /** Wirtes sub tone related columns to the given stream. */
  static bool encodeSubTone(QTextStream &stream, FMChannel *channel, const ErrorStack &err = ErrorStack());
  /** Writes the bandwidth column to the given stream */
  static bool encodeBandwidth(QTextStream &stream, FMChannel *channel, const ErrorStack &err = ErrorStack());
};


#endif // CHIRPFORMAT_HH
