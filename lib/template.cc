#include "template.hh"


/* ********************************************************************************************* *
 * Implementation of FieldDeclaration
 * ********************************************************************************************* */
FieldDeclaration::FieldDeclaration(const QString &name, const QString &description, QObject *parent)
  : QObject(parent), _name(name), _description(description)
{
  // pass...
}

FieldDeclaration::FieldDeclaration(const YAML::Node &node, QObject *parent)
  : QObject(parent), _name(), _description()
{
  if (node["name"] && (YAML::NodeType::Scalar == node["name"].Type()))
    _name = QString::fromStdString(node["name"].as<std::string>());
  if (node["description"] && (YAML::NodeType::Scalar == node["description"].Type()))
    _description = QString::fromStdString(node["description"].as<std::string>());
}

FieldDeclaration::~FieldDeclaration() {
  // pass...
}

const QString &
FieldDeclaration::name() const {
  return _name;
}

const QString &
FieldDeclaration::description() const {
  return _description;
}


/* ********************************************************************************************* *
 * Implementation of BoolFieldDeclaration
 * ********************************************************************************************* */
BoolFieldDeclaration::BoolFieldDeclaration(const QString &name, const QString &description, QObject *parent)
  : FieldDeclaration(name, description, parent)
{
  // pass...
}

BoolFieldDeclaration::BoolFieldDeclaration(const YAML::Node &node, QObject *parent)
  : FieldDeclaration(node, parent)
{
  // pass...
}

BoolFieldDeclaration::~BoolFieldDeclaration() {
  // pass...
}

bool
BoolFieldDeclaration::verify(const YAML::Node &node, QString &message) const {
  if ((! node.IsDefined()) || (! node.IsScalar()))
    return false;
  return ("true" == node.as<std::string>()) || ("false" == node.as<std::string>());
}


/* ********************************************************************************************* *
 * Implementation of IntFieldDeclaration
 * ********************************************************************************************* */
IntFieldDeclaration::IntFieldDeclaration(
    const QString &name, int min, int max, const QString &description, QObject *parent)
  : FieldDeclaration(name, description, parent), _min(min), _max(max)
{
  // pass...
}

IntFieldDeclaration::IntFieldDeclaration(const YAML::Node &node, QObject *parent)
  : FieldDeclaration(node, parent), _min(std::numeric_limits<int>::min()),
    _max(std::numeric_limits<int>::max())
{
  if (node["min"] && node["min"].IsScalar())
    _min = node["min"].as<int>();
  if (node["max"] && node["max"].IsScalar())
    _max = node["max"].as<int>();
}

IntFieldDeclaration::~IntFieldDeclaration() {
  // pass...
}

bool
IntFieldDeclaration::verify(const YAML::Node &node, QString &message) const {
  if ((! node.IsDefined()) || (! node.IsScalar()))
    return false;
  int val = node.as<int>();
  return (val >= _min) && (val <= _max);
}


/* ********************************************************************************************* *
 * Implementation of StringFieldDeclaration
 * ********************************************************************************************* */
StringFieldDeclaration::StringFieldDeclaration(
    const QString &name, size_t maxLength, const QString &description, QObject *parent)
  : FieldDeclaration(name, description, parent), _maxLength(maxLength)
{
  // pass...
}

StringFieldDeclaration::StringFieldDeclaration(const YAML::Node &node, QObject *parent)
  : FieldDeclaration(node, parent), _maxLength(std::numeric_limits<size_t>::max())
{
  if (node["max-len"] && node["max-len"].IsScalar())
    _maxLength = node["max-len"].as<size_t>();
}

StringFieldDeclaration::~StringFieldDeclaration() {
  // pass...
}

bool
StringFieldDeclaration::verify(const YAML::Node &node, QString &message) const {
  if ((! node.IsDefined()) || (! node.IsScalar()))
    return false;
  std::string val = node.as<std::string>();
  return (val.size() <= _maxLength);
}


/* ********************************************************************************************* *
 * Implementation of EnumFieldDeclaration
 * ********************************************************************************************* */
EnumFieldDeclaration::EnumFieldDeclaration(
    const QString &name, const QHash<QString, uint> &pairs, const QString &description, QObject *parent)
  : FieldDeclaration(name, description, parent), _values(pairs)
{
  // pass...
}

EnumFieldDeclaration::EnumFieldDeclaration(const YAML::Node &node, QObject *parent)
  : FieldDeclaration(node, parent), _values()
{
  if (node["values"] && node["values"].IsMap()) {
    YAML::Node values = node["values"];
    for (YAML::const_iterator it=values.begin(); it!=values.end(); it++) {
      QString key = QString::fromStdString(it->first.as<std::string>());
      uint value = it->second.as<uint>();
      _values.insert(key, value);
    }
  }
}

