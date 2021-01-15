#ifndef CSVREADER_HH
#define CSVREADER_HH

#include <QObject>
#include <QTextStream>
#include <QMap>
#include <QVector>

#include "channel.hh"
#include "contact.hh"

class Config;
class DigitalContact;
class RXGroupList;
class Zone;
class GPSSystem;
class ScanList;

/// @cond with_internal_docs

/** The lexer class divides a text stream into tokens. */
class CSVLexer: public QObject
{
  Q_OBJECT

public:
  /** The token. */
  typedef struct {
  public:
    /** Possible token types. */
    typedef enum {
      T_KEYWORD,         ///< A Keyword/Identifier.
      T_STRING,          ///< A quoted string.
      T_NUMBER,          ///< An integer or floating point number.
      T_DCS_N,           ///< A normal DCS code number.
      T_DCS_I,           ///< An inverted DCS code number.
      T_COLON,           ///< A colon.
      T_NOT_SET,         ///< A dash, being used as "not-set".
      T_ENABLED,         ///< A plus sign, being used as "enabled"
      T_COMMA,           ///< A comma

      T_WHITESPACE,      ///< Any whitespace character excluding newline.
      T_NEWLINE,         ///< A new line.
      T_COMMENT,         ///< A comment starts with # end ends at the line-end.

      T_END_OF_STREAM,   ///< Indicates the end-of-input.
      T_ERROR            ///< Indicates a lexer error.
    } TokenType;

    /// The token type.
    TokenType type;
    /// The token value.
    QString value;
    /// Line number.
    qint64 line;
    /// Column number.
    qint64 column;
  } Token;

  /// Current state of lexer.
  typedef struct {
    /// The current stream offset.
    qint64 offset;
    /// The current line count.
    qint64 line;
    /// The current column number.
    qint64 column;
  } State;

public:
  /** Constructs a lexer for the given stream. */
  CSVLexer(QTextStream &stream, QObject *parent=nullptr);

  /** Saves the current lexer state. */
  void push();
  /** Restores the last lexer state. */
  void pop();
  /** Reads the next token from the stream. */
  Token next();

  const QString &errorMessage() const;

protected:
  /** Internal used function to get the next token. Also returns ignored tokens like whitespace
   * and comment. */
  Token lex();

protected:
  /// The error message.
  QString _errorMessage;
  /// The text stream to read from.
  QTextStream &_stream;
  /// The stack of saved lexer states
  QVector<State> _stack;
  /// The current line count
  QString _currentLine;
  /// The list of patterns to match
  static QVector< QPair<QRegExp, Token::TokenType> > _pattern;
};

/// @endcond


/** Basic parse-handler interface.
 *
 * That is, a set of callbacks getting called by the parser on the occurence of a particular
 * statement in the config file.
 * @ingroup conf */
class CSVHandler: public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit CSVHandler(QObject *parent=nullptr);

