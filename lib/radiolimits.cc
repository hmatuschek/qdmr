#include "radiolimits.hh"
#include "configobject.hh"
#include "logger.hh"
#include "config.hh"
#include <QMetaProperty>
#include <ctype.h>

// Utility function to check string content for ASCII encoding
inline bool qstring_is_ascii(const QString &text) {
  foreach (QChar c, text) {
    if ((c<0x1f) && (0x7f != c))
      return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitIssue
 * ********************************************************************************************* */
RadioLimitIssue::RadioLimitIssue(Severity severity, const QStringList &stack)
  : QTextStream(), _severity(severity), _stack(stack), _message()
{
  setString(&_message);
}

RadioLimitIssue::RadioLimitIssue(const RadioLimitIssue &other)
  : QTextStream(), _severity(other._severity), _stack(other._stack), _message(other._message)
{
  setString(&_message);
}

RadioLimitIssue &
RadioLimitIssue::operator =(const RadioLimitIssue &other) {
  QTextStream::flush();
  _severity = other._severity;
  _stack = other._stack;
  _message = other._message;
  return *this;
}

RadioLimitIssue &
RadioLimitIssue::operator =(const QString &message) {
  _message = message;
  return *this;
}

RadioLimitIssue::Severity
RadioLimitIssue::severity() const {
  return _severity;
}

const QString &
RadioLimitIssue::message() const {
  return _message;
}

QString
RadioLimitIssue::format() const {
  QString res; QTextStream stream(&res);
  switch (_severity) {
  case Silent: stream << "Silent: "; break;
  case Hint: stream << "Hint: "; break;
  case Warning: stream << "Warn: "; break;
  case Critical: stream << "Crit: "; break;
  }
  stream << "In " << _stack.join(", ") << ": " << _message;
  stream.flush();
  return res;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitContext
 * ********************************************************************************************* */
RadioLimitContext::RadioLimitContext(bool ignoreFrequencyLimits)
  : _stack(), _ignoreFrequencyLimits(ignoreFrequencyLimits), _maxSeverity(RadioLimitIssue::Silent)
{
  // pass...
}

RadioLimitIssue &
RadioLimitContext::newMessage(RadioLimitIssue::Severity severity) {
  _messages.push_back(RadioLimitIssue(severity, _stack));
  if (severity > _maxSeverity)
    _maxSeverity = severity;
  return _messages.back();
}

int
RadioLimitContext::count() const {
  return _messages.count();
}

const RadioLimitIssue &
RadioLimitContext::message(int n) const {
  return _messages.at(n);
}

void
RadioLimitContext::push(const QString &element) {
  _stack.append(element);
}

void
RadioLimitContext::pop() {
  _stack.pop_back();
}

bool
RadioLimitContext::ignoreFrequencyLimits() const {
  return _ignoreFrequencyLimits;
}
void
RadioLimitContext::enableIgnoreFrequencyLimits(bool enable) {
  _ignoreFrequencyLimits = enable;
}

RadioLimitIssue::Severity
RadioLimitContext::maxSeverity() const {
  return _maxSeverity;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitElement
 * ********************************************************************************************* */
RadioLimitElement::RadioLimitElement(QObject *parent)
  : QObject(parent)
{
  // pass...
}

RadioLimitElement::~RadioLimitElement() {
  // pass ...
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitIgnored
 * ********************************************************************************************* */
RadioLimitIgnored::RadioLimitIgnored(RadioLimitIssue::Severity notify, QObject *parent)
  : RadioLimitObject(parent), _notification(notify)
{
  // pass...
}

bool
RadioLimitIgnored::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  ConfigObject *obj = prop.read(item).value<ConfigObject *>();
  if (nullptr != obj)
    return verifyObject(obj, context);

  return true;
}

bool
RadioLimitIgnored::verifyObject(const ConfigObject *item, RadioLimitContext &context) const {
  if (nullptr == item)
    return true;
  auto &msg = context.newMessage(_notification);
  msg = tr("Ignore %1 '%2'. Not applicable/supported by this radio.")
      .arg(item->metaObject()->className()).arg(item->name());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitValue
 * ********************************************************************************************* */
RadioLimitValue::RadioLimitValue(QObject *parent)
  : RadioLimitElement(parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitString
 * ********************************************************************************************* */
RadioLimitString::RadioLimitString(int minLen, int maxLen, Encoding enc, QObject *parent)
  : RadioLimitValue(parent), _minLen(minLen), _maxLen(maxLen), _encoding(enc)
{
  // pass...
}

bool
RadioLimitString::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::String != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected string.";
    return false;
  }

  QString value = prop.read(item).toString();

  if ((0<_maxLen) && (value.size() > int(_maxLen))) {
    auto &msg = context.newMessage();
    msg << "String length of " << prop.name() << " ('" << value << "', " << value.size()
        << ") exceeds maximum length " << _maxLen << ".";
  }

  if ((0<_minLen) && (value.size() < int(_minLen))) {
    auto &msg = context.newMessage();
    msg << "String length of " << prop.name() << " ('" << value << "', " << value.size()
        << ") is shorter than minimum size " << _minLen << ".";
  }

  if ((ASCII == _encoding) && (! qstring_is_ascii(value))) {
    auto &msg = context.newMessage();
    msg << "Cannot encode string '" << value << "' in ASCII.";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitStringRegEx
 * ********************************************************************************************* */
RadioLimitStringRegEx::RadioLimitStringRegEx(const QString &pattern, QObject *parent)
  : RadioLimitValue(parent), _pattern(pattern)
{
  // pass...
}

bool
RadioLimitStringRegEx::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::String != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected string.";
    return false;
  }

  QString value = prop.read(item).toString();
  if (! _pattern.exactMatch(value)) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "Value '" << value << "' of property " << prop.name()
        << " does not match pattern '" << _pattern.pattern() << "'.";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitStringIgnored
 * ********************************************************************************************* */
RadioLimitStringIgnored::RadioLimitStringIgnored(RadioLimitIssue::Severity severity, QObject *parent)
  : RadioLimitValue(parent), _severity(severity)
{
  // pass...
}

bool
RadioLimitStringIgnored::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::String != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg = tr("Expected value of '%1' to be string.").arg(prop.name());
    return true;
  }

  QVariant value = prop.read(item);
  if (! value.toString().isEmpty()) {
    auto &msg = context.newMessage(_severity);
    msg = tr("Value of '%1' is ignored. Not applicable/supported by the radio.").arg(prop.name());
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitBool
 * ********************************************************************************************* */
RadioLimitBool::RadioLimitBool(QObject *parent)
  : RadioLimitValue(parent)
{
  // pass...
}

bool
RadioLimitBool::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  Q_UNUSED(item)

  if (QVariant::Bool != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected bool.";
    return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitIgnoredBool
 * ********************************************************************************************* */
RadioLimitIgnoredBool::RadioLimitIgnoredBool(RadioLimitIssue::Severity notify, QObject *parent)
  : RadioLimitBool(parent), _severity(notify)
{
  // pass...
}

bool
RadioLimitIgnoredBool::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::Bool != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected bool.";
    return false;
  }

  bool value = prop.read(item).toBool();
  if (value) {
    auto &msg = context.newMessage(_severity);
    msg << "Setting " << prop.name() << " is ignored.";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitUInt
 * ********************************************************************************************* */
RadioLimitUInt::RadioLimitUInt(qint64 minValue, qint64 maxValue, qint64 defValue, QObject *parent)
  : RadioLimitValue(parent), _minValue(minValue), _maxValue(maxValue), _defValue(defValue)
{
  // pass...
}

bool
RadioLimitUInt::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::UInt != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected uint.";
    return false;
  }

  unsigned value = prop.read(item).toUInt();

  if ((0<_maxValue) && (value > qint64(_maxValue)) && ((0>_defValue) || (value!=_defValue))) {
    auto &msg = context.newMessage();
    msg << "Value " << value << " of " << prop.name() << " exceeds maximum " << _maxValue << ".";
  }

  if ((0<_minValue) && (value < qint64(_minValue)) && ((0>_defValue) || (value!=_defValue))) {
    auto &msg = context.newMessage();
    msg << "Value " << value << " of " << prop.name() << " is smaller than minimum " << _minValue << ".";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitDMRId
 * ********************************************************************************************* */
RadioLimitDMRId::RadioLimitDMRId(QObject *parent)
  : RadioLimitUInt(1, 16777215, -1, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitEnum
 * ********************************************************************************************* */
RadioLimitEnum::RadioLimitEnum(const std::initializer_list<unsigned> &values, QObject *parent)
  : RadioLimitValue(parent), _values(values)
{
  // pass...
}

bool
RadioLimitEnum::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isEnumType()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected enum type.";
    return false;
  }

  unsigned value = prop.read(item).toUInt();
  if (! _values.contains(value)) {
    QMetaEnum e = prop.enumerator();
    QStringList possible;
    foreach (unsigned val, _values)
      possible.append(e.valueToKey(val));

    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "The enum value '" << e.valueToKey(value) << "' cannot be encoded. "
        << "Valid values are " << possible.join(", ") << ". "
        << "Another value might be chosen automatically.";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitFrequencies::FrequencyRange
 * ********************************************************************************************* */
RadioLimitFrequencies::FrequencyRange::FrequencyRange(double lower, double upper)
  : min(lower), max(upper)
{
  // pass...
}

RadioLimitFrequencies::FrequencyRange::FrequencyRange(const std::pair<double, double> &limit)
  : min(limit.first), max(limit.second)
{
  // pass..
}

bool
RadioLimitFrequencies::FrequencyRange::contains(double f) const {
  return (f >= min) && (f <= max);
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitFrequencies
 * ********************************************************************************************* */
RadioLimitFrequencies::RadioLimitFrequencies(QObject *parent)
  : RadioLimitValue(parent), _frequencyRanges()
{
  // pass...
}

RadioLimitFrequencies::RadioLimitFrequencies(const RangeList &ranges, bool warnOnly, QObject *parent)
  : RadioLimitValue(parent), _frequencyRanges(), _warnOnly(warnOnly)
{
  for (auto range=ranges.begin(); range!=ranges.end(); range++) {
    _frequencyRanges.append(FrequencyRange(range->first, range->second));
  }
}

bool
RadioLimitFrequencies::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::Double != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected frequency in MHz.";
    return false;
  }

  double value = prop.read(item).toDouble();

  foreach (const FrequencyRange &range, _frequencyRanges) {
    if (range.contains(value))
      return true;
  }

  if (context.ignoreFrequencyLimits() || (0 == _frequencyRanges.size()) || _warnOnly) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "Frequency " << value << "MHz is outside of allowed frequency ranges or "
        << "range cannot be determined.";
    return true;
  }

  auto &msg = context.newMessage(RadioLimitIssue::Critical);
  msg << "Frequency " << value << "MHz is outside of allowed frequency ranges.";
  return false;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitTransmitFrequencies
 * ********************************************************************************************* */
RadioLimitTransmitFrequencies::RadioLimitTransmitFrequencies(QObject *parent)
  : RadioLimitFrequencies(parent)
{
  // pass...
}

RadioLimitTransmitFrequencies::RadioLimitTransmitFrequencies(const RangeList &ranges, QObject *parent)
  : RadioLimitFrequencies(ranges, false, parent)
{
  // pass...
}

bool
RadioLimitTransmitFrequencies::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::Double != prop.type()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected frequency in MHz.";
    return false;
  }

  if (item->is<Channel>() && (! item->as<Channel>()->rxOnly())) {
    return RadioLimitFrequencies::verify(item, prop, context);
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitItem
 * ********************************************************************************************* */
RadioLimitItem::RadioLimitItem(QObject *parent)
  : RadioLimitElement(parent), _elements()
{
  // pass...
}

RadioLimitItem::RadioLimitItem(const PropList &list, QObject *parent)
  : RadioLimitElement(parent), _elements(list)
{
  for (QHash<QString,RadioLimitElement*>::iterator item=_elements.begin(); item != _elements.end(); item++) {
    item.value()->setParent(this);
  }
}

bool
RadioLimitItem::add(const QString &prop, RadioLimitElement *structure) {
  if (_elements.contains(prop) || (nullptr == structure))
    return false;
  _elements.insert(prop, structure);
  structure->setParent(this);
  return true;
}

bool
RadioLimitItem::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isReadable()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not readable.";
    return false;
  }

  if (! propIsInstance<ConfigItem>(prop)) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not an instance of ConfigItem.";
    return false;
  }

  if (prop.read(item).isNull())
    return true;

  context.push(QString("Property '%1'").arg(prop.name()));
  bool success = verifyItem(prop.read(item).value<ConfigItem*>(), context);
  context.pop();
  return success;
}

bool
RadioLimitItem::verifyItem(const ConfigItem *item, RadioLimitContext &context) const {
  const QMetaObject *meta = item->metaObject();
  for (int p=QObject::staticMetaObject.propertyCount(); p<meta->propertyCount(); p++) {
    // This property
    QMetaProperty prop = meta->property(p);
    // Should never happen
    if (! prop.isValid())
      continue;
    // Verify property
    if (_elements.contains(prop.name())) {
      if (! _elements[prop.name()]->verify(item, prop, context))
        return false;
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitObject
 * ********************************************************************************************* */
RadioLimitObject::RadioLimitObject(QObject *parent)
  : RadioLimitItem(parent)
{
  // pass...
}

RadioLimitObject::RadioLimitObject(const PropList &list, QObject *parent)
  : RadioLimitItem(list, parent)
{
  // pass...
}

bool
RadioLimitObject::verifyObject(const ConfigObject *item, RadioLimitContext &context) const {
  //context.push(QString("Object '%1'").arg(item->name()));
  bool success = verifyItem(item, context);
  //context.pop();
  return success;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitObjects
 * ********************************************************************************************* */
RadioLimitObjects::RadioLimitObjects(const TypeList &list, QObject *parent)
  : RadioLimitObject(parent), _types()
{
  for (auto type=list.begin(); type!=list.end(); type++) {
    _types[type->first.className()] = type->second;
    type->second->setParent(this);
  }
}

bool
RadioLimitObjects::verifyItem(const ConfigItem *item, RadioLimitContext &context) const {
  if (! _types.contains(item->metaObject()->className())) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check item of type " << item->metaObject()->className()
        << ". Unexpected type. Expected one of " << QStringList(_types.keys()).join(", ") << ".";
    return false;
  }
  return _types[item->metaObject()->className()]->verifyItem(item, context);
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitObjRef
 * ********************************************************************************************* */
RadioLimitObjRef::RadioLimitObjRef(const QMetaObject &type, bool allowNull, QObject *parent)
  : RadioLimitElement(parent), _allowNull(allowNull), _types()
{
  _types.insert(type.className());
}

bool
RadioLimitObjRef::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  ConfigObjectReference *ref = prop.read(item).value<ConfigObjectReference *>();

  if (nullptr == ref) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check type of property '" << prop.name() << "'. Exprected ConfigObjectReference.";
    return false;
  }

  if (ref->isNull()) {
    if (_allowNull)
      return true;

    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "Property '" << prop.name() << "' must refer to an instances of "
        << QStringList(_types.begin(), _types.end()).join(", ") << ".";

    return true;
  }

  if (! validType(ref->as<ConfigObject>()->metaObject())) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Property '" << prop.name() << "' must refer to an instances of "
        << QStringList(_types.begin(), _types.end()).join(", ") << ".";
    return false;
  }

  return true;
}

bool
RadioLimitObjRef::validType(const QMetaObject *type) const {
  if (_types.contains(type->className()))
    return true;
  if (type->superClass())
    return validType(type->superClass());
  return false;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitObjRefIgnored
 * ********************************************************************************************* */
RadioLimitObjRefIgnored::RadioLimitObjRefIgnored(
    ConfigObject *defObj, RadioLimitIssue::Severity notify, QObject *parent)
  : RadioLimitObjRef(ConfigObject::staticMetaObject, true, parent), _severity(notify),
    _default(defObj)
{
  // pass...
}

bool
RadioLimitObjRefIgnored::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  // Get referenced object
  ConfigObjectReference *ref = prop.read(item).value<ConfigObjectReference *>();
  // If reference is set and not default reference
  if ((! ref->isNull()) && (_default != ref->as<ConfigObject>())) {
    auto &msg = context.newMessage(_severity);
    msg << "The reference '" << prop.name() <<
           "' is ignored. Not applicable/supported by this radio.";
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitList
 * ********************************************************************************************* */
RadioLimitList::RadioLimitList(const QMetaObject &type, int minSize, int maxSize, RadioLimitObject *element, QObject *parent)
  : RadioLimitElement(parent), _elements(), _minCount(), _maxCount()
{
  _elements.insert(type.className(), element);
  _minCount.insert(type.className(), minSize);
  _maxCount.insert(type.className(), maxSize);
  element->setParent(this);
}

RadioLimitList::RadioLimitList(const std::initializer_list<ElementLimits> &elements, QObject *parent)
  : RadioLimitElement(parent), _elements(), _minCount(), _maxCount()
{
  for (auto el=elements.begin(); el!=elements.end(); el++) {
    QString className = el->type.className();
    _elements.insert(className, el->structure); el->structure->setParent(this);
    _minCount.insert(className, el->minCount);
    _maxCount.insert(className, el->maxCount);
  }
}

bool
RadioLimitList::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isReadable()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not readable.";
    return false;
  }

  if (nullptr == prop.read(item).value<ConfigObjectList *>()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not an instance of ConfigObjectList.";
    return false;
  }

  const ConfigObjectList *plist = prop.read(item).value<ConfigObjectList*>();

  QHash<QString, unsigned> counts;
  foreach (QString type, _elements.keys())
    counts.insert(type,0);

  context.push(QString("List '%1'").arg(prop.name()));

  // Check type and structure
  for (int i=0; i<plist->count(); i++) {
    // Check type
    ConfigObject *obj = plist->get(i);
    QString className = findClassName(*(obj->metaObject()));
    if (className.isEmpty()) {
      auto &msg = context.newMessage(RadioLimitIssue::Critical);
      msg << "Unexpected element type '" << obj->metaObject()->className()
          << "'. Expected one of " << _elements.keys().join(", ") << ".";
      context.pop();
      return false;
    }

    counts[className]++;

    context.push(QString("Element %1 ('%2')").arg(i).arg(obj->name()));
    if (! _elements[className]->verifyObject(obj, context)) {
      context.pop();
      context.pop();
      return false;
    }
    context.pop();
  }

  // Check counts
  foreach (QString className, _elements.keys()) {
    if ((0 <= _minCount[className]) && (counts[className]<_minCount[className])) {
      auto &msg = context.newMessage(RadioLimitIssue::Warning);
      msg << "The number of elements of type '" << className << "' " << counts[className]
             << " is less than the required count " << _minCount[className] << ".";
    }
    if ((0 <= _maxCount[className]) && (counts[className]>_maxCount[className])) {
      auto &msg = context.newMessage(RadioLimitIssue::Warning);
      msg << "The number of elements of type '" << className << "' " << counts[className]
             << " is greater than the maximum count " << _maxCount[className] << ".";
    }
  }

  context.pop();

  return true;
}

QString
RadioLimitList::findClassName(const QMetaObject &type) const {
  if (_elements.contains(type.className()))
    return type.className();
  if (const QMetaObject *super = type.superClass())
    return findClassName(*super);
  return "";
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitRefList
 * ********************************************************************************************* */
RadioLimitRefList::RadioLimitRefList(int minSize, int maxSize, const QMetaObject &type, QObject *parent)
  : RadioLimitElement(parent), _minSize(minSize), _maxSize(maxSize), _types()
{
  _types.insert(type.className());
}

bool
RadioLimitRefList::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isReadable()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not readable.";
    return false;
  }

  if (nullptr == prop.read(item).value<ConfigObjectRefList *>()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not an instance of ConfigObjectRefList.";
    return false;
  }

  const ConfigObjectRefList *plist = prop.read(item).value<ConfigObjectRefList*>();
  if ((0 <= _minSize) && (_minSize > plist->count())) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "List '" << prop.name() << "' requires at least " << _minSize
        << " elements, " << plist->count() << " elements found.";
    return false;
  }

  if ((0 <= _maxSize) && (_maxSize < plist->count())) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "List '" << prop.name() << "' takes at most " << _minSize
        << " elements, " << plist->count() << " elements found.";
    return false;
  }

  for (int i=0; i<plist->count(); i++) {
    if (! validType(plist->get(i)->metaObject())) {
      auto &msg = context.newMessage(RadioLimitIssue::Critical);
      msg << "Reference to " << plist->get(i)->metaObject()->className() << " is not allowed here. "
          << "Must be one of " << QStringList(_types.begin(), _types.end()).join(", ") << ".";
      return false;
    }
  }

  return true;
}

bool
RadioLimitRefList::validType(const QMetaObject *type) const {
  if (_types.contains(type->className()))
    return true;
  if (type->superClass())
    return validType(type->superClass());
  return false;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitPrivateCallRefList
 * ********************************************************************************************* */
RadioLimitGroupCallRefList::RadioLimitGroupCallRefList(int minSize, int maxSize, QObject *parent)
  : RadioLimitElement(parent), _minSize(minSize), _maxSize(maxSize)
{
  // pass...
}

bool
RadioLimitGroupCallRefList::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isReadable()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not readable.";
    return false;
  }

  if (nullptr == prop.read(item).value<ConfigObjectRefList *>()) {
    auto &msg = context.newMessage(RadioLimitIssue::Critical);
    msg << "Cannot check property " << prop.name() << ": Not an instance of ConfigObjectRefList.";
    return false;
  }

  const ConfigObjectRefList *plist = prop.read(item).value<ConfigObjectRefList*>();
  if ((0 <= _minSize) && (_minSize > plist->count())) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "List '" << prop.name() << "' requires at least " << _minSize
        << " elements, " << plist->count() << " elements found.";
    return false;
  }

  if ((0 <= _maxSize) && (_maxSize < plist->count())) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg << "List '" << prop.name() << "' takes at most " << _minSize
        << " elements, " << plist->count() << " elements found.";
    return false;
  }

  for (int i=0; i<plist->count(); i++) {
    if (! plist->get(i)->is<DMRContact>()) {
      auto &msg = context.newMessage(RadioLimitIssue::Critical);
      msg << "Reference to " << plist->get(i)->metaObject()->className() << " is not allowed here. "
          << "Must be DigtialContact.";
      return false;
    }
    if (DMRContact::GroupCall != plist->get(i)->as<DMRContact>()->type()) {
      auto &msg = context.newMessage(RadioLimitIssue::Critical);
      msg << "Expected reference to a group call digital contact.";
      return false;
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitSingleZone
 * ********************************************************************************************* */
RadioLimitSingleZone::RadioLimitSingleZone(qint64 maxSize, const PropList &list, QObject *parent)
  : RadioLimitObject(list, parent)
{
  // A and B may hold up to maxSize references to channels
  _elements["A"] = new RadioLimitRefList(1, maxSize, Channel::staticMetaObject, this);
  _elements["B"] = new RadioLimitRefList(0, maxSize, Channel::staticMetaObject, this);
}

bool
RadioLimitSingleZone::verifyItem(const ConfigItem *item, RadioLimitContext &context) const {
  if (! RadioLimitObject::verifyItem(item, context))
    return false;

  const Zone *zone = item->as<Zone>();
  if (zone && zone->B()->count()) {
    auto &msg = context.newMessage();
    msg << "This radio does not support dual channel zones. The zone '" << zone->name()
        << "' will be split into two.";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimits
 * ********************************************************************************************* */
RadioLimits::RadioLimits(bool betaWarning, QObject *parent)
  : RadioLimitItem(parent), _betaWarning(betaWarning)
{
  // pass...
}

RadioLimits::RadioLimits(const std::initializer_list<std::pair<QString, RadioLimitElement *> > &list, QObject *parent)
  : RadioLimitItem(list, parent)
{
  // pass...
}

bool
RadioLimits::hasCallSignDB() const {
  return _hasCallSignDB;
}

bool
RadioLimits::callSignDBImplemented() const {
  return _callSignDBImplemented;
}

unsigned
RadioLimits::numCallSignDBEntries() const {
  return _numCallSignDBEntries;
}

bool
RadioLimits::verifyConfig(const Config *config, RadioLimitContext &context) const {
  if (_betaWarning) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg = tr("The support for this radio is still under development. Some features may sill be "
             "missing or are not well tested.");
  }

  return verifyItem(config, context);
}
