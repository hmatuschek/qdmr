/** @defgroup limits Radio Limits
 * This module collects all classes used to define the limits for each supported radio. That is,
 * a tree of objects that hold the limitations like string length, number of elements in a list etc.
 * for the various settings of a radio including their extensions.
 *
 * This system will replace the static @c Radio::Features struct. The associated limits for each
 * radio can be obtained using the @c Radio::limits method.
 *
 * Many classes in this module provide an initializer list constructor. This allows for an easy
 * construction of radio limits programmatically like
 * @code
 * new RadioLimitItem {                       // < Describes an ConfigItem
 *   { "radioIDs",                            // < with an 'radioIDs' property
 *     new RadioLimitList(                    // < that is a list,
 *       1, 10,                               //   of at least one but max 10 elements
 *       new RadioLimitObject {               // < of objects, with
 *        { "name",                           // < a name
 *          new RadioLimitString(1, 16) },    //   being a string between 1 and 10 chars
 *        { "id",                             // < and an ID
 *          new RadioLimitUInt(0, 16777216) } //   being an unsigned integer between 0 and 16777216
 *       }
 *     )
 *   }
 * };
 * @endcode
 *
 * @ingroup conf */
#ifndef RADIOLIMITS_HH
#define RADIOLIMITS_HH

#include <QObject>
#include <QTextStream>
#include <QMetaType>
#include <QSet>

// Forward declaration
class Config;
class ConfigItem;
class ConfigObject;
class RadioLimits;


/** Represents a single issue found during verification.
 * @ingroup limits */
class RadioLimitIssue: public QTextStream
{
public:
  /** Defines the possible severity levels. */
  enum Severity {
    Silent,  ///< The user will not be notified.
    Hint,    ///< Just a hint, a working codplug will be assembled.
    Warning, ///< The codeplug gets changed but a working codeplug might be assembled.
    Critical ///< Assembly of the codeplug will fail or a non-functional codeplug might be created.
  };

public:
  /** Constructs an empty message for the specified severity at the specified point of the
   * verification. */
  RadioLimitIssue(Severity severity, const QStringList &stack);
  /** Copy constructor. */
  RadioLimitIssue(const RadioLimitIssue &other);

  /** Copy assignment. */
  RadioLimitIssue &operator =(const RadioLimitIssue &other);
  /** Set message. */
  RadioLimitIssue &operator =(const QString &message);

  /** Returns the severity of the issue. */
  Severity severity() const;
  /** Formats the message. */
  QString format() const;

protected:
  /** Holds the severity of the issue. */
  Severity _severity;
  /** Holds the item-stack (where the issue occured). */
  QStringList _stack;
  /** Holds the text message. */
  QString _message;
};


/** Collects the issues found during verification.
 * This class also tracks where the issues arise.
 *
 * @ingroup limits */
class RadioLimitContext
{
public:
  /** Empty constructor. */
  explicit RadioLimitContext();

  /** Constructs a new message and puts it into the list of issues. */
  RadioLimitIssue &newMessage(RadioLimitIssue::Severity severity = RadioLimitIssue::Hint);

  /** Retunrs the number of issues. */
  int count() const;
  /** Returns the n-th issue. */
  const RadioLimitIssue &message(int n) const;

  /** Push a property name/element index onto the stack.
   * This method is used to track the origin of an issue. */
  void push(const QString &element);
  /** Pops the top-most property name/element index from the stack. */
  void pop();

protected:
  /** The current item stack. */
  QStringList _stack;
  /** The list of issues found. */
  QList<RadioLimitIssue> _messages;
};


/** Abstract base class for all radio limits.
 *
 * @ingroup limits */
class RadioLimitElement: public QObject
{
  Q_OBJECT

public:
  /** Initializer lists of ConfigItem properties. */
  typedef std::initializer_list< std::pair<QString, RadioLimitElement *> > PropList;

protected:
  /** Hidden constructor. */
  explicit RadioLimitElement(QObject *parent=nullptr);

public:
  /** Verifies the given property of the specified item.
   * This method gets implemented by the specialized classes to implement the actual verification. */
  virtual bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const = 0;

public:
  /** Destructor. */
  virtual ~RadioLimitElement();
};


/** Base class to verify values.
 *
 * That is, the verification of strings, integers, floats, etc.
 * @ingroup limits */
class RadioLimitValue: public RadioLimitElement
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit RadioLimitValue(QObject *parent=nullptr);
};


/** Checks a string valued property.
 *
 * Instances of this class can be used to verify string values. That is, checking the length of the
 * string and its encoding.
 *
 * @ingroup limits */
