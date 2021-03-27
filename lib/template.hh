#ifndef TEMPLATE_HH
#define TEMPLATE_HH

#include <QObject>
#include <QHash>
#include <yaml-cpp/yaml.h>

class FieldDeclaration: public QObject {
  Q_OBJECT

protected:
  explicit FieldDeclaration(const YAML::Node &node, QObject *parent=nullptr);
  FieldDeclaration(const QString &name, const QString &description="", QObject *parent = nullptr);

public:
  virtual ~FieldDeclaration();

  template<class FieldType>
  bool is() const {
    return nullptr != qobject_cast<const FieldType *>(this);
  }

  template<class FieldType>
  const FieldType *as() const {
    return qobject_cast<const FieldType *>(this);
  }

  template<class FieldType>
  FieldType *as() {
    return qobject_cast<FieldType *>(this);
  }

  const QString &name() const;
  const QString &description() const;

  virtual bool verify(const YAML::Node &node, QString &message) const = 0;

protected:
  QString _name;
  QString _description;
};


class BoolFieldDeclaration: public FieldDeclaration {
  Q_OBJECT
public:
  explicit BoolFieldDeclaration(const YAML::Node &node, QObject *parent=nullptr);
  BoolFieldDeclaration(const QString &name, const QString &description="", QObject *parent = nullptr);
  virtual ~BoolFieldDeclaration();

  bool verify(const YAML::Node &node, QString &message) const;
};


class IntFieldDeclaration: public FieldDeclaration {
  Q_OBJECT
public:
  explicit IntFieldDeclaration(const YAML::Node &node, QObject *parent=nullptr);
  IntFieldDeclaration(const QString &name, int min=std::numeric_limits<int>::min(),
           int max=std::numeric_limits<int>::max(), const QString &description="",
           QObject *parent = nullptr);
  virtual ~IntFieldDeclaration();

  bool verify(const YAML::Node &node, QString &message) const;

protected:
  int _min;
  int _max;
};


class StringFieldDeclaration: public FieldDeclaration {
  Q_OBJECT

public:
  typedef enum {
    ASCII_ENCODING,
    UNICODE_ENCODING
  } Encoding;

public:
  explicit StringFieldDeclaration(const YAML::Node &node, QObject *parent=nullptr);
  StringFieldDeclaration(const QString &name, size_t maxLength=std::numeric_limits<size_t>::max(),
              const QString &description="", QObject *parent = nullptr);
  virtual ~StringFieldDeclaration();

  bool verify(const YAML::Node &node, QString &message) const;

protected:
  size_t _maxLength;
};


class EnumFieldDeclaration: public FieldDeclaration {
  Q_OBJECT
public:
  explicit EnumFieldDeclaration(const YAML::Node &node, QObject *parent=nullptr);
  EnumFieldDeclaration(const QString &name, const QHash<QString, uint> &pairs=QHash<QString, uint>(),
                    const QString &description="", QObject *parent = nullptr);
  virtual ~EnumFieldDeclaration();

  bool verify(const YAML::Node &node, QString &message) const;

protected:
  QHash<QString, uint> _values;
};


class TemplateFieldDeclaration: public FieldDeclaration {
  Q_OBJECT
public:
  TemplateFieldDeclaration(const YAML::Node &node, QObject *parent = nullptr);
  TemplateFieldDeclaration(const QString &name, const QString &description="", QObject *parent = nullptr);
  virtual ~TemplateFieldDeclaration();

  bool hasField(const QString &name) const;
  const FieldDeclaration *field(const QString &field) const;
  FieldDeclaration *field(const QString &field);
  void addField(FieldDeclaration *field);

  bool verify(const YAML::Node &node, QString &message) const;

protected:
  QHash<QString, FieldDeclaration *> _fields;
};


class TemplateDeclaration : public QObject
{
  Q_OBJECT

public:
  explicit TemplateDeclaration(const YAML::Node &node, QObject *parent = nullptr);
  TemplateDeclaration(const QString &device, const QString &name, const QString &description="", QObject *parent = nullptr);

  const QString &device() const;
  const QString &name() const;

  bool hasField(const QString &name) const;
  const FieldDeclaration *field(const QString &field) const;
  FieldDeclaration *field(const QString &field);
  void addField(FieldDeclaration *field);

protected:
  QString _device;
  QString _name;
  QString _description;
  QHash<QString, FieldDeclaration *> _fields;
};


#endif // TEMPLATE_HH
