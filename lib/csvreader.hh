/** @defgroup conffile Config-file format
 *
 * The config file format is a text file format to describe the generic code-plug (a.k.a.,
 * @c Config class). This format is aimed at being human-readable and allows to write
 * codeplugs by hand (e.g., using a text editor). This section describes that format as well as all
 * classes used to read and write that file format.
 */

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
class ScanList;

/** The lexer class divides a text stream into tokens.
 * @ingroup conffile */
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
      T_NUMBER,          ///< A integer or floating point number.
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

protected:
  /** Internal used function to get the next token. Also returns ignored tokens like whitespace
   * and comment. */
  Token lex();

protected:
  /// The text stream to read from.
  QTextStream &_stream;
  /// The stack of saved lexer states
  QVector<State> _stack;
  /// The current line count
  QString _currentLine;
  /// The list of patterns to match
  static QVector< QPair<QRegExp, Token::TokenType> > _pattern;
};


/** Basic parse-handler interface.
 *
 * That is, a set of callbacks getting called by the parser on the occurence of a particular
 * statement in the config file.
 *
 * @ingroup conffile */
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
  virtual bool handleRadioId(qint64 id, qint64 line, qint64 column);
  /** Gets called once the radio name has been parsed. */
  virtual bool handleRadioName(const QString &name, qint64 line, qint64 column);
  /** Gets called once the first intro line has been parsed. */
  virtual bool handleIntroLine1(const QString &text, qint64 line, qint64 column);
  /** Gets called once the second intro line has been parsed. */
  virtual bool handleIntroLine2(const QString &text, qint64 line, qint64 column);
  /** Gets called once the VOX level has been parsed. */
  virtual bool handleVoxLevel(uint level, qint64 line, qint64 column);
  /** Gets called once the MIC level has been parsed. */
  virtual bool handleMicLevel(uint level, qint64 line, qint64 column);
  /** Gets called once a digital contact has been parsed. */
  virtual bool handleDigitalContact(qint64 idx, const QString &name, DigitalContact::Type type, qint64 id,
                                    bool rxTone, qint64 line, qint64 column);
  /** Gets called once an RX group list has been parsed. */
  virtual bool handleGroupList(qint64 idx, const QString &name, const QList<qint64> &contacts,
                               qint64 line, qint64 column);
  /** Gets called once a digital channel has been parsed. */
  virtual bool handleDigitalChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, DigitalChannel::Admit admit, qint64 color, DigitalChannel::TimeSlot slot,
      qint64 gl, qint64 contact, qint64 line, qint64 column);
  /** Gets called once a analog channel has been parsed. */
  virtual bool handleAnalogChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, float rxTone, float txTone,
      AnalogChannel::Bandwidth bw, qint64 line, qint64 column);
  /** Gets called once a zone list has been parsed. */
  virtual bool handleZone(qint64 idx, const QString &name, const QList<qint64> &channels,
                          qint64 line, qint64 column);
  /** Gets called once a scan list has been parsed. */
  virtual bool handleScanList(qint64 idx, const QString &name, qint64 pch1, qint64 pch2, qint64 txch,
                              const QList<qint64> &channels, qint64 line, qint64 column);
};


/** The actual config file parser.
 *
 * This class parses the config file and calls the associated callback functions of a handler
 * instance that is responsible to assemble the final @c Config instance.
 *
 * @ingroup conffile */
class CSVParser: public QObject
{
  Q_OBJECT

public:
  /** Constructs a parser using the given handler instance. */
  explicit CSVParser(CSVHandler *handler, QObject *parent=nullptr);

  /** Parses the given text stream. */
  bool parse(QTextStream &stream);

protected:
  /** Internal function to parse DMR IDs. */
  bool _parse_radio_id(CSVLexer &lexer);
  /** Internal function to parse radio names. */
  bool _parse_radio_name(CSVLexer &lexer);
  /** Internal function to parse intro line 1. */
  bool _parse_introline1(CSVLexer &lexer);
  /** Internal function to parse intro line 2. */
  bool _parse_introline2(CSVLexer &lexer);
  /** Internal function to parse VOX level. */
  bool _parse_vox_level(CSVLexer &lexer);
  /** Internal function to parse MIC level. */
  bool _parse_mic_level(CSVLexer &lexer);
  /** Internal function to parse a digital contact list. */
  bool _parse_digital_contacts(CSVLexer &lexer);
  /** Internal function to parse digital contact. */
  bool _parse_digital_contact(qint64 id, CSVLexer &lexer);
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
  /** Internal function to parse a scanlist list. */
  bool _parse_scanlists(CSVLexer &lexer);
  /** Internal function to parse a scanlist. */
  bool _parse_scanlist(qint64 id, CSVLexer &lexer);

protected:
  /** The handler instance. */
  CSVHandler *_handler;
};


/** Implements the text-file codeplug reader.
 * @ingroup conffile */
class CSVReader : public CSVHandler
{
	Q_OBJECT

protected:
  /** Hidden constructor. Consider using the static @c read method. */
  CSVReader(Config *config, QObject *parent=nullptr);

public:
  /** Reads a config file from @c stream and stores the read configuration into @c config.
   * @returns @c true on success. */
	static bool read(Config *config, QTextStream &stream);

  virtual bool handleRadioId(qint64 id, qint64 line, qint64 column);
  virtual bool handleRadioName(const QString &name, qint64 line, qint64 column);
  virtual bool handleIntroLine1(const QString &text, qint64 line, qint64 column);
  virtual bool handleIntroLine2(const QString &text, qint64 line, qint64 column);
  virtual bool handleVoxLevel(uint level, qint64 line, qint64 column);
  virtual bool handleMicLevel(uint level, qint64 line, qint64 column);
  virtual bool handleDigitalContact(
      qint64 idx, const QString &name, DigitalContact::Type type, qint64 id, bool rxTone,
      qint64 line, qint64 column);
  virtual bool handleGroupList(qint64 idx, const QString &name, const QList<qint64> &contacts,
                               qint64 line, qint64 column);
  virtual bool handleDigitalChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, DigitalChannel::Admit admit, qint64 color, DigitalChannel::TimeSlot slot,
      qint64 gl, qint64 contact, qint64 line, qint64 column);
  virtual bool handleAnalogChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, float rxTone, float txTone,
      AnalogChannel::Bandwidth bw, qint64 line, qint64 column);
  virtual bool handleZone(qint64 idx, const QString &name, const QList<qint64> &channels,
                          qint64 line, qint64 column);
  virtual bool handleScanList(qint64 idx, const QString &name, qint64 pch1, qint64 pch2, qint64 txch,
                              const QList<qint64> &channels, qint64 line, qint64 column);

protected:
  /** If @c true, the reader is in "link" mode. */
  bool _link;
  /** The configuration to read. */
  Config *_config;
  /** Index <-> Channel map. */
	QMap<int, Channel *> _channels;
  /** Index <-> Contact map. */
  QMap<int, DigitalContact *> _contacts;
  /** Index <-> RX group list map. */
  QMap<int, RXGroupList *> _rxgroups;
  /** Index <-> Zone map. */
  QMap<int, Zone *> _zones;
  /** Index <-> Scan list map. */
  QMap<int, ScanList *> _scanlists;
};

#endif // CSVREADER_HH
