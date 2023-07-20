/** @defgroup log Log Message Handling
 * @ingroup util */
#ifndef LOGGER_HH
#define LOGGER_HH

#include <QFile>
#include <QTextStream>
#include <QList>

/** Constructs a debug message. */
#define logDebug() LogMessage(LogMessage::DEBUG, __FILE__, __LINE__)
/** Constructs an info message. */
#define logInfo()  LogMessage(LogMessage::INFO, __FILE__, __LINE__)
/** Constructs a warning message. */
#define logWarn()  LogMessage(LogMessage::WARNING, __FILE__, __LINE__)
/** Constructs an error message. */
#define logError() LogMessage(LogMessage::ERROR, __FILE__, __LINE__)
/** Constructs a fatal error message. */
#ifdef __cpp_lib_stacktrace
#include <stacktrace>
#define logFatal() LogMessage(LogMessage::FATAL, __FILE__, __LINE__) << \
  QString::fromStdString(std::to_string(std::stacktrace::current()))
#else
#define logFatal() LogMessage(LogMessage::FATAL, __FILE__, __LINE__)
#endif

/** Implements a log-message.
 * Instances of this class will forward the content of this message automatically to the @c Logger
 * instance upon destruction. That means, you do not need to forward log messages explicitly.
 * @ingroup log */
class LogMessage: public QTextStream
{
public:
  /** Possible log-levels. */
  typedef enum {
    DEBUG,    ///< Level for debug messages. Will not be shown to the user unless requested.
    INFO,     ///< Level for informative messages. Will not be shown to the user unless requested.
    WARNING,  ///< Level for warning messages.
    ERROR,    ///< Level for error messages.
    FATAL     ///< Level for fatal error messages.
  } Level;

public:
  /** Constructor.
   * @param level Specifies the level of the log message.
   * @param file Specifies the source file.
   * @param line Specifies the source line.
   * @param message Specifies the log message content. */
  LogMessage(Level level, const QString &file, int line, const QString &message="");
  /** Copy constructor. */
  LogMessage(const LogMessage &other);
  /** Destructor. */
  virtual ~LogMessage();

  /** Returns the level of the log message. */
  Level level() const;
  /** Returns the source file. */
  const QString &file() const;
  /** Returns the source line. */
  int line() const;
  /** Returns the log message content. */
  const QString &message() const;

protected:
  /** The log level. */
  Level _level;
  /** The source file. */
  QString _file;
  /** The source line. */
  int _line;
  /** The log message content. */
  QString _message;
};


/** Interface for all log message handler.
 * @ingroup log */
class LogHandler: public QObject
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit LogHandler(QObject *parent=nullptr);
  /** Destructor. */
  virtual ~LogHandler();
  /** Callback to handle log messages. */
  virtual void handle(const LogMessage &message) = 0;
};


/** Singleton class to process log messages.
 * @ingroup log */
class Logger: public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. Use @c get method to obtain an instance. */
  Logger();

public:
  /** Destructor. */
  virtual ~Logger();

  /** Logs a message. */
  void log(const LogMessage &msg);
  /** Adds a log-handler to the logger. The ownership is transferred to the logger. */
  void addHandler(LogHandler *handler);
  /** Removes a log-handler from the logger. The ownership is transferred back to the caller. */
  void remHandler(LogHandler *handler);

protected slots:
  /** Internal callback to handle deleted handler objects. */
  void onHandlerDeleted(QObject *obj);

public:
  /** Factory method to get the singleton instance. */
  static Logger &get();

protected:
  /** The singleton instance. */
  static Logger *_instance;
  /** The list of registered log-handler. */
  QList<LogHandler *> _handler;
};


/** A log-handler that dumps log-messages into a @c QTextStream.
 * @ingroup log */
class StreamLogHandler: public LogHandler
{
  Q_OBJECT

public:
  /** Constructor.
   * @param stream Specifies the text stream to log into.
   * @param minLevel Specifies the minimum log-level to log.
   * @param color If @c true, the output will be colored.
   * @param parent Specifies the parent object. */
  StreamLogHandler(QTextStream &stream, LogMessage::Level minLevel=LogMessage::DEBUG, bool color=false, QObject *parent=nullptr);

  /** Returns the minimum log level. */
  LogMessage::Level minLevel() const;
  /** Resets the minimum log level. */
  void setMinLevel(LogMessage::Level minLevel);

  void handle(const LogMessage &message);

protected:
  /** A reference to the text stream to log into. */
  QTextStream &_stream;
  /** The minimum log level. */
  LogMessage::Level _minLevel;
  /** If true, write messages using console colors. */
  bool _color;
};


/** A log-handler that dumps log-messages into files.
 * @ingroup log */
class FileLogHandler: public LogHandler
{
  Q_OBJECT

public:
  /** Constructor.
   * @param file Specifies the filename to log to.
   * @param minLevel Specifies the minimum log-level to log.
   * @param parent Specifies the parent object. */
  FileLogHandler(const QString &file, LogMessage::Level minLevel=LogMessage::DEBUG, QObject *parent=nullptr);

  /** Destructor, closes log file. */
  virtual ~FileLogHandler();

  /** Returns the minimum log level. */
  LogMessage::Level minLevel() const;
  /** Resets the minimum log level. */
  void setMinLevel(LogMessage::Level minLevel);

  void handle(const LogMessage &message);

protected:
  /** The file to log into. */
  QFile _file;
  /** A reference to the text stream to log into. */
  QTextStream _stream;
  /** The minimum log level. */
  LogMessage::Level _minLevel;
};

#endif // LOGGER_HH