class RadioLimitString: public RadioLimitValue
{
  Q_OBJECT

public:
  /** Possible encoding of strings. */
  enum Encoding {
    ASCII,   ///< Just ASCII is allowed.
    Unicode  ///< Any Unicode character is allowed.
  };

public:
  /** Constructor.
   * @param minLen Specifies the minimum length of the string. If -1, check is disabled.
   * @param maxLen Specifies the maximum length of the string. If -1, check is disabled.
   * @param enc Specifies the allowed string encoding.
   * @param parent Specifies the QObject parent object. */
  RadioLimitString(int minLen, int maxLen, Encoding enc, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Holds the minimum length of the string. If -1, the check is disabled. */
  int _minLen;
  /** Holds the maximum length of the string. If -1, the check is disabled. */
  int _maxLen;
  /** Holds the allowed character encoding. */
  Encoding _encoding;
};


/** Verifies that a string matches a regular expression.
 * @ingroup limits */
class RadioLimitStringRegEx: public RadioLimitValue
{
  Q_OBJECT

public:
  /** Constructor.
   * @param pattern Specifies the regular expression pattern, the string must match.
   * @param parent Specifies the QObject parent. */
  RadioLimitStringRegEx(const QString &pattern, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Holds the regular expression pattern. */
  QRegExp _pattern;
};


/** Notifies the user that a string gets ignored.
 * This is usually the case, when named elements are referenced within the codeplug by index.
 * @ingroup limits */
class RadioLimitStringIgnored: public RadioLimitValue {
  Q_OBJECT

public:
  /** Constructor. */
  RadioLimitStringIgnored(RadioLimitIssue::Severity severity=RadioLimitIssue::Hint, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Holds the severity of the message. */
  RadioLimitIssue::Severity _severity;
};


/** Checks if a property is a boolean value.
 * @ingroup limits */
class RadioLimitBool: public RadioLimitValue
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RadioLimitBool(QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;
};


/** Specifies an boolean value as ignored.
 * If the boolean value is @c true, a message is generated indicating that this property is ignored.
 * If the value is @c false, nothing happens. */
class RadioLimitIgnoredBool: public RadioLimitBool
{
  Q_OBJECT

public:
  /** Constructor.
   * @param notify Specifies the severity of the generated message.
   * @param parent Specifies the QObject parent. */
  explicit RadioLimitIgnoredBool(RadioLimitIssue::Severity notify=RadioLimitIssue::Hint,
                                 QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** The severity of the issue generated. */
  RadioLimitIssue::Severity _severity;
};


/** Represents a limit for an unsigned integer value.
 * @ingroup limits */
class RadioLimitUInt: public RadioLimitValue
{
  Q_OBJECT

public:
  /** Constructor.
   * @param minValue Specifies the minimum value. If -1, no check is performed.
   * @param maxValue Specifies the maximum value. If -1, no check is performed.
   * @param defValue Specifies the default value. If -1, no default value is set.
   * @param parent Specifies the QObject parent. */
  RadioLimitUInt(qint64 minValue=-1, qint64 maxValue=-1, qint64 defValue=-1, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Holds the minimum value. If -1, the check is disabled. */
  qint64 _minValue;
  /** Holds the maximum value. If -1, the check is disabled. */
  qint64 _maxValue;
  /** Holds the default value. If -1, no default value is set. */
  qint64 _defValue;
};


/** Represents a limit for a set of enum values.
 * @ingroup limits */
class RadioLimitEnum: public RadioLimitValue
{
  Q_OBJECT

public:
  /** Constructor from initializer list of possible enum values. */
  RadioLimitEnum(const std::initializer_list<unsigned> &values, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Holds the set of valid values. */
  QSet<unsigned> _values;
};


/** Represents a limit on frequencies in MHz.
 * @ingroup limits */
class RadioLimitFrequencies: public RadioLimitValue
{
  Q_OBJECT

public:
  /** Typedef for the initializer list. */
  typedef std::initializer_list<std::pair<double, double>> RangeList;
public:
  /** Represents a single frequency range. */
  struct FrequencyRange {
    double min; ///< Lower frequency limit.
    double max; ///< Upper frequency limit.
    /** Constructs a frequency range from limits. */
    FrequencyRange(double lower, double upper);
    /** Constructs a frequency range from limits. */
    FrequencyRange(const std::pair<double, double> &limit);
    /** Returns @c true if @c f is inside this limit. */
    bool contains(double f) const;
  };

public:
  /** Empty constructor. */
  explicit RadioLimitFrequencies(QObject *parent=nullptr);
  /** Constructor from initializer list. */
  RadioLimitFrequencies(const RangeList &ranges, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Holds the frequency ranges for the device. */
  QList<FrequencyRange> _frequencyRanges;
};


/** Represents the limits for a @c ConfigItem instance.
 *
 * That is, it holds the limits for every property of the @c ConfigItem instance. This class
 * provides a initializer list constructor for easy programmatic contruction of limits.
 *
 * @ingroup limits */
class RadioLimitItem: public RadioLimitElement
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit RadioLimitItem(QObject *parent=nullptr);
  /** Constructor from initializer list.
   * The ownership of all passed elements are taken. */
  RadioLimitItem(const PropList &list, QObject *parent=nullptr);

