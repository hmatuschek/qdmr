#include "logger.hh"

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
  _stream << "in " << message.file() << "@" << message.line() << ": " << message.message() << endl;
}
