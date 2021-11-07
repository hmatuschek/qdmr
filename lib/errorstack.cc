#include "errorstack.hh"
#include <QFileInfo>
#include <logger.hh>

/* ********************************************************************************************* *
 * Implementation of ErrorStack::Message
 * ********************************************************************************************* */
ErrorStack::Message::Message()
  : _file(), _line(0), _message()
{
  // pass...
}

ErrorStack::Message::Message(const QString &file, unsigned line, const QString &message)
  : _file(file), _line(line), _message(message)
{
  // pass...
}

const QString &
ErrorStack::Message::file() const {
  return _file;
}

unsigned
ErrorStack::Message::line() const {
  return _line;
}

const QString &
ErrorStack::Message::message() const {
  return _message;
}

QString
ErrorStack::Message::format() const {
  return QString("In %1:%2: %3").arg(QFileInfo(file()).fileName()).arg(line()).arg(message());
}


/* ********************************************************************************************* *
 * Implementation of ErrorStack::MessageStream
 * ********************************************************************************************* */
ErrorStack::MessageStream::MessageStream(ErrorStack &stack, const QString &file, unsigned line)
  : QTextStream(&_message), _stack(stack), _file(file), _line(line), _message()
{
  // pass...
}

ErrorStack::MessageStream::~MessageStream() {
  _stack.pushErrorMessage(Message(_file, _line, _message));
  LogMessage(LogMessage::ERROR, _file, _line) << _message;
}

ErrorStack &
ErrorStack::MessageStream::stack() const {
  return _stack;
}


/* ********************************************************************************************* *
 * Implementation of ErrorStack
 * ********************************************************************************************* */
ErrorStack::ErrorStack()
  : _errorMessageStack()
{
  // pass...
}

void
ErrorStack::pushErrorMessage(const Message &msg) {
  _errorMessageStack.push_front(msg);
}

void
ErrorStack::pushErrorMessage(const ErrorStack &other) {
  if (! other.hasErrorMessages())
    return;
  for (int i=(other.errorMessageCount()-1); i>=0; i--) {
    pushErrorMessage(other.errorMessage(i));
  }
}

bool
ErrorStack::hasErrorMessages() const {
  return 0!=errorMessageCount();
}

unsigned
ErrorStack::errorMessageCount() const {
  return _errorMessageStack.count();
}

const ErrorStack::Message &
ErrorStack::errorMessage(unsigned i) const {
  return _errorMessageStack.at(i);
}

const ErrorStack &
ErrorStack::errorMessages() const {
  return *this;
}

QString
ErrorStack::formatErrorMessages(const QString &indent) const {
  QString res;
  if (0 == errorMessageCount())
    return res;
  res += errorMessage(0).format();
  for (unsigned i=1; i<errorMessageCount(); i++)
    res += QString("\n%1%2").arg(indent).arg(errorMessage(i).format());
  return res;
}




