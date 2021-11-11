#ifndef ERRORSTACK_HH
#define ERRORSTACK_HH

#include <QTextStream>

/** Implements a stack of error messages to provide a pretty formatted error traceback.
 *
 * This class is intended to be used as a mix-in in other classes. That is:
 * @code
 * class MyClass: public ErrorStack
 * {
 *
 * // [...]
 *
 *   bool someMethod() {
 *     // [...]
 *     if (someError) {
 *       errorMessage() << "Some error happend!";
 *       return false;
 *     }
 *     // [...]
 *   }
 *
 * // [...]
 * }
 * @endcode
 *
 * The error message can then be obtained using the public methods. E.g.,
 * @code
 * // [...]
 * MyClass instance;
 *
 * if (! instance.someMethod()) {
 *   QString msg = instance.formatErrorMessages();
 *   // []
 * }
 * @endcode
 * @ingroup log */
class ErrorStack
{
public:
  /** Represents a single error message. That is, a tuple of file, line and message. */
  class Message
  {
  public:
    /** Empty constructor. */
    Message();
    /** Constructor from file, line and message. */
    Message(const QString &file, unsigned line, const QString &message);

    /** Retunrs the file name. */
    const QString &file() const;
    /** Returns the line within the file. */
    unsigned line() const;
    /** Returns the error message. */
    const QString &message() const;
    /** Formats the error messaege. */
    QString format() const;

  protected:
    /** Holds the file path. */
    QString _file;
    /** Holds the line. */
    unsigned _line;
    /** Holds the error message. */
    QString _message;
  };

  /** A helper class to assemble error messages as streams. Use the @c errorMessage macro. */
  class MessageStream: public QTextStream
  {
  public:
    /** Constructor. */
    MessageStream(ErrorStack &stack, const QString &file, unsigned line);
    /** Destructor, puts the message on the stack. */
    virtual ~MessageStream();

    /** Returns a reference to the error stack. */
    ErrorStack &stack() const;

  protected:
    /** Holds a weak reference to the error stack to put the message on. */
    ErrorStack &_stack;
    /** The file path. */
    QString _file;
    /** The line number. */
    unsigned _line;
    /** The message buffer. */
    QString _message;
  };

protected:
  /** Default constructor. */
  ErrorStack();

public:
  /** Returns @c true if there are any error messages. */
  bool hasErrorMessages() const;
  /** Returns the number of error messages. */
  unsigned errorMessageCount() const;
  /** Returns a specific error message. */
  const Message &errorMessage(unsigned i) const;
  /** Returns a const reference to itself. Just a convenience method for more readablility. */
  const ErrorStack &errorMessages() const;
  /** Returns a formatted string of error messages. */
  QString formatErrorMessages(const QString &indent="  ") const;

  /** Adds an error message to the stack. */
  void pushErrorMessage(const Message &msg);
  /** Adds the error messages from another stack. */
  void pushErrorMessage(const ErrorStack &other);

  /** Clears the error stack. */
  void clearErrors();

private:
  /** Holds the stack of error messages. */
  QList<Message> _errorMessageStack;
};


#define errMsg() (ErrorStack::MessageStream(*this, __FILE__, __LINE__))

#endif // ERRORSTACK_HH
