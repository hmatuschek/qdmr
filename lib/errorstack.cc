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
ErrorStack::MessageStream::MessageStream(const ErrorStack &stack, const QString &file, unsigned line)
  : QTextStream(&_message), _stack(stack), _file(file), _line(line), _message()
{
  // pass...
}

ErrorStack::MessageStream::~MessageStream() {
  _stack.push(Message(_file, _line, _message));
  LogMessage(LogMessage::ERROR, _file, _line) << _message;
}


/* ********************************************************************************************* *
 * Implementation of ErrorStack::Stack
 * ********************************************************************************************* */
ErrorStack::Stack::Stack() noexcept
  : _refcount(1), _errorMessageStack()
{
  // pass...
}

void
ErrorStack::Stack::push(const Message &msg) {
  _errorMessageStack.push_front(msg);
}

void
ErrorStack::Stack::push(const Stack &other) {
  if (other.isEmpty())
    return;
  for (int i=(other.count()-1); i>=0; i--) {
    push(other.message(i));
  }
}

bool
ErrorStack::Stack::isEmpty() const {
  return 0 == count();
}

unsigned
ErrorStack::Stack::count() const {
  return _errorMessageStack.count();
}

const ErrorStack::Message &
ErrorStack::Stack::message(unsigned i) const {
  return _errorMessageStack.at(i);
}

QString
ErrorStack::Stack::format(const QString &indent) const {
  QString res;
  if (isEmpty())
    return res;
  res += message(0).format();
  for (unsigned i=1; i<count(); i++)
    res += QString("\n%1%2").arg(indent).arg(message(i).format());
  return res;
}

void
ErrorStack::Stack::clear() {
  _errorMessageStack.clear();
}

ErrorStack::Stack *
ErrorStack::Stack::ref() {
  _refcount++;
  return this;
}

void
ErrorStack::Stack::unref() {
  if (0 < _refcount)
    _refcount--;
  if (0 == _refcount)
    delete this;
}

/* ********************************************************************************************* *
 * Implementation of ErrorStack
 * ********************************************************************************************* */
ErrorStack::ErrorStack() noexcept
  : _stack(new ErrorStack::Stack())
{
  // pass...
}

ErrorStack::ErrorStack(const ErrorStack &other)
  : _stack(other._stack->ref())
{
  // pass...
}

ErrorStack::~ErrorStack() {
  _stack->unref();
  _stack = nullptr;
}

ErrorStack &
ErrorStack::operator =(const ErrorStack &other) {
  _stack->unref();
  _stack = other._stack->ref();
  return *this;
}

bool
ErrorStack::isEmpty() const {
  return _stack->isEmpty();
}

unsigned
ErrorStack::count() const {
  return _stack->count();
}

const ErrorStack::Message &
ErrorStack::message(unsigned i) const {
  return _stack->message(i);
}

void
ErrorStack::push(const Message &msg) const {
  _stack->push(msg);
}

void
ErrorStack::take(const ErrorStack &other) const {
  _stack->push(*other._stack);
  other._stack->clear();
}

QString
ErrorStack::format(const QString &indent) const {
  return _stack->format(indent);
}