public:
  /** Destructor. */
  virtual ~CSVHandler();

  /** Gets called once the DMR ID has been parsed. */
  virtual bool handleRadioId(qint64 id, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once the radio name has been parsed. */
  virtual bool handleRadioName(const QString &name, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once the first intro line has been parsed. */
  virtual bool handleIntroLine1(const QString &text, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once the second intro line has been parsed. */
  virtual bool handleIntroLine2(const QString &text, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once the MIC level has been parsed. */
  virtual bool handleMicLevel(uint level, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once the Speech flag has been parsed. */
  virtual bool handleSpeech(bool speech, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once the UserDB flag has been parsed. */
  virtual bool handleUserDB(bool userdb, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once a DTMF contact has been parsed. */
  virtual bool handleDTMFContact(qint64 idx, const QString &name, const QString &num, bool rxTone,
                                 qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once a digital contact has been parsed. */
  virtual bool handleDigitalContact(qint64 idx, const QString &name, DigitalContact::Type type, qint64 id,
                                    bool rxTone, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once an RX group list has been parsed. */
  virtual bool handleGroupList(qint64 idx, const QString &name, const QList<qint64> &contacts,
                               qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once a digital channel has been parsed. */
  virtual bool handleDigitalChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, DigitalChannel::Admit admit, qint64 color, DigitalChannel::TimeSlot slot,
      qint64 gl, qint64 contact, qint64 gps, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once a analog channel has been parsed. */
  virtual bool handleAnalogChannel(qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, Signaling::Code rxTone, Signaling::Code txTone,
      AnalogChannel::Bandwidth bw, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once a zone list has been parsed. */
  virtual bool handleZone(qint64 idx, const QString &name, bool a, const QList<qint64> &channels,
                          qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once a GPS system has been parsed. */
  virtual bool handleGPSSystem(qint64 idx, const QString &name, qint64 contactIdx, qint64 period,
                               qint64 revertChannelIdx, qint64 line, qint64 column, QString &errorMessage);
  /** Gets called once a scan list has been parsed. */
  virtual bool handleScanList(qint64 idx, const QString &name, qint64 pch1, qint64 pch2, qint64 txch,
                              const QList<qint64> &channels, qint64 line, qint64 column, QString &errorMessage);
};


/** The actual config file parser.
 *
 * This class parses the config file and calls the associated callback functions of a handler
 * instance that is responsible to assemble the final @c Config instance. */
class CSVParser: public QObject
{
  Q_OBJECT

public:
  /** Constructs a parser using the given handler instance. */
  explicit CSVParser(CSVHandler *handler, QObject *parent=nullptr);

  /** Parses the given text stream. */
  bool parse(QTextStream &stream);

  /** Returns the current error message, for example if @c parse returns @c false. */
  const QString &errorMessage() const;

protected:
  /** Internal function to parse DMR IDs. */
  bool _parse_radio_id(CSVLexer &lexer);
  /** Internal function to parse radio names. */
  bool _parse_radio_name(CSVLexer &lexer);
  /** Internal function to parse intro line 1. */
  bool _parse_introline1(CSVLexer &lexer);
  /** Internal function to parse intro line 2. */
  bool _parse_introline2(CSVLexer &lexer);
  /** Internal function to parse MIC level. */
  bool _parse_mic_level(CSVLexer &lexer);
  /** Internal function to parse Speech flag. */
  bool _parse_speech(CSVLexer &lexer);
  /** Internal function to parse UserDB flag. */
  bool _parse_userdb(CSVLexer &lexer);
  /** Internal function to parse a digital contact list. */
  bool _parse_contacts(CSVLexer &lexer);
  /** Internal function to parse digital contact. */
  bool _parse_contact(qint64 id, CSVLexer &lexer);
  /** Internal function to parse an RX group list. */
  bool _parse_rx_groups(CSVLexer &lexer);
  /** Internal function to parse an RX group. */
  bool _parse_rx_group(qint64 id, CSVLexer &lexer);
  /** Internal function to parse a digital channel list. */
  bool _parse_digital_channels(CSVLexer &lexer);
  /** Internal function to parse a digital channel. */
  bool _parse_digital_channel(qint64 id, CSVLexer &lexer);
  /** Internal function to parse an analog channel list. */
  bool _parse_analog_channels(CSVLexer &lexer);
  /** Internal function to parse an analog channel. */
  bool _parse_analog_channel(qint64 id, CSVLexer &lexer);
  /** Internal function to parse a zone list. */
  bool _parse_zones(CSVLexer &lexer);
  /** Internal function to parse a zone. */
  bool _parse_zone(qint64 id, CSVLexer &lexer);
  /** Internal function to parse a GPS system list. */
  bool _parse_gps_systems(CSVLexer &lexer);
  /** Internal function to parse a GPS system. */
  bool _parse_gps_system(qint64 id, CSVLexer &lexer);
  /** Internal function to parse a scanlist list. */
  bool _parse_scanlists(CSVLexer &lexer);
  /** Internal function to parse a scanlist. */
  bool _parse_scanlist(qint64 id, CSVLexer &lexer);

protected:
  /** Holds the current error message. */
  QString _errorMessage;
  /** The handler instance. */
  CSVHandler *_handler;
};



/** Implements the text-file codeplug reader.
 * @ingroup conf */
class CSVReader : public CSVHandler
{
	Q_OBJECT

protected:
  /** Hidden constructor. Consider using the static @c read method. */
  CSVReader(Config *config, QObject *parent=nullptr);

public:
  /** Reads a config file from @c stream and stores the read configuration into @c config.
   * @returns @c true on success. */
  static bool read(Config *config, QTextStream &stream, QString &errorMessage);

  virtual bool handleRadioId(qint64 id, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleRadioName(const QString &name, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleIntroLine1(const QString &text, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleIntroLine2(const QString &text, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleMicLevel(uint level, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleSpeech(bool speech, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleUserDB(bool userdb, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleDTMFContact(qint64 idx, const QString &name, const QString &num, bool rxTone,
                                 qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleDigitalContact(
      qint64 idx, const QString &name, DigitalContact::Type type, qint64 id, bool rxTone,
      qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleGroupList(qint64 idx, const QString &name, const QList<qint64> &contacts,
                               qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleDigitalChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, DigitalChannel::Admit admit, qint64 color, DigitalChannel::TimeSlot slot,
      qint64 gl, qint64 contact, qint64 gps, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleAnalogChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, Signaling::Code rxTone, Signaling::Code txTone,
      AnalogChannel::Bandwidth bw, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleZone(qint64 idx, const QString &name, bool a, const QList<qint64> &channels,
                          qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleGPSSystem(qint64 idx, const QString &name, qint64 contactIdx, qint64 period,
                               qint64 revertChannelIdx, qint64 line, qint64 column, QString &errorMessage);
  virtual bool handleScanList(qint64 idx, const QString &name, qint64 pch1, qint64 pch2, qint64 txch,
                              const QList<qint64> &channels, qint64 line, qint64 column, QString &errorMessage);

protected:
  /** If @c true, the reader is in "link" mode. */
  bool _link;
  /** The configuration to read. */
  Config *_config;
  /** Index <-> Channel map. */
	QMap<int, Channel *> _channels;
  /** Index <-> Digital contact map. */
  QMap<int, DigitalContact *> _digital_contacts;
  /** Index <-> RX group list map. */
  QMap<int, RXGroupList *> _rxgroups;
  /** Index <-> Zone map. */
  QMap<int, Zone *> _zones;
  /** Index <-> GPS System map. */
  QMap<int, GPSSystem *> _gpsSystems;
  /** Index <-> Scan list map. */
  QMap<int, ScanList *> _scanlists;
};

#endif // CSVREADER_HH
