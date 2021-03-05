#include "logger.hh"
#include <QFileInfo>
#include <QDir>
#include <QDateTime>


/* ********************************************************************************************* *
 * Implementation of LogMessage
 * ********************************************************************************************* */
LogMessage::LogMessage(Level level, const QString &file, int line, const QString &message)
  : QTextStream(), _level(level), _file(file), _line(line), _message(message)
{
  this->setString(&_message);
  this->seek(_message.size());
}

LogMessage::LogMessage(const LogMessage &other)
  : QTextStream(), _level(other._level), _file(other._file), _line(other._line), _message(other._message)
{
  this->setString(&_message);
  this->seek(_message.size());
}

LogMessage::~LogMessage() {
  Logger::get().log(*this);
}

LogMessage::Level
LogMessage::level() const {
  return _level;
}

const QString &
LogMessage::file() const {
  return _file;
}

int
LogMessage::line() const {
  return _line;
}

const QString &
LogMessage::message() const {
  return _message;
}


/* ********************************************************************************************* *
 * Implementation of LogHandler
 * ********************************************************************************************* */
LogHandler::LogHandler(QObject *parent)
  : QObject(parent)
{
  // pass...
}

LogHandler::~LogHandler() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of Logger
 * ********************************************************************************************* */
Logger *Logger::_instance = nullptr;

Logger::Logger()
  : QObject(nullptr), _handler()
{
  // pass...
}

Logger::~Logger() {
  _handler.clear();
}

void
Logger::log(const LogMessage &msg) {
  foreach (LogHandler *handler, _handler) {
    handler->handle(msg);
  }
}

void
Logger::addHandler(LogHandler *handler) {
  if (nullptr == handler)
    return;
  if (_handler.contains(handler))
    return;
  handler->setParent(this);
  _handler.append(handler);
  connect(handler, SIGNAL(destroyed(QObject*)), this, SLOT(onHandlerDeleted(QObject*)));
}

void
Logger::remHandler(LogHandler *handler) {
  if (_handler.contains(handler)) {
    handler->setParent(nullptr);
    disconnect(handler, SIGNAL(destroyed(QObject*)), this, SLOT(onHandlerDeleted(QObject*)));
  }
  _handler.removeAll(handler);
}

void
Logger::onHandlerDeleted(QObject *obj) {
  _handler.removeAll(dynamic_cast<LogHandler*>(obj));
}

Logger &
Logger::get() {
  if (nullptr == _instance) {
    _instance = new Logger();
  }
  return *_instance;
}


/* ********************************************************************************************* *
 * Implementation of StreamLogHandler
 * ********************************************************************************************* */
StreamLogHandler::StreamLogHandler(QTextStream &stream, LogMessage::Level minLevel, QObject *parent)
  : LogHandler(parent), _stream(stream), _minLevel(minLevel)
{
  // pass...
}

LogMessage::Level
StreamLogHandler::minLevel() const {
  return _minLevel;
}

void
StreamLogHandler::setMinLevel(LogMessage::Level minLevel) {
  _minLevel = minLevel;
}

void
StreamLogHandler::handle(const LogMessage &message) {
  if (message.level() < _minLevel)
    return;
  switch (message.level()) {
  case LogMessage::DEBUG:   _stream << "Debug "; break;
  case LogMessage::INFO:    _stream << "Info "; break;
  case LogMessage::WARNING: _stream << "Warning "; break;
  case LogMessage::ERROR:   _stream << "ERROR "; break;
  case LogMessage::FATAL:   _stream << "FATAL "; break;
  }
  _stream << "in " << message.file() << "@" << message.line()
          << ": " << message.message() << Qt::endl;
}


/* ********************************************************************************************* *
 * Implementation of FileLogHandler
 * ********************************************************************************************* */
FileLogHandler::FileLogHandler(const QString &filename, LogMessage::Level minLevel, QObject *parent)
  : LogHandler(parent), _file(filename), _stream(), _minLevel(minLevel)
{
  QFileInfo info(filename);
  // Check if logfile exists
  if (! info.exists()) {
    // check and create path to logfile (if needed)
    if (! info.absoluteDir().mkpath(info.absoluteDir().absolutePath())) {
      logError() << "Cannot create log-file directory '" << info.absoluteDir().absolutePath() << "'.";
      return;
    }
    // Create logfile
    _file.open(QFile::WriteOnly);
  } else {
    // Open logfile
    _file.open(QFile::Append);
  }

  if (_file.isOpen()) {
    // Set stream desitnation to logfile
    _stream.setDevice(&_file);
  }
}

FileLogHandler::~FileLogHandler() {
  if (_file.isOpen()) {
    _file.flush();
    _file.close();
  }
}

LogMessage::Level
FileLogHandler::minLevel() const {
  return _minLevel;
}

void
FileLogHandler::setMinLevel(LogMessage::Level minLevel) {
  _minLevel = minLevel;
}

void
FileLogHandler::handle(const LogMessage &message) {
  if (!_file.isOpen())
    return;

  if (message.level() < _minLevel)
    return;

  _stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << ": ";
  switch (message.level()) {
  case LogMessage::DEBUG:   _stream << "Debug "; break;
  case LogMessage::INFO:    _stream << "Info "; break;
  case LogMessage::WARNING: _stream << "Warning "; break;
  case LogMessage::ERROR:   _stream << "ERROR "; break;
  case LogMessage::FATAL:   _stream << "FATAL "; break;
  }
  _stream << "in " << message.file() << "@" << message.line()
          << ": " << message.message() << Qt::endl;
}
