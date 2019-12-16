#ifndef LOGGER_HH
#define LOGGER_HH

#include <QTextStream>
#include <QList>

#define logDebug() LogMessage(LogMessage::DEBUG, __FILE__, __LINE__)
#define logInfo()  LogMessage(LogMessage::INFO, __FILE__, __LINE__)
#define logWarn()  LogMessage(LogMessage::WARNING, __FILE__, __LINE__)
#define logError() LogMessage(LogMessage::ERROR, __FILE__, __LINE__)
#define logFatal() LogMessage(LogMessage::FATAL, __FILE__, __LINE__)


class LogMessage: public QTextStream
{
public:
  typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
  } Level;

public:
  LogMessage(Level level, const QString &file, int line, const QString &message="");
  LogMessage(const LogMessage &other);

  virtual ~LogMessage();

  Level level() const;
  const QString &file() const;
  int line() const;
  const QString &message() const;

protected:
  Level _level;
  QString _file;
  int _line;
  QString _message;
};


class LogHandler: public QObject
{
  Q_OBJECT

public:
  explicit LogHandler(QObject *parent=nullptr);
  virtual ~LogHandler();

  virtual void handle(const LogMessage &message) = 0;
};


class Logger: public QObject
{
  Q_OBJECT

protected:
  Logger();

public:
  virtual ~Logger();

  void log(const LogMessage &msg);
  void addHandler(LogHandler *handler);
  void remHandler(LogHandler *handler);

protected slots:
  void onHandlerDeleted(QObject *obj);

public:
  static Logger &get();

protected:
  static Logger *_instance;
  QList<LogHandler *> _handler;
};


class StreamLogHandler: public LogHandler
{
  Q_OBJECT

public:
  StreamLogHandler(QTextStream &stream, LogMessage::Level minLevel=LogMessage::DEBUG, QObject *parent=nullptr);

  void handle(const LogMessage &message);

protected:
  QTextStream &_stream;
  LogMessage::Level _minLevel;
};

#endif // LOGGER_HH