  /** Adds a property declaration.
   *
   * The item takes the ownership of the structure declaration. If a property is already defined
   * with the same name, @c false is returned.
   *
   * @param prop Specifies the name of the property.
   * @param structure Specifies the structure declaration of the propery value.
   * @returns @c false If a property with the same name is already defined. */
  bool add(const QString &prop, RadioLimitElement *structure);

  virtual bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;
  /** Verifies the properties of the given item. */
  virtual bool verifyItem(const ConfigItem *item, RadioLimitContext &context) const;

protected:
  /** Holds the property <-> limits map. */
  QHash<QString, RadioLimitElement *> _elements;
};


/** Represents the limits for all properties of a @c ConfigObject instance.
 * @ingroup limits */
class RadioLimitObject: public RadioLimitItem
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit RadioLimitObject(QObject *parent=nullptr);
  /** Constructor from initializer list.
   * The ownership of all passed elements are taken. */
  RadioLimitObject(const PropList &list, QObject *parent=nullptr);

  /** Verifies the properties of the given object. */
  virtual bool verifyObject(const ConfigObject *item, RadioLimitContext &context) const;
};


/** Represents an ignored element in the codeplug.
 *
 * Instances of this class might be used to inform the user about a configured feature not present
 * in the particular radio.
 *
 * @ingroup limits */
class RadioLimitIgnored: public RadioLimitObject
{
  Q_OBJECT

public:
  /** Constructor for a ignored setting verification element. */
  RadioLimitIgnored(RadioLimitIssue::Severity notify=RadioLimitIssue::Hint, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;
  bool verifyObject(const ConfigObject *item, RadioLimitContext &context) const;

protected:
  /** Holds the level of the notification. */
  RadioLimitIssue::Severity _notification;
};


/** Dispatch by class.
 *
 * Sometimes, a property may hold objects of different type. In these cases, a dispatcher is needed
 * to specify which limits to apply based on the type of the object. This class implements this
 * dispatcher.
 *
 * @ingroup limits */
class RadioLimitObjects: public RadioLimitObject
{
  Q_OBJECT

public:
  /** Initializer lists of type properties. */
  typedef std::initializer_list<std::pair<const QMetaObject&, RadioLimitObject *> > TypeList;

public:
  /** Constructor from initializer list.
   *
   * A list of pairs of a @c QMetaObject and a @c RadioLimitObject must be given. The meta object
   * specifies the type of the @c ConfigObject and the associated @c RadioLimitObject the limits
   * for this type.
   *
   * A dispatch for Analog and DigitalChannel may look like
   * @code
   * new RadioLimitObjects{
   *   { AnalogChannel::staticMetaObject, new RadioLimitObject{
   *       // Limits for analog channel objects
   *     } },
   *   {DigialChannel::staticMetaObject, new RadioLimitObject{
   *       // Limits for digital channel objects
   *     } }
   * };
   * @endcode
   */
  RadioLimitObjects(const TypeList &list, QObject *parent=nullptr);