EnumFieldDeclaration::~EnumFieldDeclaration() {
  // pass...
}

bool
EnumFieldDeclaration::verify(const YAML::Node &node, QString &message) const {
  if ((! node.IsDefined()) || (! node.IsScalar()))
    return false;
  QString key = QString::fromStdString(node.as<std::string>());
  return _values.contains(key);
}


/* ********************************************************************************************* *
 * Implementation of TemplateFieldDeclaration
 * ********************************************************************************************* */
TemplateFieldDeclaration::TemplateFieldDeclaration(const QString &name, const QString &description, QObject *parent)
  : FieldDeclaration(name, description, parent), _fields()
{
  // pass...
}

TemplateFieldDeclaration::TemplateFieldDeclaration(const YAML::Node &node, QObject *parent)
  : FieldDeclaration(node, parent)
{
  if (node["fields"] && node["fields"].IsSequence()) {
    YAML::Node fields = node["fields"];
    for (YAML::const_iterator field=fields.begin(); field!=fields.end(); field++) {
      if (! (*field)["type"])
        continue;
      std::string field_type = (*field)["type"].as<std::string>();
      if ("bool" == field_type) {
        addField(new BoolFieldDeclaration(*field, this));
      } else if ("int" == field_type) {
        addField(new IntFieldDeclaration(*field, this));
      } else if ("string" == field_type) {
        addField(new StringFieldDeclaration(*field, this));
      } else if ("enum" == field_type) {
        addField(new EnumFieldDeclaration(*field, this));
      } else if ("template" == field_type) {
        addField(new TemplateFieldDeclaration(*field, this));
      }
    }
  }
}

TemplateFieldDeclaration::~TemplateFieldDeclaration() {
  // pass...
}

void
TemplateFieldDeclaration::addField(FieldDeclaration *field) {
  if (nullptr == field)
    return;
  if (_fields.contains(field->name()))
    return;
  field->setParent(this);
  _fields.insert(field->name(), field);
}

bool
TemplateFieldDeclaration::verify(const YAML::Node &node, QString &message) const {
  if ((! node.IsDefined()) || (! node.IsMap()))
    return false;
  // check every member
  YAML::Node fields = node["fields"];
  for (YAML::const_iterator field=fields.begin(); field!=fields.end(); field++) {
    QString field_name = QString::fromStdString(field->first.as<std::string>());
    if (! _fields.contains(field_name))
      return false;
    if (! _fields[field_name]->verify(field->second, message))
      return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of TemplateDeclaration
 * ********************************************************************************************* */
TemplateDeclaration::TemplateDeclaration(const YAML::Node &node, QObject *parent)
  : QObject(parent), _name(""), _description("")
{
  if (node["name"] && (YAML::NodeType::Scalar == node["name"].Type()))
    _name = QString::fromStdString(node["name"].as<std::string>());
  if (node["description"] && (YAML::NodeType::Scalar == node["description"].Type()))
    _description = QString::fromStdString(node["description"].as<std::string>());

  if (node["fields"] && node["fields"].IsSequence()) {
    YAML::Node fields = node["fields"];
    for (YAML::const_iterator field = fields.begin(); field!=fields.end(); field++) {
      if (! (*field)["type"])
        continue;
      std::string field_type = (*field)["type"].as<std::string>();
      if ("bool" == field_type) {
        addField(new BoolFieldDeclaration(*field, this));
      } else if ("int" == field_type) {
        addField(new IntFieldDeclaration(*field, this));
      } else if ("string" == field_type) {
        addField(new StringFieldDeclaration(*field, this));
      } else if ("enum" == field_type) {
        addField(new EnumFieldDeclaration(*field, this));
      } else if ("template" == field_type) {
        addField(new TemplateFieldDeclaration(*field, this));
      }
    }
  }
}

TemplateDeclaration::TemplateDeclaration(const QString &device, const QString &name, const QString &description, QObject *parent)
  : QObject(parent), _name(name), _description(description), _fields()
{
  // pass...
}

const QString &
TemplateDeclaration::device() const {
  return _device;
}

const QString &
TemplateDeclaration::name() const {
  return _name;
}

bool
TemplateDeclaration::hasField(const QString &name) const {
  return _fields.contains(name);
}

const FieldDeclaration *
TemplateDeclaration::field(const QString &field) const {
  return _fields.value(field, nullptr);
}

FieldDeclaration *
TemplateDeclaration::field(const QString &field) {
  return _fields.value(field, nullptr);
}

void
TemplateDeclaration::addField(FieldDeclaration *field) {
  if (nullptr == field)
    return;
  if (_fields.contains(field->name()))
    return;
  field->setParent(this);
  _fields.insert(field->name(), field);
}
