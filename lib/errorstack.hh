#ifndef ERRORSTACK_HH
#define ERRORSTACK_HH

#include <QTextStream>

/** Implements a stack of error messages to provide a pretty formatted error traceback.
 *
 * This class is intended to be used like:
 * @code
 * class MyClass: public ErrorStack
 * {
 *
 * // [...]
 *
 *   bool someMethod(const ErrorStack &err=ErrorStack()) {
 *     // [...]
 *     if (someError) {
 *       errMsg() << "Some error happend!";
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
 * ErrorStack err;
 *
 * if (! instance.someMethod(err)) {
 *   QString msg = err.formatErrorMessages();
 *   // []
 * }
 * @endcode
 * @ingroup log */
class ErrorStack
{
public:
  class Stack;

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
    MessageStream(const ErrorStack &stack, const QString &file, unsigned line);
    /** Destructor, puts the message on the stack. */
    virtual ~MessageStream();

  protected:
    /** Holds a weak reference to the error stack to put the message on. */
    const ErrorStack &_stack;
    /** The file path. */
    QString _file;
    /** The line number. */
    unsigned _line;
    /** The message buffer. */
    QString _message;
  };

  /** The actual error message stack. */
  class Stack
  {
  public:
    /** Empty constructor. */
    Stack() noexcept;

  public:
    /** Returns @c true if there are any error messages. */
    bool isEmpty() const;
    /** Returns the number of error messages. */
    unsigned count() const;
    /** Returns a specific error message. */
    const Message &message(unsigned i) const;
    /** Returns a formatted string of error messages. */
    QString format(const QString &indent="  ") const;

    /** Adds an error message to the stack. */
    void push(const Message &msg);
    /** Adds the error messages from another stack. */
    void push(const Stack &other);

    /** Clears the error stack. */
    void clear();

    /** Returns a new reference to the stack. */
    Stack *ref();
    /** Dereferences a stack, this decreases the ref count. When 0 is reached, the stack is
     * destroyed. */
    void unref();

  private:
    /** Reference counter. */
    unsigned _refcount;
    /** Holds the stack of error messages. */
    QList<Message> _errorMessageStack;
  };

public:
  /** Default constructor. */
  ErrorStack() noexcept;
  /** Copy constructor. */
  ErrorStack(const ErrorStack &other);
  /** Destructor. */
  ~ErrorStack();

  /** Copy assignment. */
  ErrorStack &operator= (const ErrorStack &other);

  /** Returns @c true, if the stack is empty. */
  bool isEmpty() const;
  /** Returns the number of elements on the stack. */
  unsigned count() const;
  /** Returns the i-th message from the stack. */
  const Message &message(unsigned i) const;

  /** Pushes a message on the stack. */
  void push(const Message &msg) const;
  /** Takes all messages from the other stack. */
  void take(const ErrorStack &other) const;
  /** Returns a formatted string of error messages. */
  QString format(const QString &indent="  ") const;

protected:
  /** A reference to the actual message stack. */
  Stack *_stack;
};


/** Utility macro to assemble a message stream. */
#define errMsg(stack) (ErrorStack::MessageStream(stack, __FILE__, __LINE__))

#endif // ERRORSTACK_HH