  bool verifyItem(const ConfigItem *item, RadioLimitContext &context) const;

protected:
  /** Maps class-names to object limits. */
  QHash<QString,  RadioLimitObject *> _types;
};


/** Limits the possible classes a @c ConfigObjectReference may refer to.
 * @ingroup limits */
class RadioLimitObjRef: public RadioLimitElement
{
  Q_OBJECT

public:
  /** Constructor.
   * @param type Specifies the type that might be referenced.
   * @param allowNull If @c true, the reference may be a nullptr.
   * @param parent Specifies the QObject parent. */
  RadioLimitObjRef(const QMetaObject &type, bool allowNull=true, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Checks if the given type is one of the valid ones in @c _types. */
  bool validType(const QMetaObject *type) const;

protected:
  /** If @c true, a null reference is allowed. */
  bool _allowNull;
  /** Possible classes of instances, the reference may point to. */
  QSet<QString> _types;
};


/** Issues a notification if a reference is set.
 * @ingroup limits */
class RadioLimitObjRefIgnored: public RadioLimitObjRef
{
  Q_OBJECT

public:
  /** Constructor.
   * @param notify Specifies the issue severity.
   * @param parent Specifies the QObject parent. */
  RadioLimitObjRefIgnored(RadioLimitIssue::Severity notify=RadioLimitIssue::Hint, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** The severity of the issue. */
  RadioLimitIssue::Severity _severity;
};


/** Specifies the limits for a list of @c ConfigObject instances.
 * @ingroup limits */
class RadioLimitList: public RadioLimitElement
{
  Q_OBJECT

public:
  /** Helper struct to pass list entry definitions */
  struct ElementLimits {
    const QMetaObject &type;      ///< The type of the object
    int minCount;                 ///< Minimum count of elements.
    int maxCount;                 ///< Maximum count of elements.
    RadioLimitObject *structure;  ///< The structure of the elements.
  };

public:
  /** Constructor.
   * @param type Specifies the type of the elements.
   * @param minSize Specifies the minimum size of the list. If -1, no check is performed.
   * @param maxSize Specifies the maximum size of the list. If -1, no check is performed.
   * @param element Specifies the limits for all objects in the list. If the list contains instances
   *                of different ConfigObject types, use @c RadioLimitObjects dispatcher.
   * @param parent  Specifies the QObject parent. */
  RadioLimitList(const QMetaObject &type, int minSize, int maxSize, RadioLimitObject *element, QObject *parent=nullptr);
  /** Constructor from initializer list. */
  RadioLimitList(const std::initializer_list<ElementLimits> &elements, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Searches for the specified type or one of its super-clsases in the set of allowed types. */
  QString findClassName(const QMetaObject &type) const;

protected:
  /** Maps typename to element definition. */
  QHash<QString, RadioLimitObject *> _elements;
  /** Maps typename to minimum count. */
  QHash<QString, qint64> _minCount;
  /** Maps typename to maximum count. */
  QHash<QString, qint64> _maxCount;
};


/** Implements the limits for reference lists.
 * @ingroup limits */
class RadioLimitRefList: public RadioLimitElement
{
  Q_OBJECT

public:
  /** Constructor.
   * @param minSize Specifies the minimum size of the list. If -1, no check is performed.
   * @param maxSize Specifies the maximum size of the list. If -1, no check is performed.
   * @param type Specifies the type, the references must be instances of.
   * @param parent  Specifies the QObject parent. */
  RadioLimitRefList(int minSize, int maxSize, const QMetaObject &type, QObject *parent=nullptr);

  bool verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const;

protected:
  /** Checks if the given type is one of the valid ones in @c _types. */
  bool validType(const QMetaObject *type) const;

protected:
  /** Holds the minimum size of the list. */
  qint64 _minSize;
  /** Holds the maximum size of the list. */
  qint64 _maxSize;
  /** Holds the limits for all objects of the list. */
  RadioLimitObject *_element;
  /** Possible classes of instances, the references may point to. */
  QSet<QString> _types;
};


/** Specialized RadioLimitObject handling a zone for radio supporting only a single channel list
 * per zone.
 *
 * Checks if a zone contains two sets of channel lists and notifies the user that the zone gets
 * split.
 *
 * @ingroup limits */
class RadioLimitSingleZone: public RadioLimitObject
{
  Q_OBJECT

public:
  /** Constructor.
   * @param maxSize Specifies the maximum size of the zone. If -1, no check is performed.
   * @param list Initializer list for further zone properties.
   * @param parent Specifies the QObject parent. */
  RadioLimitSingleZone(qint64 maxSize, const PropList &list, QObject *parent=nullptr);

  bool verifyItem(const ConfigItem *item, RadioLimitContext &context) const;
};


/** Represents the limits or the entire codeplug.
 *
 * Use @c Radio::limits to obtain an instance.
 * @ingroup limits */
class RadioLimits : public RadioLimitItem
{
  Q_OBJECT

public:
  /** Empty constructor. */
  explicit RadioLimits(QObject *parent = nullptr);
  /** Constructor from initializer list. */
  RadioLimits(const std::initializer_list<std::pair<QString,RadioLimitElement *> > &list, QObject *parent=nullptr);

  /** Verifies the given configuration. */
  bool verifyConfig(const Config *config, RadioLimitContext &context) const;
};

#endif // RADIOLIMITS_HH
