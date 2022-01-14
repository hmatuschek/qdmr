#ifndef RADIOLIMITS_HH
#define RADIOLIMITS_HH

#include <QObject>

// Forward declaration
class RadioLimits;

class RadioLimitContext
{
public:
  explicit RadioLimitContext(const RadioLimits &limits);

protected:
  const RadioLimits &_limits;
  QList<QPair<QString, QString>> _stack;
};


class RadioLimitElement: public QObject
{
  Q_OBJECT

protected:
  explicit RadioLimitElement(QObject *parent=nullptr);

public:
  virtual ~RadioLimitElement();
};


class RadioLimitIgnored: public RadioLimitElement
{
  Q_OBJECT

public:
  enum Notification {
    Silent, Warning, Error
  };

public:
  RadioLimitIgnored(Notification notify=Silent, QObject *parent=nullptr);

protected:
  Notification _notification;
};


class RadioLimitValue: public RadioLimitElement
{
  Q_OBJECT

protected:
  explicit RadioLimitValue(QObject *parent=nullptr);

public:
  virtual ~RadioLimitValue();

  virtual bool verify(const QMetaProperty &prop, RadioLimitContext &context) const = 0;
};


class RadioLimitString: public RadioLimitValue
{
  Q_OBJECT
  enum class Encoding {
    ASCII, Unicode
  };

public:
  RadioLimitString(unsigned minLen, unsigned maxLen, Encoding enc, QObject *parent=nullptr);

  bool verify(const QMetaProperty &prop, RadioLimitContext &context);

protected:
  unsigned _minLen;
  unsigned _maxLen;
  Encoding _encoding;
};

class RadioLimitItem: public RadioLimitElement
{
  Q_OBJECT

public:
  explicit RadioLimitItem(QObject *parent=nullptr);
};


class RadioLimitObject: public RadioLimitItem
{
  Q_OBJECT

public:
  explicit RadioLimitObject(QObject *parent=nullptr);
};


class RadioLimitList: public RadioLimitElement
{
  Q_OBJECT
};


class RadioLimitRefList: public RadioLimitElement
{

};


class RadioLimits : public QObject
{
  Q_OBJECT

public:
  explicit RadioLimits(QObject *parent = nullptr);

signals:

};

#endif // RADIOLIMITS_HH
