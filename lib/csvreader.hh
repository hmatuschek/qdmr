/** @defgroup conffile Config-file format
 * @ingroup conf
 *
 * The config file format is a text-file format to describe generic code-plugs (a.k.a.,
 * the @c Config class). This format is aimed at being human-readable and allows to write
 * codeplugs by hand (e.g., using a text editor). This section describes this format, as well as all
 * classes used to read and write that file format.
 *
 * The most convenient way of reading and writing config files, however, is to use the
 * @c Config::readCSV and @c Config::writeCSV methods of the @c Config class. For example,
 * @code
 * // Read config file
 * Config cfg;
 * if (! cfg.readCSV("FILENAME")) {
 *   // Handle error...
 * }
 * @endcode
 *
 * @section confform Configuration text file format
 * As mentioned above, the configuration file represents a generic configuration (the @c Config
 * class) for a wide varity of radios. It is a simple text file containtin simple key-value
 * definitions like the DMR ID as well as tables like the table of channels, contacts, etc.
 *
 * The aim of this config format is to be human-readable and writable. This would allow
 * users to write config file by hand and share them easily, as well as enable users to modify shared
 * configurations using a text editor. To this end, the format must be intuitive and to some
 * degree self-documenting.
 *
 * Within the following sections, I will describe that text format in some detail.
 *
 * @subsection confcom Line comments
 * To document your configuration, you may use so-called line-comments. These comments start with the
 * character '#' and end at the end-of-line.
 * @code{.py}
 * # A comment, everything in this line is ignored
 * ID: 12345678   # Another comment
 * @endcode
 *
 * @subsection confgen General configuration
 * The general configuration settings of some radios can be overly complex with a huge amount of
 * options. The vast majority of these settings, however, are useless for hamradio purposes. Thus
 * the possible settings for the general configuration of the radio are reduced to 6 key-value
 * pairs.
 *
 * The DMR ID of cause, is absolutely neccessary and specifies your personal DMR number. Keep in
 * mind, that you do NOT need to get a unique DMR ID for each radio you own! All your radios can
 * share the same DMR ID. The DMR ID is specified using the "ID" keyword as
 * @code{.py}
 * ID: 12345678
 * @endcode
 *
 * The radio name is a string, that the radio may display somewhere on the screen. It does not have
 * any effect on the behavior of the radio or gets transmitted. You may set this entry to your
 * callsign. For example:
 * @code{.py}
 * Name: "DM3MAT"
 * @endcode
 *
 * The two intro lines might be shown on the screen of your radio on startup. You may set these
 * to any string you like. They are also cosmetic and don't have any effect on the behavior of
 * your radio. For example
 * @code{.py}
 * IntroLine1: "Hello"
 * IntroLine2: "MY0CALL"
 * @endcode
 *
 * The microphone sensitivity/amplification can also be set (on some radios) using the MicLevel
 * entry. This entry is also a number between 1 and 10. The larger the level the larger the
 * microphone amplification. This value may vary heavily from model to model.
 * @code{.py}
 * MicLevel: 2
 * @endcode
 * The "Speech" option enables the speech synthesis of the radio if supported. Possible settings
 * are "on" and "off".
 * @code{.py}
 * # Speech-synthesis ('On' or 'Off'):
 * Speech: Off
 * @endcode
 *
 * @subsection confcont Contact table
 * The contact table is a list of DMR contacts like
 * @code
 * Contact Name             Type    ID       RxTone
 * 1       "DM3MAT"         Private 2621370  +
 * 2       "DMR All Call"   All     16777215 -
 * 3       "Simplex TG99"   Group   99       -
 * 4       "Global"         Group   91       -
 * 5       "EU"             Group   92       -
 * 6       "Regional"       Group   8        -
 * 7       "Lokal"          Group   9        -
 * 8       "Deutschland"    Group   262      -
 * 9       "Berlin/Brand"   Group   2621     -
 * 10      "Berlin City"    Group   26212    -
 * 11      "Brandenburg"    Group   26209    -
 * 12      "Sachs/Thu"      Group   2629     -
 * 13      "R Brandenburg"  Private 4044     -
 * @endcode
 * These contacts can be personal contacts like DM3MAT, so-called all-calls and group calls.
 * The contact table starts with the "Contact" keyword and ends with an empty line. The remaining
 * keywords ("Name", "Type", "ID", "RxTone") are ignored, however, they are part of the
 * self-documentation of the config file.
 *
 * Following the "Contact" keyword, each line
 * represents a single contact in the contact list. The first column represents a unique
 * internal ID for the contact. It must not necessarily be in ascending order, any unique number
 * will do. The second column is the name of the contact. Any string can be used here. The third
 * column specifies the type of the contact. This must be one of the keywords "Private", "Group"
 * or "All", meaning private, group or all-calls, respectively. The fourth column specifies the DMR ID
 * for the contact. Please note, that an all-call requires the specific DMR ID 16777215 to work
 * as an all-call. The last colum specifies, whether an incomming call from this contact will cause a
 * ring-tone. Here "+" means enabled/yes and "-" disabled/no.
 *
 * @subsection confrxgr RX group list table
 * RX group lists are simple named lists of one ore more contacts. These lists may include group,
 * all or even private calls. RX group lists are assigned to channels. They form a group of
 * contacts (e.g., talk groups) you may want to listen to on a particular channel. Usually these
 * group lists form a collection of talk groups that are specific for a particular region.
 *
 * RX group lists are defined within the config file like
 * @code
 * Grouplist Name            Contacts
 * 1         "Lokal"          7,9
 * 2         "Deutschland"    8
 * 3         "Global"         4
 * 4         "EU"             5
 * 5         "Sachs/Thu"      13,12
 * 6         "Simplex"        2,3,6,7
 * 7         "Berlin/Brand"   9,10,11,13
 * @endcode
 *
 * The group list table starts with the keyword "Grouplist". The following keywords (Name &
 * Contacts) are ignored, but form a kind of self-documentation for the config file.
 *
 * Following the "Grouplist" keyword, each RX group list is defined by a single line. The first
 * column specifies the internal unique ID for the group list. This can be any number as long as
 * it is unique. The second column contains the name of the group list as a string. This can be
 * any non-empty string. The third column contains the commma-separated list of contact IDs that form
 * that group list.
 *
 * @subsection confdig Digital channel table
 * The digital channel table defines all digital DMR channels. As digital channels have some
 * different options compared to analog channels, they are not defined within the same table.
 * However, they share the same IDs. So be careful not to assign the same identifier to analog and
 * digital channels.
 *
 * The digital channel table has the form
 * @code
 * Digital Name             Receive   Transmit  Power Scan TOT RO Admit  CC TS RxGL TxC GPS
 * 11      "DM0TT Ref"      439.0870  -7.6000   High  1    -   -  Free   1  1  7    12  -  #    Regional
 * 12      "DM0TT BB"       439.0875  -7.6000   High  -    -   -  Free   1  2  7    15  1  #    Berlin/Brand
 * 84      "DMR_S0"         433.4500  433.4500  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * 85      "DMR_S1"         433.6120  433.6120  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * 86      "DMR_S2"         433.6250  433.6250  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * 87      "DMR_S3"         433.6380  433.6380  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * 88      "DMR_S4"         433.6500  433.6500  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * 89      "DMR_S5"         433.6630  433.6630  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * 90      "DMR_S6"         433.6750  433.6750  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * 91      "DMR_S7"         433.6880  433.6880  High  2    -   -  Free   1  1  6    9   -  #    Simplex TG99
 * @endcode
 * The digital-channel table starts with the keyword "Digital" and ends with an empty line. The
 * next keywords (Name, Receive, Transmit, Power, Scan, TOT, RO, Admit, CC, TS, RxGL and
 * TxC) are ignored and are maintained for the self-documentation of the configuraion file.
 *
 * Each channel is defined within a single line. The first column is the unique channel
 * identifier (any unique number among analog AND digital channels). The second column specifies the
 * channel name as a string. The third column specifies the RX frequency in MHz and the fourth
 * column the TX frequency in MHz. Alternatively, a TX frequency can also be specified in terms of
 * an offset relative to the RX frequency. In this case, the offset must be prefixed with either
 * "+" or "-". The 5th (Power) column specifies the power level to use. Here, either the "High" or "Low"
 * keyword must be used. The 6th (Scan) column specifies the ID of the scanlist (see below) attached to
 * the channel. This list will be used whenever a scan is started on this channel. The 7th column
 * (TOT) column speifies the TX time-out-timer in seconds or "-", if disabled. The 8th column
 * (RO) specifies whether the channel is RX only ("+") or not ("-"). If enabled, you cannot transmit
 * on that particular channel. The 9th (Admit) colum specifies the TX admit criterion for the
 * channel. This must be either "-" or one of the keywords "Free" and "Color". "-" indicates that
 * there is no restriction in transmitting on that channel. The radio will transmit whenever PTT is
 * pressed. The "Free" keyword indicates that the radio will only transmit if the channel is free.
 * The "Color" keyword indicates that the radio will only transmit if the channel is free and the
 * colorcode of the repeater matches the specified color-code of the channel (see next column). The
 * 10th column specifies the colorcode of the channel. The 10th (CC) column specifies the
 * color-code of the channel. To avoid interference between neighbouring radios and repreaters on
 * the same frequency (in case of DX conditions), the repeater and radio will only react to
 * tranmissions on a channel with the matching color-code. The color-code can be any number between
 * 0 and 15. The 11th (TS) column specifies the time-slot for this channel. Due to the audio
 * compression used in DMR, it is possible to operate two independent channels on a single
 * frequency by using time-sliceing. DMR uses two time-slots. This option specifies which of the
 * two time-slots is used for the channel. On simplex channels, this time-sliceing is irrelevant, as
 * there is no central instance (the repeater) that defines what time-slot 1 or 2 is. The 12th (GPS)
 * column specifies the GPS system (see below) to use on that channel.
 *
 * @subsection confana Analog channel table
 * The analog channel table collects all analog (FM) channels. As digital channels have some
 * different options compared to analog channels, they are not defined within the same table.
 * However, they share the same IDs. So be careful not to assign the same identifier to analog and
 * digital channels.
 *
 * The analog channel table has the form
 * @code {.py}
 * Analog  Name             Receive   Transmit Power Scan TOT RO Admit  Squelch RxTone TxTone Width
 * 1       "Y07"            144.6750  144.6750  High  1    -   -  Free   1       -      -      12.5
 * 2       "S20"            145.3000  145.3000  High  -    -   -  Free   1       -      -      12.5
 * 3       "Mobil 2m"       145.5000  145.5000  High  -    -   -  Free   1       -      -      12.5
 * 4       "DB0RAG"         439.3000  -7.6000   High  1    -   -  Free   3       -      -      12.5
 * 5       "DB0LOS"         438.7750  -7.6000   High  1    -   -  Free   1       -      -      12.5
 * 6       "DB0LUD"         438.5750  -7.6000   High  1    -   -  Free   1       -      67     12.5
 * 19      "DB0BLO"         439.2750  -7.6000   High  1    -   -  Free   3       -      -      12.5
 * 20      "DB0SP-2"        145.6000  -0.6000   High  1    -   -  Free   3       -      -      12.5
 * 21      "DB0SP-70"       439.4250  -7.6000   High  1    -   -  Free   3       -      -      12.5
 * @endcode
 *
 * The analog channel table starts with the "Analog" keyword and ends with an empty line. The
 * remaining keywords right after "Analog" (i.e., "Name", "Receive", "Transmit", "Power", "Scan",
 * "TOT", "RO", "Admit", "Squelch", "RxTone", "TxTone" and "Width") are ignored but are part of the
 * self-documentation of the config file.
 *
 * Each line within the table specifies a single channel. The first column specifies the unique ID
 * of the channel. This ID can by any number that is unique among analog AND digital channels. The
 * second (Name) column specifies the name of the channel as a string. Any string can be used here.
 * The third (Receive) column specifies the RX frequency of the channel in MHz. The fourth
 * (Transmit) column specifies the TX frequency in MHz or alternatively, an offset relative to the
 * receive frequency in MHz by prefixing "+" or "-". The 5th (Power) column specifies the transmit
 * power. This must be either the "High" or "Low" keyword. The 6th (Scan) column specifies the
 * scanlist ID for this channel or "-" if there is no scanlist assigned to the channel. A scanlist
 * (see below) is just a collection of channels that gets scanned whenever scanning is started on a
 * particular channel. The 7th (TOT) column specifies the transmit time-out in seconds or "-" if
 * disabled. The 8th (RO) column specifies whether this channel is receive-only with either "-"
 * meaing disabled and "+" enabled. If enabled, it is impossible to transmit on that channel. The
 * 9th column specifies the admit criterion on that channel. This must be either "-" meaning that
 * there is no restriction when to send on that channel, the keyword "Free" meaning that the channel
 * must be free to transmit or the keyword "Tone" meaning that the channel must be free and the
 * RxTone must match. The 10th (Squelch) column specifies the squelch level for the channel. This
 * must be a number between [0-10]. The larger the value, the stronger the signal must be to open
 * the squelch. The value 0 disables the squelch. The 11th (RxTone) specifies the receive CTCSS tone
 * frequency in Hz. The quelch will then only open, if the signal is strong enough (see previous
 * column) and the specified tone is received. If set "-" the RX tone is disbled and the squelch
 * will open if the signal is strong enough. The 12th (TxTone) column specifies the CTCSS tone
 * to transmit in Hz or "-" if disabled. This feature is used by some repeaters to open their
 * squelch and to start repeating to avoid conflicts between repeaters operating on the same
 * frequency (e.g., in case of DX conditions). Finally the 13th (Width) colum specifies the
 * bandwidth of the channel in kHz. This can be 12.5kHz narrow-band or 25kHz wide-band.
 *
 * @subsection confzone Zone lists
 * Zones are just collections of channels. Typical radios can hold thousands of channels. To keep
 * large numbers of channels manageable, they can be organized into zones. Usualy, these zones
 * represent a geographical area and all repeaters in that area are then grouped into zones. Of
 * cause, a single channel can be added to multiple zones. Please note that for many radios,
 * channels can only be accessed via a zone. That means, a channel that is not a member of any zone
 * may not be accessible.
 *
 * The zone table is defined within the configuration file as
 * @code
 * Zone    Name             VFO Channels
 * 1       "KW"             A   1,9,11,12,14,8,55,15,4,5,6,20,21,22,19,48
 * 1       "KW"             B   1,3,2,81,82,84,85,86,87,88,89,90,91
 * 2       "Berlin DMR"     A   10,9,11,12,34,35,31,32,33,27,28,29,30,38,39
 * 2       "Berlin DMR"     B   1,3,2,81,82,84,85,86,87,88,89,90,91
 * 3       "Berlin FM"      A   20,21,19,18,22,23,24,25
 * 4       "Potsdam"        A   42,43,44,45,46,47,40,41
 * 7       "Leipzig"        A   75,76,72,71,73,70,74,77,78,80,79,69
 * 8       "Simplex"        A   1,3,2,81,82,84,85,86,87,88,89,90,91
 * @endcode
 *
 * The zone table starts with the keyword "Zone" and ends with an empty line. The remaining
 * keywords (Name and Channels) are ignored but are part of the self-documentation of the
 * configuration file. The first colum specifies an unique identifier for each zone. This can be
 * any integer as log as it is unique. The second (Name) column specifies the name of the zone as a
 * string. Any string is valid here. The third column specifies the VFO (either A or B) for that
 * zone. This allows to specify different channels for the two VFOs of the radio. For example, it
 * allows to specify a list of repeater channels for VFO A and some simplex and calling frequencies
 * on VFO B. The fourth column contains the comma-separated list of channel IDs for the zone anc VFO.
 * A reference to any channel-type can be used here, analog and digital.
 *
 * @subsection confscan Scan lists
 * A scan list is list of channels, that are scanned whenever scanning is started on a
 * channel the scan list is associated with. A single scan list might be associated with several
 * channels. For example, all channels within that scan list.
 *
 * The list of scan lists has the following form
 * @code
 * Scanlist Name            PCh1 PCh2 TxCh Channels
 * 1        "KW"            1    84   Sel  1,84,2,9,11,8,14,4,5,20,21,19,6
 * 2        "DMR Simplex"   84   -    Sel  84,85,86,87,88,89,90,91
 * @endcode
 *
 * The list of scan lists starts with the "Scanlist" keyword and ends with an empty line. The
 * remaining keywords (Name, PCh1, PCh2 & Channels) are ignored but part of the self-documentation
 * of the configuration file format. A scan list is defined with every other line. The first column
 * specifies the unique identifier of the scan list. The second (Name) column specifies the name of
 * the scan list as a string. Any string will do. The third and fourth columns specify the first
 * and second priority channels for the scan list respectively. These priority channels are visited
 * more frequently during the scan. That is, the first priority channel is visited 50% of the time
 * while the second is visited 25% of the time. These channels might also be set to "-" indicating
 * that there is no priority channel. The 5th column specifies the transmit channel during the scan.
 * Possible options are "Last", "Sel" and any valid channel index. The "Sel" keyword implies that
 * the radio will transmit on the selected channel when the scan started. The "Last" keyword implies
 * that the radio will transmit on the channel at which the scan stopped on, while specifying any
 * channel index implies, that the radio will transmit on that channel. Finally the 6th column
 * specifies the comma-separated list of channels that form the scan list.
 *
 * @subsection confgps GPS Systems
 * The GPS System list just specifies the contact to which some positional information is send to
 * (which usually gets forwarded to the APRS system) and at which period this information is send.
 * @code
 * GPS  Name                Dest Period Revert
 * 1    "BM APRS"           20   300    -
 * @endcode
 * The first column specifies the ID of the GPS system. This can be any number >0. The second column
 * (Name) specifies the name of the GPS system. The third column specifies the destination contact
 * ID (see Contacts above), the position information is send to. The fourth column (Period) specifies
 * the update period in seconds. The fifth column (Revert) specifies the revert channel. In amateur
 * radio, this can be left blank ("-").
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
  virtual bool handleAnalogChannel(
      qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
      qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, float rxTone, float txTone,
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
      qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, float rxTone, float txTone,
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
