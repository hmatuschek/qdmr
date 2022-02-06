#include "radiolimits.hh"
#include "configobject.hh"
#include "logger.hh"
#include "config.hh"
#include <QMetaProperty>


/* ********************************************************************************************* *
 * Implementation of RadioLimitContext::Message
 * ********************************************************************************************* */
RadioLimitContext::Message::Message(Severity severity, const QStringList &stack)
  : QTextStream(), _severity(severity), _stack(stack), _message()
{
  setString(&_message);
}

RadioLimitContext::Message::Message(const Message &other)
  : QTextStream(), _severity(other._severity), _stack(other._stack), _message(other._message)
{
  setString(&_message);
}

RadioLimitContext::Message &
RadioLimitContext::Message::operator =(const Message &other) {
  QTextStream::flush();
  _severity = other._severity;
  _stack = other._stack;
  _message = other._message;
  return *this;
}

QString
RadioLimitContext::Message::format() const {
  QString res; QTextStream stream(&res);
  switch (_severity) {
  case Hint: stream << "Hint: "; break;
  case Warning: stream << "Warn: "; break;
  case Critical: stream << "Crit: "; break;
  }
  stream << " In " << _stack.join(", ") << ": " << _message;
  stream.flush();
  return res;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitContext
 * ********************************************************************************************* */
RadioLimitContext::RadioLimitContext()
  : _stack()
{
  // pass...
}

RadioLimitContext::Message &
RadioLimitContext::newMessage(Message::Severity severity) {
  _messages.push_back(Message(severity, _stack));
  return _messages.back();
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
RadioLimitIgnored::RadioLimitIgnored(Notification notify, QObject *parent)
  : RadioLimitElement(parent), _notification(notify)
{
  // pass...
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
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected string.";
    return false;
  }

  QString value = prop.read(item).toString();

  if ((0<_maxLen) && (value.size() > int(_maxLen))) {
    auto &msg = context.newMessage();
    msg << "String length of " << prop.name() << " exceeds maximum length " << _maxLen << ".";
  }

  if ((0<_minLen) && (value.size() < int(_minLen))) {
    auto &msg = context.newMessage();
    msg << "String length of " << prop.name() << " is shorter than " << _minLen << ".";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitUInt
 * ********************************************************************************************* */
RadioLimitUInt::RadioLimitUInt(qint64 minValue, qint64 maxValue, QObject *parent)
  : RadioLimitValue(parent), _minValue(minValue), _maxValue(maxValue)
{
  // pass...
}

bool
RadioLimitUInt::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::UInt != prop.type()) {
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected uint.";
    return false;
  }

  unsigned value = prop.read(item).toUInt();

  if ((0<_maxValue) && (value > qint64(_maxValue))) {
    auto &msg = context.newMessage();
    msg << "Value " << value << " of " << prop.name() << " exceeds maximum " << _maxValue << ".";
  }

  if ((0<_minValue) && (value < qint64(_minValue))) {
    auto &msg = context.newMessage();
    msg << "Value " << value << " of " << prop.name() << " is smaller than minimum " << _minValue << ".";
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitUInt
 * ********************************************************************************************* */
RadioLimitEnum::RadioLimitEnum(const std::initializer_list<unsigned> &values, QObject *parent)
  : RadioLimitValue(parent), _values(values)
{
  // pass...
}

bool
RadioLimitEnum::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isEnumType()) {
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected enum type.";
    return false;
  }

  unsigned value = prop.read(item).toUInt();
  if (! _values.contains(value)) {
    QMetaEnum e = prop.enumerator();
    QStringList possible;
    foreach (unsigned val, _values)
      possible.append(e.valueToKey(val));

    auto &msg = context.newMessage(RadioLimitContext::Message::Warning);
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

RadioLimitFrequencies::RadioLimitFrequencies(const std::initializer_list<std::pair<double, double> > &ranges, QObject *parent)
  : RadioLimitValue(parent), _frequencyRanges()
{
  for (auto range=ranges.begin(); range!=ranges.end(); range++)
    _frequencyRanges.append(FrequencyRange(range->first, range->second));
}

bool
RadioLimitFrequencies::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (QVariant::Double != prop.type()) {
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Expected frequency in MHz.";
    return false;
  }

  double value = prop.read(item).toDouble();

  foreach (const FrequencyRange &range, _frequencyRanges) {
    if (range.contains(value))
      return true;
  }

  RadioLimitContext::Message &msg = context.newMessage(RadioLimitContext::Message::Warning);
  msg << "Frequency " << value << "MHz is outside of allowed frequency ranges.";
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

RadioLimitItem::RadioLimitItem(const std::initializer_list<std::pair<QString, RadioLimitElement *> > &list, QObject *parent)
  : RadioLimitElement(parent), _elements(list)
{
  for (QHash<QString,RadioLimitElement*>::iterator item=_elements.begin(); item != _elements.end(); item++) {
    item.value()->setParent(this);
  }
}

bool
RadioLimitItem::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isReadable()) {
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Not readable.";
    return false;
  }

  if (! propIsInstance<ConfigItem>(prop)) {
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Not an instance of ConfigItem.";
    return false;
  }

  if (prop.read(item).isNull())
    return true;

  return verifyItem(prop.read(item).value<ConfigItem*>(), context);
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

RadioLimitObject::RadioLimitObject(const std::initializer_list<std::pair<QString, RadioLimitElement *> > &list, QObject *parent)
  : RadioLimitItem(list, parent)
{
  // pass...
}

bool
RadioLimitObject::verifyObject(const ConfigObject *item, RadioLimitContext &context) const {
  return verifyItem(item, context);
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitObjects
 * ********************************************************************************************* */
RadioLimitObjects::RadioLimitObjects(const std::initializer_list<std::pair<const QMetaObject &, RadioLimitObject *> > &list, QObject *parent)
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
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check item of type " << item->metaObject()->className()
        << ". Unexpected type. Expected one of " << QStringList(_types.keys()).join(", ") << ".";
    return false;
  }
  return _types[item->metaObject()->className()]->verifyItem(item, context);
}


/* ********************************************************************************************* *
 * Implementation of RadioLimitList
 * ********************************************************************************************* */
RadioLimitList::RadioLimitList(int minSize, int maxSize, RadioLimitObject *element, QObject *parent)
  : RadioLimitElement(parent), _minSize(minSize), _maxSize(maxSize), _element(element)
{
  _element->setParent(this);
}

bool
RadioLimitList::verify(const ConfigItem *item, const QMetaProperty &prop, RadioLimitContext &context) const {
  if (! prop.isReadable()) {
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Not readable.";
    return false;
  }

  if (nullptr == prop.read(this).value<ConfigObjectList *>()) {
    auto &msg = context.newMessage(RadioLimitContext::Message::Critical);
    msg << "Cannot check property " << prop.name() << ": Not an instance of ConfigObjectList.";
    return false;
  }

  const ConfigObjectList *plist = prop.read(item).value<ConfigObjectList*>();
  for (int i=0; i<plist->count(); i++) {
    if (! _element->verifyObject(plist->get(i), context))
      return false;
  }

  return false;
}


/* ********************************************************************************************* *
 * Implementation of RadioLimits
 * ********************************************************************************************* */
RadioLimits::RadioLimits(QObject *parent)
  : RadioLimitItem(parent)
{
  // pass...
}

RadioLimits::RadioLimits(const std::initializer_list<std::pair<QString, RadioLimitElement *> > &list, QObject *parent)
  : RadioLimitItem(list, parent)
{
  // pass...
}

bool
RadioLimits::verifyConfig(const Config *config, RadioLimitContext &context) {
  return verifyItem(config, context);
}
